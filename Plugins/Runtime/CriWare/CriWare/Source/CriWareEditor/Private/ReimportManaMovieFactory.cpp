/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Reimport Factory of Mana Data
 * File     : ReimportManaFactory.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "ReimportManaMovieFactory.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "CriWarePreviewer.h"
#include "CriWareInitializer.h"
#include "ManaMovie.h"

/* Unreal Editor関連ヘッダ */
#include "EditorFramework/AssetImportData.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "ReimportManaMovieFactory"

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
UReimportManaMovieFactory::UReimportManaMovieFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UReimportManaMovieFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UManaMovie* ManaMovie = Cast<UManaMovie>(Obj);

	if (ManaMovie && ManaMovie->AssetImportData) {
		ManaMovie->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}

	return false;
}

void UReimportManaMovieFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UManaMovie* ManaMovie = Cast<UManaMovie>(Obj);
	if (ManaMovie && ensure(NewReimportPaths.Num() == 1)) {
		ManaMovie->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UReimportManaMovieFactory::Reimport(UObject* Obj)
{
	/* インポート情報の取得 */
	UManaMovie* ManaMovie = Cast<UManaMovie>(Obj);
	if (!ManaMovie || !ManaMovie->AssetImportData) {
		return EReimportResult::Failed;
	}

	// TODO: check playing

	/* ファイルパスの作成 */
	FString ResolvedSourceFilePath = ManaMovie->AssetImportData->GetFirstFilename();
	if (!ResolvedSourceFilePath.Len()) {
		return EReimportResult::Failed;
	}

	UE_LOG(LogCriWareEditor, Log, TEXT("Performing atomic reimport of [%s]."), *ResolvedSourceFilePath);

	/* ファイルの有無をチェック */
	if (IFileManager::Get().FileSize(*ResolvedSourceFilePath) == INDEX_NONE) {
		UE_LOG(LogCriWareEditor, Error, TEXT("Cannot reimport: source file cannot be found."));
		return EReimportResult::Failed;
	}

	bool Result = EReimportResult::Failed;
	
	/* アセットの差し替え */
	if (ManaMovie->GetClass() == UFileManaMovie::StaticClass()) {
		auto* FileManaMovie = CastChecked<UFileManaMovie>(ManaMovie);

		ManaMovie->AssetImportData->Update(ResolvedSourceFilePath);

		// Get file path if imported
		auto PrevFile = FileManaMovie->GetFilePath();
		/* DefaultCriWare.iniファイルで設定したContentDirへの相対パスを保管する */
		FPaths::MakePathRelativeTo(ResolvedSourceFilePath, *UCriWareInitializer::GetContentDir());
		FileManaMovie->SetFilePath(ResolvedSourceFilePath);

		// if movie is nor validated, nor validating, it means path is not loadable
		if (ManaMovie->IsValidating() || ManaMovie->Validate()) {
			Result = EReimportResult::Succeeded;

			UE_LOG(LogCriWareEditor, Log, TEXT("Imported successfully"));

			// FIXME block with a timeout until validation is posible for asset importation.
		} else {
			// restore 
			FileManaMovie->SetFilePath(PrevFile);
		}
	}

	if (Result == EReimportResult::Succeeded) {
		// Try to find the outer package so we can dirty it up
		if (ManaMovie->GetOuter()) {
			ManaMovie->GetOuter()->MarkPackageDirty();
		} else {
			ManaMovie->MarkPackageDirty();
		}
		return EReimportResult::Succeeded;
	}

	UE_LOG(LogCriWareEditor, Warning, TEXT("-- import failed"));

	return EReimportResult::Failed;
}

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
