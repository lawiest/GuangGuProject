#pragma once
#include "dcoreader.h"
#include "dcowriter.h" 
GDC_NS

class GS_API GsDCOTranslater :public UTILITY_NAME::GsRefObject
{
	GsVector<Utility::GsString> m_vecParameter;
public:
	GsDCOTranslater(const GsVector<Utility::GsString>& vecParameter);
	~GsDCOTranslater();
	/// \brief 执行数据转换
	/// \param mapFile 映射文件的路径
	bool Translate(const char* mapFile);
};

GDC_ENDNS