/****************************************************************************
*
* CRI Middleware SDK
*
* Copyright (c) 2017 CRI Middleware Co., Ltd.
*
* Library  : CRIWARE plugin for Unreal Engine 4
* Module   : Asset Factory of ManaPlaylist
* File     : ManaPlaylistFactory.cpp
*
****************************************************************************/

/* モジュールヘッダ */
#include "ManaPlaylistFactory.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "CriWareInitializer.h"

#include "ManaPlaylist.h"

#define LOCTEXT_NAMESPACE "UManaPlaylistFactory"

/* UManaPlaylistFactoryクラスの実装 */
UManaPlaylistFactory::UManaPlaylistFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditorImport = false;

	SupportedClass = UManaPlaylist::StaticClass();
}

UObject* UManaPlaylistFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto ManaPlaylist = NewObject<UManaPlaylist>(InParent, InClass, InName, Flags);
	
	if (ManaPlaylist != nullptr)
	{
		// add some movies if present in intial array
		for (auto ManaMovie : InitialMovies) {
			ManaPlaylist->Add(ManaMovie.Get());
		}
	}

	return ManaPlaylist;
}

#undef LOCTEXT_NAMESPACE
