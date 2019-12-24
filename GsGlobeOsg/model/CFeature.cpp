//********************************************************************************** 
//** �ļ����ƣ�Feature.cpp
//** CopyRight (c) 2000-2012 ��󼪰���Ϣ�������޹�˾����һ��
//** �ļ������� Ҫ����ʵ��
//** �� �� �ˣ� ����
//** �������ڣ� 2012-04-7
//********************************************************************************** 


#include "CFeature.h"



LOD_INFO_BASE::LOD_INFO_BASE():
byRelation(0),
	dSwitchIn(0),
	dSwitchOut(0),
	unDataSize(0),
	unTriangleNum(0),
	unVertexNum(0)
{

}

LOD_INFO_BASE::LOD_INFO_BASE(const LOD_INFO_BASE& lodInfo)
{
	copy(lodInfo);
}

void LOD_INFO_BASE::copy(const LOD_INFO_BASE& lodInfo)
{
	byRelation = lodInfo.byRelation;
	dSwitchIn = lodInfo.dSwitchIn;
	dSwitchOut = lodInfo.dSwitchOut;
	unDataSize = lodInfo.unDataSize;
	unTriangleNum = lodInfo.unTriangleNum;
	unVertexNum = lodInfo.unVertexNum;
}

// LOD_INFO_BASE* LOD_INFO_BASE::clone()
// {
// 	LOD_INFO_BASE* pLodInfo = new LOD_INFO_BASE();
// 	if (pLodInfo != NULL)
// 		pLodInfo->copy(*this);
// 	return pLodInfo;
// }

bool LOD_INFO_BASE::readBuffer(Buffer &buffer)
{
	//����һ����LOD�Ĺ�ϵ
	buffer.read((char*)&byRelation,sizeof(byRelation));
	//modify by yangling 20131208 begin
	if(byRelation > 1)
	{
		return false;
	}
	//modify by yangling 20131208 end
	//���Ӿ���
	buffer.read((char*)&dSwitchIn,sizeof(dSwitchIn));
	buffer.read((char*)&dSwitchOut,sizeof(dSwitchOut));
	//������
	buffer.read((char*)&unDataSize,sizeof(unDataSize));
	//�����θ���
	buffer.read((char*)&unTriangleNum,sizeof(unTriangleNum));
	//�������
	buffer.read((char*)&unVertexNum,sizeof(unVertexNum));
	return true;
}

void LOD_INFO_BASE::writeBuffer(Buffer &buffer)
{
	//����һ����LOD�Ĺ�ϵ
	buffer.write((char*)&byRelation,sizeof(byRelation));
	//���Ӿ���
	buffer.write((char*)&dSwitchIn,sizeof(dSwitchIn));
	buffer.write((char*)&dSwitchOut,sizeof(dSwitchOut));
	//������
	buffer.write((char*)&unDataSize,sizeof(unDataSize));
	//�����θ���
	buffer.write((char*)&unTriangleNum,sizeof(unTriangleNum));
	//�������
	buffer.write((char*)&unVertexNum,sizeof(unVertexNum));

}

size_t LOD_INFO_BASE::sizeBuffer()
{
	return sizeof(byte) + 2*sizeof(double) + 3*sizeof(UINT);
}

bool LOD_INFO::readBuffer(Buffer &buffer)
{
	bool bRet = LOD_INFO_BASE::readBuffer(buffer);

	//��ȡ����ID
	buffer.read((char*)&unGeometryID, sizeof(unGeometryID));
	//modify by yangling 20131208 begin
	return bRet;
	//modify by yangling 20131208 end
	//int nMTCount = 0;
	//buffer.read((char*)&nMTCount,sizeof(int));

	//for (int i=0; i<nMTCount; i++)
	//{
	//	//��ȡ�������ݴ�С
	//	MTData data;
	//	data.readBuffer(buffer);
	//	m_vecMTData.push_back(data);
	//}


}

