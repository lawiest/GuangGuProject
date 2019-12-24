
#include "IDConverter.h"
#include <string>



IDConverter::IDConverter(void)
{
}

IDConverter::~IDConverter(void)
{
}

GID IDConverter::FeatureIDToGeometryID(FID featureID, int lodLevel)
{

	if(lodLevel > 6 || lodLevel < 0)
		return 0;

	UINT64 uTemp = ~((UINT64)7 << 56);
	UINT64 uIDAnd  = featureID & uTemp;
	UINT64 uLOD = (UINT64)lodLevel << 56;
	UINT64 uIDOr = uIDAnd | uLOD;
	return uIDOr;

// 	if(lodLevel > 6 || lodLevel < 0)
// 		return 0;
// 
// 	//UINT64 uTemp = 0xFFFFFFFFFFFFFF;
// 	GeoINT64 uIDAnd  = featureID & 0xFFFFFFFFFFFFFF;
// 	GeoINT64 uLOD = lodLevel;
// 	return uIDAnd | uLOD;

//	GeoINT64 lod = (GeoINT64)lodLevel << 56;

//	return featureID | lod;
}

MatID IDConverter::MaterialLodIDToMatID(MatID matLodID, int lodLevel)
{
	UINT64 temp = ~((UINT64)7 << 56);
	matLodID = matLodID & temp;
	UINT64 lod = (UINT64)lodLevel << 56;
	return matLodID | lod;

// 	if(lodLevel > 6 || lodLevel < 0)
// 		return 0;
// 
// 	//__________________________________
// 	GeoINT64 uIDAnd  = matLodID & 0xFFFFFFFFFFFFFF;
// 	GeoINT64 uLOD = lodLevel;
// 	return uIDAnd | uLOD;

	//GeoINT64 lod = (GeoINT64)lodLevel << 56;

	//return matLodID | lod;
}

string IDConverter::ConvertInt64ToString(GID geoID)
{
	GeoINT64 i = (GeoINT64)0x01 << 63;

	char str[65];
	memset(str,0,65);
	int count = 64;
	while (count)
	{
		if (geoID & i)
		{
			strcat(str,"1");
		}
		else
			strcat(str,"0");

		i = i>> 1;
		count --;
	}
	return string(str);
}

int IDConverter::CheckSharedModel(GeoINT64 geoID)
{
	GeoINT64 temp = (GeoINT64)0x01 << 63;

	return ((geoID & temp) > 0) ? 1: 0;

}

int IDConverter::GetLodLevelFromFeatureID(FID featureID)
{
	GeoINT64 lod = (GeoINT64)7 << 56;
	GeoINT64 temp = lod & featureID;
	return temp >> 56;
}

int IDConverter::GetLodLevelFromMaterialLodID(MatID materialID)
{
	GeoINT64 lod = (GeoINT64)7 << 56;
	GeoINT64 temp = lod & materialID;
	return temp >> 56;
}