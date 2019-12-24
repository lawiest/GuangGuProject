#include "Material.h"
#include <sys/stat.h>
//********************************************************************************** 
//** �������ƣ� CMaterial
//** ���������� ���캯��
//** ��    �룺 ��          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
CMaterial::CMaterial(void)
{
    //memset(m_Material.caMaterialName,0,sizeof(m_Material.caMaterialName));
	memset(&m_Material,0,sizeof(m_Material));
	m_byMaterialType = -1;
}

//********************************************************************************** 
//** �������ƣ� ~CMaterial
//** ���������� ��������
//** ��    �룺 ��          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
CMaterial::~CMaterial(void)
{
    if((0 == m_Material.byMaterialType)&&(m_Material.nMaterialDataLen != 0))
    {
		if ( m_Material.MatData.pcTextureBuf!=NULL)
			delete[] m_Material.MatData.pcTextureBuf;

        m_Material.MatData.pcTextureBuf = NULL;
    }

    if((2 ==m_Material.byMaterialType)&& (m_Material.MatData.ComPlexMat.pComm_item != NULL))
    {
        for(int i = 0; i < m_Material.MatData.ComPlexMat.nItemNum; i++)
        {
            delete[] m_Material.MatData.ComPlexMat.pComm_item[i].pComm_item;
        }

        delete[] m_Material.MatData.ComPlexMat.pComm_item;
    }
}

//********************************************************************************** 
//** �������ƣ� readBuffer
//** ���������� �ӻ������ж�����
//** ��    �룺 buffer:Buffer���������           
//** ��    ���� ��
//** ����ֵ���ɹ�Ϊtrue��ʧ��Ϊfalse
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
bool CMaterial::readBuffer(Buffer &buffer)
{
	buffer.read((char*)&m_Material.m_bVersionLength,sizeof(byte));
	if ( m_Material.m_bVersionLength == 6 )
	{
		//读取要素版本号
		char ch[256];
		memset(ch,0,256);
		buffer.read((char*)ch,m_Material.m_bVersionLength);
		m_Material.m_sMaterialVersion.assign(ch);
		string str(ch);
		if(str.empty() || str.find("GMDL") == string::npos)
		{
			buffer.setPos(buffer.getPos() - 1 - m_Material.m_bVersionLength);
			m_Material.m_bVersionLength = 0;
			m_Material.m_sMaterialVersion = "";
		}
	}
	else
	{
		buffer.setPos(buffer.getPos() - 1 );
		m_Material.m_bVersionLength = 0;
		m_Material.m_sMaterialVersion = "";
	}
	//---------------------------------------------------------
	//读取材质ID

	buffer.read((char*)&m_Material.unMaterialID,sizeof(UINT64));

	//读取材质类型
	buffer.read((char*)&m_Material.byMaterialType,sizeof(byte));

	//读取材质名称长度
	buffer.read((char*)&m_Material.nMaterialNameLen,sizeof(int));
	//modify by yangling 20131208 begin
	if(m_Material.nMaterialNameLen > sizeof(m_Material.caMaterialName)/sizeof(char))
	{
		return false;
	}
	//modify by yangling 20131208 end
	//读取材质名称
	buffer.read(m_Material.caMaterialName,m_Material.nMaterialNameLen);


	//读取材质数据体长度
	buffer.read((char*)&m_Material.nMaterialDataLen,sizeof(int));

	switch(m_Material.byMaterialType)
	{
	case 0:
		{
			break;
		}

	case 1:
		{

			readSimpleMatBuff(buffer);
			break;
		}
	case 2:
		{
			readComplexMatBuff(buffer);
			break;
		}
	default:
		return false;
	}
	return true;
}

//********************************************************************************** 
//** �������ƣ� writeBuffer
//** ���������� ������д��������
//** ��    �룺 buffer:Buffer���������          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
void CMaterial::writeBuffer(Buffer &buffer)
{

    //д�����ID
    buffer.write((char*)&m_Material.unMaterialID,sizeof(UINT64));

    //д���������
    buffer.write((char*)&m_Material.byMaterialType,sizeof(byte));

    //д��������Ƴ���
    buffer.write((char*)&m_Material.nMaterialNameLen,sizeof(int));

    //д���������
    buffer.write(m_Material.caMaterialName,m_Material.nMaterialNameLen);

    //д����������峤��
    buffer.write((char*)&m_Material.nMaterialDataLen,sizeof(int));

    switch(m_Material.byMaterialType)
    {
    case 0:
        {
            writeTexureBuffer(buffer);
            break;
        }
        
    case 1:
        {

            
            writeSimpleMatBuff(buffer);
            break;
        }
        
    case 2:
        {

            writeComplexMatBuff(buffer);
            break;
        }
        
    }
}