void LOD_INFO::writeBuffer(Buffer &buffer)
{
	LOD_INFO_BASE::writeBuffer(buffer);

	//��ȡ����ID
	buffer.write((char*)&unGeometryID, sizeof(unGeometryID));

	//int nMtCount = m_vecMTData.size();
	//buffer.write((char*)&nMtCount, sizeof(nMtCount));

	//for (int i=0; i<nMtCount; i++)
	//{
	//	MTData& data = m_vecMTData.at(i);
	//	data.writeBuffer(buffer);
	//}
}

void LOD_INFO::copy(const LOD_INFO& lodInfo)
{
	LOD_INFO_BASE::copy(lodInfo);

	unGeometryID = lodInfo.unGeometryID;
	//for (int i=0; i<lodInfo.m_vecMTData.size(); i++)
	//{
	//	m_vecMTData.push_back(lodInfo.m_vecMTData.at(i));
	//}

}

size_t LOD_INFO::sizeBuffer()
{
	size_t nSize = LOD_INFO_BASE::sizeBuffer();

	//���� ����ID�Ĵ�С
	nSize += sizeof(unGeometryID);

	////���ʸ��� ռ��4�ֽ�
	//int nMTCount = m_vecMTData.size();
	//nSize += sizeof(nMTCount);

	////���ϲ������ݵĴ�С
	//for (size_t i=0; i<m_vecMTData.size(); i++)
	//{
	//	nSize += sizeof(m_vecMTData.at(i).nDataLen);
	//	nSize += m_vecMTData.at(i).nDataLen;
	//}
	return nSize;
}


void LOD_INFO::clear()
{
	//for (size_t i=0; i<m_vecMTData.size(); i++)
	//{
	//	MTData data = m_vecMTData.at(i);
	//	if (data.pMTData != NULL)
	//	{
	//		delete data.pMTData;
	//		data.pMTData = NULL;
	//	}
	//}
}

CFeature::CFeature(void)
{
	
    m_nFID =0;
    m_strNameLen = -1;
    m_strName = "";
    m_nFCLID = -1;
    m_nCodeLen = -1;
    m_strCode = "";
    m_nState = -1;
    m_location.dElevation = 0.0;
    m_location.dLongitude = 0.0;
    m_location.dLatitude = 0.0;
    m_rotation.dLongitudeDegree = 0.0;
    m_rotation.dLatitudeDegree = 0.0;
    m_rotation.dNormalDegree = 0.0;
    m_zoom.dLongitudeOrientation = 0.0;
    m_zoom.dLatitudeOrientation = 0.0;
    m_zoom.dNormalOrientation = 0.0;
    m_byLodLeverNum = 0;
	_curLOD=-1;
 /*   m_MinVertex.x = 0.0;
    m_MinVertex.y = 0.0;
    m_MinVertex.z = 0.0;
    m_MaxVertex.x = 0.0;
    m_MaxVertex.y = 0.0;
    m_MaxVertex.z = 0.0;*/
    m_uFeatureCreateTime = 0;
    m_uFeatureUpdateTime = 0;
}

//********************************************************************************** 
//** �������ƣ� CFeature
//** ���������� �������캯��
//** ��    �룺 feature          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-07
//**********************************************************************************
CFeature::CFeature( CFeature& feature )
{
    copy(feature);
}

//********************************************************************************** 
//** �������ƣ� ~CFeature
//** ���������� ��������
//** ��    �룺 ��          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-07
//**********************************************************************************
CFeature::~CFeature(void)
{
//  	for (size_t i=0; i<m_LOD_Info_List.size(); i++)
//  	{
//  		LOD_INFO& lodInfo = m_LOD_Info_List.at(i);
// 		lodInfo.clear();
//  	}
}

//********************************************************************************** 
//** �������ƣ� copy
//** ���������� ���������Ҫ�ض���ĸ�������
//** ��    �룺 feature:Ҫ�ض���          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-07
//**********************************************************************************
void CFeature::copy(const CFeature& feature)
{
    //����������Ϣ
	copyBaseInfo(feature);

	m_byLodLeverNum = feature.m_byLodLeverNum;
    //����LOD��Ϣ
    for( byte byIndex = 0;byIndex < m_byLodLeverNum; byIndex++ )
    {
		LOD_INFO lodInfo;
		lodInfo.copy(feature.m_LOD_Info_List.at(byIndex));
		m_LOD_Info_List.push_back(lodInfo);
    }
}

