/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2019 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Detail panel for PlatformManaMovie asset
 * File     : PlatformManaMovieDetails.h
 *
 ****************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"

struct FAssetData;
class IDetailLayoutBuilder;
class IPropertyHandle;
class SWidget;
class UManaMovie;

/**
 * Implements a details view customization for the UPlatformMediaSource class.
 */
class FPlatformManaMovieDetails
	: public IDetailCustomization
{
public:

	//~ IDetailCustomization interface

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

public:

	/**
	 * Creates an instance of this class.
	 *
	 * @return The new instance.
	 */
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FPlatformManaMovieDetails());
	}

protected:

	/**
	 * Makes a widget for the DefaultPlayers property value.
	 *
	 * @return The widget.
	 */
	TSharedRef<SWidget> MakePlatformManaMoviesValueWidget();

	/**
	 * Set the value of the PlatformManaMovies property.
	 *
	 * @param ManaSource The media source to set.
	 */
	void SetPlatformManaMoviesValue(FString PlatformName, UManaMovie* ManaMovie);

private:

	/** Callback for when a per-platform media source property changed. */
	void HandleManaMovieEntryBoxChanged(const FAssetData& AssetData, FString PlatformName);

	/** Callback for getting the currently selected object in a per-platform media source property. */
	FString HandleManaMovieEntryBoxObjectPath(FString PlatformName) const;

	/** Callback for filtering media source assets. */
	bool HandleManaMovieEntryBoxShouldFilterAsset(const FAssetData& AssetData);

private:

	/** Pointer to the DefaultPlayers property handle. */
	TSharedPtr<IPropertyHandle> PlatformManaMoviesProperty;
};

