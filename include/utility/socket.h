#pragma once
#include "preconfig.h"   
#include "object.h"
UTILITY_NS

/// \brief Socket环境
class GS_API GsSocketEnvironment:public GsRefObject
{
public:
	GsSocketEnvironment();
	
	~GsSocketEnvironment();

};
/// \brief GsSocketEnvironmentPtr
GS_SMARTER_PTR(GsSocketEnvironment);

UTILITY_ENDNS
