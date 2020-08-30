/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Factory of Atom Data
 * File     : SoundAtomCueSheetFactory.cpp
 *
 ****************************************************************************/

#include "SoundAtomCueSheetFactory.h"

#include "Editor.h"
#include "PropertyHandle.h"
#include "PackageTools.h"
#include "ObjectTools.h"
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#include "Misc/MessageDialog.h"
#include "Misc/FileHelper.h"

#include "CriWareEditorPrivatePCH.h"
#include "CriWareInitializer.h"
#include "CriWarePluginSettings.h"
#include "SoundAtomCueSheet.h"
#include "SoundAtomCue.h"
#include "AtomComponent.h"
#include "AssetTypeActions_SoundAtomCueSheet.h"
#include "AssetTypeActions_SoundAtomCue.h"
#include "AtomComponentBroker.h"
#include "CriWareEditorSettings.h"
#include "SAtomCueSheetFactoryDialog.h"

#define LOCTEXT_NAMESPACE "SoundAtomCueSheetFactory"

void FAtomCueListItem::GetCueInfo(FTimespan& OutDuration, bool& OutIsLooping, bool& OutIs3D,
	CriAtomExFormat& OutFormat, int32& OutNumChannels) const
{
	CriAtomExAcbHn Handle = nullptr;
	if (AcbHn) {
		Handle = AcbHn;
	}
	else if (CueAsset && CueAsset->CueSheet) {
		Handle = CueAsset->CueSheet->GetAcbHn();
	}

	if (Handle) {
		CriAtomExCueInfo AcbCueInfo;
		CriBool bCueResult = FCriWareApi::criAtomExAcb_GetCueInfoByName(AcbHn, TCHAR_TO_UTF8(*CueName.ToString()), &AcbCueInfo);
		if (bCueResult != CRI_FALSE) {
			if (AcbCueInfo.length < 0) {
				OutDuration = FTimespan::Zero();
				OutIsLooping = true;
			}
			else {
				OutDuration = FTimespan::FromMilliseconds(AcbCueInfo.length);
				OutIsLooping = false;
			}
			// TODO: 
			// OutIs3D = ?
		}

		// Get more precise infos
		CriAtomExWaveformInfo WaveformInfo;
		CriBool bWaveResult = FCriWareApi::criAtomExAcb_GetWaveformInfoByName(AcbHn, TCHAR_TO_UTF8(*CueName.ToString()), &WaveformInfo);
		if (bWaveResult != CRI_FALSE) {
			if (WaveformInfo.sampling_rate == 0) {
				OutDuration = FTimespan::Zero();
			}
			else {
				OutDuration = FTimespan::FromSeconds(static_cast<float>(WaveformInfo.num_samples) / WaveformInfo.sampling_rate);
			}
			OutFormat = WaveformInfo.format;
			OutNumChannels = WaveformInfo.num_channels;
		}
	}
}


FText FAtomCueListItem::ToDisplayString() const
{
	FTimespan Duration;
	bool bIsLooping = false, bIs3D = false;
	CriAtomExFormat Format = CRIATOMEX_FORMAT_NONE;
	int32 NumChannels;
	GetCueInfo(Duration, bIsLooping, bIs3D, Format, NumChannels);

	FTextBuilder TextBuilder;
	TextBuilder.AppendLineFormat(LOCTEXT("CueName", "Cue Name: {0}"), CueName);
	TextBuilder.AppendLineFormat(LOCTEXT("CueID", "Cue ID: {0}"), CueID);
	TextBuilder.AppendLineFormat(LOCTEXT("CueDuration", "Duration: {0}"), FText::FromString(Duration.ToString(TEXT("%h:%m:%s.%f"))));
	TextBuilder.AppendLineFormat(LOCTEXT("CueLoop", "Loop: {0}"), bIsLooping ? LOCTEXT("Loop", "Loop") : LOCTEXT("OneShot", "OneShot"));
	TextBuilder.AppendLineFormat(LOCTEXT("CueDetails", "Number of Channels: {0}"), NumChannels);
	if (CueAsset) {
		FString AssetPath = CueAsset->GetPathName();
		TextBuilder.AppendLineFormat(LOCTEXT("CueAsset", "SoundAtomCue Asset: {0}"), FText::FromString(AssetPath));
	}
	return TextBuilder.ToText();
}

