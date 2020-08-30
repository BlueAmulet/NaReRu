/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2019 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Detail panel for PlatformManaMovie asset
 * File     : PlatformManaMovieDetails.cpp
 *
 ****************************************************************************/

#include "PlatformManaMovieDetails.h"
#include "ManaMovie.h"
#include "PlatformManaMovie.h"
#include "Styling/SlateColor.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "EditorStyleSet.h"
#include "IDetailPropertyRow.h"
#include "Widgets/Text/STextBlock.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "ICriWareRuntime.h"
#include "PlatformInfo.h"
#include "PropertyCustomizationHelpers.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Images/SImage.h"
#include "../Launch/Resources/Version.h"


#define LOCTEXT_NAMESPACE "FPlatformManaMovieCustomization"


/* IDetailCustomization interface
 *****************************************************************************/

void FPlatformManaMovieDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// customize 'Platforms' category
	IDetailCategoryBuilder& SourcesCategory = DetailBuilder.EditCategory("Movies");
	{
		// PlatformManaMovies
		PlatformManaMoviesProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UPlatformManaMovie, PlatformManaMovies));
		{
			IDetailPropertyRow& PlatformManaMoviesRow = SourcesCategory.AddProperty(PlatformManaMoviesProperty);

			PlatformManaMoviesRow
				.ShowPropertyButtons(false)
				.CustomWidget()
				.NameContent()
				[
					PlatformManaMoviesProperty->CreatePropertyNameWidget()
				]
			.ValueContent()
				.MaxDesiredWidth(0.0f)
				[
					MakePlatformManaMoviesValueWidget()
				];
		}
	}
}


/* FPlatformMediaSourceCustomization implementation
 *****************************************************************************/

TSharedRef<SWidget> FPlatformManaMovieDetails::MakePlatformManaMoviesValueWidget()
{
	// get available platforms
	TArray<const PlatformInfo::FPlatformInfo*> AvailablePlatforms;

#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 22)
	for (const PlatformInfo::FPlatformInfo& PlatformInfo : PlatformInfo::EnumeratePlatformInfoArray())
#else
	for (const PlatformInfo::FPlatformInfo& PlatformInfo : PlatformInfo::GetPlatformInfoArray())
#endif
	{
#if (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 24)
		if (PlatformInfo.IsVanilla() && (PlatformInfo.PlatformType == EBuildTargetType::Game) && (PlatformInfo.PlatformInfoName != TEXT("AllDesktop")))
#else
		if (PlatformInfo.IsVanilla() && (PlatformInfo.PlatformType == PlatformInfo::EPlatformType::Game) && (PlatformInfo.PlatformInfoName != TEXT("AllDesktop")))
#endif
		{
			if (PlatformInfo.PlatformInfoName == TEXT("TVOS"))
			{
				continue; // tvOS is just iOS for now
			}

			AvailablePlatforms.Add(&PlatformInfo);
		}
	}

	// sort available platforms alphabetically
	AvailablePlatforms.Sort([](const PlatformInfo::FPlatformInfo& One, const PlatformInfo::FPlatformInfo& Two) -> bool
	{
		return One.DisplayName.CompareTo(Two.DisplayName) < 0;
	});

	// build value widget
	TSharedRef<SGridPanel> PlatformPanel = SNew(SGridPanel);

	for (int32 PlatformIndex = 0; PlatformIndex < AvailablePlatforms.Num(); ++PlatformIndex)
	{
		const PlatformInfo::FPlatformInfo* Platform = AvailablePlatforms[PlatformIndex];

		// platform icon
		PlatformPanel->AddSlot(0, PlatformIndex)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush(Platform->GetIconStyleName(PlatformInfo::EPlatformIconSize::Normal)))
			];

		// platform name
		PlatformPanel->AddSlot(1, PlatformIndex)
			.Padding(4.0f, 0.0f, 16.0f, 0.0f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(Platform->DisplayName)
			];

		// player combo box
		PlatformPanel->AddSlot(2, PlatformIndex)
			.VAlign(VAlign_Center)
			[
				SNew(SObjectPropertyEntryBox)
				.AllowedClass(UManaMovie::StaticClass())
			.AllowClear(true)
			.ObjectPath(this, &FPlatformManaMovieDetails::HandleManaMovieEntryBoxObjectPath, Platform->IniPlatformName)
			.OnObjectChanged(this, &FPlatformManaMovieDetails::HandleManaMovieEntryBoxChanged, Platform->IniPlatformName)
			.OnShouldFilterAsset(this, &FPlatformManaMovieDetails::HandleManaMovieEntryBoxShouldFilterAsset)
			];
	}

	return PlatformPanel;
}


void FPlatformManaMovieDetails::SetPlatformManaMoviesValue(FString PlatformName, UManaMovie* ManaMovie)
{
	TArray<UObject*> OuterObjects;
	{
		PlatformManaMoviesProperty->GetOuterObjects(OuterObjects);
	}

	for (auto Object : OuterObjects)
	{
		UManaMovie*& OldManaMovie = Cast<UPlatformManaMovie>(Object)->PlatformManaMovies.FindOrAdd(PlatformName);;

		if (OldManaMovie != ManaMovie)
		{
			Object->Modify(true);
			OldManaMovie = ManaMovie;

			PlatformManaMoviesProperty->NotifyPostChange(EPropertyChangeType::ValueSet);
		}
	}
}


/* FPlatformMediaSourceCustomization callbacks
 *****************************************************************************/

void FPlatformManaMovieDetails::HandleManaMovieEntryBoxChanged(const FAssetData& AssetData, FString PlatformName)
{
	TArray<UObject*> OuterObjects;
	{
		PlatformManaMoviesProperty->GetOuterObjects(OuterObjects);
	}

	for (auto Object : OuterObjects)
	{
		UManaMovie*& OldManaMovie = Cast<UPlatformManaMovie>(Object)->PlatformManaMovies.FindOrAdd(PlatformName);;

		if (OldManaMovie != AssetData.GetAsset())
		{
			Object->Modify(true);
			OldManaMovie = Cast<UManaMovie>(AssetData.GetAsset());

			PlatformManaMoviesProperty->NotifyPostChange(EPropertyChangeType::ValueSet);
		}
	}
}


FString FPlatformManaMovieDetails::HandleManaMovieEntryBoxObjectPath(FString PlatformName) const
{
	TArray<UObject*> OuterObjects;
	{
		PlatformManaMoviesProperty->GetOuterObjects(OuterObjects);
	}

	if (OuterObjects.Num() == 0)
	{
		return FString();
	}

	UManaMovie* ManaMovie = Cast<UPlatformManaMovie>(OuterObjects[0])->PlatformManaMovies.FindRef(PlatformName);

	for (int32 ObjectIndex = 1; ObjectIndex < OuterObjects.Num(); ++ObjectIndex)
	{
		if (Cast<UPlatformManaMovie>(OuterObjects[ObjectIndex])->PlatformManaMovies.FindRef(PlatformName) != ManaMovie)
		{
			return FString();
		}
	}

	if (ManaMovie == nullptr)
	{
		return FString();
	}

	return ManaMovie->GetPathName();
}


bool FPlatformManaMovieDetails::HandleManaMovieEntryBoxShouldFilterAsset(const FAssetData& AssetData)
{
	// Don't allow nesting platform media sources.
	UClass* AssetClass = FindObject<UClass>(ANY_PACKAGE, *AssetData.AssetClass.ToString());
	return AssetClass->IsChildOf(UPlatformManaMovie::StaticClass());
}


#undef LOCTEXT_NAMESPACE
