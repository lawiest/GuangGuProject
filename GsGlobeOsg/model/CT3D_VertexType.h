//********************************************************************************** 
//** 文件名称：CT3D_VertexType.h
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 几何顶点定义
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-7
//********************************************************************************** 
#ifndef _VERTEXTYPE_H_
#define _VERTEXTYPE_H_

//********************************************************************************** 
//**简记法释义：	V-（三维坐标点）
//**T纹理坐标（0～无穷大）
//**N-法线（0.0～1.0）
//**C-颜色（0.0～1.0）
//**********************************************************************************
enum
{
    VERTEXTYPE_V3f ,				// for Vertex3f
    VERTEXTYPE_V3d,				// for Vertex3d

    VERTEXTYPE_V3fT2f,			// for TexturedVertex3f
    VERTEXTYPE_V3dT2d,			// for TexturedVertex3d

    VERTEXTYPE_V3fN3fC3f,		// for ShadedVertex3f
    VERTEXTYPE_V3dN3dC3f,		// for ShadedVertex3d

    VERTEXTYPE_V3fT2fN3fC3f,	// for VersatileVertex3f
    VERTEXTYPE_V3dT2dN3dC3f,	// for VersatileVertex3d
};


struct Vertex3f
{
    /** X坐标值 */
    float x;

    /** Y坐标值 */
    float y;

    /** Z坐标值 */
    float z;
};

struct Vertex3d
{
    /** X坐标值 */
    double x;

    /** Y坐标值 */
    double y;

    /** Z坐标值 */
    double z;
};

struct TexturedVertex3f
{
    /** X坐标值 */
    float x;

    /** Y坐标值 */
    float y;

    /** Z坐标值 */
    float z;
   
    /** 纹理坐标 */
    float tu;
 
    /** 纹理坐标 */
    float tv;
};


struct TexturedVertex3d
{
    /** X坐标值 */
    double x;

    /** Y坐标值 */
    double y;

    /** Z坐标值 */
    double z;

    /** 纹理坐标 */
    double tu;

    /** 纹理坐标 */
    double tv;
};

struct ShadedVertex3f
{
    /** X坐标值 */
    float x;

    /** Y坐标值 */
    float y;

    /** Z坐标值 */
    float z;

    /** X轴法线 */
    float nx;
    
    /** Y轴法线 */
    float ny;

    /** Z轴法线 */
    float nz;
 
    /** 颜色r,g,b分量 */
    float r;
    float g;
    float b;
};

struct ShadedVertex3d
{
    /** X坐标值 */
    double x;

    /** Y坐标值 */
    double y;

    /** Z坐标值 */
    double z;

    /** X轴法线 */
    double nx;

    /** Y轴法线 */
    double ny;

    /** Z轴法线 */
    double nz;
    
    /** 颜色r,g,b分量 */
    float r;
    float g;
    float b;
};

struct VersatileVertex3f
{/** X坐标值 */
    float x;

    /** Y坐标值 */
    float y;

    /** Z坐标值 */
    float z;

    /** 纹理坐标 */
    float tu;

    /** 纹理坐标 */
    float tv;

    /** X轴法线 */
    float nx;

    /** Y轴法线 */
    float ny;

    /** Z轴法线 */
    float nz;

    /** 颜色r,g,b分量 */
    float r;
    float g;
    float b;
};

struct VersatileVertex3d
{/** X坐标值 */
    double x;

    /** Y坐标值 */
    double y;

    /** Z坐标值 */
    double z;

    /** 纹理坐标 */
    double tu;

    /** 纹理坐标 */
    double tv;

    /** X轴法线 */
    double nx;

    /** Y轴法线 */
    double ny;

    /** Z轴法线 */
    double nz;

    /** 颜色r,g,b分量 */
    float r;
    float g;
    float b;
};
#endif