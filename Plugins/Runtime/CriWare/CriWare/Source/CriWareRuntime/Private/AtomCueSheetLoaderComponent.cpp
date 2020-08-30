/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Atom Cue Sheet Loader Component
 * File     : AtomCueSheetLoaderComponent.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "AtomCueSheetLoaderComponent.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "CriFsBinderComponent.h"

/* Unreal Engine 4関連ヘッダ */
#include "Engine/AssetManager.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "GameFramework/WorldSettings.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "AtomCueSheetLoaderComponent"

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

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UAtomCueSheetLoaderComponent::UAtomCueSheetLoaderComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bTickInEditor = true;
	bNeverNeedsRenderUpdate = true;
	bAutoActivate = true;
	Status = EAtomCueSheetLoaderComponentStatus::Stop;
}

void UAtomCueSheetLoaderComponent::PostLoad()
{
	Super::PostLoad();

	/* デフォルトオブジェクトかどうかチェック */
	if (HasAnyFlags(RF_ClassDefaultObject)) {
		/* デフォルトオブジェクトの場合は何もしない */
		return;
	}

#if WITH_EDITOR
	/* ゲーム実行中かどうかチェック */
	/* 注意）マップロード時は処理をスキップ。 */
	UWorld* World = GetWorld();
	bool bIsGameWorld = (World ? World->IsGameWorld() : false);
	if ((GIsEditor != false) && (bIsGameWorld == false)) {
		return;
	}
#endif

	/* ロードの開始 */
	if (bAutoActivate) {
		Load();
	}
}

void UAtomCueSheetLoaderComponent::BeginDestroy()
{
	/* 処理の停止 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::Stop);

	Super::BeginDestroy();
}

void UAtomCueSheetLoaderComponent::Load()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	/* アセットのロード要求が可能かどうかチェック */
	if ((Status != EAtomCueSheetLoaderComponentStatus::Stop)
		&& (Status != EAtomCueSheetLoaderComponentStatus::Complete)
		&& (Status != EAtomCueSheetLoaderComponentStatus::Error)) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("UAtomCueSheetLoaderComponent::Load is not allowed while loading."));
		return;
	}

	/* 再利用前に一旦既存のリソースを破棄する */
	SetStatus(EAtomCueSheetLoaderComponentStatus::Stop);

	/* キューシートの参照が設定されているかどうかチェック */
	bool bResult = CueSheetReference.IsValid();
	if (bResult == false) {
		/* データ未指定時はロードを開始しない */
		return;
	}

	/* ローカライズされたアセットがないか確認 */
	/* 備考）Localizeフォルダにファイルがあった場合はLocalize設定にあったファイルがロードされる。	*/
	LocalizedSoftObjectPath = USoundAtomCueSheet::GetLocalizedAssetPath(CueSheetReference.ToString());

	/* 非同期ロードの開始 */
	Streamable.RequestAsyncLoad(LocalizedSoftObjectPath);

	/* 非同期ロード処理の開始を通知 */
	UE_LOG(LogCriWareRuntime, Verbose, TEXT("Starting asynchronous load of '%s'."), *LocalizedSoftObjectPath.ToString());

	/* 非同期ロードの開始を通知 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::Loading);
}

USoundAtomCueSheet* UAtomCueSheetLoaderComponent::GetAtomCueSheet()
{
	/* 既に停止している場合は保持しているハンドルを返す */
	if ((Status == EAtomCueSheetLoaderComponentStatus::Stop)
		|| (Status == EAtomCueSheetLoaderComponentStatus::Complete)
		|| (Status == EAtomCueSheetLoaderComponentStatus::Error)) {
		return CueSheet;
	}

	/* バインドの中止 */
	if (BinderComponent != nullptr) {
		BinderComponent->Stop();
		if(BinderComponent->IsRegistered()) {
			BinderComponent->UnregisterComponent();
		}
		BinderComponent->DestroyComponent();
		BinderComponent = nullptr;
	}

	/* アセットの非同期ロードが完了しているかどうかチェック */
	if (CueSheet == nullptr) {
		/* 強制ロード */
		UE_LOG(LogCriWareRuntime, Log, TEXT("'%s' is syncrhonously loaded."), *LocalizedSoftObjectPath.ToString());
		CueSheet = Cast<USoundAtomCueSheet>(LocalizedSoftObjectPath.TryLoad());
	}

	/* エラーチェック */
	if (CueSheet == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("TryLoad failed. (%s)"), *LocalizedSoftObjectPath.ToString());
		SetStatus(EAtomCueSheetLoaderComponentStatus::Error);
		return nullptr;
	}

	/* ステータスの更新 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::Complete);

	return CueSheet;
}

void UAtomCueSheetLoaderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/* 停止中は何もしない */
	if ((Status == EAtomCueSheetLoaderComponentStatus::Stop)
		|| (Status == EAtomCueSheetLoaderComponentStatus::Complete)
		|| (Status == EAtomCueSheetLoaderComponentStatus::Error)) {
		return;
	}

	bool bResult;
	do {
		bResult = Process();
	} while (bResult);
}

