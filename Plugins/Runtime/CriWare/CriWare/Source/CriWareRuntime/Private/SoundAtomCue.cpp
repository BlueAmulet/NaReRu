/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2020 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Sound Data (Cue)
 * File     : SoundAtomCue.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "SoundAtomCue.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "AtomStatics.h"

/* Unreal Engine 4関連ヘッダ */
#include "Serialization/CustomVersion.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "SoundAtomCue"

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/
/* バージョン番号 */

/* AtomCueアセットのバージョン管理用 */
struct FAtomCueVersion
{
	enum Type
	{
		/* バージョン番号が付与される前 */
		BeforeCustomVersionWasAdded = 0,

		/* バージョン情報を追加 */
		StoreVersionInfo,

		/* キュー名をアセットに保持 */
		StoreCueName,

		/* SoundConcurrency情報をアセットに保持 */
		StoreSoundConcurrency,

		/* New culling solver */
		UseNewCullingSolver,

		/* Custom tick frequency */
		StoreCustomTickFrequency,

		/* CueSheet is async loaded */
		UseAsyncCueSheet,

		/* Store AudioVolume settings */
		StoreAudioVolume,

		/* 最新バージョンの番号を取得するための番兵 */
		VersionPlusOne,

		/* 最新バージョンの番号を指す */
		LatestVersion = VersionPlusOne - 1,
	};

	/* GUID */
	static const FGuid GUID;
};

/* GUID */
const FGuid FAtomCueVersion::GUID(0x0FAD527A, 0xEEC1492E, 0x8819E1BB, 0xE3B780B9);

/* バージョン登録 */
FCustomVersionRegistration GAtomCueVersion(
	FAtomCueVersion::GUID, FAtomCueVersion::LatestVersion, TEXT("AtomCueAssetVersion"));

/***************************************************************************
 *      定数定義
 *      Constant Definition
 ***************************************************************************/
namespace {
	float ATTENUATION_DISTANCE_NOT_INITIALIZED = -1.0f;
}

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
USoundAtomCue::USoundAtomCue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CueSheet(nullptr)
	, LoopSetting(EAtomLoopSetting::Inherited)
	, bApplyAtomParameter(false)
	, Duration(0.0f)
	, FirstWaveDuration(0.0f)
	, AttenuationSettings(nullptr)
	, bEnableAudioVolume(false)
	, bIsMovable(false)
	, bCanStraddleAudioVolume(false)
	, bUseDistanceToEnableAudioVolume(false)
	, MinDistanceToEnableAudioVolume(0.0f)
	, bUseAreaSoundVolume(false)
	, UpdateRateForFrame(1)
	, bEconomicTick(false)
	, bOverrideEconomicTickSettings(false)
	, EconomicTickDistanceMargin(WORLD_MAX)
	, EconomicTickFrequency(15)
	, bEnableDistanceCulling(false)
	, bOverrideCullSettings(false)
	, CullDistanceMargin(WORLD_MAX)
	, EconomicTickMarginDistance_DEPRECATED(0.0f)
	, bCullingSoundPlayingProcessByDistance_DEPRECATED(false)
	, bUseLegacyCullingSettings_DEPRECATED(false)
	, MaxProcessDistance_DEPRECATED(0.0f)
	, bOverrideCullingSettings_DEPRECATED(0.0f)
	, CullingMarginDistance_DEPRECATED(0.0f)
	, bLoop_DEPRECATED(false)
	, DefaultAttenuation_DEPRECATED(nullptr)

#if WITH_EDITOR
	, RawPCMData(nullptr)
	, RawPCMDataChannels(0)
	, RawPCMDataSamplingRate(0)
	, RawPCMDataSamples(0)
#endif
	, CueInfo(nullptr)
	, CueId(-1)
	, bIsRequestingCueInfo(false)
{
#if WITH_EDITOR
	/* Localization of unreal properties metadata with LOCTEXT markups and reflection */
	CRI_LOCCLASS(GetClass());
#endif

#if WITH_EDITOR
	UserData.Reset();
	CategoryName.Reset();
	AisacControlName.Reset();
#endif
}

