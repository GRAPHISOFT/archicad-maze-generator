#ifndef MIGRATIONUTILS_HPP
#define MIGRATIONUTILS_HPP

#include "ACAPinc.h"

#include "MemoryIChannel.hpp"
#include "MemoryOChannel.hpp"

#if defined (ServerMainVers_2500)
	using MemoryIChannel = GS::MemoryIChannel;
	using MemoryOChannel = GS::MemoryOChannel;
#else
	using MemoryIChannel = IO::MemoryIChannel;
	using MemoryOChannel = IO::MemoryOChannel;
#endif

void SetAPIElementType (API_Element& element, API_ElemTypeID elemTypeId);
GSErrCode Register_Menu (short menuStrResID, short promptStrResID, APIMenuCodeID menuPosCode, GSFlags menuFlags);
GSErrCode Install_MenuHandler (short menuStrResID, APIMenuCommandProc* handlerProc);
GSErrCode ElementGroup_Create (const GS::Array<API_Guid>& elemGuids, API_Guid* groupGuid = nullptr, const API_Guid* parentGroupGuid = nullptr);

#endif
