#ifndef MAZESETTINGS_HPP
#define MAZESETTINGS_HPP

#include "Object.hpp"

class MazeSettings
{
public:
	MazeSettings ();
	MazeSettings (UInt32 rowCount, UInt32 columnCount, double cellSize, bool createGroup, bool createSlab);

	UInt32	rowCount;
	UInt32	columnCount;
	double	cellSize;
	bool	createGroup;
	bool	createSlab;
};

#endif
