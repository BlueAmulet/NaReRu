/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2015-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRIWARE plugin for Unreal Engine 4
 * Module   : Asset Type Action of Mana YUV Texture
 * File     : AssetTypeActions_ManaTexture.cpp
 *
 ****************************************************************************/

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
/* モジュールヘッダ */
#include "AssetTypeActions_ManaTexture.h"

/* ProjectPluginとして利用する場合MonolithicHeaderBoilerplate.hをインクルード */
#if !defined(UE_IS_ENGINE_MODULE)
#include "Misc/MonolithicHeaderBoilerplate.h"
MONOLITHIC_HEADER_BOILERPLATE()
#endif

/* CRIWAREプラグインヘッダ */
#include "CriWareEditorPrivatePCH.h"
#include "ManaTexture.h"
#include "CriWareInitializer.h"

/* Unreal Engine 4関連ヘッダ */
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include "Interfaces/ITextureEditorToolkit.h"
#include "Interfaces/ITextureEditorModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "EditorFramework/AssetImportData.h"

#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Editor.h"
#include "PropertyHandle.h"

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
#define LOCTEXT_NAMESPACE "AssetTypeActions"

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/

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

/***************************************************************************
 *      変数定義
 *      Variable Definition
 ***************************************************************************/

/***************************************************************************
 *      クラス定義
 *      Class Definition
 ***************************************************************************/

/***************************************************************************
 *      関数定義
 *      Function Definition
 ***************************************************************************/

#include "CriWareEditorStyle.h"

 /* Local helpers
 *****************************************************************************/

 /* Local helpers
 *****************************************************************************/

