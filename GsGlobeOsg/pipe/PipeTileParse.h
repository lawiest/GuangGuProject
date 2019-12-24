//#pragma once
//#include "SafeArrayVector.h"
//#include "GeoPipeModel.pb.cc"
#include "PipeFeature.h"

#include "Pipe/GeoPipeModel.pb.h"
class CPipePointFeature;
class CPipeLineFeature;
class CPipeFeatureBase;
class CPBPipeTile
{
	GEOPIPEMODEL::PIPETile m_PipeTile;
	long m_nPipeNum;
public:
	CPBPipeTile(void)
	{
		m_nPipeNum = 0;
	}
	virtual ~CPBPipeTile(void)
	{

	}
	//��Ƭ��Protobuffָ��
	GEOPIPEMODEL::PIPETile* TilePtr()
	{
		return &m_PipeTile;
	}

	long PipeCount(){return m_nPipeNum;}
	//���ڴ�����л�������
	virtual bool Serialize(const unsigned char* pBuff,int nSize)
	{
		if (m_PipeTile.ParseFromArray(pBuff,nSize))
		{
			m_nPipeNum = m_PipeTile.pipes_size();
			return true;
		}
		m_nPipeNum = m_PipeTile.pipes_size();
		return true;	//TODO:ParseFromArray returnֵ������
	}
	//virtual bool Serialize(VARIANT& pVarData)
	//{
	//	CSafeArrayVector<unsigned char>& var = (CSafeArrayVector<unsigned char>&)pVarData;
	//	return Serialize(var.begin(),var.size());
	//}

	bool GetPipeLineFeature(CPipeLineFeature& pPipeLineFeature,long nIndex);
	bool GetPipeFeature(CPipeFeatureBase** pPipeFeature,long nIndex);
	bool GetPipePointFeature(CPipePointFeature& PipePointFeature,long nIndex);

	//ת����Variant
	//virtual void UnSerialize(VARIANT* var)
	//{
	//	CSafeArrayVector<unsigned char> varOut;
	//	varOut.resize(m_PipeTile.ByteSize());
	//	m_PipeTile.SerializeToArray(varOut.begin(),varOut.size());
	//	varOut.Detach(var);
	//}
	long long Version()
	{
		m_PipeTile.version();
	}
	void Version(long long  ver)
	{
		m_PipeTile.set_version(ver);
	}

	long long TileKey()
	{
		return m_PipeTile.tilekey();
	}
	void TileKey(long long nKey)
	{
		m_PipeTile.set_tilekey(nKey);
	}
	//����Ϊ64λ��key
	void TileKey(int l,int r,int c)
	{
		long long nKey = 0;
		m_PipeTile.set_tilekey(nKey);
	}

	//��TileKey������level��row��col
	int Level()
	{
		long long nKey = m_PipeTile.tilekey();
		return nKey;
	}
	int Row()
	{
		long long nKey = m_PipeTile.tilekey();
		return nKey;
	}
	int Col()
	{
		long long nKey = m_PipeTile.tilekey();
		return nKey;
	}
	

};

class CPBPipe
{
	GEOPIPEMODEL::PIPE* m_Pipe; 
	GEOPIPEMODEL::PIPETile* m_pTile;
public:
	CPBPipe(GEOPIPEMODEL::PIPE* Pipe,GEOPIPEMODEL::PIPETile* pTile)
	{
		m_pTile = pTile;
		m_Pipe = Pipe;
	}
};