bool UAtomCueSheetLoaderComponent::Process()
{
	bool bResult = false;

	switch (Status) {
		case EAtomCueSheetLoaderComponentStatus::Loading:
		bResult = OnLoading();
		break;

		case EAtomCueSheetLoaderComponentStatus::LoadEnd:
		bResult = OnLoadEnd();
		break;

		case EAtomCueSheetLoaderComponentStatus::Serializing:
		bResult = OnSerializing();
		break;

		case EAtomCueSheetLoaderComponentStatus::SerializeEnd:
		bResult = OnSerializeEnd();
		break;

		case EAtomCueSheetLoaderComponentStatus::Binding:
		bResult = OnBinding();
		break;

		case EAtomCueSheetLoaderComponentStatus::BindEnd:
		bResult = OnBindEnd();
		break;

		case EAtomCueSheetLoaderComponentStatus::LoadCueSheet:
		bResult = OnLoadCueSheet();
		break;

		default:
		/* 停止中は何もしない */
		break;
	}

	return bResult;
}

bool UAtomCueSheetLoaderComponent::OnLoading()
{
	/* ロード完了チェック */
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	bool Result = Streamable.IsAsyncLoadComplete(LocalizedSoftObjectPath);
	if (Result == false) {
		return false;
	}

	/* アセットの取得 */
	CueSheet = Cast<USoundAtomCueSheet>(LocalizedSoftObjectPath.ResolveObject());
	if (CueSheet == nullptr) {
		UE_LOG(LogCriWareRuntime, Error, TEXT("ResolveObject failed. (%s)"), *LocalizedSoftObjectPath.ToString());
		SetStatus(EAtomCueSheetLoaderComponentStatus::Error);
		return false;
	}

	/* 既にロード済みのアセットかどうかチェック */
	bool bIsLoaded = CueSheet->IsLoaded();
	if (bIsLoaded != false) {
		/* 既にロード済みの場合は即座に完了 */
		SetStatus(EAtomCueSheetLoaderComponentStatus::Complete);
		return true;
	}

	/* ステータスの更新 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::LoadEnd);

	return true;
}

bool UAtomCueSheetLoaderComponent::OnLoadEnd()
{
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
	bool bResult = false;
#if WITH_EDITOR
	/* BulkDataの非同期読み込みを開始 */
	bResult = CueSheet->RawData.LoadBulkDataWithFileReader();
#endif

	/* 非同期読み込みが実行されたかどうかチェック */
	if (bResult != false) {
		SetStatus(EAtomCueSheetLoaderComponentStatus::Serializing);
	} else {
		SetStatus(EAtomCueSheetLoaderComponentStatus::SerializeEnd);
	}
#else
#if WITH_EDITOR
	CueSheet->RawData.ForceBulkDataResident();
#endif
	SetStatus(EAtomCueSheetLoaderComponentStatus::SerializeEnd);
#endif

	return true;
}

bool UAtomCueSheetLoaderComponent::OnSerializing()
{
	/* BulkDataの非同期読み込みを開始 */
	bool bResult = CueSheet->RawData.IsAsyncLoadingComplete();
	if (bResult == false) {
		return false;
	}

	/* ステータスの更新 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::SerializeEnd);

	return true;
}

bool UAtomCueSheetLoaderComponent::OnSerializeEnd()
{
	/* ストリームファイルが必要かどうかチェック */
	if (CueSheet->NumSlots <= 0) {
		/* オンメモリデータの場合はこの時点で準備完了 */
		SetStatus(EAtomCueSheetLoaderComponentStatus::BindEnd);
		return true;
	}

	/* ストリームファイルのパスを取得 */
	FString BaseName;
	if (CueSheet->bOverrideAwbDirectory == false) {
		BaseName = UCriWareInitializer::GetContentDir();
	} else {
		BaseName = FPaths::ProjectContentDir() + CueSheet->AwbDirectory.Path;
	}

	/* パスの終端に「/」を付加 */
	if (BaseName.EndsWith(TEXT("/")) == false) {
		BaseName += "/";
	}

	/* キューシート名を付加 */
	BaseName += CueSheet->CueSheetName;

	/* AWBファイル名の作成 */
	FString AwbName = BaseName + TEXT(".awb");

	/* バインダの作成 */
	BinderComponent = NewObject<UCriFsBinderComponent>(this);
	BinderComponent->bAutoActivate = false;
	BinderComponent->RegisterComponent();

	/* バインドの開始 */
	BinderComponent->FilePath = AwbName;
	BinderComponent->Bind();

	/* ステータスの更新 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::Binding);

	return true;
}

bool UAtomCueSheetLoaderComponent::OnBinding()
{
	check(BinderComponent != nullptr);

	/* エラーチェック */
	ECriFsBinderStatus BinderStatus = BinderComponent->GetStatus();
	if (BinderStatus == ECriFsBinderStatus::Error) {
		SetStatus(EAtomCueSheetLoaderComponentStatus::Error);
		return false;
	}

	/* バインド完了チェック */
	if (BinderStatus != ECriFsBinderStatus::Complete) {
		return false;
	}

	/* バインダのTickを停止 */
	BinderComponent->UnregisterComponent();

	/* ステータスの更新 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::BindEnd);

	return true;
}

bool UAtomCueSheetLoaderComponent::OnBindEnd()
{
	/* ACBハンドルが即座に使用できる状態にしておく */
	if (CueSheet->IsLoaded() == false) {
		SetStatus(EAtomCueSheetLoaderComponentStatus::LoadCueSheet);

		// request load cue sheet
		CueSheet->AsyncLoadCueSheet();

		return true;
	}

	/* ステータスの更新 */
	SetStatus(EAtomCueSheetLoaderComponentStatus::Complete);

	return true;
}

