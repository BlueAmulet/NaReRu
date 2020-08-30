#pragma once

#include "CoreTypes.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "UObject/ObjectMacros.h"
#include "ManaMovie.h"

#include "PlatformManaMovie.generated.h"


/**
 * A Mana source that selects other Mana movies based on target platform.
 *
 * Use this asset to override Mana movies on a per-platform basis.
 */
UCLASS(BlueprintType)
class CRIWARERUNTIME_API UPlatformManaMovie : public UManaMovie
{
	GENERATED_UCLASS_BODY()

public:

#if WITH_EDITORONLY_DATA

	/** Mana movies per platform. */
	UPROPERTY(EditAnywhere, Category = Sources, Meta = (DisplayName = "Mana Movies"))
	TMap<FString, UManaMovie*> PlatformManaMovies;

#endif

public:

	//~ UObject interface

#if WITH_EDITOR
	/** USMファイルパスがエディタ上で変更されたときの処理 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
	virtual void Serialize(FArchive& Ar) override;

public:

	//~ UManaMovie interface

	virtual FString GetUrl() const override;
	virtual bool Validate() const override;

private:

	/** 
	 * Reload movie information from Mana movie.
	 *
	 *  @return true if information was reloaded.
	 */
	bool ReloadMovieInfo() override;

	/**
     * Get the Mana movie for the running target platform.
     *
     * @return The Mana movie, or nullptr if not set.
     */
	UManaMovie* GetManaMovie() const;

private:

	/**
	 * Default Mana movie.
	 *
	 * This Mana movie will be used if no source was specified for a target platform.
	 */
	UPROPERTY()
	UManaMovie* ManaMovie;
};