void FAtomCueListItem::GetStatusTextAndColor(FText& OutText, FColor& OutColor) const
{
	if (ImportStatus == EAtomCueImportStatus::NewCue) {
		OutText = LOCTEXT("NewCueLabel", "New");
		OutColor = FColor(122, 167, 240);
	}
	else if (ImportStatus == EAtomCueImportStatus::ExistCue) {
		OutText = LOCTEXT("ExistCueLabel", "Exist");
		OutColor = FColor(140, 230, 149);
	}
	else {
		OutText = LOCTEXT("DeletedCueLabel", "Deleted");
		OutColor = FColor(237, 183, 74);
	}
}

void FAtomCueListItem::GetActionTextAndColor(FText& OutText, FColor& OutColor) const
{
	if (ImportStatus == EAtomCueImportStatus::NewCue && Selected) {
		OutText = LOCTEXT("CreateCueLabel", "Create");
		OutColor = FColor(122, 167, 240);
	}
	else if (ImportStatus == EAtomCueImportStatus::ExistCue && Selected) {
		OutText = LOCTEXT("UpdateCueLabel", "Link");
		OutColor = FColor(140, 230, 149);
	}
	else if ((ImportStatus == EAtomCueImportStatus::ExistCue && !Selected) ||
		(ImportStatus == EAtomCueImportStatus::DeletedCue && Selected)) {
		OutText = LOCTEXT("DeleteCueLabel", "Delete");
		OutColor = FColor(237, 78, 78);
	}
	else {
		OutText = FText::GetEmpty();
		OutColor = FColor::Transparent;
	}
}


USoundAtomCueSheetFactory::USoundAtomCueSheetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/* Content BrowserでACBファイルをImportできるようにする */
	Formats.Add(TEXT("acb;Atom CueSheet File"));

	/* バイナリとして読み込む */
	bText = false;
	bCreateNew = false;
	bEditorImport = true;
	bEditAfterNew = true;

	/* パラメータのリセット */
	CueSheet = nullptr;

	SupportedClass = USoundAtomCueSheet::StaticClass();
}

bool USoundAtomCueSheetFactory::ConfigureProperties()
{
	return true;
}

CriAtomExAcbHn USoundAtomCueSheetFactory::SyncLoadACBDataFromBuffer(UObject* InParent, const uint8*& Buffer, const uint8* BufferEnd)
{
	int DataSize = BufferEnd - Buffer;
	void* Data = const_cast<void*>(static_cast<const void*>(Buffer));
	CriAtomExAcbHn AcbHn = FCriWareApi::criAtomExAcb_LoadAcbData(Data, DataSize, nullptr, nullptr, nullptr, 0);

	/* エラーチェック */
	if (AcbHn == nullptr) {
		/* エラー発生時 */
		FString PackageName = InParent->GetOutermost()->GetName();
		FMessageDialog::Open(EAppMsgType::Ok,
			FText::Format(NSLOCTEXT("UnrealEd", "Error_ImportFailed_f", "Import failed for {0}"), FText::FromString(PackageName)));
		return nullptr;
	}

	return AcbHn;
}