class SManaMaterialFactoryDialog
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SManaMaterialFactoryDialog) { }
	SLATE_END_ARGS()

		/** Construct this widget. */
		void Construct(const FArguments& InArgs, FManaMaterialFactoryOptions& InOptions, TSharedRef<SWindow> InWindow, bool& InResult)
	{
		Options = &InOptions;
		Window = InWindow;
		Result = &InResult;

		ChildSlot
		[
			SNew(SBorder)
			.Visibility(EVisibility::Visible)
			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.FillHeight(1)
				.VAlign(VAlign_Top)
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.Padding(4.0f)
					.Content()
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("CreateManaMaterialLabel", "Choose options to create material:"))
						]

						+ SVerticalBox::Slot()
						.Padding(0.0f, 8.0f, 0.0f, 0.0f)
						[
							SNew(SCheckBox)
							.Style(FEditorStyle::Get(), "RadioButton")
							.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioChecked, EManaMaterialFactoryCreateChoices::CreateComponentMaterial)
							.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioChanged, EManaMaterialFactoryCreateChoices::CreateComponentMaterial)
							.Content()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("CreateComponentsLabel", "Creates new material using component textures."))
							]
							.ToolTipText(LOCTEXT("CreateComponentsMaterialToolTip", "This link new material to each YUV component textures of the selected Sofdec2 Movie Texture."))
						]

						+ SVerticalBox::Slot()
						.Padding(0.0f, 6.0f, 0.0f, 0.0f)
						[
							SNew(SCheckBox)
							.Style(FEditorStyle::Get(), "RadioButton")
							.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioChecked, EManaMaterialFactoryCreateChoices::CreateRenderToTextureMaterial)
							.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioChanged, EManaMaterialFactoryCreateChoices::CreateRenderToTextureMaterial)
							.Content()
							[
								SNew(STextBlock)
								.Text(LOCTEXT("CreateRenderToTextureMaterialLabel", "Creates new material using Render to Texture."))
							]
							.ToolTipText(LOCTEXT("CreateRenderToTextureMaterialToolTip", "This link new material to the rendered RGBA texture of the selected Sofdec2 Movie Texture. (This automatically enable 'Render to Texture' in ManaTexture.)"))
						]

						+ SVerticalBox::Slot()
						.Padding(0.0f, 6.0f, 0.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("AlphaChannelSupportLabel", "Alpha channel support:"))
						]

						+ SVerticalBox::Slot()
						.Padding(20.0f, 6.0f, 0.0f, 0.0f)
						[
							SNew(SUniformGridPanel)
							
							+ SUniformGridPanel::Slot(0, 0)
							[
								SNew(SCheckBox)
								.Style(FEditorStyle::Get(), "RadioButton")
								.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioAlphaChecked, EManaMaterialFactoryOptionChoices::OptionAuto)
								.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioAlphaChanged, EManaMaterialFactoryOptionChoices::OptionAuto)
								.Content()
								[
									SNew(STextBlock)
									.Text(LOCTEXT("ManaOptionAlphaAutoLabel", "Automatic"))
								]
								.ToolTipText(LOCTEXT("ManaOptionAlphaAutoToolTip", "Determined by Movie source of Sofdec2 Movie Texture."))
							]

							+ SUniformGridPanel::Slot(1, 0)
							[
								SNew(SCheckBox)
								.Style(FEditorStyle::Get(), "RadioButton")
								.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioAlphaChecked, EManaMaterialFactoryOptionChoices::OptionFalse)
								.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioAlphaChanged, EManaMaterialFactoryOptionChoices::OptionFalse)
								.Content()
								[
									SNew(STextBlock)
									.Text(LOCTEXT("ManaOptionDiscardAlphaLabel", "Discard Alpha"))
								]
								.ToolTipText(LOCTEXT("ManaOptionDiscardAlphaToolTip", "Do not use alpha channel from Sofdec2 Movie Texture."))
							]
						]

						+ SVerticalBox::Slot()
						.Padding(0.0f, 6.0f, 0.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("AlphaPlusSupportLabel", "Alpha Plus support:"))
						]

						+ SVerticalBox::Slot()
						.Padding(20.0f, 6.0f, 0.0f, 0.0f)
						[
							SNew(SUniformGridPanel)

							+ SUniformGridPanel::Slot(0, 0)
							[
								SNew(SCheckBox)
								.Style(FEditorStyle::Get(), "RadioButton")
								.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioAlphaPlusChecked, EManaMaterialFactoryOptionChoices::OptionFalse)
								.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioAlphaPlusChanged, EManaMaterialFactoryOptionChoices::OptionFalse)
								.Content()
								[
									SNew(STextBlock)
									.Text(LOCTEXT("ManaOptionAlphaPlusNoLabel", "No"))
								]
								.ToolTipText(LOCTEXT("ManaOptionAlphaPlusNoToolTip", "Non Alpha Plus movie."))
							]

							+ SUniformGridPanel::Slot(1, 0)
							[
								SNew(SCheckBox)
								.Style(FEditorStyle::Get(), "RadioButton")
								.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioAlphaPlusChecked, EManaMaterialFactoryOptionChoices::OptionTrue)
								.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioAlphaPlusChanged, EManaMaterialFactoryOptionChoices::OptionTrue)
								.Content()
								[
									SNew(STextBlock)
									.Text(LOCTEXT("ManaOptionAlphaPlusYesLabel", "Yes"))
								]
								.ToolTipText(LOCTEXT("ManaOptionAlphaPlusYesToolTip", "Alpha Plus movie. (Premutiplied alpha blending for alpha movie or additive blending for non aplha movie.)"))
							]
						]

						+ SVerticalBox::Slot()
						.Padding(0.0f, 6.0f, 0.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("HardwareDecodedMovieSupportLabel", "H/W decoded movie support:"))
						]

						+ SVerticalBox::Slot()
						.Padding(20.0f, 6.0f, 0.0f, 0.0f)
						[
							SNew(SUniformGridPanel)
							+ SUniformGridPanel::Slot(0, 0)
							[
								SNew(SCheckBox)
								.IsEnabled(this, &SManaMaterialFactoryDialog::IsHWDecodedMovieSupportOptionEnabled)
								.Style(FEditorStyle::Get(), "RadioButton")
								.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioHWDecodedMovieChecked, EManaMaterialFactoryOptionChoices::OptionAuto)
								.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioHWDecodedMovieChanged, EManaMaterialFactoryOptionChoices::OptionAuto)
								.Content()
								[
									SNew(STextBlock)
									.Text(LOCTEXT("ManaOptionHWDecodedAutoLabel", "Automatic"))
								]
								.ToolTipText(LOCTEXT("ManaOptionHWDecodedAutoToolTip", "Determined by Movie source of Sofdec2 Movie Texture."))
							]

							+ SUniformGridPanel::Slot(1, 0)
							[
								SNew(SCheckBox)
								.Style(FEditorStyle::Get(), "RadioButton")
								.IsChecked(this, &SManaMaterialFactoryDialog::IsRadioHWDecodedMovieChecked, EManaMaterialFactoryOptionChoices::OptionTrue)
								.OnCheckStateChanged(this, &SManaMaterialFactoryDialog::OnRadioHWDecodedMovieChanged, EManaMaterialFactoryOptionChoices::OptionTrue)
								.Content()
								[
									SNew(STextBlock)
									.Text(LOCTEXT("ManaOptionHWDecodedAlwaysLabel", "Always"))
								]
								.ToolTipText(LOCTEXT("ManaOptionHWDecodedAlwaysToolTip", "Mana Movie Format (YUV, NV12...) is always checked for H/W decoded movie support on any platfrom."))
							]
						]
					]
				]

				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				.Padding(8)
				[
					SNew(SUniformGridPanel)
					.SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
					.MinDesiredSlotWidth(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
					.MinDesiredSlotHeight(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))

					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
						.OnClicked_Lambda([this]() -> FReply { CloseDialog(true); return FReply::Handled(); })
						.Text(LOCTEXT("OkButtonLabel", "Create"))
					]

					+ SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
						.OnClicked_Lambda([this]() -> FReply { CloseDialog(false); return FReply::Handled(); })
						.Text(LOCTEXT("CancelButtonLabel", "Cancel"))
					]
				]
			]
		];
	}