//********************************************************************************** 
//** �������ƣ� readSimpleMatBuff
//** ���������� �ӻ������ж��򵥲�������
//** ��    �룺 buffer:Buffer���������           
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
void CMaterial::readSimpleMatBuff(Buffer &buffer)
{
    //��ȡ��־λ
    buffer.read((char*)&m_Material.MatData.SimpleMat.byflag,sizeof(byte));
    //��ȡ͸���ķ�ʽ
    buffer.read((char*)&m_Material.MatData.SimpleMat.byTransparent,sizeof(byte));
    //��ȡ���޳���ʽ
    buffer.read((char*)&m_Material.MatData.SimpleMat.byCullface,sizeof(byte));
    //��ȡ��ȼ��
    buffer.read((char*)&m_Material.MatData.SimpleMat.byDepth,sizeof(byte));
    //��ȡ������
    buffer.read(m_Material.MatData.SimpleMat.byReserve,sizeof(byte)*4);
    //��ȡ͸����
    buffer.read((char*)&m_Material.MatData.SimpleMat.fAlpha,sizeof(float));
    //��ȡ������
    buffer.read((char*)&m_Material.MatData.SimpleMat.ulAmbient,sizeof(unsigned long));
    //��ȡ������
    buffer.read((char*)&m_Material.MatData.SimpleMat.ulDiffuse,sizeof(unsigned long));
    //��ȡ���淴��
    buffer.read((char*)&m_Material.MatData.SimpleMat.ulSpecular,sizeof(unsigned long));
    //��ȡ��ɫ
    buffer.read((char*)&m_Material.MatData.SimpleMat.ulColor,sizeof(unsigned long));
    //��ȡ����ƽ��u����
    buffer.read((char*)&m_Material.MatData.SimpleMat.fuOffset,sizeof(float));
     //��ȡ����ƽ��v����
    buffer.read((char*)&m_Material.MatData.SimpleMat.gvfOffset,sizeof(float)); 
    //��ȡ������ת����
    buffer.read((char*)&m_Material.MatData.SimpleMat.fRotAngle,sizeof(float));
    //��ȡ�߹�
    buffer.read((char*)&m_Material.MatData.SimpleMat.fShininess,sizeof(float));
    //��ȡ�ն�
    buffer.read((char*)&m_Material.MatData.SimpleMat.fIllumination,sizeof(float));
    //��ȡ����ID
    buffer.read((char*)&m_Material.MatData.SimpleMat.nTextureID,sizeof(UID64));

}

//********************************************************************************** 
//** �������ƣ� writeSimpleMatBuff
//** ���������� ���򵥲�������д��������
//** ��    �룺 buffer:Buffer���������          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//**********************************************************************************
void CMaterial::writeSimpleMatBuff(Buffer &buffer)
{

    //д���־λ
    buffer.write((char*)&m_Material.MatData.SimpleMat.byflag,sizeof(byte));
    //д��͸���ķ�ʽ
    buffer.write((char*)&m_Material.MatData.SimpleMat.byTransparent,sizeof(byte));
    //д�����޳���ʽ
    buffer.write((char*)&m_Material.MatData.SimpleMat.byCullface,sizeof(byte));
    //д����ȼ��
    buffer.write((char*)&m_Material.MatData.SimpleMat.byDepth,sizeof(byte));
    //д�뱣����
    buffer.write((char*)&m_Material.MatData.SimpleMat.byReserve,sizeof(byte)*4);
    //д��͸����
    buffer.write((char*)&m_Material.MatData.SimpleMat.fAlpha,sizeof(float));
    //д�뻷����
    buffer.write((char*)&m_Material.MatData.SimpleMat.ulAmbient,sizeof(unsigned long));
    //д��������
    buffer.write((char*)&m_Material.MatData.SimpleMat.ulDiffuse,sizeof(unsigned long));
    //д�뾵�淴��
    buffer.write((char*)&m_Material.MatData.SimpleMat.ulSpecular,sizeof(unsigned long));
    //д����ɫ
    buffer.write((char*)&m_Material.MatData.SimpleMat.ulColor,sizeof(unsigned long));
    //д������ƽ��u����
    buffer.write((char*)&m_Material.MatData.SimpleMat.fuOffset,sizeof(float));
    //д������ƽ��v����
    buffer.write((char*)&m_Material.MatData.SimpleMat.gvfOffset,sizeof(float)); 
    //д��������ת����
    buffer.write((char*)&m_Material.MatData.SimpleMat.fRotAngle,sizeof(float));
    //д��߹�
    buffer.write((char*)&m_Material.MatData.SimpleMat.fShininess,sizeof(float));
    //д���ն�
    buffer.write((char*)&m_Material.MatData.SimpleMat.fIllumination,sizeof(float));
    //д������ID
    buffer.write((char*)&m_Material.MatData.SimpleMat.nTextureID,sizeof(UID64));
}


