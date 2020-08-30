/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2013-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Factory of Atom Data
 * File     : SoundAtomCueSheetFactory.h
 *
 ****************************************************************************/

/* 多重定義防止 */
#pragma once

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* Unreal Engine 4関連ヘッダ */
#include "Factories/Factory.h"

/* モジュールヘッダ */
#include "SoundAtomCueSheetFactory.generated.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
class USoundAtomCueSheet;
class USoundAtomCue;

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      クラス宣言
 *      Prototype Classes
 ***************************************************************************/

enum EAtomCueImportStatus {
	NewCue,		/* New cue in cue sheet */
	ExistCue,	/* Unchanged, will be updated with last data */
	DeletedCue	/* No more existing cue */
};

struct FAtomCueListItem {
	bool Selected; // selection status
	FText CueName; // name of the cue
	int32 CueID; // ID of the cue
	EAtomCueImportStatus ImportStatus; // Status of Cue asset with importing acb
	USoundAtomCue* CueAsset; // cue asset to update, may be nullptr
	CriAtomExAcbHn AcbHn; // handle on importing acb, may be nullptr

	FAtomCueListItem(bool InSelected, const FText& InCueName, int32 InCueID,
		EAtomCueImportStatus InImportStatus, USoundAtomCue* InCueAsset = nullptr, CriAtomExAcbHn InAcbHn = nullptr)
		: Selected(InSelected)
		, CueName(InCueName)
		, CueID(InCueID)
		, ImportStatus(InImportStatus)
		, CueAsset(InCueAsset)
		, AcbHn(InAcbHn)
	{}

	void GetCueInfo(FTimespan& OutDuration, bool& OutIsLooping, bool& OutIs3D,
		CriAtomExFormat& OutFormat, int32& OutNumChannels) const;
	FText ToDisplayString() const;
	void GetStatusTextAndColor(FText& OutText, FColor& OutColor) const;
	void GetActionTextAndColor(FText& OutText, FColor& OutColor) const;
};

/**
 * Options for UAtomCueSheetFactory.
 */
struct FAtomCueSheetFactoryOptions
{
	FString CueSheetPackageName;
	TArray<TSharedPtr<FAtomCueListItem>> Items;
	bool EnableEconomicTick;
	bool EnableDistanceCulling;
	bool OkClicked;
};

UCLASS(hidecategories = Object)
class USoundAtomCueSheetFactory : public UFactory
{
	GENERATED_BODY()

public:
	USoundAtomCueSheetFactory(const FObjectInitializer& ObjectInitializer);

	// Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual UClass* ResolveSupportedClass() override;
	virtual UObject* FactoryCreateBinary(
		UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, const TCHAR* Type,
		const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	// End UFactory Interface

	// Begin UObject interface.
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	// End  UObject interface

protected:
	/* 再インポート直前のオブジェクト */
	UPROPERTY(Transient)
	class USoundAtomCueSheet* CueSheet;

private:
	/** User options with import dialog. */
	FAtomCueSheetFactoryOptions Options;

	/** Load ACB data from a buffer. */
	CriAtomExAcbHn SyncLoadACBDataFromBuffer(UObject* InParent, const uint8*& Buffer, const uint8* BufferEnd);

	/** Retrive SoundAtomCue assets used when reimporting this SoundAtomCueSheet. */
	TArray<USoundAtomCue*> GetExistingCueAssets(UObject* InParent, FString PackagePath);

	/** Show import modal dialog and return user options. */
	bool GetImportOptions(CriAtomExAcbHn AcbHn, TArray<USoundAtomCue*> ExistingCues);

	/** Create a unique name for a SoundAtomCue from a cue name. */
	FString CreateUniqueCueAssetPackageName(FString CueName);

	/** Load and add additional information for cue sheet and cues from a Json file. */
	bool LoadAdditionalCueSheetDataFile();
};

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/

 /* --- end of file --- */
