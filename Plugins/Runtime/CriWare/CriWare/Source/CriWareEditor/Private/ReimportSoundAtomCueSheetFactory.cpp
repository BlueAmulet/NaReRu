/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Reimport Factory of Atom Data
 * File     : ReimportAtomFactory.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ReimportSoundAtomCueSheetFactory.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "CriWarePreviewer.h"
#include "CriWareInitializer.h"
#include "SoundAtomCueSheet.h"

/* Unreal Editor関連ヘッダ */
#include "EditorFramework/AssetImportData.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "ReimportSoundAtomCueSheetFactory"

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
UReimportSoundAtomCueSheetFactory::UReimportSoundAtomCueSheetFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UReimportSoundAtomCueSheetFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	USoundAtomCueSheet* AcbObject = Cast<USoundAtomCueSheet>(Obj);

	if (AcbObject && AcbObject->AssetImportData) {
		AcbObject->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}

	return false;
}

void UReimportSoundAtomCueSheetFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	USoundAtomCueSheet* AcbObject = Cast<USoundAtomCueSheet>(Obj);
	if (AcbObject && ensure(NewReimportPaths.Num() == 1)) {
		AcbObject->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UReimportSoundAtomCueSheetFactory::Reimport(UObject* Obj)
{
	/* プレビューを停止 */
	UCriWarePreviewer::StopPreviewing();

	/* インポート情報の取得 */
	// Only handle valid atom cue sheet waves
	if (!Obj || !Obj->IsA(USoundAtomCueSheet::StaticClass())) {
		return EReimportResult::Failed;
	}

	CueSheet = Cast<USoundAtomCueSheet>(Obj);
	check(CueSheet);

	/* ファイルパスの作成 */
	if (!CueSheet->AssetImportData) {
		return EReimportResult::Failed;
	}

	FString ResolvedSourceFilePath = CueSheet->AssetImportData->GetFirstFilename();
	if (ResolvedSourceFilePath.IsEmpty()) {
		return EReimportResult::Failed;
	}

	UE_LOG(LogCriWareEditor, Log, TEXT("Performing atomic reimport of [%s]"), *ResolvedSourceFilePath);

	/* ファイルの有無をチェック */
	if (IFileManager::Get().FileSize(*ResolvedSourceFilePath) == INDEX_NONE) {
		UE_LOG(LogCriWareEditor, Error, TEXT("Cannot reimport: source file cannot be found."));
		return EReimportResult::Failed;
	}

	/* パラメータを一旦退避 */
	/* 備考）Contextにハンドルを渡してもFactoryCreateBinaryには届かない模様。 */
	bool bOverrideAwbDirectory = CueSheet->bOverrideAwbDirectory;
	FDirectoryPath AwbDirectory = CueSheet->AwbDirectory;

	/* アセットの差し替え */
	bool bCancelled = false;
	UObject* ResultObject = ImportObject(CueSheet->GetClass(), CueSheet->GetOuter(), *CueSheet->GetName(),
										 RF_Public | RF_Standalone, *ResolvedSourceFilePath, nullptr, bCancelled);
	
	CueSheet = Cast<USoundAtomCueSheet>(ResultObject);

	// check results
	if (!CueSheet) {
		if (bCancelled) {
			UE_LOG(LogCriWareEditor, Warning, TEXT("-- import canceled"));
			return EReimportResult::Cancelled;
		}

		UE_LOG(LogCriWareEditor, Warning, TEXT("-- import failed"));
		return EReimportResult::Failed;
	}

	UE_LOG(LogCriWareEditor, Log, TEXT("-- imported successfully"));

	/* パラメータの復元 */
	CueSheet->bOverrideAwbDirectory = bOverrideAwbDirectory;
	CueSheet->AwbDirectory = AwbDirectory;

	// Try to find the outer package so we can dirty it up
	if (CueSheet->GetOuter()) {
		CueSheet->GetOuter()->MarkPackageDirty();
	} else {
		CueSheet->MarkPackageDirty();
	}

	/* 参照の破棄 */
	CueSheet = nullptr;

	return EReimportResult::Succeeded;
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
