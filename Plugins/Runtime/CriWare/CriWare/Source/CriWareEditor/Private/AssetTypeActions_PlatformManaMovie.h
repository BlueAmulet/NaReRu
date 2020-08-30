
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Styling/ISlateStyle.h"
#include "AssetTypeActions_ManaMovie.h"

struct FAssetData;

/**
 * Implements an action for UPlatformManaMovie assets.
 */
class FAssetTypeActions_PlatformManaMovie
	: public FAssetTypeActions_ManaMovie
{
public:

	/**
	 * Creates and initializes a new instance.
	 *
	 * @param InStyle The style set to use for asset editor toolkits.
	 */
	FAssetTypeActions_PlatformManaMovie(/*const TSharedRef<ISlateStyle>& InStyle, */EAssetTypeCategories::Type InCategory);

public:

	//~ FAssetTypeActions_Base interface

	virtual FColor GetTypeColor() const override { return FColor::White; }
	virtual bool CanFilter() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual bool IsImportedAsset() const override { return false; }

private:
	//EAssetTypeCategories::Type MyAssetCategory;

	/** Pointer to the style set to use for toolkits. */
	//TSharedRef<ISlateStyle> Style;
};
