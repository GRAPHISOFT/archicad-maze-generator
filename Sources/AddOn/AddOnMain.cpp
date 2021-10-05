#include "APIEnvir.h"
#include "ACAPinc.h"

#include "ResourceIds.hpp"
#include "MazeGenerator.hpp"
#include "MazeSettings.hpp"
#include "MazeSettingsDialog.hpp"

#include "MigrationUtils.hpp"

static const GSResID AddOnInfoID			= ID_ADDON_INFO;
	static const Int32 AddOnNameID			= 1;
	static const Int32 AddOnDescriptionID	= 2;

static const short AddOnMenuID				= ID_ADDON_MENU;
	static const Int32 AddOnCommandID		= 1;

static const GSResID AddOnStringsID			= ID_ADDON_STRINGS;
	static const Int32 UndoStringID			= 1;

static const Int32 PreferencesVersion		= 1;

static void GenerateMazeWallGeometries (int rowCount, int colCount, double cellSize, std::vector<MG::WallGeometry>& mazeWalls)
{
	MG::MazeGenerator generator (rowCount, colCount);
	if (!generator.Generate ()) {
		return;
	}

	const MG::Maze& maze = generator.GetMaze ();
	mazeWalls = maze.GetWallGeometries (cellSize);
}

static GSErrCode CreateWallElement (double begX, double begY, double endX, double endY, API_Guid& placedWallGuid)
{
	GSErrCode err = NoError;

	API_Element wallElement = {};
	SetAPIElementType (wallElement, API_WallID);
	err = ACAPI_Element_GetDefaults (&wallElement, nullptr);
	if (err != NoError) {
		return err;
	}

	wallElement.wall.begC = { begX, begY };
	wallElement.wall.endC = { endX, endY };
	wallElement.wall.referenceLineLocation = APIWallRefLine_Center;
	err = ACAPI_Element_Create (&wallElement, nullptr);
	if (err != NoError) {
		return err;
	}

	placedWallGuid = wallElement.header.guid;
	return NoError;
}

static GSErrCode CreateSlabElement (double begX, double begY, double endX, double endY, API_Guid& placedSlabGuid)
{
	GSErrCode err = NoError;

	API_Element slabElement = {};
	SetAPIElementType (slabElement, API_SlabID);
	err = ACAPI_Element_GetDefaults (&slabElement, nullptr);
	if (err != NoError) {
		return err;
	}

	slabElement.slab.poly.nCoords = 5;
	slabElement.slab.poly.nSubPolys = 1;

	API_ElementMemo	slabMemo = {};

	slabMemo.coords = (API_Coord**) BMhAllClear ((slabElement.slab.poly.nCoords + 1) * sizeof (API_Coord));
	(*slabMemo.coords)[1] = { begX, begY };
	(*slabMemo.coords)[2] = { endX, begY };
	(*slabMemo.coords)[3] = { endX, endY };
	(*slabMemo.coords)[4] = { begX, endY };
	(*slabMemo.coords)[5] = (*slabMemo.coords)[1];

	slabMemo.pends = (Int32**) BMhAllClear ((slabElement.slab.poly.nSubPolys + 1) * sizeof (Int32));
	(*slabMemo.pends)[1] = slabElement.slab.poly.nCoords;

	err = ACAPI_Element_Create (&slabElement, &slabMemo);
	ACAPI_DisposeElemMemoHdls (&slabMemo);
	if (err != NoError) {
		return err;
	}

	placedSlabGuid = slabElement.header.guid;
	return NoError;
}

static bool LoadMazeSettingsFromPreferences (MazeSettings& mazeSettings)
{
	GSErrCode err = NoError;

	Int32 version = 0;
	GSSize bytes = 0;
	err = ACAPI_GetPreferences (&version, &bytes, nullptr);
	if (err != NoError || version == 0 || bytes == 0) {
		return false;
	}

	char* data = new char[bytes];
	err = ACAPI_GetPreferences (&version, &bytes, data);
	if (err != NoError) {
		delete[] data;
		return false;
	}

	MazeSettings tempMazeSettings;
	MemoryIChannel inputChannel (data, bytes);
	err = tempMazeSettings.Read (inputChannel);
	if (err != NoError) {
		delete[] data;
		return false;
	}

	mazeSettings = tempMazeSettings;

	delete[] data;
	return true;
}