const FSoundAttenuationSettings* USoundAtomCue::GetAttenuationSettingsToApply() const
{
	if (AttenuationSettings)
	{
		return &AttenuationSettings->Attenuation;
	}
	return nullptr;
}

float USoundAtomCue::GetMaxAttenuationDistanceToApply() const
{
	if (AttenuationSettings) {
		return AttenuationSettings->Attenuation.FalloffDistance;
	} else {
		if (CueInfo) {
			return CueInfo->AttenuationDistance.MaxDistance;
		}

		return 0.0f;
	}
}

float USoundAtomCue::GetCullDistanceMarginToApply() const
{
	if (bOverrideCullSettings) {
		return CullDistanceMargin;
	} else {
		return UCriWareInitializer::GetCullDistanceMargin();
	}
}

float USoundAtomCue::GetCullDistance() const
{
	// (距離減衰の最大値+CullDistanceMargin)を返す
	return GetMaxAttenuationDistanceToApply() + GetCullDistanceMarginToApply();
}

bool USoundAtomCue::IsLooping() const
{
	// local value
	if (LoopSetting != EAtomLoopSetting::Inherited) {
		return (LoopSetting == EAtomLoopSetting::Loop);
	}

	// inherited not present
	if (!CueSheet) {
		return (LoopSetting == EAtomLoopSetting::Loop);
	}

	// cached ptr
	if (CueInfo) {
		return CueInfo->bLooping;
	}

	// direct to cueinfo array in rare case cueinfo ptr is not yet initialized
	if (CueSheet->IsLoaded()) {
		const FAtomCueInfo* CueInfoPtr = CueSheet->GetCueInfoFromName(CueName);
		if (CueInfoPtr) {
			return CueInfoPtr->bLooping;
		} else {
			UE_LOG(LogCriWareRuntime, Error, TEXT("AtomCue '%s' is not present in AtomCueSheet '%s'."), *CueName, *CueSheet->CueSheetName);
			return false;
		}

	}

	UE_LOG(LogCriWareRuntime, Error, TEXT("CueSheet is not loaded."));
	return false;
}

float USoundAtomCue::GetDuration()
{
	// if editor or if not initialized, re-calc duration
	if (GIsEditor || (Duration < SMALL_NUMBER)) {
		// check if linked to a cue sheet
		if (!CueSheet) {
			return IsLooping() ? INDEFINITELY_LOOPING_DURATION : Duration;
		}

		// cached ptr or direct to cueinfo array in rare case cueinfo ptr is not yet initialized
		if (CueInfo) {
			// @todo: get delay time caused by bus reverb/chorus/delay etc... not yet possible criatom do not let acces to bus information yet.
			Duration = IsLooping() ? INDEFINITELY_LOOPING_DURATION :
					   !CueInfo->bLooping ? CueInfo->Duration.GetTotalSeconds() : GetFirstWaveDuration();

		} else if (CueSheet->IsLoaded()) {
			
			const FAtomCueInfo* CueInfoPtr = CueSheet->GetCueInfoFromName(CueName);
			if (CueInfoPtr) {
				Duration = IsLooping() ? INDEFINITELY_LOOPING_DURATION :
				           !CueInfoPtr->bLooping ? CueInfoPtr->Duration.GetTotalSeconds() : GetFirstWaveDuration();
			} else {
				UE_LOG(LogCriWareRuntime, Error, TEXT("AtomCue '%s' is not present in AtomCueSheet '%s'."), *CueName, *CueSheet->CueSheetName);
			}

		} else {
			UE_LOG(LogCriWareRuntime, Error, TEXT("CueSheet is not loaded."));
		}
	}

	return Duration;
}

