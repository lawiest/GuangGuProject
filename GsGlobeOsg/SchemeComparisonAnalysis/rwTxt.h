#ifndef _RWTXT_H_    //如果没有定义这个宏
#define _RWTXT_H_     //定义这个宏
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include<iomanip>//setiosflags 头文件

#include<stdio.h>  
#include<math.h>  
#include<stdlib.h>  


using namespace std;

class RWTxt
{

public:
	RWTxt()
	{
/*
		_FMNum=new ULONGLONG;
		_FPointsArr=new osg::Vec3Array;
		_FMHight=new float;*/
		aFmNums=0;
		aMatNums=0;
		_CutCoorMark=',';
	}

	struct double44Arr
	{
		double matData[4][4];
	};

	class FMData
	{
	public:
		FMData()
		{
			_FMNum=-1;
			_FPointsArr=new osg::Vec3dArray;
			_FMFloorNums=1;
			_FMFloorHeight=3.0;
		}

		~FMData()
		{

		}

		ULONGLONG _FMNum;
		osg::ref_ptr<osg::Vec3dArray> _FPointsArr;
		std::vector<int> _TexNums;
		int _FMFloorNums;
		float _FMFloorHeight;
	};

	class FMatData
	{
	public:
		FMatData()
		{
			fmatNum=-1;
			matData44Arr=new double44Arr;
		}
		~FMatData()
		{

		}

		ULONGLONG fmatNum;
		double44Arr *matData44Arr;
	};

	void ReadCmf(string inFilename,string outFilename)
	{
		ifstream inFile;  
		inFile.open(inFilename); // 打开文件  
		if (!inFile)
		{
			cout<<"Error:RWTXT inFile!"<<endl;
			return;
		}
		string str; // 行字符串缓存 

		ofstream outfile;
		outfile.open(outFilename);
		if (!outfile)
		{
			cout<<"Error:RWTXT outFile!"<<endl;
			return;
		}

		if(inFile.is_open())
		{ // 若成功打开文件  
			while(!inFile.eof())
			{   
				const int cnt = 6;  //一行有几个数
				int j = 0;

				size_t comma = 0;
				size_t comma2 = 0;


				getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符 

				if (str=="")//空行
				{
					continue;
				}
				//房屋编号
				if (str=="m")
				{

#if 1
					if(inFile.is_open())
					{ // 若成功打开文件  
						while(!inFile.eof())
						{ // 若未到文件结束  
							getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符  

							if (str=="")//空行
							{
								continue;
							}
							if (str=="#m")//编号结束
							{
								break;
							}
							aFmNums+=1;
							_AllFMDatas.push_back(new FMData);//每次有新房屋数据，保存

							_AllFMDatas.at(aFmNums-1)->_FMNum=atoi(str.c_str());
							outfile<<atoi(str.c_str())<<endl;
					//		cout<<atoi(str.c_str())<<endl;
						}
					}  
#endif
				}

				if (str=="v")//顶点读取
				{
#if 1
					if(inFile.is_open())
					{ // 若成功打开文件  
						while(!inFile.eof())
						{ // 若未到文件结束  
							getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符  

							if (str=="")//空行
							{
								continue;
							}
							if (str=="#v")//顶点结束
							{
								break;
							}
							osg::Vec3d tempArr;
							comma = str.find(' ',0);//从str的0开始查找返回空格空格位置

							tempArr.x()=atof(str.substr(0,comma).c_str());
							outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(0,comma).c_str())<<' ';
					//		cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(0,comma).c_str())<<' ';

							int num0w=0;
							while (comma < str.size() && j != cnt-1)
							{
								comma2 = str.find(' ',comma + 1);
								outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(comma + 1,comma2-comma-1).c_str());
					//			cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(comma + 1,comma2-comma-1).c_str());

								++j;

								if (num0w==1)
								{
									tempArr.z()=atof(str.substr(comma + 1,comma2-comma-1).c_str());

									outfile<<endl;
					//				cout<<endl;
								}
								else
								{
									tempArr.y()=atof(str.substr(comma + 1,comma2-comma-1).c_str());
									outfile<<' ';
					//				cout<<' ';
								}

								comma = comma2;

								num0w++;
							}

							if (num0w!=2)
							{
								tempArr.z()=0;
					//			cout<<'0'<<endl;
								outfile<<'0'<<endl;

							}

							_AllFMDatas.at(aFmNums-1)->_FPointsArr->push_back(tempArr);
							j = 0;
						}
					}  
#endif
				}
				//楼层高
				if (str=="h")
				{
#if 1
					if(inFile.is_open())
					{ // 若成功打开文件  
						while(!inFile.eof())
						{ // 若未到文件结束  
							getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符  

							if (str=="")//空行
							{
								continue;
							}
							if (str=="#h")//高度结束
							{
								break;
							}
							_AllFMDatas.at(aFmNums-1)->_FMFloorHeight=atof(str.c_str());
							outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.c_str())<<endl;
					//		cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.c_str())<<endl;
						}
					}  
#endif
				}

