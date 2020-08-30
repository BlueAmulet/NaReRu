#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"

struct FAtomCueSheetFactoryOptions;
struct FAtomCueListItem;

namespace EAtomCueSortMode
{
	enum Type
	{
		ByID,
		ByName
	};
}

class SAtomCueSheetFactoryDialog : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAtomCueSheetFactoryDialog) { }
	SLATE_END_ARGS()

	/** Construct this widget. */
	void Construct(const FArguments& InArgs, FAtomCueSheetFactoryOptions& InOptions, TSharedRef<SWindow> InWindow);

	/**
	 * Create and return a widget for the given item and column ID
	 *
	 * @param	Item		The item being queried
	 * @param	ColumnID	The column ID being queried
	 *
	 * @return	The widget which was created
	 */
	TSharedRef<SWidget> GenerateWidgetForItemAndColumn( TSharedPtr<FAtomCueListItem> Item, const FName& ColumnID ) const;

protected:
	/** Callback for generating a row widget for the dispatch state list view. */
	TSharedRef<ITableRow> OnGenerateWidgetForList(TSharedPtr<FAtomCueListItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	TArray< TSharedPtr<FAtomCueListItem> > GetSelectedItems(bool bAllIfNone) const;

	ECheckBoxState GetToggleSelectedState() const;

	FReply OnClickCreateNewButton();
	FReply OnClickDontCreateNewButton();
	FReply OnClickDeleteExistButton();
	FReply OnClickDontDeleteExistButton();

	void OnToggleSelectedCheckBox(ECheckBoxState State);
	
	EColumnSortMode::Type GetNameSortMode() const;
	EColumnSortMode::Type GetIDSortMode() const;
	
	void OnSortByChanged(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode);
	void RebuildItemList();
	TSharedPtr<SWidget> GetListContextMenu();
	
	void CloseDialog(bool InOkClicked);

public:
	/** Current way we are sorting cues */
	EAtomCueSortMode::Type	SortBy;
	/** Current way we are setting sort direction */
	EColumnSortMode::Type	SortDirection;

	static const FName ColumnID_CheckBox;
	static const FName ColumnID_Name;
	static const FName ColumnID_ID;
	static const FName ColumnID_Status;
	static const FName ColumnID_Action;

private:
	
	FAtomCueSheetFactoryOptions* Options;
	TWeakPtr<SWindow> Window;
	
	/** The list view for showing all checkboxes */
	TSharedPtr<SListView<TSharedPtr<FAtomCueListItem>>> CueListView;
	
	/** A Checkbox used to toggle multiple packages. */
	TSharedPtr<SCheckBox> ToggleSelectedCheckBox;
};

/** Widget that represents a row in the AtomCueSheetImportDialog's list view.  Generates widgets for each column on demand. */
class SACSFCueListTableRow : public SMultiColumnTableRow< TSharedPtr<FAtomCueListItem> >
{
public:

	SLATE_BEGIN_ARGS(SACSFCueListTableRow) {}

		/** The AtomCueSheetImportDialog that owns the tree.  We'll only keep a weak reference to it. */
		SLATE_ARGUMENT(TSharedPtr<SAtomCueSheetFactoryDialog>, ImportDialog)

		/** The list item for this row */
		SLATE_ARGUMENT(TSharedPtr<FAtomCueListItem>, Item)

	SLATE_END_ARGS()


	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	/** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the list row. */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:

	/** Weak reference to the AtomCueSheetImportDialog widget that owns our list */
	TWeakPtr<SAtomCueSheetFactoryDialog> ImportDialogWeak;

	/** The item associated with this row of data */
	TSharedPtr<FAtomCueListItem> Item;
};

