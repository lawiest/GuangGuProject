#pragma once
 
#include <utility>
#define GS_DO_JOIN2(a, b)    a##b
#define GS_DO_JOIN(a, b)     GS_DO_JOIN2(a, b)  // a##b
#define GS_JOIN(a, b)        GS_DO_JOIN(a, b)   // a##b

/// \brief 创建接口类
template <class TSignature>
class GsDelegate;


/// \brief 无参数的接口
#define GS_DELEGATE_NUM_ARGS	0 
#define GS_DELEGATE_TEMPLATE_ARGS 
#define GS_DELEGATE_TEMPLATE_PARAMS
#define GS_DELEGATE_FUNCTION_ARGS
#define GS_DELEGATE_FUNCTION_PARAMS
#include "delegateT.h"

/// \brief 1个参数和1种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	1 
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1
#define GS_DELEGATE_TEMPLATE_ARGS  T1
#define GS_DELEGATE_FUNCTION_ARGS t1
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1
#include "delegateT.h" 

/// \brief 2个参数和2种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	2 
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2
#define GS_DELEGATE_FUNCTION_ARGS t1,t2
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2
#include "delegateT.h" 

/// \brief 3个参数和3种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	3 
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3
#include "delegateT.h"  

/// \brief 4个参数和4种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	4 
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3,typename T4
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3,T4
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3,t4
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3,T4 t4
#include "delegateT.h"  

/// \brief 5个参数和5种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	5
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3,typename T4,typename T5
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3,T4,T5
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3,t4,t5
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3,T4 t4,T5 t5
#include "delegateT.h"  

/// \brief 6个参数和6种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	6 
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3,typename T4,typename T5,typename T6
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3,T4,T5,T6
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3,t4,t5,t6
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6
#include "delegateT.h"  

/// \brief 7个参数和7种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	7
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3,T4,T5,T6,T7
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3,t4,t5,t6,t7
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7
#include "delegateT.h"  

/// \brief 8个参数和8种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	8
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3,T4,T5,T6,T7,T8
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3,t4,t5,t6,t7,t8
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7,T8 t8
#include "delegateT.h"  

/// \brief 9个参数和9种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	9 
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3,T4,T5,T6,T7,T8,T9
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3,t4,t5,t6,t7,t8,t9
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7,T8 t8,T9 t9
#include "delegateT.h"  

/// \brief 10个参数和10种参数类型的接口
#define GS_DELEGATE_NUM_ARGS	10 
#define GS_DELEGATE_TEMPLATE_PARAMS typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10
#define GS_DELEGATE_TEMPLATE_ARGS  T1,T2,T3,T4,T5,T6,T7,T8,T9,T10
#define GS_DELEGATE_FUNCTION_ARGS t1,t2,t3,t4,t5,t6,t7,t8,t9,t10
#define GS_DELEGATE_FUNCTION_PARAMS T1 t1,T2 t2,T3 t3,T4 t4,T5 t5,T6 t6,T7 t7,T8 t8,T9 t9,T10 t10
#include "delegateT.h"  
