//********************************************************************************** 
//** 文件名称：CT3D_MathEx.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 三维数学类定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-13
//********************************************************************************** 

#ifndef _CT3D_MATH_H_
#define _CT3D_MATH_H_

//#pragma once
#include <math.h>
#include <functional>
#include <algorithm>
#include <iterator>
//#include "ModelDef.h"


//#ifdef /*DLL_EXPORT*/_EXPORTS
//#define /*DLL_EXPORT*/ __declspec(dllexport)
//#else
//#define /*DLL_EXPORT*/ __declspec(dllimport)
//#endif


class  MathEx
{
	/** @defgroup MathEx MathEx-数学库 
    *  @{
    */

public:

	
	/** @name 属性（成员变量）
	* @{
	*/
	
	/** 成员变量 */	
	static const double  TOL ;
	static const double	 TOL_F;
	static const double  ONE_PI;
	static const double  TWO_PI ;
	static const double  HALF_PI ;
	static const double  dDegToRad;
	static const double  dRadToDeg;
	static const double  dDoubleMax;
	static const double  dDoubleMin;

	/** @} */ // 属性结尾

	/** @defgroup MathEx 操作（成员函数）
	* @{
	*/

	/** @name 构造与析构函数
	*  @{
	*/
protected:
	/** 构造函数
	*/
	MathEx(void);
	/** 析构函数
	*/
	~MathEx(void);
	/** @} */ // 构造与析构函数结尾
	
	/** @name 计算操作
	*  @{
	*/
public:

    /** 判断符号
	* @param [in] iValue 输入数值	
	* @return 正负
    */
	static inline int ISign (int iValue){return ( iValue > 0 ? +1 : ( iValue < 0 ? -1 : 0 ) );}
	/** 判断符号
	* @param [in] dValue 输入数值	
	* @return 正负
	* -1 表示为正
	* --1 表示为负
	* -0 表示为0
    */
	static double Sign(double dValue);
	/** 计算反余弦值
	* @param [in] fValue 输入数据	
	* @return 角度
    */
	static double ACos (double fValue);	

	/** 计算反正弦值
	* @param [in] fValue 输入数据	
	* @return 角度
    */
	static double ASin (double fValue);

	/** 计算反正切值
	* @param [in] dValue 输入数据	
	* @return 角度
    */
	static inline double ATan (double fValue) { return (atan(fValue)); }
	
	/** 计算正弦函数
	* @param [in] dValue 数据	
	* @return 正弦值
    */
	static inline double ATan2 (double fY, double fX) { return (atan2(fY,fX)); }

	/** 计算正弦函数
	* @param [in] dValue 数据	
	* @return 正弦值
    */
	static  double RadianToDegree(double angle);

	/** 计算正弦函数
	* @param [in] dValue 数据	
	* @return 正弦值
    */
	static double DegreeToRadian(double angle);

	/** 使用标准模版库（STL）的泛型式快速排序，来源于wikipedia
	* @param BidirectionalIterator:	列表的迭代器类型
	* @param Compare:两个对象的比较函数
	* @return 
	*/
	template< typename BidirectionalIterator, typename Compare >
	static void quick_sortEx( BidirectionalIterator first, BidirectionalIterator last, Compare cmp ) {
		if( first != last ) {
			BidirectionalIterator left  = first;
			BidirectionalIterator right = last;
			BidirectionalIterator pivot = left++;

			while( left != right ) {
				if( cmp( *left, *pivot ) ) {
					++left;
				} else {
					while( (left != right) && cmp( *pivot, *right ) )
						right--;
					std::iter_swap( left, right );
				}
			}

			if(cmp( *pivot, *left ))
				--left;
			std::iter_swap( first, left );

			quick_sortEx( first, left, cmp );
			quick_sortEx( right, last, cmp );
		}
	}
	/** 使用标准模版库（STL）的泛型式快速排序，来源于wikipedia
	* @param BidirectionalIterator:	列表的迭代器
	* @return 
	*/
	template< typename BidirectionalIterator >
	static void quick_sort( BidirectionalIterator first, BidirectionalIterator last ) {
		quick_sortEx( first, last,
			std::less_equal< typename std::iterator_traits< BidirectionalIterator >::value_type >()
			);
	}
	/** @} */ // 计算操作组结尾

	/** @} */ // 操作结尾

	/** @} */ // 模块结尾
	
};


#endif//_CT3D_MATH_H_
