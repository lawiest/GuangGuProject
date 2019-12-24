//#include "StdAfx.h"
#include "PipeTileParse.h"


bool CPBPipeTile::GetPipeFeature(CPipeFeatureBase** pPipeFeature,long nIndex)
{
	if (nIndex > m_nPipeNum-1)
	{
		return false;
	}
	
	*pPipeFeature = NULL;
	GEOPIPEMODEL::PIPE* pPipe = m_PipeTile.mutable_pipes(nIndex);
	if (pPipe == NULL)
	{
		return false;
	}

	if (GEOPIPEMODEL::ePoint == pPipe->geometrytype())
	{
		//管点
		*pPipeFeature = new CPipePointFeature();
		if (!(*pPipeFeature)->Serialize(pPipe))
		{
			delete *pPipeFeature;
		}
	}
	if (GEOPIPEMODEL::eLine == pPipe->geometrytype())
	{
		//管段
		*pPipeFeature = new CPipeLineFeature(); 
		if (!(*pPipeFeature)->Serialize(pPipe))
		{
			delete *pPipeFeature;
		}
	}

	return true;
}

bool CPBPipeTile::GetPipeLineFeature(CPipeLineFeature& PipeLineFeature,long nIndex)
{
	if (nIndex >= m_nPipeNum)
	{
		return false;
	}

	GEOPIPEMODEL::PIPE* pPipe = m_PipeTile.mutable_pipes(nIndex);
	if(pPipe == NULL)
		int a=0;

	if (pPipe == NULL || GEOPIPEMODEL::ePoint == pPipe->geometrytype())
	{
		return false;
	}

	if (GEOPIPEMODEL::eLine == pPipe->geometrytype())
	{
		//管段
		PipeLineFeature.Serialize(pPipe);
	}

	return true;
}

bool CPBPipeTile::GetPipePointFeature(CPipePointFeature& PipePointFeature,long nIndex)
{
	if (nIndex >= m_nPipeNum)
	{
		return false;
	}

	GEOPIPEMODEL::PIPE* pPipe = m_PipeTile.mutable_pipes(nIndex);
	if (pPipe == NULL || GEOPIPEMODEL::eLine == pPipe->geometrytype())
	{
		return false;
	}

	if (GEOPIPEMODEL::ePoint == pPipe->geometrytype())
	{
		//管段
		PipePointFeature.Serialize(pPipe);
	}

	return true;
}