				//房屋层数
				if (str=="n")
				{
#if 1
					if(inFile.is_open())
					{ // 若成功打开文件  
						while(!inFile.eof())
						{ // 若未到文件结束  
							getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符  

							if (str=="")//空行
							{
								continue;
							}
							if (str=="#n")//高度结束
							{
								break;
							}
							_AllFMDatas.at(aFmNums-1)->_FMFloorNums=atof(str.c_str());
							outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.c_str())<<endl;
					//		cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.c_str())<<endl;
						}
					}  
#endif
				}

				if (str=="t")//纹理编号读取
				{
#if 1
					if(inFile.is_open())
					{ // 若成功打开文件  
						while(!inFile.eof())
						{ // 若未到文件结束  
							getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符  

							if (str=="")//空行
							{
								continue;
							}
							if (str=="#t")//纹理结束
							{
								break;
							}

							osg::Vec3d tempArr;

							_AllFMDatas.at(aFmNums-1)->_TexNums.push_back(atoi(str.c_str()));
					//		cout<<atoi(str.c_str())<<endl;
							outfile<<atoi(str.c_str())<<endl;

						}
					}  

#endif
				}

			}  
		}  
		inFile.close();
		outfile.close();
	}

	void WriteVertexToCmf(std::vector<FMData*> fMData,string outFilename)
	{

		ofstream outfile;
		outfile.open(outFilename);
		if (!outfile)
		{
			cout<<"Error:RWCMF outFile!"<<endl;
			return;
		}

		for (int i=0;i<fMData.size();i++)
		{

			//输出房屋编号
			outfile<<"m"<<endl;
			outfile<<fMData.at(i)->_FMNum<<endl;
			outfile<<"#m"<<endl;
			outfile<<endl;

			//开始输出顶点坐标
			outfile<<"v"<<endl;
			for (osg::Vec3dArray::iterator iter=fMData.at(i)->_FPointsArr->begin();iter!=fMData.at(i)->_FPointsArr->end();iter++)
			{

				outfile<<setiosflags(ios::fixed)<< setprecision(15)<<(*iter).x()
					<<" "<<setiosflags(ios::fixed)<< setprecision(15)<<(*iter).y()
					<<" "<<setiosflags(ios::fixed)<< setprecision(15)<<(*iter).z()<<endl;

			}
			outfile<<"#v"<<endl;
			outfile<<endl;

			//输出楼层高度
			outfile<<"h"<<endl;
			outfile<<fMData.at(i)->_FMFloorHeight<<endl;
			outfile<<"#h"<<endl;
			outfile<<endl;

			//输出楼层数
			outfile<<"n"<<endl;
			outfile<<fMData.at(i)->_FMFloorNums<<endl;
			outfile<<"#n"<<endl;
			outfile<<endl;

			//开始输出纹理编号
			outfile<<"t"<<endl;
			for ( std::vector<int>::iterator iter=fMData.at(i)->_TexNums.begin();iter!=fMData.at(i)->_TexNums.end();iter++)
			{
				outfile<<*iter<<endl;
			}
			outfile<<"#t"<<endl;
			outfile<<endl;
		}


	}

	void ReadMat(string inFilename,string outFilename)
	{
		ifstream inFile;  
		inFile.open(inFilename); // 打开文件  
		if (!inFile)
		{
			cout<<"Error:RWMAT inFile!"<<endl;
			return;
		}
		string str; // 行字符串缓存 

		ofstream outfile;
		outfile.open(outFilename);
		if (!outfile)
		{
			cout<<"Error:RWMAT outFile!"<<endl;
			return;
		}

		if(inFile.is_open())
		{ // 若成功打开文件  
			while(!inFile.eof())
			{   
				const int cnt = 6;  //一行有几个数
				int j = 0;

				size_t comma = 0;
				size_t comma2 = 0;

				int matRow=0;

				getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符 

				if (str=="")//空行
				{
					continue;
				}
				//矩阵房屋编号
				if (str=="m")
				{

#if 1
					if(inFile.is_open())
					{ // 若成功打开文件  
						while(!inFile.eof())
						{ // 若未到文件结束  
							getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符  

							matRow=0;//每个房屋第0行开始

							if (str=="")//空行
							{
								continue;
							}
							if (str=="#m")//编号结束
							{
								break;
							}
							aMatNums++;
							_AllMatDatas.push_back(new FMatData);//每次有新房屋矩阵数据，保存

							_AllMatDatas.at(aMatNums-1)->fmatNum=atoi(str.c_str());

							outfile<<atoi(str.c_str())<<endl;
							//cout<<atoi(str.c_str())<<endl;
						}
					}  
#endif
				}

				if (str=="d")//矩阵数据读取
				{
#if 1
					if(inFile.is_open())
					{ // 若成功打开文件  
						while(!inFile.eof())
						{ // 若未到文件结束  
							getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符  

							matRow++;

							if (str=="")//空行
							{
								continue;
							}
							if (str=="#d")//结束
							{
								break;
							}
							double tempArr[4];
							comma = str.find(' ',0);//从str的0开始查找返回空格空格位置

							tempArr[0]=atof(str.substr(0,comma).c_str());
							outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(0,comma).c_str())<<' ';
							//cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(0,comma).c_str())<<' ';

							int num0w=0;
							while (comma < str.size() && j != cnt-1)
							{
								comma2 = str.find(' ',comma + 1);
								outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(comma + 1,comma2-comma-1).c_str());
								//cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(comma + 1,comma2-comma-1).c_str());

								++j;

								if (num0w==1)
								{
									tempArr[2]=atof(str.substr(comma + 1,comma2-comma-1).c_str());

									outfile<<' ';
									//cout<<' ';
								}
								else if (num0w==2)
								{
									tempArr[3]=atof(str.substr(comma + 1,comma2-comma-1).c_str());

									outfile<<endl;
									//cout<<endl;
								}
								else
								{
									tempArr[1]=atof(str.substr(comma + 1,comma2-comma-1).c_str());
									outfile<<' ';
									//cout<<' ';
								}

								comma = comma2;

								num0w++;
							}

							if (num0w!=3)
							{
								tempArr[3]=0;
								//cout<<'0'<<endl;
								outfile<<'0'<<endl;

							}

							for (int i=0;i<4;i++)
							{
								_AllMatDatas.at(aMatNums-1)->matData44Arr->matData[matRow-1][i]=tempArr[i];
							}

							j = 0;
						}
					}  
#endif
				}
			}  
		}  
		inFile.close();
		outfile.close();
	}

	void WriteMat(std::vector<FMatData*> fMatData,string outFilename)
	{
		ofstream outfile;
		outfile.open(outFilename);
		if (!outfile)
		{
			cout<<"Error:RWMAT outFile!"<<endl;
			return;
		}

		for (int i=0;i<fMatData.size();i++)
		{
			//输出房屋编号
			outfile<<"m"<<endl;
			outfile<<fMatData.at(i)->fmatNum<<endl;
			outfile<<"#m"<<endl;
			outfile<<endl;

			//开始输出顶点坐标
			outfile<<"d"<<endl;
			for (int j=0;j<4;j++)
			{
				outfile<<setiosflags(ios::fixed)<< setprecision(15)<<fMatData.at(i)->matData44Arr->matData[j][0]
				<<" "<<fMatData.at(i)->matData44Arr->matData[j][1]<<" "<<fMatData.at(i)->matData44Arr->matData[j][2]
				<<" "<<fMatData.at(i)->matData44Arr->matData[j][3]<<endl;
			}
			outfile<<"#d"<<endl;
			outfile<<endl;
		}
	}

	void ReadTxt(string inFilename,string outFilename)
	{
		ifstream inFile;  
		inFile.open(inFilename); // 打开文件  
		if (!inFile)
		{
			cout<<"Error:RWTXT inFile!"<<endl;
			return;
		}

		ofstream outfile;
		outfile.open(outFilename);
		if (!outfile)
		{
			cout<<"Error:RWTXT outFile!"<<endl;
			return;
		}

		string prelinestr;//记录字符串
		string str="0"; // 行字符串缓存 

		if(inFile.is_open())
		{ // 若成功打开文件  
			//初始添加
			aFmNums+=1;
			_AllFMDatas.push_back(new FMData);
			_AllFMDatas.at(aFmNums-1)->_FMNum=aFmNums;

			while(!inFile.eof())
			{   
				const int cnt = 6;  //一行有几个数
				int j = 0;

				size_t comma = 0;
				size_t comma2 = 0;

				osg::Vec3d tempArr;

				prelinestr=str;
				getline(inFile, str, '\n'); // 读取一行内容，并存入缓存str中，'\n'表示一行结束的回车符 

				if (str==""&&prelinestr!="")//空行
				{
					aFmNums+=1;
					_AllFMDatas.push_back(new FMData);//每次有新房屋数据，保存

					_AllFMDatas.at(aFmNums-1)->_FMNum=aFmNums;

					continue;
				}
				if (str.find(_CutCoorMark)==-1)
				{
					_CutCoorMark=' ';
				}
				
#if 1
				if (str=="")//空行
				{
					continue;
				}
				
				comma = str.find(_CutCoorMark,0);//从str的0开始查找返回空格空格位置

				tempArr.x()=atof(str.substr(0,comma).c_str());
				outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(0,comma).c_str())<<_CutCoorMark;
				//cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(0,comma).c_str())<<_CutCoorMark;

				int num0w=0;
				while (comma < str.size() && j != cnt-1)
				{
					comma2 = str.find(_CutCoorMark,comma + 1);
					outfile<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(comma + 1,comma2-comma-1).c_str());
					//cout<<setiosflags(ios::fixed)<< setprecision(15)<<atof(str.substr(comma + 1,comma2-comma-1).c_str());

					++j;

					if (num0w==1)
					{
						tempArr.z()=atof(str.substr(comma + 1,comma2-comma-1).c_str());

						outfile<<endl;
						//cout<<endl;
					}
					else
					{
						tempArr.y()=atof(str.substr(comma + 1,comma2-comma-1).c_str());
						outfile<<_CutCoorMark;
						//cout<<_CutCoorMark;
					}

					comma = comma2;

					num0w++;
				}

				if (num0w!=2)
				{
					tempArr.z()=0;
					//cout<<'0'<<endl;
					outfile<<'0'<<endl;

				}
				_AllFMDatas.at(aFmNums-1)->_FPointsArr->push_back(tempArr);
#endif
			}  

			if (_AllFMDatas.at(_AllFMDatas.size()-1)->_FPointsArr->size()==0)
			{
				_AllFMDatas.pop_back();
			}
		}  
		inFile.close();
		outfile.close();
	}

	void WriteVertexToTxt(std::vector<FMData*> fMData,string outFilename)
	{
		ofstream outfile;
		outfile.open(outFilename);
		if (!outfile)
		{
			cout<<"Error:RWTXT outFile!"<<endl;
			return;
		}

		for (int i=0;i<fMData.size();i++)
		{
			//cout<<"房屋"<<fMData.at(i)->_FMNum<<":"<<endl;
			//开始输出顶点坐标
			for (osg::Vec3dArray::iterator iter=fMData.at(i)->_FPointsArr->begin();iter!=fMData.at(i)->_FPointsArr->end();iter++)
			{

				outfile<<setiosflags(ios::fixed)<< setprecision(15)<<(*iter).x()
					<<_CutCoorMark<<setiosflags(ios::fixed)<< setprecision(15)<<(*iter).y()
					<<_CutCoorMark<<setiosflags(ios::fixed)<< setprecision(15)<<(*iter).z()<<endl;
				
			}
			outfile<<endl;
		}

	}

	~RWTxt()
	{

	}

public:

//顶点
	std::vector<FMData*> _AllFMDatas;


	ULONGLONG aFmNums;

//矩阵
	std::vector<FMatData*> _AllMatDatas;

	ULONGLONG aMatNums;

//坐标间隔符
	char _CutCoorMark;
};

#endif