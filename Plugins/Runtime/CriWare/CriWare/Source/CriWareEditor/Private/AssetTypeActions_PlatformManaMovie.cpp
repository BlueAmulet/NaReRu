// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_PlatformManaMovie.h"
#include "PlatformManaMovie.h"

#include "CriWareEditorStyle.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"


/* FPlatformManaMovieActions constructors
 *****************************************************************************/

FAssetTypeActions_PlatformManaMovie::FAssetTypeActions_PlatformManaMovie(/*const TSharedRef<ISlateStyle>& InStyle,*/ EAssetTypeCategories::Type InCategory)
	: FAssetTypeActions_ManaMovie(InCategory)
{ }


/* FAssetTypeActions_Base interface
 *****************************************************************************/

bool FAssetTypeActions_PlatformManaMovie::CanFilter()
{
	return true;
}


FText FAssetTypeActions_PlatformManaMovie::GetName() const
{
	return LOCTEXT("AssetTypeActions_PlatformManaMovie", "Platform Sofdec2 Movie");
}


UClass* FAssetTypeActions_PlatformManaMovie::GetSupportedClass() const
{
	return UPlatformManaMovie::StaticClass();
}


#undef LOCTEXT_NAMESPACE