protected:

	void CloseDialog(bool InOkClicked)
	{
		*Result = InOkClicked;

		if (Window.IsValid())
		{
			Window.Pin()->RequestDestroyWindow();
		}
	}

	ECheckBoxState IsRadioChecked(EManaMaterialFactoryCreateChoices ButtonId) const
	{
		return (Options->CreateChoice == ButtonId)
			? ECheckBoxState::Checked
			: ECheckBoxState::Unchecked;
	}

	void OnRadioChanged(ECheckBoxState NewRadioState, EManaMaterialFactoryCreateChoices RadioThatChanged)
	{
		if (NewRadioState == ECheckBoxState::Checked)
		{
			Options->CreateChoice = RadioThatChanged;
		}
	}

	ECheckBoxState IsRadioAlphaChecked(EManaMaterialFactoryOptionChoices ButtonId) const
	{
		return (Options->AlphaChannelSupport == ButtonId)
			? ECheckBoxState::Checked
			: ECheckBoxState::Unchecked;
	}

	void OnRadioAlphaChanged(ECheckBoxState NewRadioState, EManaMaterialFactoryOptionChoices RadioThatChanged)
	{
		if (NewRadioState == ECheckBoxState::Checked)
		{
			Options->AlphaChannelSupport = RadioThatChanged;
		}
	}

	ECheckBoxState IsRadioAlphaPlusChecked(EManaMaterialFactoryOptionChoices ButtonId) const
	{
		return (Options->AlphaPlusSupport == ButtonId)
			? ECheckBoxState::Checked
			: ECheckBoxState::Unchecked;
	}

	void OnRadioAlphaPlusChanged(ECheckBoxState NewRadioState, EManaMaterialFactoryOptionChoices RadioThatChanged)
	{
		if (NewRadioState == ECheckBoxState::Checked)
		{
			Options->AlphaPlusSupport = RadioThatChanged;
		}
	}

	ECheckBoxState IsRadioHWDecodedMovieChecked(EManaMaterialFactoryOptionChoices ButtonId) const
	{
		return (Options->CreateChoice != EManaMaterialFactoryCreateChoices::CreateRenderToTextureMaterial &&
				Options->HWDecodedMovieSupport == ButtonId) ||
			   (Options->CreateChoice == EManaMaterialFactoryCreateChoices::CreateRenderToTextureMaterial &&
				ButtonId == EManaMaterialFactoryOptionChoices::OptionTrue)
			? ECheckBoxState::Checked
			: ECheckBoxState::Unchecked;
	}

	void OnRadioHWDecodedMovieChanged(ECheckBoxState NewRadioState, EManaMaterialFactoryOptionChoices RadioThatChanged)
	{
		if (NewRadioState == ECheckBoxState::Checked)
		{
			Options->HWDecodedMovieSupport = RadioThatChanged;
		}
	}

	bool IsHWDecodedMovieSupportOptionEnabled() const
	{
		return (Options->CreateChoice != EManaMaterialFactoryCreateChoices::CreateRenderToTextureMaterial);
	}