//********************************************************************************** 
//** �������ƣ� readBuffer
//** ���������� �ӻ������ж�����
//** ��    �룺 buffer:Buffer���������           
//** ��    ���� ��
//** ����ֵ���ɹ�Ϊtrue��ʧ��Ϊfalse
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-11
//**********************************************************************************
bool CFeature::readBuffer(Buffer &buffer)
{
	return readBuffer(buffer, true);
}

//********************************************************************************** 
//** �������ƣ� writeBuffer
//** ���������� ������д��������
//** ��    �룺 buffer:Buffer���������          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-11
//**********************************************************************************
void CFeature::writeBuffer(Buffer &buffer)
{
	 writeBuffer(buffer, false);
}

//********************************************************************************** 
//** �������ƣ� readBufferForService
//** ���������� �ӻ������ж�����,�˴�����ȡҪ�ض����е�FeatureClassID
//** ��    �룺 buffer:Buffer���������           
//** ��    ���� ��
//** ����ֵ���ɹ�Ϊtrue��ʧ��Ϊfalse
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-11
//**********************************************************************************
bool CFeature::readBufferForService(Buffer &buffer)
{
   return readBuffer(buffer, true);
}

//********************************************************************************** 
//** �������ƣ� writeBufferForService
//** ���������� ������д�����������˴���д��Ҫ�ض����е�FeatureClassID
//** ��    �룺 buffer:Buffer���������          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-11
//**********************************************************************************
 void CFeature::writeBufferForService(Buffer &buffer)
 {
	 writeBuffer(buffer, true);
 }
void CFeature::setFeatureID(UINT64 nfid)
{
	m_nFID=nfid;
}
UINT64 CFeature::getFeatureID()
{
	return m_nFID;
}

void CFeature::setFeatureName(string strFeatureName)
{
	m_strName=strFeatureName;
}
string CFeature::getFeatureName()
{
	return m_strName;
}
int CFeature::getFeatureNameLen()
{
	return m_strName.size();
}

void CFeature::setFCLID(UINT64 nFCLID)
{
	m_nFCLID=nFCLID;
}
UINT64 CFeature::getFCLID()
{
	return m_nFCLID;
}

void CFeature::setCode(string strCode)
{
	m_strCode=strCode;
}
string CFeature::getCode()
{
	return m_strCode;
}
int CFeature::getCodeLen()
{
	return m_strCode.size();
}

void CFeature::setStatus(int nState)
{
	m_nState=nState;
}
int CFeature::getStatus()
{
	return m_nState;
}

void CFeature::setLocation(Location v)
{
	m_location=v;
}
Location CFeature::getLocation()
{
	return m_location;
}

void CFeature::setRotation(Rotation v)
{
	m_rotation=v;
}
Rotation CFeature::getRotation()
{
	return m_rotation;
}

void CFeature::setZoom(Zoom v)
{
	m_zoom=v;
}
Zoom CFeature::getZoom()
{
	return m_zoom;
}

void CFeature::setLodLeverNum(byte bLodLeverNum)
{
	m_byLodLeverNum=bLodLeverNum;
}
byte CFeature::getLodLeverNum()
{
	return m_byLodLeverNum;
}

void CFeature::setLOD_INFOList(LOD_INFOList& v)
{
	m_LOD_Info_List=v;
}
LOD_INFOList& CFeature::getLOD_INFOList()
{
	return m_LOD_Info_List;
}