TArray<USoundAtomCue*> USoundAtomCueSheetFactory::GetExistingCueAssets(UObject* InParent, FString PackagePath)
{
	TArray<USoundAtomCue*> Results;

	// use cached info to obtain cue names fastly
	TArray<FString> CueNames;
	if (CueSheet) {
		for (int i = 0; i < CueSheet->GetNumCues(); i++) {
			const FAtomCueInfo* CueInfo = CueSheet->GetCueInfoFromIndex(i);
			if (CueInfo) {
				CueNames.Add(CueInfo->Name);
			}
		}
	}

	for (FString AssetName : CueNames) {

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 21
		/* UPackageTools class is deprecated from UE4.21. */
		FString PackageName = UPackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);
		UPackage* Pkg = CreatePackage(NULL, *PackageName);

		/* 備考）検索前にPackageTools::HandleFullyLoadingPackages関数で強制的にロードする。 */
		{
			TArray<UPackage*> TopLevelPackages;
			TopLevelPackages.Add(Pkg);
			UPackageTools::HandleFullyLoadingPackages(TopLevelPackages, LOCTEXT("CreateANewObject", "Create a new object"));
		}
#else
		FString PackageName = PackageTools::SanitizePackageName(PackagePath + TEXT("/") + AssetName);
		UPackage* Pkg = CreatePackage(NULL, *PackageName);

		/* 備考）検索前にPackageTools::HandleFullyLoadingPackages関数で強制的にロードする。 */
		{
			TArray<UPackage*> TopLevelPackages;
			TopLevelPackages.Add(Pkg);
			PackageTools::HandleFullyLoadingPackages(TopLevelPackages, LOCTEXT("CreateANewObject", "Create a new object"));
		}
#endif

		/* 既存アセットの取得 */
		USoundAtomCue* ExistingAsset = (USoundAtomCue*)StaticFindObject(USoundAtomCue::StaticClass(), Pkg, *AssetName);

		if (ExistingAsset) {
			Results.Add(ExistingAsset);
		}
	}

	return Results;
}

bool USoundAtomCueSheetFactory::GetImportOptions(CriAtomExAcbHn AcbHn, TArray<USoundAtomCue*> ExistingCues)
{
	// get cue info from ACB
	/* キューの数を取得 */
	CriSint32 NumCues = FCriWareApi::criAtomExAcb_GetNumCues(AcbHn);

	/* キューアセットを作成するかどうかチェック */
	GetMutableDefault<UCriWarePluginSettings>()->LoadConfig();
	bool DefaultSelectCue = GetDefault<UCriWarePluginSettings>()->AutomaticallyCreateCueAsset;

	Options.Items.Empty();

	TMap<FString, USoundAtomCue*> OldCues;
	for (auto Cue : ExistingCues) {
		const FAtomCueInfo* CueInfo = Cue->GetCueInfo();
		if (CueInfo) {
			OldCues.Add(CueInfo->Name, Cue);
		}
	}

	/* ACBファイルの情報を列挙 */
	for (int i = 0; i < NumCues; i++) {
		/* キュー情報の取得 */
		CriAtomExCueInfo AtomExCueInfo;
		CriBool bResult = FCriWareApi::criAtomExAcb_GetCueInfoByIndex(AcbHn, i, &AtomExCueInfo);
		if (bResult == CRI_FALSE) {
			UE_LOG(LogCriWareEditor, Error, TEXT("Failed to get cue information of '%s'."), UTF8_TO_TCHAR(AtomExCueInfo.name));
			continue;
		}

		/* プライベートキューを除外 */
		if (AtomExCueInfo.header_visibility == 0) {
			continue;
		}

		FString CueName = UTF8_TO_TCHAR(AtomExCueInfo.name);

		/* 「/」を含むキューを除外 */
		/* 備考）インゲームプレビュー用バイナリを使用すると、       */
		/* 　　　サブシンセが「/」を含むキュー名が出力されるため。  */
		int32 Index;
		if (CueName.FindChar('/', Index)) {
			continue;
		}

		auto ImportStatus = EAtomCueImportStatus::NewCue;
		USoundAtomCue* CueAsset = nullptr;

		// check and remove same cue
		auto Cue = OldCues.Find(CueName);
		if (Cue) {
			ImportStatus = EAtomCueImportStatus::ExistCue;
			CueAsset = *Cue;
			OldCues.Remove(CueName);
		}

		Options.Items.Add(MakeShared<FAtomCueListItem>(DefaultSelectCue, FText::FromString(CueName),
			AtomExCueInfo.id, ImportStatus, CueAsset, AcbHn));
	}

	// Get remaining cue that was deleted
	for (auto CueInfoPair : OldCues) {
		Options.Items.Add(MakeShared<FAtomCueListItem>(DefaultSelectCue, FText::FromString(CueInfoPair.Key),
			CueInfoPair.Value->GetCueInfo()->Id, EAtomCueImportStatus::DeletedCue, CueInfoPair.Value));
	}

	// other options
	// FIXME -> REMOVE EDITOR SETTINGS
	const UCriWareEditorSettings* EditorSettings = GetDefault <UCriWareEditorSettings>();
	if (EditorSettings) {
		Options.EnableEconomicTick = EditorSettings->EnableEconomicTickWhenImportingCueSheet;
		Options.EnableDistanceCulling = EditorSettings->EnableCullingWhenImportingCueSheet;
	}
	else {
		Options.EnableEconomicTick = false;
		Options.EnableDistanceCulling = false;
	}

	// reslut
	Options.OkClicked = false;
	if (!IsAutomatedImport()) {
		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(LOCTEXT("CreateAtomCueSheetFactoryDialogTitle", "Import Atom Cue Sheet"))
			.ClientSize(FVector2D(600, 600))
			.SupportsMinimize(false)
			.SupportsMaximize(false);

		Window->SetContent(SNew(SAtomCueSheetFactoryDialog, Options, Window));
		GEditor->EditorAddModalWindow(Window);
	} else {
		Options.OkClicked = true;
	}
	return Options.OkClicked;
}