static bool WriteMazeSettingsToPreferences (const MazeSettings& mazeSettings)
{
	GSErrCode err = NoError;

	MemoryOChannel outputChannel;
	err = mazeSettings.Write (outputChannel);
	if (err != NoError) {
		return false;
	}

	UInt64 bytes = outputChannel.GetDataSize ();
	const char* data = outputChannel.GetDestination ();

	err = ACAPI_SetPreferences (PreferencesVersion, (GSSize) bytes, data);
	if (err != NoError) {
		return false;
	}

	return true;
}

static bool GetMazeSettingsFromDialog (MazeSettings& mazeSettings)
{
	MazeSettings initialMazeSettings (10, 20, 1.0, true, true);
	LoadMazeSettingsFromPreferences (initialMazeSettings);

	MazeSettingsDialog mazeSettingsDialog (initialMazeSettings);
	if (mazeSettingsDialog.Invoke ()) {
		mazeSettings = mazeSettingsDialog.GetMazeSettings ();
		WriteMazeSettingsToPreferences (mazeSettings);
		return true;
	} else {
		return false;
	}
}

static void GenerateMaze ()
{
	MazeSettings mazeSettings;
	if (!GetMazeSettingsFromDialog (mazeSettings)) {
		return;
	}

	std::vector<MG::WallGeometry> mazeWalls;
	GenerateMazeWallGeometries (mazeSettings.rowCount, mazeSettings.columnCount, mazeSettings.cellSize, mazeWalls);

	static const double SlabPadding = 2.0;
	double slabBegX = -SlabPadding;
	double slabBegY = -SlabPadding;
	double slabEndX = mazeSettings.cellSize * mazeSettings.columnCount + SlabPadding;
	double slabEndY = mazeSettings.cellSize * mazeSettings.rowCount + SlabPadding;

	GS::UniString undoString = RSGetIndString (AddOnStringsID, UndoStringID, ACAPI_GetOwnResModule ());
	ACAPI_CallUndoableCommand (undoString, [&] () -> GSErrCode {
		GS::Array<API_Guid> placedElementGuids;
		GSErrCode err = NoError;
		for (const MG::WallGeometry& mazeWall : mazeWalls) {
			API_Guid placedWallGuid;
			err = CreateWallElement (mazeWall.begX, mazeWall.begY, mazeWall.endX, mazeWall.endY, placedWallGuid);
			if (err != NoError) {
				return APIERR_CANCEL;
			}
			placedElementGuids.Push (placedWallGuid);
		}
		if (mazeSettings.createSlab) {
			API_Guid placedSlabGuid;
			err = CreateSlabElement (slabBegX, slabBegY, slabEndX, slabEndY, placedSlabGuid);
			if (err != NoError) {
				return APIERR_CANCEL;
			}
			placedElementGuids.Push (placedSlabGuid);
		}
		if (mazeSettings.createGroup) {
			err = ACAPI_ElementGroup_Create (placedElementGuids);
			if (err != NoError) {
				return APIERR_CANCEL;
			}
		}
		return NoError;
	});
}

static GSErrCode MenuCommandHandler (const API_MenuParams *menuParams)
{
	switch (menuParams->menuItemRef.menuResID) {
		case AddOnMenuID:
			switch (menuParams->menuItemRef.itemIndex) {
				case AddOnCommandID:
					GenerateMaze ();
					break;
			}
			break;
	}
	return NoError;
}

API_AddonType __ACDLL_CALL CheckEnvironment (API_EnvirParams* envir)
{
	RSGetIndString (&envir->addOnInfo.name, AddOnInfoID, AddOnNameID, ACAPI_GetOwnResModule ());
	RSGetIndString (&envir->addOnInfo.description, AddOnInfoID, AddOnDescriptionID, ACAPI_GetOwnResModule ());

	return APIAddon_Normal;
}

GSErrCode __ACDLL_CALL RegisterInterface (void)
{
	return ACAPI_Register_Menu (AddOnMenuID, 0, MenuCode_Extras, MenuFlag_Default);
}

GSErrCode __ACENV_CALL Initialize (void)
{
	return ACAPI_Install_MenuHandler (AddOnMenuID, MenuCommandHandler);
}

GSErrCode __ACENV_CALL FreeData (void)
{
	return NoError;
}
