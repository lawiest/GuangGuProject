#pragma once
#include "preconfig.h"
#include "linesymbol.h"
#include "pointsymbol.h"

namespace GeoStar
{
	namespace Electric
	{ 
		class GS_API GsElectricMarkerLineSymbol :public KERNEL_NAME::GsLineSymbol
		{
			KERNEL_NAME::GsLineSymbolPtr m_ptrBaseLine;
			KERNEL_NAME::GsPointSymbolPtr m_ptrMarker;
		protected:
			/// \brief 当绘制的时候发生
			/// \details 子类通过覆盖此方法实现实际的绘制
			virtual void OnDraw(KERNEL_NAME::GsGeometryBlob* pBuffer);

			/// \brief 几何数据操作
			/// \details 子类通过覆盖此函数实现绘制前对几何数据的处理，例如计算平行线
			virtual geostar::gobjptr GeometryOperator(geostar::gobjptr& pPath);

			/// \brief 当开始绘制的时候发生
			/// \details 子类通过覆盖此方法实现自定义的数据准备
			virtual void OnStartDrawing();
			/// \brief 当结束绘制的时候发生
			/// \details 子类通过覆盖此方法实现自定义的数据回收过程
			virtual void OnEndDrawing();
		public:
			GsElectricMarkerLineSymbol(KERNEL_NAME::GsLineSymbol* baseLine, KERNEL_NAME::GsPointSymbol* pMarker);
			~GsElectricMarkerLineSymbol();

		};
		GS_SMARTER_PTR(GsElectricMarkerLineSymbol);

	}
}