UClass* USoundAtomCueSheetFactory::ResolveSupportedClass()
{
	return USoundAtomCueSheet::StaticClass();
}

void USoundAtomCueSheetFactory::PostInitProperties()
{
	Super::PostInitProperties();
}

void USoundAtomCueSheetFactory::BeginDestroy()
{
	Super::BeginDestroy();
}

UObject* USoundAtomCueSheetFactory::FactoryCreateBinary
(
	UClass*				Class,
	UObject*			InParent,
	FName				Name,
	EObjectFlags		Flags,
	UObject*			Context,
	const TCHAR*		FileType,
	const uint8*&		Buffer,
	const uint8*		BufferEnd,
	FFeedbackContext*	Warn,
	bool&				bOutOperationCanceled
)
{
	// Get ACB handle
	CriAtomExAcbHn AcbHn = SyncLoadACBDataFromBuffer(InParent, Buffer, BufferEnd);
	if (AcbHn == nullptr) {
		return nullptr;
	}

	FString PackagePath = FPackageName::GetLongPackagePath(InParent->GetOutermost()->GetName());

	// seek for re-importaion of same acb file and get corresponding CueSheet 
	if (!CueSheet) {
		/* その他のケースでは念のため同名のアセットを検索 */
		CueSheet = USoundAtomCueSheet::Find(*Name.ToString(), PackagePath, NULL);
	}

	// ensure we have access to cue info
	if (CueSheet) {
		CueSheet->MakeSureCueSheetIsLoaded();
	}

	// collect exsisting SoundAtomCue assets using this CueSheet
	auto ExistingAssets = GetExistingCueAssets(InParent, PackagePath);

	// show import options widget
	bool result = GetImportOptions(AcbHn, ExistingAssets);

	// importation cancelled
	if (result == false) {
		bOutOperationCanceled = true;

		FCriWareApi::criAtomExAcb_Release(AcbHn);

		return nullptr;
	}

	/* Atom Cue Sheetアセットが見つからない場合は新規に作成 */
	if (CueSheet == nullptr) {
		CueSheet = NewObject<USoundAtomCueSheet>(InParent, Name, Flags);
	}

	// Import ACB data to Asset

	/* ACB使用中は強制的にアンロード */
	CueSheet->ForceUnload();

	/* ACBデータをuasset内に取り込む */
	CueSheet->RawData.Lock(LOCK_READ_WRITE);
	void* LockedData = CueSheet->RawData.Realloc(BufferEnd - Buffer);
	FMemory::Memcpy(LockedData, Buffer, BufferEnd - Buffer);
	CueSheet->RawData.Unlock();

	/* ACBファイル情報の保存領域を確保 */
	if (CueSheet->AssetImportData == nullptr) {
		CueSheet->AssetImportData = NewObject<UAssetImportData>(CueSheet, TEXT("AssetImportData"));
	}

	/* ACBファイル再インポート用に情報を保存 */
	if (CueSheet->AssetImportData != nullptr) {
		CueSheet->AssetImportData->Update(CurrentFilename);
	}

	/* キューシート名の保存 */
	if (CueSheet->AssetImportData != nullptr && CueSheet->AssetImportData->SourceData.SourceFiles.Num() > 0) {
		FString AcbFilePath = CueSheet->AssetImportData->SourceData.SourceFiles[0].RelativeFilename;
		CueSheet->CueSheetName = FPaths::GetBaseFilename(AcbFilePath);
	};

	/* CueSheetName はこの後のキューのロード時に必要な情報なので注意 */
	if (CueSheet->CueSheetName.Len() <= 0) {
		CueSheet->CueSheetName = Name.ToString();
	}

	// update cue sheet
	CueSheet->MakeSureCueSheetIsLoaded();

	// generate selection
	/* === ACB内のキューの列挙 === */
	TArray<UObject*> AssetsToDelete;

	for (auto Item : Options.Items) {
		// import
		/* アセット名、パッケージ名の生成 */
		FString AssetName = Item->CueName.ToString();

		/* 既存アセットの取得 */
		USoundAtomCue* CueAsset = Item->CueAsset;

		// create asset
		if (Item->ImportStatus == NewCue && Item->Selected) {
			if (CueAsset == nullptr) {
				/* 同名のアセットが存在しないかチェック */
				UObject* SameNameAsset = StaticFindObject(nullptr, InParent, *AssetName);
				if (SameNameAsset != nullptr) {
					/* 同名のアセットを作成することはできない */
					FMessageDialog::Open(EAppMsgType::Ok,
						FText::Format(NSLOCTEXT("UnrealEd", "Error_AssetExist", "Create AtomCue uasset is failed ('{0}'). Asset with same name exists. Can't overwrite another asset."), FText::FromString(AssetName)));
				}
				else {
					/* アセットが存在しない場合は新規に作成 */
					/* 注意）ここではFactoryは指定してはいけない！！ */
					/* 　　　→USoundAtomCue破棄時にFactoryが一緒に削除されてしまうため。 */
					FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
					CueAsset = (USoundAtomCue*)AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, USoundAtomCue::StaticClass(), NULL);
				}
			}
		}

		// delete asset
		if ((Item->ImportStatus == DeletedCue && Item->Selected) ||
			(Item->ImportStatus == ExistCue && !Item->Selected)) {
			if (CueAsset) {
				AssetsToDelete.Add(CueAsset);
			}
		}

		/* アセットの作成に成功したかどうかチェック */
		/* 注意）アセットの上書きを行わなかった（行えなかった）場合、NULLが返される。 */
		if (CueAsset == nullptr) {
			continue;
		}

		// serialize asset with cuesheet information
		bool bUpdated = false;

		/* キュー情報の保存 */
		if (CueAsset->CueSheet != CueSheet) {
			CueAsset->CueSheet = CueSheet;
			bUpdated = true;
		}

		if (CueAsset->CueName != AssetName) {
			CueAsset->CueName = AssetName;
			bUpdated = true;
		}

		if (Options.EnableEconomicTick != CueAsset->IsEconomicTickEnabled()) {
			CueAsset->EnableEconomicTick(Options.EnableEconomicTick);
			bUpdated = true;
		}

		if (Options.EnableDistanceCulling != CueAsset->IsDistanceCullingEnabled()) {
			CueAsset->EnableDistanceCulling(Options.EnableEconomicTick);
			bUpdated = true;
		}

		/* キューアセットのプリセットを更新するため */
		CueAsset->PostInitProperties();

		if (bUpdated) {
			CueAsset->MarkPackageDirty();
		}
	}

	// delete asset list
	int32 NumAssetToDelete = AssetsToDelete.Num();
	if (NumAssetToDelete > 0) {
		// Delete the existing object
		bool bDeleteSucceeded = (ObjectTools::ForceDeleteObjects(AssetsToDelete, true) == NumAssetToDelete);

		if (bDeleteSucceeded) {
			// Force GC
			//CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
		}
		else {
			// failed to delete
			FMessageLog("CriWare").Error()
				->AddToken(FUObjectToken::Create(this))
				->AddToken(FTextToken::Create(
					LOCTEXT("CriWare_SoundAtomCueSHeetFactory_CannotDeleteObjects", "Cue wasn't deleted.\n\nSome assets are referenced by other contents.")));
		}
	}

	// other ACB info for cuesheet 

	/* AWBスロット数の取得 */
	CueSheet->NumSlots = FCriWareApi::criAtomExAcb_GetNumAwbFileSlots(AcbHn);

	/* ACBのリリース */
	FCriWareApi::criAtomExAcb_Release(AcbHn);

	/* Json読み込み部分 */
	if (!LoadAdditionalCueSheetDataFile()) {
		CueSheet->AdditionalData = nullptr;
	}

	USoundAtomCueSheet* OutCueSheet = CueSheet;

	// clean up
	CueSheet = nullptr;
	Options = FAtomCueSheetFactoryOptions();

	return OutCueSheet;
}

