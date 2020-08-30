/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Factory of ManaMovie
* File     : ManaMovieFactory.cpp
*
****************************************************************************/

/* モジュールヘッダ */
#include "ManaMovieFactory.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "ManaMovie.h"
#include "ManaTexture.h"
#include "ManaTextureFactory.h"
#include "CriWareInitializer.h"

/* Unreal Engine 4関連ヘッダ */
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "UManaMovieFactory"

/* UManaMovieFactoryクラスの実装 */
UManaMovieFactory::UManaMovieFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Formats.Add(TEXT("usm;Sofdec2 Movie File"));

	bCreateNew = true;
	bEditorImport = true;

	SupportedClass = UFileManaMovie::StaticClass();
}

UObject* UManaMovieFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto ManaMovie = NewObject<UFileManaMovie>(InParent, InClass, InName, Flags);

	if (ManaMovie != nullptr)
	{
		// Get file path if imported
		FString MovieFilePath = GetCurrentFilename();
		if (!MovieFilePath.IsEmpty()) {
			/* DefaultCriWare.iniファイルで設定したContentDirへの相対パスを保管する */
			FPaths::MakePathRelativeTo(MovieFilePath, *UCriWareInitializer::GetContentDir());
		}

		ManaMovie->SetFilePath(MovieFilePath);

		// if movie is nor validated, nor validating, it means path is not loadable
		if (!ManaMovie->IsValidating() && !ManaMovie->Validate()) {
			return nullptr;
		}

		// FIXME block with a timeout until validation is posible for asset importation.

		if (ManaMovie->AssetImportData == nullptr) {
			ManaMovie->AssetImportData = NewObject<UAssetImportData>(ManaMovie, TEXT("AssetImportData"));
		}

		if (ManaMovie->AssetImportData != nullptr) {
			ManaMovie->AssetImportData->Update(CurrentFilename);
		}
	}

	return ManaMovie;
}

#undef LOCTEXT_NAMESPACE