private:

	FManaMaterialFactoryOptions* Options;
	TWeakPtr<SWindow> Window;
	bool* Result;
};

FAssetTypeActions_ManaTexture::FAssetTypeActions_ManaTexture(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

void FAssetTypeActions_ManaTexture::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	/* ManaTextureアセットの右クリックメニューに「マテリアルを作成」項目を追加 */
	auto Textures = GetTypedWeakObjectPtrs<UManaTexture>(InObjects);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("Texture_CreateMaterial", "Create Material"),
		LOCTEXT("Texture_CreateMaterialTooltip", "Creates a new material using selected Sofdec2 Movie Textures.)"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.Material"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_ManaTexture::ExecuteCreateMaterial, Textures),
			FCanExecuteAction()
		)
	);
}

void FAssetTypeActions_ManaTexture::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Texture = Cast<UTexture>(*ObjIt);
		if (Texture != nullptr)
		{
			ITextureEditorModule* TextureEditorModule = &FModuleManager::LoadModuleChecked<ITextureEditorModule>("TextureEditor");
			TextureEditorModule->CreateTextureEditor(Mode, EditWithinLevelEditor, Texture);
		}
	}
}

void FAssetTypeActions_ManaTexture::ExecuteCreateMaterial(TArray<TWeakObjectPtr<UManaTexture>> Textures)
{
	if (!ConfigureProperties()) {
		return;
	}

	/* AssetTypeActions_Texture.cppを参考 */
	const FString Suffix = TEXT("_Mat");

	IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	TArray<UObject*> ObjectsToSync;

	for (auto TextureIt = Textures.CreateConstIterator(); TextureIt; ++TextureIt) {
		auto Texture = (*TextureIt).Get();
		if (Texture) {
			FString Name;
			FString PackagePath;
			CreateUniqueAssetName(Texture->GetOutermost()->GetName(), Suffix, PackagePath, Name);

			UManaMaterialFactory* Factory = NewObject<UManaMaterialFactory>();
			Factory->InitialTexture = Texture;
			Factory->Options = Options;

			FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
			UObject* NewAsset = AssetToolsModule.Get().CreateAsset(Name, FPackageName::GetLongPackagePath(PackagePath), UMaterial::StaticClass(), Factory);

			if (NewAsset) {
				ObjectsToSync.Add(NewAsset);
			}
		}
	}

	if (ObjectsToSync.Num() > 0) {
		ContentBrowserSingleton.SyncBrowserToAssets(ObjectsToSync);
	}
}

bool FAssetTypeActions_ManaTexture::ConfigureProperties()
{
	Options.CreateChoice = EManaMaterialFactoryCreateChoices::CreateComponentMaterial;
	Options.AlphaChannelSupport = EManaMaterialFactoryOptionChoices::OptionAuto;
	Options.AlphaPlusSupport = EManaMaterialFactoryOptionChoices::OptionFalse;
	Options.HWDecodedMovieSupport = EManaMaterialFactoryOptionChoices::OptionAuto;
	
	bool DialogResult = false;
	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("CreateManaMaterialDialogTitle", "Create Material for Sofdec2 Movie Texture"))
		.ClientSize(FVector2D(450, 200))
		.SupportsMinimize(false)
		.SupportsMaximize(false)
		.SizingRule(ESizingRule::FixedSize);

	Window->SetContent(SNew(SManaMaterialFactoryDialog, Options, Window, DialogResult));
	GEditor->EditorAddModalWindow(Window);

	return DialogResult;
}

#undef LOCTEXT_NAMESPACE

/* --- end of file --- */
