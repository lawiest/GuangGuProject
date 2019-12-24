#include "HeightAnalysisCallBack.h"

HeightAnalysisCallBack::HeightAnalysisCallBack()
{
}


HeightAnalysisCallBack::~HeightAnalysisCallBack()
{

}


void HeightAnalysisCallBack::OnFinish(double dbResult)
{
	m_dbResult = dbResult;
}