bool USoundAtomCue::BP_GetCueInfo(FAtomCueInfo& InCueInfo) const
{
	if (CueInfo) {
		InCueInfo = *CueInfo;
		return true;
	} else if (CueSheet->IsLoaded()) {
		CueSheet->BP_GetCueInfoFromName(CueName, InCueInfo);
		return true;
	}

	InCueInfo = FAtomCueInfo();
	return false;
}

void USoundAtomCue::Serialize(FArchive& Ar)
{
	// Always force the duration to be updated when we are saving or cooking
	if (Ar.IsSaving() || Ar.IsCooking())
	{
		if (CueSheet) {
			// If cue sheet is not yet loaded, wait until data is availabe
			CueSheet->MakeSureCueSheetIsLoaded();
		}

		Duration = GetDuration();
		CacheAggregateValues();
	}

	Super::Serialize(Ar);

	/* アセットバージョンの使用を通知 */
	Ar.UsingCustomVersion(FAtomCueVersion::GUID);

	/* バージョンチェック */
	if (Ar.IsLoading()) {
		/* アーカイブのバージョンをチェック */
		int32 archiveVer = Ar.CustomVer(FAtomCueVersion::GUID);

		/* キューシート名がない場合は補完 */
		if (archiveVer < FAtomCueVersion::StoreCueName) {
			CueName = GetFName().ToString(); // FIXME: seems incorect asset name != cue name in ACB
		}

#if WITH_EDITORONLY_DATA
		bool bUpdated = false;

		if (archiveVer < FAtomCueVersion::UseNewCullingSolver) {

			if (bCullingSoundPlayingProcessByDistance_DEPRECATED) {
				bEnableDistanceCulling = bCullingSoundPlayingProcessByDistance_DEPRECATED;
				bCullingSoundPlayingProcessByDistance_DEPRECATED = false;
			}

			// If a max distance was set and activated (use legacy), 
			// or, for older versions, if CullDistanceMargin was not set (WORLD_MAX))
			if (MaxProcessDistance_DEPRECATED > KINDA_SMALL_NUMBER
				&& (bUseLegacyCullingSettings_DEPRECATED == true || CullingMarginDistance_DEPRECATED == WORLD_MAX))
			{
				// enable new values
				bOverrideCullSettings = true;
				// set new margin
				CullDistanceMargin = MaxProcessDistance_DEPRECATED - AttenuationDistanceParam.MaxDistance;
				// reset deprected value
				MaxProcessDistance_DEPRECATED = 0.0f;
				bUseLegacyCullingSettings_DEPRECATED = false;
				bOverrideCullingSettings_DEPRECATED = false;
				CullingMarginDistance_DEPRECATED = 0.0f;
			}

			if (bOverrideCullingSettings_DEPRECATED) {
				bOverrideCullSettings = bOverrideCullingSettings_DEPRECATED;
				bOverrideCullingSettings_DEPRECATED = false;
			}

			if (CullingMarginDistance_DEPRECATED > KINDA_SMALL_NUMBER) {
				CullDistanceMargin = CullingMarginDistance_DEPRECATED;
				CullingMarginDistance_DEPRECATED = 0.0f;
			}

			bUpdated = true;
		}

		if (archiveVer < FAtomCueVersion::StoreCustomTickFrequency) {
			if (EconomicTickMarginDistance_DEPRECATED > KINDA_SMALL_NUMBER) {
				EconomicTickDistanceMargin = EconomicTickMarginDistance_DEPRECATED;
				EconomicTickMarginDistance_DEPRECATED = 0.0f;
				bUpdated = true;
			}
		}

		if (archiveVer < FAtomCueVersion::UseAsyncCueSheet) {
			AttenuationSettings = DefaultAttenuation_DEPRECATED;
			bUpdated = true;
		}

		/* - UObjectBaseUtility::MarkPackageDirty関数を参考。エディタ上でも保存できるようにする。 */
		if (bUpdated) {
			auto Package = GetOutermost();
			if (Package) {
				Package->SetDirtyFlag(true);
				Package->PackageMarkedDirtyEvent.Broadcast(Package, true);
			}
		}
#endif
	}
}