bool USoundAtomCueSheetFactory::LoadAdditionalCueSheetDataFile()
{
	// TODO: Log errors to message

	if (!CueSheet || !CueSheet->AssetImportData) {
		return false;
	}

	FString Filename = CueSheet->AssetImportData->GetFirstFilename();
	if (Filename.IsEmpty()) {
		return false;
	}

	// get json file
	FString JsonFilename = FPaths::Combine(FPaths::GetPath(Filename), FPaths::GetBaseFilename(Filename) + TEXT("_acb.json"));
	
	if (FPaths::FileExists(JsonFilename))
	{
		FString JsonText;
		bool bLoadSuccess = FFileHelper::LoadFileToString(JsonText, *JsonFilename);
		if (bLoadSuccess && !JsonText.IsEmpty())
		{
			// create data table to hold json data
			const FString Suffix = TEXT("_AddtionalDataTable");
			FString PackageName;
			FString AssetName;

			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
			AssetToolsModule.Get().CreateUniqueAssetName(Options.CueSheetPackageName, Suffix, PackageName, AssetName);

			UDataTable* NewDataTable = NewObject<UDataTable>(CueSheet, *AssetName);
			if (!NewDataTable)
			{
				UE_LOG(LogCriWareEditor, Error, TEXT("Failed to create DataTable for file '%s'."), *JsonFilename);

				return false;
			}

			if (NewDataTable->AssetImportData != nullptr) 
			{
				NewDataTable->AssetImportData->Update(JsonFilename);
			}

			// load json to data table
			NewDataTable->RowStruct = FAtomCueSheetDataTable::StaticStruct();
			auto ReadErrors = NewDataTable->CreateTableFromJSONString(JsonText);

			if (ReadErrors.Num() != 0)
			{
				const int32 MaxLen = 20;
				UE_LOG(LogCriWareEditor, Error, TEXT("CreateTableFromJSONString failed for text \"%s\""), (JsonText.Len() > MaxLen ? *(JsonText.Left(MaxLen) + TEXT("...")) : *JsonText));
				for (const FString& ReadError : ReadErrors)
				{
					UE_LOG(LogCriWareEditor, Error, TEXT("%s"), *ReadError);
				}
				return false;
			}

			// store to cue sheet
			CueSheet->AdditionalData = NewDataTable;

			return true;
		}

		UE_LOG(LogCriWareEditor, Error, TEXT("Failed to load json file '%s'."), *JsonFilename);
	}

	return false;
}


#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
