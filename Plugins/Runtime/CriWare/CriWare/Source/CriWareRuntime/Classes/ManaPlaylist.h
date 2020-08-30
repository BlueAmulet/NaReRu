/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Mana play list asset
 * File     : ManaPlaylist.h
 *
 ****************************************************************************/
#pragma once

#include "ManaMovie.h"
#include "ManaPlaylist.generated.h"

/** ManaPlaylist class represents a play list of Sofdec2 movies. */
UCLASS(BlueprintType, hidecategories = (Object), meta = (ToolTip = "ManaPlaylist class."))
class CRIWARERUNTIME_API UManaPlaylist : public UManaSource
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	/** USMファイルパスがエディタ上で変更されたときの処理 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** After loading asset. */
	virtual void PostLoad() override;

	/*+ When serializing asset. */
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface

public:
	/**
	* Add a media source to the play list.
	*
	* @param ManaMovie The media source to append.
	* @see Insert, RemoveAll, Remove
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist")
	void Add(UManaMovie* ManaMovie);

	/**
	* Get the media source at the specified index.
	*
	* @param Index The index of the media source to get.
	* @return The media source, or nullptr if the index doesn't exist.
	* @see GetNext, GetRandom
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist", meta = (ToolTip = "Gets the movie source at Index. (Does not count empty slots.)"))
	UManaMovie* Get(int32 Index);

	/**
	* Get the next media source in the play list.
	*
	* @param InOutIndex Index of the current media source (will contain the new index).
	* @return The media source after the current one, or nullptr if the list is empty.
	* @see , GetPrevious, GetRandom
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist")
	UManaMovie* GetNext(int32& InOutIndex);

	/**
	* Get the previous media source in the play list.
	*
	* @param InOutIndex Index of the current media source (will contain the new index).
	* @return The media source before the current one, or nullptr if the list is empty.
	* @see , GetNext, GetRandom
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist")
	UManaMovie* GetPrevious(int32& InOutIndex);

	/**
	* Get a random media source in the play list.
	*
	* @param InOutIndex Index of the current media source (will contain the new index).
	* @return The random media source, or nullptr if the list is empty.
	* @see Get, GetNext, GetPrevious
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist")
	UManaMovie* GetRandom(int32& InOutIndex);

	/**
	* Insert a media source into the play list at the given position.
	*
	* @param ManaMovie The media source to insert.
	* @param Index The index to insert into.
	* @see Add, Remove, RemoveAll
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist")
	void Insert(UManaMovie* ManaMovie, int32 Index);

	/**
	* Get the number of media sources in the play list.
	*
	* @return Number of media sources.
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist", meta = (ToolTip = "The number of playable movies in this play list."))
	int32 Num()
	{
		return Movies.Num();
	}

	/**
	* Remove all occurrences of the given media source in the play list.
	*
	* @param ManaMovie The media source to remove.
	* @see Add, Insert, Remove
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist")
	void Remove(UManaMovie* ManaMovie);

	/**
	* Remove the media source at the specified position.
	*
	* @param Index The index of the media source to remove.
	* @see Add, Insert, RemoveAll
	*/
	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist")
	void RemoveAt(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist", meta = (ToolTip = "Is play list use some alpha movies.", ScriptName = "IsAlphaMethod"))
	bool IsAlpha() 
	{ 
		return bIsAlpha;
	}

	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist", meta = (ToolTip = "Is play list use mixed types of movies (ex. Prime and H.264). Play list with mixed types request that the attached Sofdec2 ManaTexture use 'Rendrer to Texture' option and corresponding material.", ScriptName = "IsMixedTypesMethod"))
	bool IsMixedTypes()
	{ 
		return bIsMixedTypes;
	}

	UFUNCTION(BlueprintCallable, Category = "Mana|ManaPlaylist", meta = (ToolTip = "Is play list can be played seamless.", ScriptName = "IsSeamlessMethod"))
	bool IsSeamless()
	{
		return bIsSeamless;
	}

protected:
	/** List of media sources to play. */
	UPROPERTY(EditAnywhere, Category = Mana, meta = (ToolTip = "The playlist."))
	TArray<UManaMovie*> Movies;

	/** Sofdec2 movies to play. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Is play list use some alpha movies."))
	bool bIsAlpha;

	/** Is the playlist use varions types. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Is play list use mixed types of movies (ex. Prime and H.264). Play list with mixed types request that the attached Sofdec2 ManaTexture use 'Rendrer to Texture' option and corresponding material."))
	bool bIsMixedTypes;

	/** Is the playlist can be played seamless. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mana, meta = (ToolTip = "Is play list can be played seamless."))
	bool bIsSeamless;
	
private:
	void UpdatePlaylistInfo();
	bool CheckAlpha();
	bool CheckMixedTypes();
	bool CheckSeamless();
};