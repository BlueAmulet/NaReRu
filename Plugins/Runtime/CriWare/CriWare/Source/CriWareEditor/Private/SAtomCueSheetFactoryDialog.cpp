#include "SAtomCueSheetFactoryDialog.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/SlateTypes.h"
#include "EditorStyleSet.h"

#include "SoundAtomCueSheetFactory.h"

#define LOCTEXT_NAMESPACE "SoundAtomCueSheetFactory"

const FName SAtomCueSheetFactoryDialog::ColumnID_CheckBox(TEXT("Check"));
const FName SAtomCueSheetFactoryDialog::ColumnID_Name(TEXT("Name"));
const FName SAtomCueSheetFactoryDialog::ColumnID_ID(TEXT("ID"));
const FName SAtomCueSheetFactoryDialog::ColumnID_Status(TEXT("Status"));
const FName SAtomCueSheetFactoryDialog::ColumnID_Action(TEXT("Action"));

/** Construct this widget. */
void SAtomCueSheetFactoryDialog::Construct(const FArguments& InArgs, FAtomCueSheetFactoryOptions& InOptions, TSharedRef<SWindow> InWindow)
{
	Options = &InOptions;
	Window = InWindow;

	SortBy = EAtomCueSortMode::ByID;
	SortDirection = EColumnSortMode::Descending;

	ChildSlot
	[
		SNew(SBorder)
		.Visibility(EVisibility::Visible)
		.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(0.8)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(4.0f)
				.Content()
				[
					SNew(SVerticalBox)
	// Label				
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ImportCueSheetLabel", "Adx2 ACB file Importation options:"))
					]
	// Options
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 6.0f, 0.0f, 0.0f)
					[
						SNew(SCheckBox)
						.IsChecked_Lambda([this] { return Options->EnableEconomicTick ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
						.OnCheckStateChanged_Lambda([this](ECheckBoxState CheckState) { Options->EnableEconomicTick = CheckState == ECheckBoxState::Checked; })
						[
							SNew(STextBlock)
							.Text(LOCTEXT("ImportCueSheetVariableTickLabel", "Enable Economic Tick"))
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 6.0f, 0.0f, 0.0f)
					[
						SNew(SCheckBox)
						.IsChecked_Lambda([this] { return Options->EnableDistanceCulling ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
						.OnCheckStateChanged_Lambda([this](ECheckBoxState CheckState) { Options->EnableDistanceCulling = CheckState == ECheckBoxState::Checked; })
						[
							SNew(STextBlock)
							.Text(LOCTEXT("ImportCueSheetDistanceCullingLabel", "Enable Distance Culling"))
						]
					]
	// Cue ListView
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 12.0f, 0.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ImportCueSheetCueListLabel", "Select cues to link with a SoundAtomCue asset."))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(20.0f, 6.0f, 0.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("PreselectionCueListLabel", "Preselectors:"))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(20.0f, 6.0f, 0.0f, 0.0f)
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
							.OnClicked(this, &SAtomCueSheetFactoryDialog::OnClickCreateNewButton)
							.Text(LOCTEXT("ImportAllCueButtonLabel", "Create New"))
						]
						+ SUniformGridPanel::Slot(1, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
							.OnClicked(this, &SAtomCueSheetFactoryDialog::OnClickDontCreateNewButton)
							.Text(LOCTEXT("DontCreateCueButtonLabel", "Don't Create New"))
						]
						+ SUniformGridPanel::Slot(2, 0)
						[
							SNew(SButton)
							.HAlign(HAlign_Center)
							.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
							.OnClicked(this, &SAtomCueSheetFactoryDialog::OnClickDeleteExistButton)
							.Text(LOCTEXT("DeleteAllCueButtonLabel", "Delete Exist"))
						]
						+ SUniformGridPanel::Slot(3, 0)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
							.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
							.OnClicked(this, &SAtomCueSheetFactoryDialog::OnClickDontDeleteExistButton)
							.Text(LOCTEXT("DontDeleteCueButtonLabel", "Don't Delete Exist"))
							]
					]
					+ SVerticalBox::Slot()
					.FillHeight(0.8)
					.Padding(0.0f, 10.0f, 0.0f, 0.0f)
					[
						SNew(SBorder)
						.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
						.Padding(1.0)
						[
							SAssignNew(CueListView, SListView< TSharedPtr<FAtomCueListItem> > )
							.ItemHeight(20.0f)
							.ListItemsSource(&Options->Items)
							.SelectionMode(ESelectionMode::None)
							.OnGenerateRow(this, &SAtomCueSheetFactoryDialog::OnGenerateWidgetForList)
							.OnContextMenuOpening(this, &SAtomCueSheetFactoryDialog::GetListContextMenu)
							.HeaderRow
							(
								SNew(SHeaderRow)
								// Check
								+ SHeaderRow::Column(ColumnID_CheckBox)
								.HAlignCell(HAlign_Left)
								.FixedWidth(23)
								[
									SAssignNew(ToggleSelectedCheckBox, SCheckBox)
									.IsChecked(this, &SAtomCueSheetFactoryDialog::GetToggleSelectedState)
									.OnCheckStateChanged(this, &SAtomCueSheetFactoryDialog::OnToggleSelectedCheckBox)
								]

								// Cue Name
								+ SHeaderRow::Column(ColumnID_Name)
								.SortMode(this, &SAtomCueSheetFactoryDialog::GetNameSortMode)
								.OnSort(this, &SAtomCueSheetFactoryDialog::OnSortByChanged)
								.FillWidth(1.5)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("CueNameCueListHeader", "Name"))
									.ToolTipText(LOCTEXT("CueNameCueListHeaderTip", "Name of the Cue."))
								]
								// Cue ID
								+ SHeaderRow::Column(ColumnID_ID)
								.SortMode(this, &SAtomCueSheetFactoryDialog::GetIDSortMode)
								.OnSort(this, &SAtomCueSheetFactoryDialog::OnSortByChanged)
								.FillWidth(0.5)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("CueIDCueListHeader", "ID"))
									.ToolTipText(LOCTEXT("CueIDCueListHeaderTip", "ID of the Cue."))
								]
								// Cue Status
								+ SHeaderRow::Column(ColumnID_Status)
								.FixedWidth(50)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("CueStatusCueListHeader", "Status"))
									.ToolTipText(LOCTEXT("CueStatusCueListHeaderTip", "Cue importation status:"
														"\n'New': This cue is new and is not used by any SoundAtomCue asset yet."
														"\n'Exist': This cue is already referenced by a SoundAtomCue asset."
														"\n'Deleted': This cue does not exist anymore but is referenced by a SoundAtomCue asset."))
								]
								// Cue Action
								+ SHeaderRow::Column(ColumnID_Action)
								.FixedWidth(50)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("CueSActionCueListHeader", "Action"))
									.ToolTipText(LOCTEXT("CueActionCueListHeaderTip", "Action performed when importing a cue:"
														"\n'Create': Creates a new SoundAtomCue asset."
														"\n'Link': Links cue information with existing SoundAtomCue asset."
														"\n'Delete': Deletes an existing SoundAtomCue asset."))
								]
							)
						]
					]
				]
			]

			+ SVerticalBox::Slot()
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
					.Text(LOCTEXT("OkButtonLabel", "OK"))
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

