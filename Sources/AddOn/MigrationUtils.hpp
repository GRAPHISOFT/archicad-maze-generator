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

#endif
