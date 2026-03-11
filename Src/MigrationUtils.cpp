#include "MigrationUtils.hpp"

void SetAPIElementType (API_Element& element, API_ElemTypeID elemTypeId)
{
#ifdef ServerMainVers_2600
	element.header.type = API_ElemType (elemTypeId);
#else
	element.header.typeID = elemTypeId;
#endif
}


GSErrCode Register_Menu (short menuStrResID, short promptStrResID, APIMenuCodeID menuPosCode, GSFlags menuFlags)
{
#if defined(ServerMainVers_2700)
	return ACAPI_MenuItem_RegisterMenu (menuStrResID, promptStrResID, menuPosCode, menuFlags);
#else
	return ACAPI_Register_Menu (menuStrResID, promptStrResID, menuPosCode, menuFlags);
#endif
}


GSErrCode Install_MenuHandler (short menuStrResID, APIMenuCommandProc* handlerProc)
{
#if defined(ServerMainVers_2700)
	return ACAPI_MenuItem_InstallMenuHandler (menuStrResID, handlerProc);
#else
	return ACAPI_Install_MenuHandler (menuStrResID, handlerProc);
#endif
}


GSErrCode ElementGroup_Create (const GS::Array<API_Guid>&	elemGuids, API_Guid* groupGuid, const API_Guid* parentGroupGuid)
{
#if defined(ServerMainVers_2700)
	return ACAPI_Grouping_CreateGroup (elemGuids, groupGuid, parentGroupGuid);
#else
	return ACAPI_ElementGroup_Create (elemGuids, groupGuid, parentGroupGuid);
#endif
}