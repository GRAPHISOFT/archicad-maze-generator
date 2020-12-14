#include "MazeSettings.hpp"

MazeSettings::MazeSettings () :
	MazeSettings (0, 0, 0.0, false, false)
{

}

MazeSettings::MazeSettings (UInt32 rowCount, UInt32 columnCount, double cellSize, bool createGroup, bool createSlab) :
	rowCount (rowCount),
	columnCount (columnCount),
	cellSize (cellSize),
	createGroup (createGroup),
	createSlab (createSlab)
{

}
