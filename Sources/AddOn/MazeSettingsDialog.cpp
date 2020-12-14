#include "MazeSettingsDialog.hpp"
#include "ResourceIds.hpp"
#include "ACAPinc.h"

enum DialogResourceIds
{
	MazeDialogResourceId = ID_ADDON_DLG,
	OKButtonId = 1,
	CancelButtonId = 2,
	DrawingId = 3,
	GridSettingsTextId = 4,
	RowTextId = 5,
	RowEditId = 6,
	ColumnTextId = 7,
	ColumnEditId = 8,
	CellSizeTextId = 9,
	CellSizeEditId = 10,
	Separator1Id = 11,
	OptionsTextId = 12,
	GroupElementsCheckId = 13,
	PlaceSlabCheckId = 14,
	Separator2Id = 15
};

MazeSettingsDialog::MazeSettingsDialog (const MazeSettings& mazeSettings) :
	DG::ModalDialog (ACAPI_GetOwnResModule (), MazeDialogResourceId, ACAPI_GetOwnResModule ()),
	okButton (GetReference (), OKButtonId),
	cancelButton (GetReference (), CancelButtonId),
	rowEdit (GetReference (), RowEditId),
	columnEdit (GetReference (), ColumnEditId),
	cellSizeEdit (GetReference (), CellSizeEditId),
	groupElementsCheck (GetReference (), GroupElementsCheckId),
	placeSlabCheck (GetReference (), PlaceSlabCheckId),
	mazeSettings (mazeSettings)
{
	AttachToAllItems (*this);
	Attach (*this);
}

MazeSettingsDialog::~MazeSettingsDialog ()
{
	Detach (*this);
	DetachFromAllItems (*this);
}

const MazeSettings& MazeSettingsDialog::GetMazeSettings () const
{
	return mazeSettings;
}

void MazeSettingsDialog::PanelOpened (const DG::PanelOpenEvent&)
{
	rowEdit.SetValue (mazeSettings.rowCount);
	columnEdit.SetValue (mazeSettings.columnCount);
	cellSizeEdit.SetValue (mazeSettings.cellSize);
	groupElementsCheck.SetState (mazeSettings.createGroup);
	placeSlabCheck.SetState (mazeSettings.createSlab);
}

void MazeSettingsDialog::PanelCloseRequested (const DG::PanelCloseRequestEvent& ev, bool*)
{
	if (ev.IsAccepted ()) {
		mazeSettings.rowCount = rowEdit.GetValue ();
		mazeSettings.columnCount = columnEdit.GetValue ();
		mazeSettings.cellSize = cellSizeEdit.GetValue ();
		mazeSettings.createGroup = groupElementsCheck.IsChecked ();
		mazeSettings.createSlab = placeSlabCheck.IsChecked ();
	}
}

void MazeSettingsDialog::ButtonClicked (const DG::ButtonClickEvent& ev)
{
	if (ev.GetSource () == &okButton) {
		PostCloseRequest (DG::ModalDialog::Accept);
	} else if (ev.GetSource () == &cancelButton) {
		PostCloseRequest (DG::ModalDialog::Cancel);
	}
}