// FIXME update culling solver properties
void USoundAtomCue::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	// キューシートが設定されているかどうかチェック
	if (!CueSheet) {
		return;
	}

	// Cache any needed data from CueSheet
	CacheAggregateValues();
#endif // WITH EDITOR
}

#if WITH_EDITOR
void USoundAtomCue::PostInitProperties()
{
	Super::PostInitProperties();

	CacheAggregateValues();
}

void USoundAtomCue::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property != nullptr) {
		FName PropertyName = PropertyChangedEvent.Property->GetFName();

		if (PropertyName == GET_MEMBER_NAME_CHECKED(USoundAtomCue, CueSheet) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(USoundAtomCue, CueName)) {
			
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);

	CacheAggregateValues();
}
#endif // WITH EDITOR

void USoundAtomCue::BeginDestroy()
{
#if WITH_EDITOR
	/* リソースの解放 */
	if (RawPCMData != nullptr) {
		FMemory::Free(RawPCMData);
		RawPCMData = nullptr;
	}
#endif
	Super::BeginDestroy();
}

float USoundAtomCue::GetEconomicTickFrequencyToApply() const
{
	if (bOverrideEconomicTickSettings) {
		return EconomicTickFrequency;
	}
	else {
		return UCriWareInitializer::GetEconomicTickFrequency();
	}
}

float USoundAtomCue::GetEconomicTickDistanceMarginToApply() const
{
	if (bOverrideEconomicTickSettings) {
		return EconomicTickDistanceMargin;
	}
	else {
		return UCriWareInitializer::GetEconomicTickDistanceMargin();
	}
}

float USoundAtomCue::GetEconomicTickDistance() const
{
	return GetMaxAttenuationDistanceToApply() + GetEconomicTickDistanceMarginToApply();
}

#if WITH_EDITOR
float USoundAtomCue::GetMaxAttenuationDistance() const
{
	return FMath::Max(AttenuationDistanceParam.MaxDistance, 0.0f);
}

float USoundAtomCue::GetMinAttenuationDistance() const
{
	return FMath::Max(AttenuationDistanceParam.MinDistance, 0.0f);
}
#endif // WITH_EDITOR

void USoundAtomCue::Tick(float /*DeltaTime*/)
{
	if (bIsRequestingCueInfo) {
		if (CueSheet && CueSheet->IsLoaded()) {
			bIsRequestingCueInfo = false; // stop tick
			CacheAggregateValues();
		}
	}
}

void USoundAtomCue::CacheAggregateValues()
{
	if (!CueSheet) {
		return;
	}

	// キューシートハンドルへのアクセスを保証
	// Load CueSheet on memory if not loaded.
	if (!CueSheet->IsLoaded()) {
		//UE_LOG(LogCriWareRuntime, Info, TEXT("Async Load CueSheet."));

		// async load
		CueSheet->AsyncLoadCueSheet();
		bIsRequestingCueInfo = true; // start tick

		// anyway, try to link cached info if present
		CueInfo = CueSheet->GetCueInfoFromName(CueName);
		
		return;
	}
	// direct exec from already loaded cue sheet
		
	// transient
	CueInfo = CueSheet->GetCueInfoFromName(CueName);
	if (!CueInfo) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("AtomCue '%s' is not present in AtomCueSheet '%s'."), *CueName, *CueSheet->CueSheetName);
		return;
	}

	// transient
	CueId = CueInfo->Id;

	// updated
	bool bUpdated = false;

	if (!CueInfo->Name.IsEmpty()) {
#if WITH_EDITORONLY_DATA
		// カテゴリ名一覧の取得
		// CategoryName list
		if (CategoryName != CueInfo->CategoryNames) {
			CategoryName.Reset();
			CategoryName.Append(CueInfo->CategoryNames);
			bUpdated = true;
		}

		// AISACコントロール名一覧の取得
		// AISACControlName list
		if (AisacControlName != CueInfo->AisacControlNames) {
			AisacControlName.Reset();
			AisacControlName.Append(CueInfo->AisacControlNames);
			bUpdated = true;
		}

		// Attenuation distance
		if (AttenuationDistanceParam != CueInfo->AttenuationDistance) {
			AttenuationDistanceParam = CueInfo->AttenuationDistance;
			bUpdated = true;
		}
#endif // WITH_EDITORONLY_DATA

		float NewDuration = CueInfo->Duration.GetTotalSeconds();
		if (Duration != NewDuration) {
			Duration = NewDuration;
			bUpdated = true;
		}
	}

	// cache first wave info
	float NewFirstWaveDuration = GetFirstWaveDuration();
	if (FirstWaveDuration != NewFirstWaveDuration) {
		FirstWaveDuration = NewFirstWaveDuration;
		bUpdated = true;
	}

	if (Duration < SMALL_NUMBER) {
		Duration = FirstWaveDuration;
		bUpdated = true;
	}

	if (bUpdated) {
		MarkPackageDirty();
	}
}

