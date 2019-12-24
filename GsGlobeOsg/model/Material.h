//********************************************************************************** 
//** �ļ����ƣ�Material.h
//** CopyRight (c) 2000-2012 ��󼪰���Ϣ�������޹�˾����һ��
//** �ļ������� �����ඨ��
//** �� �� �ˣ� ����
//** �������ڣ� 2012-04-11
//********************************************************************************** 
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#pragma once
#include "ModelDef.h"
#include "Buffer.h"
#include <fstream>
#include <vector>
using namespace std;

namespace CT3D
{
typedef struct tagMat_Param
{

    /** ��־λ(�������ʡ���ɫ) */
    byte		byflag;               
    /** ͸���ķ�ʽ */
    unsigned char		byTransparent;        
    /** ���޳���ʽ */
    unsigned char		byCullface;		

    /** ��ȼ�� */
    unsigned char		byDepth;	

    /** ����*/
    unsigned char		byReserve[4];			

    /** ͸����  [ 0.0 (��͸��)  ~  1.0 (��ȫ͸��) ] */
    float		fAlpha;              

    /** ������ */
    unsigned long	ulAmbient;	

    /** ������ */
    unsigned long	ulDiffuse;			

    /** ���淴�� */
    unsigned long	ulSpecular;			

    /** ��ɫ */
    unsigned long	ulColor;				

    /** ����ƽ�Ʋ��� */
    float		fuOffset; 
    float      gvfOffset;	

    /** ������ת��������λΪ�ȣ���ת��Ϊ (0, 0, 1)�� */
    float		fRotAngle;			

    /** �߹�*/
    float		fShininess;			
    /** �ն� */
    float		fIllumination;
    /** ����ID */
    UID64 nTextureID;

}MAT_PARAM;//�򵥲���

//typedef struct ComMat_Item
//{
//
//    /** ����ID */
//    int nTextureID;
//    /** ǿ�� */
//    float fIntensity;
//    /** ��ɫ */
//    unsigned long ulColor;
//    /** �� u ���������ظ��Ĵ��� */
//    double  duRepeat;	
//    /** �� v ���������ظ��Ĵ��� */
//    double  dvRepeat;	
//    /** ����ƽ�� */
//    float	fuOffset;	
//    /** ����ƽ�� */
//    float	fvOffset;	
//    /** ������ת */
//    float	fRotAngle;	
//    /** ͸����  [ 0.0 (��͸��)  ~  1.0 (��ȫ͸��) ] */
//    float	fAlpha;      
//
//}COMMAT_ITEM;//���Ӳ���

/// �ںϷ�ʽ
typedef struct Layer_Blend_Mode
{
	/** �ں����ͣ�0 : ��ɫ�� 1 ��Alpha */
	int nBlendType;
	/** �ںϲ���������ΪD3DTEXTUREOP,ȱʡֵΪD3DTOP_MODULATE=4 */
	int nOperation;
	/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_CURRENT=1 */
	int nSource1;
	/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_TEXTURE=2 */
	int nSource2;
}LAYER_BLEND_MODE;

///����Ѱַģʽ
typedef struct Texture_Addressing_Mode
{
	/** U�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
	int nUMode;
	/** V�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
	int nVMode;
	/** W�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
	int nWMode;       
}TEXTURE_ADDRESSING_MODE;

///������״̬����
typedef struct Texture_Item
{
	/** ����ID */
	UINT64 nTextureID;
	//�������꼯����
	unsigned int nTexIndex;
	/** ���ں���ɫ���� */
	LAYER_BLEND_MODE nColorBlendMode;
	/** ���ں�Alpha���� */
	LAYER_BLEND_MODE nAlphaBlendMode;
	/** ����Ѱַģʽ */
	TEXTURE_ADDRESSING_MODE nTextureAddressingMode;
	/** ����߿�Ѱַģʽ�еı߿���ɫ */
	unsigned long nBorderColour;
	/** �������ģʽ������ΪD3DTEXTUREFILTERTYPE, ȱʡֵΪD3DTEXF_LINEAR */
	int nMagFilter;
	int nMinFilter;
	int nMipFilter;
	/** ��u v��������ƽ��*/
	double  dUScroll, dVScroll;
	/** ��u v������������*/
	double  dUScale, dVScale;
	/** ������ת*/
	double  dRotate;    
}TEXTURE_ITEM;//������״̬����

///���Ӳ��ʼ������
typedef struct ComMat_Item
{
	
	unsigned long nAmbient;
	
	unsigned long nDiffuse;
	
	unsigned long nSpecular;

	unsigned long nEmissive;

	bool bDepthCheck;
	bool bDepthWrite;
	
	int nDepthFunc;
	
	int nSourceBlendFactor;
	int nDestBlendFactor;
	/** �����ںϲ���������ΪD3DBLENDOP��ȱʡֵΪD3DBLENDOP_ADD*/
	int nBlendOperation;
	int nAlphaBlendOperation;
	/** Alpha�������ã�����ΪD3DCMPFUNC��ȱʡֵΪD3DCMP_ALWAYS*/
	int nAlphaRejectFunc;

	/** Alpha����ֵ*/
	unsigned char nAlphaRejectVal;

	/** ��ɫģʽ������ΪD3DSHADEMODE��ȱʡֵΪD3DSHADE_FLAT*/
	int nShadeMode;

	/** �޳���ʽ������ΪD3DCULL��ȱʡֵΪD3DCULL_CW*/
	int nCullMode;

	/** ͼԪ�Ļ��Ʒ�ʽ������ΪD3DFILLMODE��ȱʡΪD3DFILLMODE_SOLID*/
	int nPolygonMode;
	int nItemNum;//����Ԫ����
	TEXTURE_ITEM* pComm_item;//����Ԫ�ṹ����ָ��
}COMMAT_ITEM;


typedef struct ComMat
{
    int nItemNum;//���Ӳ��ʸ���
    COMMAT_ITEM* pComm_item;//���Ӳ��ʽṹ����ָ��

}ComMat_PARAM;

typedef struct _tagMaterial
{
	//---------------------------------------------
	byte m_bVersionLength;/** 材质版本号长度 */
	string m_sMaterialVersion;/** 材质版本号 */
	//---------------------------------------------
    UINT64 unMaterialID;//����ID
    byte   byMaterialType;//��������
    int nMaterialNameLen;//�������Ƴ���
    char caMaterialName[255];//��������
    int   nMaterialDataLen;//���������峤��
    union
    {
        MAT_PARAM SimpleMat;//�򵥲���
        ComMat_PARAM ComPlexMat;//���Ӳ���
        char* pcTextureBuf;//��������
    }MatData;//��������
}Material;//���ʽṹ
}