//********************************************************************************** 
//** �������ƣ� readComplexMatBuff
//** ���������� �ӻ������ж����Ӳ�������
//** ��    �룺 buffer:Buffer���������           
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//** ��    �ģ� �ΰ 20130829 ��չ���Ӳ���
//**********************************************************************************
void CMaterial::readComplexMatBuff(Buffer &buffer)
{
	//д����ʸ���
	buffer.read((char*)&m_Material.MatData.ComPlexMat.nItemNum,sizeof(int));
    //�������Ӳ�������
    m_Material.MatData.ComPlexMat.pComm_item = new CT3D::COMMAT_ITEM[m_Material.MatData.ComPlexMat.nItemNum];
    CT3D::COMMAT_ITEM* pMat = m_Material.MatData.ComPlexMat.pComm_item;
	for(int nMat = 0; nMat < m_Material.MatData.ComPlexMat.nItemNum;nMat++)
	{
		
		buffer.read((char*)&pMat[nMat].nAmbient,sizeof(unsigned long));
	
		buffer.read((char*)&pMat[nMat].nDiffuse,sizeof(unsigned long));
	
		buffer.read((char*)&pMat[nMat].nSpecular,sizeof(unsigned long));
	
		buffer.read((char*)&pMat[nMat].nEmissive,sizeof(unsigned long));
	
		buffer.read((char*)&pMat[nMat].bDepthCheck,sizeof(bool));
		buffer.read((char*)&pMat[nMat].bDepthWrite,sizeof(bool));
		//��Ⱥ������ã�����ΪD3DCMPFUNC
		buffer.read((char*)&pMat[nMat].nDepthFunc,sizeof(int));
		//�����ں����ӣ�����ΪD3DBLEND
		int nSourceBlendFactor;
		buffer.read((char*)&pMat[nMat].nSourceBlendFactor,sizeof(int));
		buffer.read((char*)&pMat[nMat].nDestBlendFactor,sizeof(int));
		//�����ںϲ���������ΪD3DBLENDOP��ȱʡֵΪD3DBLENDOP_ADD
		buffer.read((char*)&pMat[nMat].nBlendOperation,sizeof(int));
		buffer.read((char*)&pMat[nMat].nAlphaBlendOperation,sizeof(int));
		//Alpha�������ã�����ΪD3DCMPFUNC��ȱʡֵΪD3DCMP_ALWAYS
		buffer.read((char*)&pMat[nMat].nAlphaRejectFunc,sizeof(int));
		// Alpha����ֵ
		buffer.read((char*)&pMat[nMat].nAlphaRejectVal,sizeof(unsigned char));
		/** ��ɫģʽ������ΪD3DSHADEMODE��ȱʡֵΪD3DSHADE_FLAT*/
		buffer.read((char*)&pMat[nMat].nShadeMode,sizeof(int));
		//�޳���ʽ������ΪD3DCULL��ȱʡֵΪD3DCULL_CW
		buffer.read((char*)&pMat[nMat].nCullMode,sizeof(int));
		//ͼԪ�Ļ��Ʒ�ʽ������ΪD3DFILLMODE��ȱʡΪD3DFILLMODE_SOLID
		buffer.read((char*)&pMat[nMat].nPolygonMode,sizeof(int));    
		//д����ʸ���
		buffer.read((char*)&pMat[nMat].nItemNum,sizeof(int)); 
        //������������
        pMat[nMat].pComm_item = new CT3D::TEXTURE_ITEM[pMat[nMat].nItemNum];
		//�õ����Ӳ�������
        CT3D::TEXTURE_ITEM *p = pMat[nMat].pComm_item;
		for(int i = 0; i < pMat[nMat].nItemNum;i++)
		{
			//��ȡ��������ID
			buffer.read((char*)&(p[i].nTextureID),sizeof(UINT64));
			//�������꼯����
			buffer.read((char*)&(p[i].nTexIndex),sizeof(unsigned int));
			/********************************���ں���ɫ����***************************/
			//�ں�����  0 : ��ɫ�� 1 ��Alpha */
			buffer.read((char*)&(p[i].nColorBlendMode.nBlendType),sizeof(int));
			//�ںϲ���������ΪD3DTEXTUREOP,ȱʡֵΪD3DTOP_MODULATE=4 */
			buffer.read((char*)&(p[i].nColorBlendMode.nOperation),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_CURRENT=1 */
			buffer.read((char*)&(p[i].nColorBlendMode.nSource1),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_TEXTURE=2 */
			buffer.read((char*)&(p[i].nColorBlendMode.nSource2),sizeof(int));
			/**************************************************************************/

			/********************************���ں�Alpha����***************************/
			//�ں�����  0 : ��ɫ�� 1 ��Alpha */
			buffer.read((char*)&(p[i].nAlphaBlendMode.nBlendType),sizeof(int));
			//�ںϲ���������ΪD3DTEXTUREOP,ȱʡֵΪD3DTOP_MODULATE=4 */
			buffer.read((char*)&(p[i].nAlphaBlendMode.nOperation),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_CURRENT=1 */
			buffer.read((char*)&(p[i].nAlphaBlendMode.nSource1),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_TEXTURE=2 */
			buffer.read((char*)&(p[i].nAlphaBlendMode.nSource2),sizeof(int));
			/**************************************************************************/

			/********************************����Ѱַģʽ******************************/
			/** U�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
			buffer.read((char*)&(p[i].nTextureAddressingMode.nUMode),sizeof(int));
			/** V�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
			buffer.read((char*)&(p[i].nTextureAddressingMode.nVMode),sizeof(int));
			/** W�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
			buffer.read((char*)&(p[i].nTextureAddressingMode.nWMode),sizeof(int));  
			/***************************************************************************/

			//����߿�Ѱַģʽ�еı߿���ɫ
			buffer.read((char*)&(p[i].nBorderColour),sizeof(unsigned long));
			//�������ģʽ������ΪD3DTEXTUREFILTERTYPE, ȱʡֵΪD3DTEXF_LINEAR
			buffer.read((char*)&(p[i].nMagFilter),sizeof(int));
			buffer.read((char*)&(p[i].nMinFilter),sizeof(int));
			buffer.read((char*)&(p[i].nMipFilter),sizeof(int));
			//��u v��������ƽ��
			buffer.read((char*)&(p[i].dUScroll),sizeof(double));
			buffer.read((char*)&(p[i].dVScroll),sizeof(double));
			//��u v������������
			buffer.read((char*)&(p[i].dUScale),sizeof(double));
			buffer.read((char*)&(p[i].dVScale),sizeof(double));
			//������ת
			buffer.read((char*)&(p[i].dRotate),sizeof(double));
		}
		p = NULL;
	}
	pMat = NULL;   
}

//********************************************************************************** 
//** �������ƣ� writeComplexMatBuff
//** ���������� �����Ӳ�������д��������
//** ��    �룺 buffer:Buffer���������          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-16
//** ��    �ģ� �ΰ 20130829 ��չ���Ӳ���
//**********************************************************************************
void CMaterial::writeComplexMatBuff(Buffer &buffer)
{
	//д����ʸ���
	buffer.write((char*)&m_Material.MatData.ComPlexMat.nItemNum,sizeof(int));
	CT3D::COMMAT_ITEM* pMat = m_Material.MatData.ComPlexMat.pComm_item;
	for(int nMat = 0; nMat < m_Material.MatData.ComPlexMat.nItemNum;nMat++)
	{
		//��������ɫ
		buffer.write((char*)&pMat[nMat].nAmbient,sizeof(unsigned long));
		//ɢ�����ɫ
		buffer.write((char*)&pMat[nMat].nDiffuse,sizeof(unsigned long));
		//�۹���ɫ
		buffer.write((char*)&pMat[nMat].nSpecular,sizeof(unsigned long));
		//�Է�����ɫ
		buffer.write((char*)&pMat[nMat].nEmissive,sizeof(unsigned long));
		// �������
		buffer.write((char*)&pMat[nMat].bDepthCheck,sizeof(bool));
		buffer.write((char*)&pMat[nMat].bDepthWrite,sizeof(bool));
		//��Ⱥ������ã�����ΪD3DCMPFUNC
		buffer.write((char*)&pMat[nMat].nDepthFunc,sizeof(int));
		//�����ں����ӣ�����ΪD3DBLEND
		int nSourceBlendFactor;
		buffer.write((char*)&pMat[nMat].nSourceBlendFactor,sizeof(int));
		buffer.write((char*)&pMat[nMat].nDestBlendFactor,sizeof(int));
		//�����ںϲ���������ΪD3DBLENDOP��ȱʡֵΪD3DBLENDOP_ADD
		buffer.write((char*)&pMat[nMat].nBlendOperation,sizeof(int));
		buffer.write((char*)&pMat[nMat].nAlphaBlendOperation,sizeof(int));
		//Alpha�������ã�����ΪD3DCMPFUNC��ȱʡֵΪD3DCMP_ALWAYS
		buffer.write((char*)&pMat[nMat].nAlphaRejectFunc,sizeof(int));
		// Alpha����ֵ
		buffer.write((char*)&pMat[nMat].nAlphaRejectVal,sizeof(unsigned char));
		/** ��ɫģʽ������ΪD3DSHADEMODE��ȱʡֵΪD3DSHADE_FLAT*/
		buffer.write((char*)&pMat[nMat].nShadeMode,sizeof(int));
		//�޳���ʽ������ΪD3DCULL��ȱʡֵΪD3DCULL_CW
		buffer.write((char*)&pMat[nMat].nCullMode,sizeof(int));
		//ͼԪ�Ļ��Ʒ�ʽ������ΪD3DFILLMODE��ȱʡΪD3DFILLMODE_SOLID
		buffer.write((char*)&pMat[nMat].nPolygonMode,sizeof(int));    
		//д����ʸ���
		buffer.write((char*)&pMat[nMat].nItemNum,sizeof(int));    
		//�õ����Ӳ�������
		CT3D::TEXTURE_ITEM *p = pMat[nMat].pComm_item;
		for(int i = 0; i < pMat[nMat].nItemNum;i++)
		{
			//��ȡ��������ID
			buffer.write((char*)&(p[i].nTextureID),sizeof(UINT64));
			//�������꼯����
			buffer.write((char*)&(p[i].nTexIndex),sizeof(unsigned int));
			/********************************���ں���ɫ����***************************/
			//�ں�����  0 : ��ɫ�� 1 ��Alpha */
			buffer.write((char*)&(p[i].nColorBlendMode.nBlendType),sizeof(int));
			//�ںϲ���������ΪD3DTEXTUREOP,ȱʡֵΪD3DTOP_MODULATE=4 */
			buffer.write((char*)&(p[i].nColorBlendMode.nOperation),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_CURRENT=1 */
			buffer.write((char*)&(p[i].nColorBlendMode.nSource1),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_TEXTURE=2 */
			buffer.write((char*)&(p[i].nColorBlendMode.nSource2),sizeof(int));
			/**************************************************************************/

			/********************************���ں�Alpha����***************************/
			//�ں�����  0 : ��ɫ�� 1 ��Alpha */
			buffer.write((char*)&(p[i].nAlphaBlendMode.nBlendType),sizeof(int));
			//�ںϲ���������ΪD3DTEXTUREOP,ȱʡֵΪD3DTOP_MODULATE=4 */
			buffer.write((char*)&(p[i].nAlphaBlendMode.nOperation),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_CURRENT=1 */
			buffer.write((char*)&(p[i].nAlphaBlendMode.nSource1),sizeof(int));
			/** �ںϲ����ĵ�һ��Դ��ȱʡֵΪD3DTA_TEXTURE=2 */
			buffer.write((char*)&(p[i].nAlphaBlendMode.nSource2),sizeof(int));
			/**************************************************************************/

			/********************************����Ѱַģʽ******************************/
			/** U�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
			buffer.write((char*)&(p[i].nTextureAddressingMode.nUMode),sizeof(int));
			/** V�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
			buffer.write((char*)&(p[i].nTextureAddressingMode.nVMode),sizeof(int));
			/** W�����ϵ�����Ѱַģʽ������ΪD3DTEXTUREADDRESS��ȱʡֵΪD3DTADDRESS_WRAP = 1 */
			buffer.write((char*)&(p[i].nTextureAddressingMode.nWMode),sizeof(int));  
			/***************************************************************************/

			//����߿�Ѱַģʽ�еı߿���ɫ
			buffer.write((char*)&(p[i].nBorderColour),sizeof(unsigned long));
			//�������ģʽ������ΪD3DTEXTUREFILTERTYPE, ȱʡֵΪD3DTEXF_LINEAR
			buffer.write((char*)&(p[i].nMagFilter),sizeof(int));
			buffer.write((char*)&(p[i].nMinFilter),sizeof(int));
			buffer.write((char*)&(p[i].nMipFilter),sizeof(int));
			//��u v��������ƽ��
			buffer.write((char*)&(p[i].dUScroll),sizeof(double));
			buffer.write((char*)&(p[i].dVScroll),sizeof(double));
			//��u v������������
			buffer.write((char*)&(p[i].dUScale),sizeof(double));
			buffer.write((char*)&(p[i].dVScale),sizeof(double));
			//������ת
			buffer.write((char*)&(p[i].dRotate),sizeof(double));
		}
		p = NULL;
	}
	pMat = NULL;  
}

//********************************************************************************** 
//** �������ƣ� readTexureBuff
//** ���������� �ӻ������ж���������
//** ��    �룺 buffer:Buffer���������           
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-06-11
//**********************************************************************************
void CMaterial::readTexureBuffer(Buffer &buffer)
{
        m_Material.nMaterialDataLen = buffer.size();
        m_Material.MatData.pcTextureBuf = new char[m_Material.nMaterialDataLen];
        buffer.read(m_Material.MatData.pcTextureBuf,m_Material.nMaterialDataLen);     
}

//********************************************************************************** 
//** �������ƣ� writeTexureBuffer
//** ���������� ����������д��������
//** ��    �룺 buffer:Buffer���������          
//** ��    ���� ��
//** ����ֵ����
//** ��    �ߣ� ����
//** �������ڣ� 2012-06-11
//**********************************************************************************
void CMaterial::writeTexureBuffer(Buffer &buffer)
{
      
    buffer.write(m_Material.MatData.pcTextureBuf,m_Material.nMaterialDataLen);
    
}
void CMaterial::setMaterialID(UINT64 nMaterialID)
{
	m_Material.unMaterialID=nMaterialID;
}
UINT64 CMaterial::getMaterialID()
{
	return m_Material.unMaterialID;
}

void CMaterial::setMaterialType(int nMaterialType)
{
	m_Material.byMaterialType=nMaterialType;
}
int CMaterial::getMaterialType()
{
	return m_Material.byMaterialType;
}

void CMaterial::setMaterialName(string strMaterialName)
{
	strcpy(m_Material.caMaterialName,strMaterialName.c_str());
}
string CMaterial::getMaterialName()
{
	return m_Material.caMaterialName;
}
int CMaterial::getMaterialNameLen()
{
	return strlen(m_Material.caMaterialName);
}

void CMaterial::setSimpleMat(CT3D::MAT_PARAM v)
{
	m_Material.MatData.SimpleMat=v;
}
CT3D::MAT_PARAM CMaterial::getSimpleMat()
{
	return m_Material.MatData.SimpleMat;
}

void CMaterial::setComPlexMat(CT3D::ComMat_PARAM v)
{
	m_Material.MatData.ComPlexMat=v;	
}
CT3D::ComMat_PARAM CMaterial::getComPlexMat()
{
	return m_Material.MatData.ComPlexMat;
}

void CMaterial::setDataLen(int nDataLen)
{
	m_Material.nMaterialDataLen=nDataLen;
}
int CMaterial::getDataLen()
{
	return m_Material.nMaterialDataLen;
}
//********************************************************************************** 
//** �������ƣ� sizeBuffer
//** ���������� �õ����ʴ�С
//** ��    �룺 ��          
//** ��    ���� ��
//** ����ֵ�����ʴ�С
//** ��    �ߣ� ����
//** �������ڣ� 2012-04-11
//** ��    �ģ� �ΰ 20130829 ��չ���Ӳ���
//**********************************************************************************
size_t CMaterial::sizeBuffer()
{
    size_t sMaterialSize =0;
    //����
    if( 0 == m_Material.byMaterialType )
    {
        sMaterialSize = sizeof(UINT64) + sizeof(byte) + sizeof(int) + m_Material.nMaterialNameLen
            + sizeof(int) + m_Material.nMaterialDataLen;
    }
    else if (1 == m_Material.byMaterialType)//�򵥲���
    {
        sMaterialSize = sizeof(UINT64) + sizeof(byte) + sizeof(int) + m_Material.nMaterialNameLen
            + sizeof(int) + 56;
    }
    else if(2 == m_Material.byMaterialType)//���Ӳ���
    {
        sMaterialSize = sizeof(UINT64) + sizeof(byte) + sizeof(int) + m_Material.nMaterialNameLen
            + sizeof(int) + sizeof(int);
		for (int i = 0;i<m_Material.MatData.ComPlexMat.nItemNum;i++)
		{
			sMaterialSize += 4* sizeof(unsigned long) + 2*sizeof(bool) + 6*sizeof(int) + sizeof(unsigned char) + 4 * sizeof(int) ;
			int nTexNum = m_Material.MatData.ComPlexMat.pComm_item[i].nItemNum;
			CT3D::Texture_Item *ptex = m_Material.MatData.ComPlexMat.pComm_item[i].pComm_item;

			for (int j = 0;j<nTexNum;j++)	
			{
				sMaterialSize += sizeof(UINT64) + sizeof(unsigned int) + 4*sizeof(int) + 4*sizeof(int) + 3*sizeof(int) + sizeof(unsigned long) + 3*sizeof(int) + 5*sizeof(double);
			}
		}		
    }
    else
    {
        sMaterialSize = 0;
    }
    return sMaterialSize;
}
//********************************************************************************** 
//** �������ƣ� getFileSize
//** ���������� �õ��ļ���С
//** ��    �룺 ��         
//** ��    ���� ��
//** ����ֵ���ļ���С
//** ��    �ߣ� ����
//** �������ڣ� 2012-06-11
//**********************************************************************************
size_t CMaterial::getFileSize(string strFileName)
{
    
    if(m_Material.byMaterialType == 0)
    {
        struct stat f_stat;
        if(stat(strFileName.c_str(),&f_stat)==-1)
        {
            return -1;
        }
        return f_stat.st_size;
    }
    return 0;
}
void CMaterial::setTextureFolderPath(string strTextureFolderPath)
{
    m_strTextureFolderPath = strTextureFolderPath;
}

TexIDList CMaterial::getTextureID()
{
    m_TexIDList.clear();
    //����
    if( 0 == m_Material.byMaterialType )
    {
        m_TexIDList.push_back(m_Material.unMaterialID);
    }
    else if (1 == m_Material.byMaterialType)//�򵥲���
    {
        m_TexIDList.push_back(m_Material.MatData.SimpleMat.nTextureID);
    }
    else if(2 == m_Material.byMaterialType)//���Ӳ���
    {
        if (!m_Material.MatData.ComPlexMat.pComm_item)
            return m_TexIDList;

        for(int i = 0; i <  m_Material.MatData.ComPlexMat.nItemNum; i++)
        {
        	CT3D::COMMAT_ITEM& comItem = m_Material.MatData.ComPlexMat.pComm_item[i];

            if (!comItem.pComm_item)
                return m_TexIDList;
        
            for(int j = 0; j <comItem.nItemNum; j++)
            {
            	CT3D::Texture_Item& texItem = comItem.pComm_item[j];

                 m_TexIDList.push_back(texItem.nTextureID);

                 
            }
        }  
    }
    return m_TexIDList;
}
