//********************************************************************************** 
//** �ļ����ƣ�CT3D_3DReference.cpp
//** CopyRight (c) 2000-2012 ��󼪰���Ϣ�������޹�˾����һ��
//** �ļ������� ��ά����ͼԪ��ʵ��
//** �� �� �ˣ� ����
//** �������ڣ� 2012-04-11
//********************************************************************************** 

#include "CT3D_3DReference.h"
#include "Buffer.h"
//********************************************************************************** 
//** �������ƣ� C3DReference
//** ���������� ���캯��
//** ��    �룺 ��           
//** ��    ���� ��
//** ����ֵ��	��
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
C3DReference::C3DReference()
{
	m_byTypeID = GEOTYPE_3D_REFERENCE;
	
	m_nLinkType = 0;	

	m_nExtParaSize = 0;

	m_pExtParaData = NULL;

	m_nLinkID = -1;
	m_strLinkName = "";
    m_strLinkName = "";
    m_vPosition = Vertex3d(0,0,0);
    m_qOrientation = Quaternion(1,0,0,0);
    m_Scale.x=0;
	m_Scale.y=0;
	m_Scale.z=0;
}

//********************************************************************************** 
//** �������ƣ� ~C3DReference
//** ���������� ��������
//** ��    �룺 ��           
//** ��    ���� ��
//** ����ֵ��	��
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
C3DReference::~C3DReference()
{
 	if(m_pExtParaData)
 	{
 		free(m_pExtParaData);
 	}

}

//********************************************************************************** 
//** �������ƣ� GetExtParaData
//** ���������� ����������ݴ�С�������׵�ַ
//** ��    �룺 nExtParaSize  �������ݴ�С��pExeParaData  ���������յ�ַ           
//** ��    ���� ��
//** ����ֵ��	��
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
void C3DReference::GetExtParaData(int& nExtParaSize, void*& pExeParaData) const
{
	nExtParaSize = m_nExtParaSize;	
	pExeParaData = m_pExtParaData;
}

//********************************************************************************** 
//** �������ƣ� SetExtParaData
//** ���������� �����������ݴ�С�������׵�ַ
//** ��    �룺 nExtParaSize  �������ݴ�С��pExeParaData  ���������յ�ַ           
//** ��    ���� ��
//** ����ֵ��	��
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
void C3DReference::SetExtParaData(int nExtParaSize, void* pExeParaData) 
{ 
	m_nExtParaSize = nExtParaSize;	
	m_pExtParaData = pExeParaData;	
}

//********************************************************************************** 
//** �������ƣ� readBuffer
//** ���������� �ӻ������н�����ǰ���󣬲�����Ա������ֵ
//** ��    �룺 buf	������������           
//** ��    ���� ��
//** ����ֵ��	�ɹ�Ϊtrue��ʧ��Ϊfalse
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
bool C3DReference::readBuffer( Buffer & buf)
{
    bool bRet = C3DPoint::readBuffer(buf);
	//modify by yangling 20131208 begin
	if(!bRet)
	{
		return false;
	}
	//modify by yangling 20131208 end
	//������Ϣ*************************************

    buf.read((char*)&m_vPosition,Vertex3d::sizeBuffer());
    buf.read((char*)&m_qOrientation,Quaternion::sizeBuffer());
	buf.read((char*)&m_Scale,sizeof(Scale));
	buf.read((char*)&m_nLinkType,sizeof(int));
	buf.read((char*)&m_nLinkID,sizeof(UID64));  
	
	int ll = 0;
	buf.read((char*)&ll,sizeof(int));
	if(ll > 0)
	{
		char tempbuf[50];
		buf.read(tempbuf,sizeof(char) * ll);
		tempbuf[ll] = '\0';
		m_strLinkName = tempbuf;
	}

	buf.read((char*)&m_nExtParaSize,sizeof(int));//�ⲿ��������
	if(m_nExtParaSize > 0)
	{
		m_pExtParaData = new int[m_nExtParaSize];
	buf.read((char*)m_pExtParaData,m_nExtParaSize);  
	}
	return true;
}

//********************************************************************************** 
//** �������ƣ� writeBuffer
//** ���������� �ѵ�ǰ����ĳ�Ա�������ݣ������һ��������
//** ��    �룺 buf	������������           
//** ��    ���� ��
//** ����ֵ��	��
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
void C3DReference::writeBuffer(Buffer& buf)
{
    C3DPoint::writeBuffer(buf);
	
	//������Ϣ*************************************

    buf.write((char*)&m_vPosition,Vertex3d::sizeBuffer());
    buf.write((char*)&m_qOrientation,Quaternion::sizeBuffer());
	buf.write((char*)&m_Scale,sizeof(Scale));
	buf.write((char*)&m_nLinkType,sizeof(int));
	buf.write((char*)&m_nLinkID,sizeof(UID64));  
	
	int ll = m_strLinkName.length();
	buf.write((char*)&ll,sizeof(int));
	if(ll > 0)
	{
		buf.write((char*)m_strLinkName.c_str(),sizeof(char) * ll);
	}
	buf.write((char*)&m_nExtParaSize,sizeof(int));//�ⲿ��������
	if(m_nExtParaSize > 0)
	{
	buf.write((char*)m_pExtParaData,m_nExtParaSize);
	}
}

//********************************************************************************** 
//** �������ƣ� CreateObject
//** ���������� ����һ����ǰ�����������ڴ�ռ�
//** ��    �룺 ��           
//** ��    ���� ��
//** ����ֵ��	���ٵĵ�ǰ������ڴ�ռ���׵�ַ
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
CGeometry* C3DReference::CreateObject()
{
	return new C3DReference;
}

size_t C3DReference::sizeBuffer()
{
	size_t nSize = 0;
	nSize += C3DPoint::sizeBuffer();
	nSize += Vertex3d::sizeBuffer();
	nSize += Quaternion::sizeBuffer();
	nSize += sizeof(double)*3; //���ŷ���
	nSize += sizeof(int);//�����ļ�����
	nSize += sizeof(UINT64);//��������ID
	nSize += sizeof(int);//�����������Ƴ���
	nSize += m_strLinkName.length();//������������
	nSize += sizeof(int);//��������(ռ���ڴ�)��С
	nSize += m_nExtParaSize;//��������
	

	return nSize;
}