// FIXME get all 1st wave info and gave then to user :D - or adda way to let user requested any wave info :D:D
float USoundAtomCue::GetFirstWaveDuration()
{
	/* 既に情報を取得済みの場合はそれを返す */
	if (FirstWaveDuration > KINDA_SMALL_NUMBER) {
		return FirstWaveDuration;
	}

	/* 情報を取得し、それを変数"FirstWaveDuration"にキャッシュしてから値を返す */
	/* キューシートハンドルへのアクセスを保証 */
	if (CueSheet && CueSheet->IsLoaded()) { // sync loading will occur
	
		CriAtomExWaveformInfo WaveformInfo;
		CriBool bWaveResult = criAtomExAcb_GetWaveformInfoByName(CueSheet->GetAcbHn(), TCHAR_TO_UTF8(*CueName), &WaveformInfo);

		/* 情報の取得に失敗又はゼロ除算が発生しそうな場合はゼロ値を返す */
		if ((bWaveResult == CRI_FALSE) || (WaveformInfo.sampling_rate == 0)) {
			FirstWaveDuration = 0.0f;
		} else {
			FirstWaveDuration = static_cast<float>(WaveformInfo.num_samples) / WaveformInfo.sampling_rate;
		}

		/* クック時やコンテンツブラウザからのロード時はACBハンドルを解放 */
		//if (GIsCookerLoadingPackage || (GIsEditorLoadingPackage && (!CueSheet->IsRooted()))) {
		//	CueSheet->ForceUnload();
		//}
	}

	return FirstWaveDuration;
}

bool USoundAtomCue::IsEconomicTickEnabled() const
{
	return bEconomicTick;
}

// DEPRECATED - will de deleted in next release

float USoundAtomCue::GetEconomicTickFrequencyToUse() const { return GetEconomicTickFrequencyToApply(); }
float USoundAtomCue::GetEconomicTickMarginDistanceToUse() const { return GetEconomicTickDistanceMarginToApply(); }
float USoundAtomCue::GetEconomicTickBoundaryDistanceToUse() const { return GetEconomicTickDistance(); }
float USoundAtomCue::GetMaxAttenuationDistanceToUse() const { return GetMaxAttenuationDistanceToApply(); }
float USoundAtomCue::GetCullingMarginDistanceToUse() const { return GetCullDistanceMarginToApply(); }
float USoundAtomCue::GetCullingBoundaryDistanceToUse() const { return GetCullDistance(); }
float USoundAtomCue::GetMaxAcbAttenuationDistance() const {
#if WITH_EDITOR
	return GetMaxAttenuationDistance();
#else
	return WORLD_MAX;
#endif
}
float USoundAtomCue::GetMinACbAttenuationDistance() const {
#if WITH_EDITOR
	return GetMinAttenuationDistance();
#else
	return 0.0f;
#endif
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
