//#include "StdAfx.h"
#include "PipeFeature.h"


bool CPipeLineFeature::Serialize(GEOPIPEMODEL::PIPE* pPBPipe)
{
	m_nFeatureID = pPBPipe->featureid();
	m_nSymbolID = pPBPipe->symbolid();
	m_eCrossSecType = (enumCrossSectionType)pPBPipe->scetiontype();

	m_dbDiameter = pPBPipe->caliber();
	m_dbHeight = pPBPipe->height();
	m_dbWidth = pPBPipe->width();

	m_ptStartGeo.x()= pPBPipe->coordinates(0);
	m_ptStartGeo.y()= pPBPipe->coordinates(1);
	m_ptStartGeo.z()= pPBPipe->coordinates(2);
	m_ptEndGeo.x() = pPBPipe->coordinates(3);
	m_ptEndGeo.y() = pPBPipe->coordinates(4);
	m_ptEndGeo.z() = pPBPipe->coordinates(5);

	m_nStartID = pPBPipe->startpointid();
	m_nEndID = pPBPipe->endpointid();

	return true;
}

bool CPipePointFeature::Serialize(GEOPIPEMODEL::PIPE* pPBPipe)
{
	m_nFeatureID = pPBPipe->featureid();
	m_nSymbolID = pPBPipe->symbolid();
	m_ptGeo.x() = pPBPipe->coordinates(0);
	m_ptGeo.y() = pPBPipe->coordinates(1);
	m_ptGeo.z() = pPBPipe->coordinates(2);

	m_dbRotateX = pPBPipe->attitudes(0);
	m_dbRotateY = pPBPipe->attitudes(1);
	m_dbRotateZ = pPBPipe->attitudes(2);
	m_dbScaleX = pPBPipe->attitudes(3);
	m_dbScaleY = pPBPipe->attitudes(4);
	m_dbScaleZ = pPBPipe->attitudes(5);

	return true;
}
