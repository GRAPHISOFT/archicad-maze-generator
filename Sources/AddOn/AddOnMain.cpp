#include "APIEnvir.h"
#include "ACAPinc.h"

#include "ResourceIds.hpp"
#include "MazeGenerator.hpp"

static const GSResID AddOnInfoID			= ID_ADDON_INFO;
	static const Int32 AddOnNameID			= 1;
	static const Int32 AddOnDescriptionID	= 2;

static const short AddOnMenuID				= ID_ADDON_MENU;
	static const Int32 AddOnCommandID		= 1;

static const GSResID AddOnStringsID			= ID_ADDON_STRINGS;
	static const Int32 UndoStringID			= 1;

static void GenerateMazeWallGeometries (int rowCount, int colCount, double cellSize, std::vector<MG::WallGeometry>& mazeWalls)
{
	MG::MazeGenerator generator (rowCount, colCount);
	if (!generator.Generate ()) {
		return;
	}

	const MG::Maze& maze = generator.GetMaze ();
	mazeWalls = maze.GetWallGeometries (cellSize);
}

static GSErrCode CreateWallElement (double begX, double begY, double endX, double endY)
{
	GSErrCode err = NoError;

	API_Element wallElement = {};
	wallElement.header.typeID = API_WallID;
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

	return NoError;
}

static GSErrCode CreateSlabElement (double begX, double begY, double endX, double endY)
{
	GSErrCode err = NoError;

	API_Element slabElement = {};
	slabElement.header.typeID = API_SlabID;
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

	return NoError;
}

static void GenerateMaze ()
{
	static const int MazeRowCount = 10;
	static const int MazeColCount = 20;
	static const double MazeCellSize = 2.0;
	static const double SlabPadding = 2.0;

	std::vector<MG::WallGeometry> mazeWalls;
	GenerateMazeWallGeometries (MazeRowCount, MazeColCount, MazeCellSize, mazeWalls);

	double slabBegX = -SlabPadding;
	double slabBegY = -SlabPadding;
	double slabEndX = MazeCellSize * MazeColCount + SlabPadding;
	double slabEndY = MazeCellSize * MazeRowCount + SlabPadding;

	GS::UniString undoString = RSGetIndString (AddOnStringsID, UndoStringID, ACAPI_GetOwnResModule ());
	ACAPI_CallUndoableCommand (undoString, [&] () -> GSErrCode {
		GSErrCode err = NoError;
		for (const MG::WallGeometry& mazeWall : mazeWalls) {
			err = CreateWallElement (mazeWall.begX, mazeWall.begY, mazeWall.endX, mazeWall.endY);
			if (err != NoError) {
				return APIERR_CANCEL;
			}
		}
		err = CreateSlabElement (slabBegX, slabBegY, slabEndX, slabEndY);
		if (err != NoError) {
			return APIERR_CANCEL;
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