//********************************************************************************** 
//** �������ƣ� sizeBuffer
//** ���������� �õ�Ҫ�ش�С
//** ��    �룺 ��          
//** ��    ���� ��
//** ����ֵ��Ҫ�ش�С
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-11
//**********************************************************************************
size_t  CFeature::sizeBuffer()
{
	
	size_t sFeatureSize = 0;
	//sFeatureSize = sizeof(UINT64) + sizeof(int) + m_strName.length() + sizeof(UINT64)
 //       + sizeof(int) + m_strCode.length() + sizeof(int) + 9*sizeof(double) + 2*sizeof(UINT64) + 2*Vertex3d::sizeBuffer()+sizeof(byte);

	//for (size_t i=0; i<m_LOD_Info_List.size(); i++)
	//{
	//	sFeatureSize += m_LOD_Info_List.at(i).sizeBuffer();
	//}
	
    return sFeatureSize;
}

//********************************************************************************** 
//** �������ƣ� sizeBufferForService
//** ���������� �õ�Ҫ�ش�С,�˴�������Ҫ�ض����е�FeatureClassID
//** ��    �룺 ��          
//** ��    ���� ��
//** ����ֵ��Ҫ�ش�С
//** ��    �ߣ� ����
//** �������ڣ� 2012-06-08
//**********************************************************************************
//size_t CFeature::sizeBufferForService()
//{
//    size_t sFeatureSize = 0;
//    sFeatureSize = sizeof(UINT64) + sizeof(int) + m_strName.length()
//        + sizeof(int) + m_strCode.length() + sizeof(int) + 9*sizeof(double) + 2*sizeof(UINT64) + 2*Vertex3d::sizeBuffer()+sizeof(byte);
//    
//	//����lod��Ϣ���ڴ�
//	for (size_t i=0; i<m_LOD_Info_List.size(); i++)
//		sFeatureSize += m_LOD_Info_List.at(i).sizeBuffer();
//
//    return sFeatureSize;
//}

//void CFeature::getMinVertex(Vertex3d& minVertex)
//{
//    minVertex = m_MinVertex;
//}
//void CFeature::getMaxVertex(Vertex3d& maxVertex)
//{
//    maxVertex = m_MaxVertex;
//}

void CFeature::copyBaseInfo(const CFeature& feature)
{
	//���������Ҫ�صĸ�������

	m_nFID =feature.m_nFID;

	m_strNameLen =feature.m_strNameLen;
	m_strName = feature.m_strName;

	m_nFCLID = feature.m_nFCLID;
	m_nCodeLen = feature.m_nCodeLen;
	m_strCode = feature.m_strCode;
	m_nState = feature.m_nState;

	m_location.dElevation = feature.m_location.dElevation;
	m_location.dLongitude = feature.m_location.dLongitude;
	m_location.dLatitude = feature.m_location.dLatitude;

	m_rotation.dLongitudeDegree = feature.m_rotation.dLongitudeDegree;
	m_rotation.dLatitudeDegree = feature.m_rotation.dLatitudeDegree;
	m_rotation.dNormalDegree = feature.m_rotation.dNormalDegree;

	m_zoom.dLongitudeOrientation = feature.m_zoom.dLongitudeOrientation;
	m_zoom.dLatitudeOrientation = feature.m_zoom.dLatitudeOrientation;
	m_zoom.dNormalOrientation = feature.m_zoom.dNormalOrientation;

	//m_MinVertex.x = feature.m_MinVertex.x;
	//m_MinVertex.y = feature.m_MinVertex.y;
	//m_MinVertex.z = feature.m_MinVertex.z;
	//m_MaxVertex.x = feature.m_MaxVertex.x;
	//m_MaxVertex.y = feature.m_MaxVertex.y;
	//m_MaxVertex.z = feature.m_MaxVertex.z;

	m_uFeatureCreateTime = feature.m_uFeatureCreateTime;
	m_uFeatureUpdateTime = feature.m_uFeatureUpdateTime;
}