typedef std::vector<UINT64> TexIDList;//���ʶ�Ӧ������ID�б�
#include <osg/Referenced>
class /*DLL_EXPORT*/ CMaterial: public osg::Referenced
{
public:
    //********************************************************************************** 
    //** �������ƣ� CMaterial
    //** ���������� ���캯��
    //** ��    �룺 ��
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    CMaterial(void);

    //********************************************************************************** 
    //** �������ƣ� ~CMaterial
    //** ���������� ��������
    //** ��    �룺 ��
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    virtual ~CMaterial(void);

    //********************************************************************************** 
    //** �������ƣ� readBuffer
    //** ���������� �ӻ������ж�����
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ���ɹ�Ϊtrue��ʧ��Ϊfalse
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    virtual bool readBuffer(Buffer &buffer);

    //********************************************************************************** 
    //** �������ƣ� writeBuffer
    //** ���������� ������д��������
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    virtual void writeBuffer(Buffer &buffer);

    //********************************************************************************** 
    //** �������ƣ� readTexureBuff
    //** ���������� �ӻ������ж���������
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-06-11
    //**********************************************************************************
    void readTexureBuffer(Buffer &buffer);

    //********************************************************************************** 
    //** �������ƣ� writeTexureBuffer
    //** ���������� ����������д��������
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-06-11
    //**********************************************************************************
    void writeTexureBuffer(Buffer &buffer);
private:

   
    //********************************************************************************** 
    //** �������ƣ� readSimpleMatBuff
    //** ���������� �ӻ������ж��򵥲�������
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    void readSimpleMatBuff(Buffer &buffer);

    //********************************************************************************** 
    //** �������ƣ� writeSimpleMatBuff
    //** ���������� ���򵥲�������д��������
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    void writeSimpleMatBuff(Buffer &buffer);


    //********************************************************************************** 
    //** �������ƣ� readComplexMatBuff
    //** ���������� �ӻ������ж����Ӳ�������
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    void readComplexMatBuff(Buffer &buffer);

    //********************************************************************************** 
    //** �������ƣ� writeComplexMatBuff
    //** ���������� �����Ӳ�������д��������
    //** ��    �룺 buffer:Buffer���������
    //** ��    ���� ��
    //** ����ֵ����
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-16
    //**********************************************************************************
    void writeComplexMatBuff(Buffer &buffer);


    

    

    //********************************************************************************** 
    //** �������ƣ� getFileSize
    //** ���������� �õ��ļ���С
    //** ��    �룺 strFileName:�ļ���
    //** ��    ���� ��
    //** ����ֵ���ļ���С
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-06-11
    //**********************************************************************************
    size_t getFileSize(string strFileName);

    CT3D::Material m_Material;//���ʽṹ
    byte m_byMaterialType;//��������
    string m_strTextureFolderPath;//�����ļ���·��
    TexIDList m_TexIDList;//���ʶ�Ӧ������ID�б�

public:
	void setMaterialID(UINT64 nMaterialID);
	UINT64 getMaterialID();

	void setMaterialType(int nMaterialType);
	int getMaterialType();

	void setMaterialName(string strMaterialName);
	string getMaterialName();
	int getMaterialNameLen();

	void setDataLen(int nDataLen);
	int getDataLen();

	void setSimpleMat(CT3D::MAT_PARAM v);
	CT3D::MAT_PARAM getSimpleMat();

	void setComPlexMat(CT3D::ComMat_PARAM v);
	CT3D::ComMat_PARAM getComPlexMat();
    void setTextureFolderPath(string strTextureFolderPath);
    TexIDList getTextureID();
    //********************************************************************************** 
    //** �������ƣ� sizeBuffer
    //** ���������� �õ����ʴ�С
    //** ��    �룺 ��
    //** ��    ���� ��
    //** ����ֵ�����ʴ�С
    //** ��    �ߣ� ����
    //** �������ڣ� 2012-04-11
    //**********************************************************************************
    size_t sizeBuffer();
};

#endif
