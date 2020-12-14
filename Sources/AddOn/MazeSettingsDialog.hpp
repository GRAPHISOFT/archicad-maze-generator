#ifndef MAZESETTINGSDIALOG_HPP
#define MAZESETTINGSDIALOG_HPP

#include "DGModule.hpp"
#include "MazeSettings.hpp"

class MazeSettingsDialog :	public DG::ModalDialog,
							public DG::PanelObserver,
							public DG::ButtonItemObserver,
							public DG::CompoundItemObserver
{
public:
	MazeSettingsDialog (const MazeSettings& mazeSettings);
	~MazeSettingsDialog ();

	const MazeSettings&		GetMazeSettings () const;

private:
	virtual void	PanelOpened (const DG::PanelOpenEvent& ev) override;
	virtual	void	PanelCloseRequested (const DG::PanelCloseRequestEvent& ev, bool* accepted) override;
	virtual void	ButtonClicked (const DG::ButtonClickEvent& ev) override;

	DG::Button		okButton;
	DG::Button		cancelButton;
	DG::PosIntEdit	rowEdit;
	DG::PosIntEdit	columnEdit;
	DG::LengthEdit	cellSizeEdit;
	DG::CheckBox	groupElementsCheck;
	DG::CheckBox	placeSlabCheck;

	MazeSettings	mazeSettings;
};

#endif