bool CFeature::readBaseInfo(Buffer& buffer, bool bForService)
{
	//��ȡҪ��ID
	buffer.read((char*)&m_nFID,sizeof(UINT64));
	//��ȡҪ�����Ƴ���
	buffer.read((char*)&m_strNameLen,sizeof(int));
	char ca[4096];
	memset(ca,0,4096);
	//��ȡҪ������
	//modify by yangling 20131208 begin
	if(m_strNameLen > 4096)
	{
		return false;
	}
	//modify by yangling 20131208 end
	buffer.read((char*)ca,m_strNameLen);
	m_strName.assign(ca);

	//�����������ά�ͻ���,��Ҫ��ȡҪ����ID
	if (!bForService)
	{
		//��ȡҪ����ID
		buffer.read((char*)&m_nFCLID,sizeof(UINT64));
	}
	
	//��ȡ���볤��
	buffer.read((char*)&m_nCodeLen,sizeof(int));
	//modify by yangling 20131208 begin
	if(m_nCodeLen > 4096)
	{
		return false;
	}
	//modify by yangling 20131208 end
	//�ٴγ�ʼ����ʱ�ַ���
	memset(ca,0,4096);

	//��ȡ������Ϣ
	buffer.read((char*)ca,m_nCodeLen);
	m_strCode.assign(ca);

	//��ȡҪ��״̬
	buffer.read((char*)&m_nState,sizeof(int));

	//��ȡҪ�ض�λ��Ϣ
	buffer.read((char*)&m_location.dLongitude,sizeof(double));
	buffer.read((char*)&m_location.dLatitude,sizeof(double));
	buffer.read((char*)&m_location.dElevation,sizeof(double));


	//��ȡҪ����ת��Ϣ
	buffer.read((char*)&m_rotation.dLongitudeDegree,sizeof(double));
	buffer.read((char*)&m_rotation.dLatitudeDegree,sizeof(double));
	buffer.read((char*)&m_rotation.dNormalDegree,sizeof(double));

	//��ȡҪ��������Ϣ
	buffer.read((char*)&m_zoom.dLongitudeOrientation,sizeof(double));
	buffer.read((char*)&m_zoom.dLatitudeOrientation,sizeof(double));
	buffer.read((char*)&m_zoom.dNormalOrientation,sizeof(double));

	//��ȡҪ�ش���ʱ��
	buffer.read((char*)&m_uFeatureCreateTime,sizeof(UINT64));

	//��ȡҪ�ظ���ʱ��
	buffer.read((char*)&m_uFeatureUpdateTime,sizeof(UINT64));

	//// ��ȡ��γ��AABB����С����λ��
	
	buffer.read((char*)&m_MinVertex.x(),sizeof(double));
	buffer.read((char*)&m_MinVertex.y(),sizeof(double));
	buffer.read((char*)&m_MinVertex.z(),sizeof(double));

	//// ��ȡ��γ��AABB����󶥵�λ��
	buffer.read((char*)&m_MaxVertex.x(),sizeof(double));
	buffer.read((char*)&m_MaxVertex.y(),sizeof(double));
	buffer.read((char*)&m_MaxVertex.z(),sizeof(double));
	return true;
}

