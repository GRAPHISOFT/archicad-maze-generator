#ifndef MAZESETTINGS_HPP
#define MAZESETTINGS_HPP

#include "Object.hpp"

class MazeSettings : public GS::Object
{
	DECLARE_CLASS_INFO;

public:
	MazeSettings ();
	MazeSettings (UInt32 rowCount, UInt32 columnCount, double cellSize, bool createGroup, bool createSlab);

	virtual	GSErrCode	Read (GS::IChannel& ic) override;
	virtual	GSErrCode	Write (GS::OChannel& oc) const override;

	UInt32	rowCount;
	UInt32	columnCount;
	double	cellSize;
	bool	createGroup;
	bool	createSlab;
};

#endif