bool UAtomCueSheetLoaderComponent::OnLoadCueSheet()
{
	if (CueSheet->IsLoaded() == false) {
		return false;
	}

	SetStatus(EAtomCueSheetLoaderComponentStatus::Complete);

	return true;
}

void UAtomCueSheetLoaderComponent::SetStatus(EAtomCueSheetLoaderComponentStatus NewStatus)
{
	if (Status == NewStatus) {
		return;
	}

	/* 停止時やエラー発生時は不要になったリソースを解放 */
	if ((NewStatus == EAtomCueSheetLoaderComponentStatus::Stop)
		|| (NewStatus == EAtomCueSheetLoaderComponentStatus::Error)) {
		/* キューシートの解放 */
		CueSheet = nullptr;

		/* バインドの中止 */
		/* 備考）Complete時にBinderComponentはnullptrに設定されるので、 */
		/* 　　　ここに到達するのはロードを強制停止した場合のみ。       */
		if (BinderComponent != nullptr) {
			BinderComponent->Stop();
			if(BinderComponent->IsRegistered()) {
				BinderComponent->UnregisterComponent();
			}
			BinderComponent->DestroyComponent();
			BinderComponent = nullptr;
		}
	}

	Status = NewStatus;

	if (Status == EAtomCueSheetLoaderComponentStatus::Complete) {
		/* バインダハンドルを移動 */
		if (BinderComponent != nullptr) {
			CueSheet->Binder = BinderComponent->MoveHandle();
		}

		// バインダコンポーネントの破棄
		BinderComponent = nullptr;

		// 完了を通知
		if (OnLoadCompleted.IsBound()) {
			OnLoadCompleted.Broadcast();
		}
		if (OnLoadCompletedNative.IsBound()) {
			OnLoadCompletedNative.Broadcast(this);
		}
	} else if (Status == EAtomCueSheetLoaderComponentStatus::Error) {

		/* エラーの発生を通知 */
		if (OnLoadError.IsBound()) {
			OnLoadError.Broadcast();
		}
		if (OnLoadErrorNative.IsBound()) {
			OnLoadErrorNative.Broadcast(this);
		}
	}
}

void UAtomCueSheetLoaderComponent::LoadAtomCueSheet(UObject* WorldContextObject, TSoftObjectPtr<USoundAtomCueSheet> Asset, FOnAtomCueSheetLoaded OnLoaded, FLatentActionInfo LatentInfo)
{
	struct FLoadAtomCueSheetAction
	{
		FOnAtomCueSheetLoaded OnLoaded;
		void HandleAtomCueSheetLoaded(UAtomCueSheetLoaderComponent* Loader) {
			OnLoaded.ExecuteIfBound(Loader->GetAtomCueSheet());
			delete this;
		}
		void HandleAtomCueSheetError(UAtomCueSheetLoaderComponent* Loader) {
			OnLoaded.ExecuteIfBound(nullptr);
			delete this;
		}
	};

	if (!GEngine) {
		OnLoaded.ExecuteIfBound(nullptr);
		return;
	}

	/* データロードの可否をチェック */
	UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!ThisWorld || !ThisWorld->bAllowAudioPlayback || ThisWorld->GetNetMode() == NM_DedicatedServer) {
		OnLoaded.ExecuteIfBound(nullptr);
		return;
	}

	/* ローダの作成 */
	const bool bIsInGameWorld = ThisWorld->IsGameWorld();
	AWorldSettings *WorldSettings = ThisWorld->GetWorldSettings();
	UAtomCueSheetLoaderComponent* AtomCueSheetLoader = NewObject<UAtomCueSheetLoaderComponent>(WorldSettings);
	if (AtomCueSheetLoader == nullptr) {
		OnLoaded.ExecuteIfBound(nullptr);
		return;
	}

	/* 終了後に呼び出すコールバックを登録 */
	FLoadAtomCueSheetAction* NewAction = new FLoadAtomCueSheetAction();
	NewAction->OnLoaded = OnLoaded;
	AtomCueSheetLoader->OnLoadCompletedNative.AddRaw(NewAction, &FLoadAtomCueSheetAction::HandleAtomCueSheetLoaded);
	AtomCueSheetLoader->OnLoadErrorNative.AddRaw(NewAction, &FLoadAtomCueSheetAction::HandleAtomCueSheetError);

	/* Atom Cue Sheetアセットのロードを開始 */
	AtomCueSheetLoader->CueSheetReference = Asset.ToSoftObjectPath();
	AtomCueSheetLoader->Load();
	AtomCueSheetLoader->RegisterComponent();
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