void CFeature::writeBaseInfo(Buffer& buffer, bool bForService)
{
	//д��Ҫ��ID
	buffer.write((char*)&m_nFID,sizeof(UINT64));
	//д��Ҫ�����Ƴ���
	buffer.write((char*)&m_strNameLen,sizeof(int));

	//д��Ҫ������

	buffer.write((char*)m_strName.c_str(),m_strNameLen);

	//�����SynBuilder��Ҫ��ȡҪ����ID���ͻ��˲���Ҫ��ȡ������
	if (!bForService)
	{
		//д��Ҫ����ID
		buffer.write((char*)&m_nFCLID,sizeof(UINT64));
	}


	//д����볤��

	buffer.write((char*)&m_nCodeLen,sizeof(int));


	//д�������Ϣ
	buffer.write((char*)m_strCode.c_str(),m_nCodeLen);


	//д��Ҫ��״̬
	buffer.write((char*)&m_nState,sizeof(int));

	//д��Ҫ�ض�λ��Ϣ
	buffer.write((char*)&m_location.dLongitude,sizeof(double));
	buffer.write((char*)&m_location.dLatitude,sizeof(double));
	buffer.write((char*)&m_location.dElevation,sizeof(double));

	//д��Ҫ����ת��Ϣ
	buffer.write((char*)&m_rotation.dLongitudeDegree,sizeof(double));
	buffer.write((char*)&m_rotation.dLatitudeDegree,sizeof(double));
	buffer.write((char*)&m_rotation.dNormalDegree,sizeof(double));

	//д��Ҫ��������Ϣ
	buffer.write((char*)&m_zoom.dLongitudeOrientation,sizeof(double));
	buffer.write((char*)&m_zoom.dLatitudeOrientation,sizeof(double));
	buffer.write((char*)&m_zoom.dNormalOrientation,sizeof(double));

	//д��Ҫ�ش���ʱ��
	buffer.write((char*)&m_uFeatureCreateTime,sizeof(UINT64));

	//д��Ҫ�ظ���ʱ��
	buffer.write((char*)&m_uFeatureUpdateTime,sizeof(UINT64));

	//// д�뾭γ��AABB����С����λ��
	//buffer.write((char*)&m_MinVertex.x,sizeof(double));
	//buffer.write((char*)&m_MinVertex.y,sizeof(double));
	//buffer.write((char*)&m_MinVertex.z,sizeof(double));

	//// д�뾭γ��AABB����󶥵�λ��
	//buffer.write((char*)&m_MaxVertex.x,sizeof(double));
	//buffer.write((char*)&m_MaxVertex.y,sizeof(double));
	//buffer.write((char*)&m_MaxVertex.z,sizeof(double));
}

bool CFeature::readBuffer(Buffer& buffer, bool bForService)
{
	bool bRet = readBaseInfo(buffer, bForService);
	//modify by yangling 20131208 begin
	if(!bRet)
	{
		return false;
	}
	//modify by yangling 20131208 end
	//��ȡLOD������
	buffer.read((char*)&m_byLodLeverNum,sizeof(byte));
	//modify by yangling 20131208 begin
	if(m_byLodLeverNum > 255)
	{
		return false;
	}
	//modify by yangling 20131208 end
	for( byte byIndex = 0;byIndex < m_byLodLeverNum; byIndex++ )
	{
		LOD_INFO lodInfo;
		lodInfo.readBuffer(buffer);
 		m_LOD_Info_List.push_back(lodInfo);
	}
	return true;
}

void CFeature::writeBuffer(Buffer& buffer, bool bForService)
{
	writeBaseInfo(buffer, bForService);


	//д��LOD������
	buffer.write((char*)&m_byLodLeverNum,sizeof(byte));
	for( byte byIndex = 0;byIndex < m_byLodLeverNum; byIndex++ )
	{
		m_LOD_Info_List[byIndex].writeBuffer(buffer);
	}
}

//��д����
//bForService �Ƿ�Ϊ�ͻ�������
bool CFeature50::readBuffer(Buffer& buffer, bool bForService)
{
	bool bRet = readBaseInfo(buffer, bForService);
	//modify by yangling 20131208 begin
	if(!bRet)
	{
		return false;
	}
	//modify by yangling 20131208 end
	//��ȡLOD������
	buffer.read((char*)&m_byLodLeverNum,sizeof(byte));
	//modify by yangling 20131208 begin
	if(m_byLodLeverNum > 255)
	{
		return false;
	}
	//modify by yangling 20131208 end
	for( byte byIndex = 0;byIndex < m_byLodLeverNum; byIndex++ )
	{
		LOD_INFO_BASE baseInfo;
		baseInfo.readBuffer(buffer);
		m_vecLodInfo.push_back(baseInfo);
	}
	return true;
}

void CFeature50::writeBuffer(Buffer& buffer, bool bForService)
{
	writeBaseInfo(buffer, bForService);

	//д��LOD������
	buffer.write((char*)&m_byLodLeverNum,sizeof(byte));
	for( byte byIndex = 0;byIndex < m_byLodLeverNum; byIndex++ )
	{
		m_vecLodInfo[byIndex].writeBuffer(buffer);
	}
}


