#include "MazeSettings.hpp"

GS::ClassInfo MazeSettings::classInfo ("MazeSettings", GS::Guid ("B45089A9-B372-460B-B145-80E6EBF107C3"), GS::ClassVersion (1, 0));

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

GSErrCode MazeSettings::Read (GS::IChannel& ic)
{
	GS::InputFrame frame (ic, classInfo);
	ic.Read (rowCount);
	ic.Read (columnCount);
	ic.Read (cellSize);
	ic.Read (createGroup);
	ic.Read (createSlab);
	return ic.GetInputStatus ();
}

GSErrCode MazeSettings::Write (GS::OChannel& oc) const
{
	GS::OutputFrame frame (oc, classInfo);
	oc.Write (rowCount);
	oc.Write (columnCount);
	oc.Write (cellSize);
	oc.Write (createGroup);
	oc.Write (createSlab);
	return oc.GetOutputStatus ();
}
