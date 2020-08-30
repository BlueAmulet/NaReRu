/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2017-2018 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Movie Scene Atom Section
 * File     : MovieSceneAtomSection.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "Sections/MovieSceneAtomSection.h"

/* CRIWAREプラグインヘッダ */
#include "CriWareRuntimePrivatePCH.h"
#include "Evaluation/MovieSceneAtomTemplate.h"

/* Unreal Engine 4関連ヘッダ */
#include "Audio.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "UObject/SequencerObjectVersion.h"

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
#if WITH_EDITOR
struct FAtomChannelEditorData
{
	FAtomChannelEditorData()
	{
		Data[0].SetIdentifiers("Volume", NSLOCTEXT("MovieSceneAtomSection", "SoundVolumeText", "Volume"));
		Data[1].SetIdentifiers("Pitch", NSLOCTEXT("MovieSceneAtomSection", "PitchText", "Pitch"));
	}

	FMovieSceneChannelMetaData Data[2];
};
#endif // WITH_EDITOR

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
namespace MovieSceneAtomSection {
	float GetStartOffsetAtTrimTime(FQualifiedFrameTime TrimTime, float StartOffset, FFrameNumber StartFrame);
};

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/
UMovieSceneAtomSection::UMovieSceneAtomSection(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Sound = nullptr;
	StartOffset = 0.f;
	SoundVolume.SetDefault(1.f);
	PitchMultiplier.SetDefault(1.f);
	bSuppressSubtitles = false;

	EvalOptions.EnableAndSetCompletionMode(EMovieSceneCompletionMode::RestoreState);

	// Set up the channel proxy
	FMovieSceneChannelProxyData Channels;

#if WITH_EDITOR
	static const FAtomChannelEditorData EditorData;
	Channels.Add(SoundVolume,     EditorData.Data[0], TMovieSceneExternalValue<float>());
	Channels.Add(PitchMultiplier, EditorData.Data[1], TMovieSceneExternalValue<float>());
#else
	Channels.Add(SoundVolume);
	Channels.Add(PitchMultiplier);
#endif

	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));
}

void UMovieSceneAtomSection::Serialize(FArchive& Ar)
{
	if (Ar.IsLoading()) {
		/* エラー回避措置の開始 */
		/* 備考）bSupportsInfiniteRangeがfalseの場合、UE4.19以前に作成したループ音を含むデータがエラーになる。 */
		bSupportsInfiniteRange = true;
	}

	/* アセットの読み書き */
	Super::Serialize(Ar);

	if (Ar.CustomVer(FSequencerObjectVersion::GUID) < FSequencerObjectVersion::FloatToIntConversion) {
		/* UE4.19以前で作成されたループデータの範囲を補正 */
		if (EndTime_DEPRECATED > INDEFINITELY_LOOPING_DURATION) {
			EndTime_DEPRECATED = INDEFINITELY_LOOPING_DURATION;
			const FFrameRate LegacyFrameRate = GetLegacyConversionFrameRate();
			FFrameNumber StartFrame = UpgradeLegacyMovieSceneTime(this, LegacyFrameRate, StartTime_DEPRECATED);
			FFrameNumber LastFrame  = UpgradeLegacyMovieSceneTime(this, LegacyFrameRate, EndTime_DEPRECATED);
			SectionRange = TRange<FFrameNumber>(StartFrame, LastFrame + 1);
		}
	}

	if (Ar.IsLoading()) {
		/* エラー回避措置の終了 */
		bSupportsInfiniteRange = false;
	}
}

FMovieSceneEvalTemplatePtr UMovieSceneAtomSection::GenerateTemplate() const
{
	return FMovieSceneAtomSectionTemplate(*this);
}

TOptional<FFrameTime> UMovieSceneAtomSection::GetOffsetTime() const
{
	return TOptional<FFrameTime>(StartOffset * GetTypedOuter<UMovieScene>()->GetTickResolution());
}

TOptional<TRange<FFrameNumber>> UMovieSceneAtomSection::GetAutoSizeRange() const
{
	if (!Sound) {
		return TRange<FFrameNumber>();
	}

	/* フレームレートの取得 */
	FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();

	/* 音声データの長さ（秒単位）を取得 */
	float SoundDuration = Sound->GetDuration();
	if (SoundDuration < INDEFINITELY_LOOPING_DURATION) {
		SoundDuration = Sound->GetDuration(); // seconds
	} else {
		SoundDuration = Sound->GetFirstWaveDuration(); // if not a wave this return INDEFINITELY_LOOPING_DURATION
	}

	/* フレーム単位に換算 */
	FFrameTime DurationToUse = SoundDuration * FrameRate;

	return TRange<FFrameNumber>(GetInclusiveStartFrame(), GetInclusiveStartFrame() + DurationToUse.FrameNumber);
}

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 24

void UMovieSceneAtomSection::TrimSection(FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys)
{
	SetFlags(RF_Transactional);

	if (TryModify()) {
		if (bTrimLeft) {
			StartOffset = HasStartFrame() ? MovieSceneAtomSection::GetStartOffsetAtTrimTime(TrimTime, StartOffset, GetInclusiveStartFrame()) : 0;
		}

		Super::TrimSection(TrimTime, bTrimLeft, bDeleteKeys);
	}
}

UMovieSceneSection* UMovieSceneAtomSection::SplitSection(FQualifiedFrameTime SplitTime, bool bDeleteKeys)
{
	const float InitialStartOffset = StartOffset;
	const float NewOffset = HasStartFrame() ? MovieSceneAtomSection::GetStartOffsetAtTrimTime(SplitTime, StartOffset, GetInclusiveStartFrame()) : 0;

	UMovieSceneSection* NewSection = Super::SplitSection(SplitTime, bDeleteKeys);
	if (NewSection != nullptr)
	{
		UMovieSceneAtomSection* NewAudioSection = Cast<UMovieSceneAtomSection>(NewSection);
		NewAudioSection->StartOffset = NewOffset;
	}

	// 無意味な処理のはずだが、UMovieSceneAudioSection::SplitSection関数の実装と同じ処理を行う
	StartOffset = InitialStartOffset;

	return NewSection;
}

#else

void UMovieSceneAtomSection::TrimSection(FQualifiedFrameTime TrimTime, bool bTrimLeft)
{
	SetFlags(RF_Transactional);

	if (TryModify()) {
		if (bTrimLeft) {
			StartOffset = HasStartFrame() ? MovieSceneAtomSection::GetStartOffsetAtTrimTime(TrimTime, StartOffset, GetInclusiveStartFrame()) : 0;
		}

		Super::TrimSection(TrimTime, bTrimLeft);
	}
}

UMovieSceneSection* UMovieSceneAtomSection::SplitSection(FQualifiedFrameTime SplitTime)
{
	const float NewOffset = HasStartFrame() ? MovieSceneAtomSection::GetStartOffsetAtTrimTime(SplitTime, StartOffset, GetInclusiveStartFrame()) : 0;

	UMovieSceneSection* NewSection = Super::SplitSection(SplitTime);
	if (NewSection != nullptr)
	{
		UMovieSceneAtomSection* NewAudioSection = Cast<UMovieSceneAtomSection>(NewSection);
		NewAudioSection->StartOffset = NewOffset;
	}
	return NewSection;
}

#endif

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/
float MovieSceneAtomSection::GetStartOffsetAtTrimTime(FQualifiedFrameTime TrimTime, float StartOffset, FFrameNumber StartFrame)
{
	return StartOffset + (TrimTime.Time - StartFrame) / TrimTime.Rate;
}

#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