/** Callback for generating a row widget for the dispatch state list view. */
TSharedRef<ITableRow> SAtomCueSheetFactoryDialog::OnGenerateWidgetForList(TSharedPtr<FAtomCueListItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	CueListView->SetItemSelection(InItem, InItem->Selected, ESelectInfo::OnMouseClick);

	return	SNew(SACSFCueListTableRow, OwnerTable)
			.Item(InItem)
			.ImportDialog(SharedThis(this));
}

TSharedRef<SWidget> SAtomCueSheetFactoryDialog::GenerateWidgetForItemAndColumn(TSharedPtr<FAtomCueListItem> Item, const FName& ColumnID) const
{
	check(Item.IsValid());

	const FMargin RowPadding(3, 0, 0, 0);

	if (ColumnID == SAtomCueSheetFactoryDialog::ColumnID_CheckBox)
	{
		return	SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(RowPadding)
				[
					SNew(SCheckBox)
					.IsChecked_Lambda([Item] { return Item->Selected ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
					.OnCheckStateChanged_Lambda([Item](ECheckBoxState CheckState) { Item->Selected = CheckState == ECheckBoxState::Checked; })
				];
	}
	else if (ColumnID == SAtomCueSheetFactoryDialog::ColumnID_Name)
	{
		auto TextBlock = SNew(STextBlock)
						.Text(Item->CueName)
						.ToolTipText(Item->ToDisplayString());

		TextBlock->SetOnMouseButtonUp(FPointerEventHandler::CreateLambda([Item](const FGeometry&, const FPointerEvent& MouseEvent) -> FReply {
			if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
				Item->Selected = !Item->Selected;
				return FReply::Handled();
			}
			return FReply::Unhandled();
		}));

		return	SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(RowPadding)
				[
					TextBlock
				];
	}
	else if (ColumnID == SAtomCueSheetFactoryDialog::ColumnID_ID)
	{
		return	SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(RowPadding)
				[
					SNew(STextBlock)
					.Text(FText::AsNumber(Item->CueID))
					.ToolTipText(Item->ToDisplayString())
				];
	}
	else if (ColumnID == SAtomCueSheetFactoryDialog::ColumnID_Status)
	{
		FText StatusText;
		FColor StatusColor;
		Item->GetStatusTextAndColor(StatusText, StatusColor);
					
		return	SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(RowPadding)
				[
					SNew(STextBlock)
					.Text(StatusText)
					.ColorAndOpacity(StatusColor)
					.ToolTipText(Item->ToDisplayString())
				];
	}
	else if (ColumnID == SAtomCueSheetFactoryDialog::ColumnID_Action)
	{
		return	SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(RowPadding)
				[
					SNew(STextBlock)
					.Text_Lambda([Item] { FText ActionText; FColor ActionColor; Item->GetActionTextAndColor(ActionText, ActionColor); return ActionText; })
					.ColorAndOpacity_Lambda([Item] { FText ActionText; FColor ActionColor; Item->GetActionTextAndColor(ActionText, ActionColor); return FSlateColor(ActionColor); })
				];
	}

	return SNullWidget::NullWidget;
}

TArray< TSharedPtr<FAtomCueListItem> > SAtomCueSheetFactoryDialog::GetSelectedItems(bool bAllIfNone) const
{
	//get the list of highlighted packages
	TArray< TSharedPtr<FAtomCueListItem> > SelectedItems = CueListView->GetSelectedItems();
	if (SelectedItems.Num() == 0 && bAllIfNone)
	{
		//If no cue are explicitly highlighted, return all cues in the list.
		SelectedItems = Options->Items;
	}

	return SelectedItems;
}

ECheckBoxState SAtomCueSheetFactoryDialog::GetToggleSelectedState() const
{
	// default to a Checked state
	ECheckBoxState PendingState = ECheckBoxState::Checked;

	TArray< TSharedPtr<FAtomCueListItem> > SelectedItems = GetSelectedItems(true);

	// Iterate through the list of selected packages
	for (auto SelectedItem = SelectedItems.CreateConstIterator(); SelectedItem; ++SelectedItem)
	{
		if (SelectedItem->Get()->Selected == false)
		{
			// if any cue in the selection is Unchecked, then represent the entire set of highlighted cues as Unchecked,
			// so that the first (user) toggle of ToggleSelectedCheckBox consistently Checks all highlighted cues
			PendingState = ECheckBoxState::Unchecked;
		}
	}

	return PendingState;
}

void SAtomCueSheetFactoryDialog::OnToggleSelectedCheckBox(ECheckBoxState InNewState)
{
	TArray< TSharedPtr<FAtomCueListItem> > SelectedItems = GetSelectedItems(true);

	for (auto SelectedItem = SelectedItems.CreateConstIterator(); SelectedItem; ++SelectedItem)
	{
		FAtomCueListItem* Item = SelectedItem->Get();
		if (InNewState == ECheckBoxState::Checked)
		{
			/*if (Item->IsDisabled())
			{
				item->SetState(ECheckBoxState::Undetermined);
			}
			else*/
			{
				Item->Selected = true;
			}
		}
		else
		{
			Item->Selected = false;
		}
	}

	CueListView->RequestListRefresh();
}

FReply SAtomCueSheetFactoryDialog::OnClickCreateNewButton()
{
	TArray< TSharedPtr<FAtomCueListItem> > SelectedItems = GetSelectedItems(true);

	for (auto SelectedItem = SelectedItems.CreateConstIterator(); SelectedItem; ++SelectedItem)
	{
		FAtomCueListItem* Item = SelectedItem->Get();

		if (Item->ImportStatus == EAtomCueImportStatus::NewCue) {
			Item->Selected = true;
		}
	}

	CueListView->RequestListRefresh();

	return FReply::Handled();
}

FReply SAtomCueSheetFactoryDialog::OnClickDontCreateNewButton()
{
	TArray< TSharedPtr<FAtomCueListItem> > SelectedItems = GetSelectedItems(true);

	for (auto SelectedItem = SelectedItems.CreateConstIterator(); SelectedItem; ++SelectedItem)
	{
		FAtomCueListItem* Item = SelectedItem->Get();

		if (Item->ImportStatus == EAtomCueImportStatus::NewCue) {
			Item->Selected = false;
		}
	}

	CueListView->RequestListRefresh();

	return FReply::Handled();
}

FReply SAtomCueSheetFactoryDialog::OnClickDeleteExistButton()
{
	TArray< TSharedPtr<FAtomCueListItem> > SelectedItems = GetSelectedItems(true);

	for (auto SelectedItem = SelectedItems.CreateConstIterator(); SelectedItem; ++SelectedItem)
	{
		FAtomCueListItem* Item = SelectedItem->Get();

		if (Item->ImportStatus == EAtomCueImportStatus::ExistCue) {
			Item->Selected = false;
		}
	}

	CueListView->RequestListRefresh();

	return FReply::Handled();
}

FReply SAtomCueSheetFactoryDialog::OnClickDontDeleteExistButton()
{
	TArray< TSharedPtr<FAtomCueListItem> > SelectedItems = GetSelectedItems(true);

	for (auto SelectedItem = SelectedItems.CreateConstIterator(); SelectedItem; ++SelectedItem)
	{
		FAtomCueListItem* Item = SelectedItem->Get();

		if (Item->ImportStatus == EAtomCueImportStatus::ExistCue) {
			Item->Selected = true;
		}
	}

	CueListView->RequestListRefresh();

	return FReply::Handled();
}

EColumnSortMode::Type SAtomCueSheetFactoryDialog::GetNameSortMode() const
{
	return (SortBy == EAtomCueSortMode::ByName) ? SortDirection : EColumnSortMode::None;
}

EColumnSortMode::Type SAtomCueSheetFactoryDialog::GetIDSortMode() const
{
	return (SortBy == EAtomCueSortMode::ByID) ? SortDirection : EColumnSortMode::None;
}
		
void SAtomCueSheetFactoryDialog::OnSortByChanged(const EColumnSortPriority::Type SortPriority, const FName& ColumnID, const EColumnSortMode::Type NewSortMode)
{
	if (ColumnID == ColumnID_Name)
	{
		// If already sorting by Name, flip direction
		if (SortBy == EAtomCueSortMode::ByName)
		{
			SortDirection = (SortDirection == EColumnSortMode::Descending) ? EColumnSortMode::Ascending : EColumnSortMode::Descending;
		}
		// If not, sort by ID, and default to ascending
		else
		{
			SortBy = EAtomCueSortMode::ByName;
			SortDirection = EColumnSortMode::Descending;
		}
	}
	else if (ColumnID == ColumnID_ID)
	{
		// If already sorting by ID, flip direction
		if (SortBy == EAtomCueSortMode::ByID)
		{
			SortDirection = (SortDirection == EColumnSortMode::Descending) ? EColumnSortMode::Ascending : EColumnSortMode::Descending;
		}
		// If not, sort by ID, and default to ascending
		else
		{
			SortBy = EAtomCueSortMode::ByID;
			SortDirection = EColumnSortMode::Descending;
		}
	}
	else if (ColumnID == ColumnID_Status)
	{
		
	}

	RebuildItemList();
}

/** Functor for comparing cue by Name */
struct FCompareAtomCueByName
{
	EColumnSortMode::Type SortMode;

	FCompareAtomCueByName(EColumnSortMode::Type InSortMode)
		: SortMode(InSortMode)
	{}

	FORCEINLINE bool operator()(const TSharedPtr<FAtomCueListItem> A, const TSharedPtr<FAtomCueListItem> B) const
	{
		check(A.IsValid());
		check(B.IsValid());

		bool CompareResult = A->CueName.CompareToCaseIgnored(B->CueName) <= 0;
		return SortMode == EColumnSortMode::Ascending ? CompareResult : !CompareResult;
	}
};

/** Functor for comparing cue by ID */
struct FCompareAtomCueByID
{
	EColumnSortMode::Type SortMode;

	FCompareAtomCueByID(EColumnSortMode::Type InSortMode)
		: SortMode(InSortMode)
	{}

	FORCEINLINE bool operator()(const TSharedPtr<FAtomCueListItem> A, const TSharedPtr<FAtomCueListItem> B) const
	{
		check(A.IsValid());
		check(B.IsValid());

		if (SortMode == EColumnSortMode::Descending)
		{
			return (A->CueID < B->CueID);
		}
		else
		{
			return (A->CueID > B->CueID);
		}
	}
};

void SAtomCueSheetFactoryDialog::RebuildItemList()
{
	// We have built all the lists, now sort (if desired)
	if (SortBy == EAtomCueSortMode::ByName)
	{
		Options->Items.Sort(FCompareAtomCueByName(SortDirection));
	}
	else if (SortBy == EAtomCueSortMode::ByID)
	{
		Options->Items.Sort(FCompareAtomCueByID(SortDirection));
	}

	// When underlying array changes, refresh list
	CueListView->RequestListRefresh();
}

TSharedPtr<SWidget> SAtomCueSheetFactoryDialog::GetListContextMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	//MenuBuilder.AddMenuEntry();
	//MenuBuilder.AddMenuEntry(FLogWidgetCommands::Get().FindLogText);

	return MenuBuilder.MakeWidget();
}

// On Close Dialog
void SAtomCueSheetFactoryDialog::CloseDialog(bool InOkClicked)
{
	Options->OkClicked = InOkClicked;

	// grab selected cue
	if (CueListView->IsEnabled()) {
		TArray<TSharedPtr<FAtomCueListItem>> Selection = CueListView->GetSelectedItems();
		for (auto Item : Selection) {
			Item->Selected = true;
		}
	}

	// close window
	if (Window.IsValid())
	{
		Window.Pin()->RequestDestroyWindow();
	}
}

void SACSFCueListTableRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	ImportDialogWeak = InArgs._ImportDialog;
	Item = InArgs._Item;

	SMultiColumnTableRow< TSharedPtr<FAtomCueListItem> >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SACSFCueListTableRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	// Create the widget for this item
	auto ImportDialogShared = ImportDialogWeak.Pin();
	if (ImportDialogShared.IsValid())
	{
		return ImportDialogShared->GenerateWidgetForItemAndColumn(Item, ColumnName);
	}

	// Packages dialog no longer valid; return a valid, null widget.
	return SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE
