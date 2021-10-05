#include "MigrationUtils.hpp"

void SetAPIElementType (API_Element& element, API_ElemTypeID elemTypeId)
{
#ifdef ServerMainVers_2600
	element.header.type = API_ElemType (elemTypeId);
#else
	element.header.typeID = elemTypeId;
#endif
}
