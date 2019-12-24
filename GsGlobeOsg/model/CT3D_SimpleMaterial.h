#pragma once
#include "ModelDef.h"
#include <fstream>
//#include "CT3D_Material.h"
using namespace std;
 //------------------- ����������ݽṹ --------------------------------
struct MAT_PARAM
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
    /** @} */ // ���Խ�β

    MAT_PARAM()
    {
        memset(this, 0, sizeof(MAT_PARAM));
    }
};
class SimpleMaterial/*:public CMaterial*/
{
public:
    SimpleMaterial(void);
    virtual ~SimpleMaterial(void);
public:
   
    MAT_PARAM m_mat_param;
    virtual void read(ifstream& f);
    virtual void write(ifstream& f);


};
