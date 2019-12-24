//********************************************************************************** 
//** �ļ����ƣ�CT3D_TexturedVertex3d.cpp
//** CopyRight (c) 2000-2012 ��󼪰���Ϣ�������޹�˾����һ��
//** �ļ������� ��ά����(��������ӳ��Ķ���)���ʵ��
//** �� �� �ˣ� ������
//** �������ڣ� 2014-08-27
//********************************************************************************** 


#include "CT3D_NormaledVertex3f.h"

//********************************************************************************** 
//** �������ƣ� ShadedVertex3f
//** ���������� ���캯��1
//** ��    �룺 ��           
//** ��    ���� ��
//** ����ֵ��	��
//** ��    �ߣ� ������
//** �������ڣ� 2014-08-27
//**********************************************************************************
NormaledVertex3f::NormaledVertex3f(float dx,float dy,float dz,float nx,float ny,float nz)
:Vertex3f(dx,dy,dz),
nx(nx),
ny(ny),
nz(nz)
{

}

//********************************************************************************** 
//** �������ƣ� ShadedVertex3f
//** ���������� ���캯��2
//** ��    �룺 ��           
//** ��    ���� ��
//** ����ֵ��	��
//** ��    �ߣ� ������
//** �������ڣ� 2014-08-27
//**********************************************************************************
NormaledVertex3f::NormaledVertex3f(Vertex3f& vVertex,float nx,float ny,float nz)
:Vertex3f(vVertex),
nx(nx),
ny(ny),
nz(nz)
{

}

//********************************************************************************** 
//** �������ƣ� sizeBuffer
//** ����������������ά��ṹ�Ĵ�С
//** ��    �룺 ��           
//** ��    ���� ��
//** ����ֵ��	��ṹ��С
//** ��    �ߣ� ������
//** �������ڣ� 2014-08-27
//**********************************************************************************
size_t NormaledVertex3f::sizeBuffer()
{
    size_t nSize = Vertex3f::sizeBuffer() + 3*sizeof(float);
    return nSize;
}