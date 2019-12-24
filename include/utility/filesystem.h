#pragma once
#include "preconfig.h" 
#include "stringhelp.h"
#include "vectorhelp.h"
#include "memorymanager.h"
#include "event.h"
UTILITY_NS 

/// \brief 环境变量
class GS_API GsEnvironment
{
public:
	/// \brief 环境变量分隔符
	/// \details 多个环境变量如PATH环境变量多个路径的分隔符。linux下为冒号（:）,windows下为分号（;）
	static char EnvironmentVariableSeparator();

	/// \brief 获取环境变量的值
	static GsString EnvironmentVariable(const char* name);

	/// \brief 修改环境变量的值
	static void ChangeEnvironmentVariable(const char* name, const char* value);

	/// \brief 删除环境变量的值
	static void RemoveEnvironmentVariable(const char* name);

	/// \brief 删除环境变量中指定的内容，如果存在的话
	static GsString RemoveEnvironmentVariable(const char* name,const char* content);

	/// \brief 向环境变量中插入一段内容。如果存在此内容则不工作任何动作
	/// \param name 环境变量的名称
	/// \param content 要添加的内容
	/// \param bAddToTail 是否添加到尾部，否则就添加到头部
	/// \return 返回添加后的的环境变量内容
	static GsString AppendEnvironmentVariable(const char* name, const char* content,bool bAddToTail = true);

};

/// \brief 文件路径相关的操作静态函数
class GS_API GsFileSystem
{
public:
	/// \brief 路径是否使用utf8字符串
	static bool IsUtf8Path();
	/// \brief 文件路径的的分隔符
	static char PathSeparator();

	/// \brief 环境变量分隔符
	/// \details 多个环境变量如PATH环境变量多个路径的分隔符。linux下为冒号（:）,windows下为分号（;）
	/// \details 不推荐使用，请调用GsEnvironment
	GS_DEPRECATED(static char EnvironmentVariableSeparator());

	/// \brief 获取环境变量的值
	/// \details 不推荐使用，请调用GsEnvironment
	GS_DEPRECATED(static GsString EnvironmentVariable(const char* name));

	/// \brief 修改环境变量的值
	/// \details 不推荐使用，请调用GsEnvironment
	GS_DEPRECATED(static void ChangeEnvironmentVariable(const char* name,const char* value));

	/// \brief 删除环境变量的值
	/// \details 不推荐使用，请调用GsEnvironment
	GS_DEPRECATED(static void RemoveEnvironmentVariable(const char* name));

	/// \brief 判断文件路径、目录路径是否存在
	/// \param strPath 文件或者目录的路径
    ///\return 返回路径是或否存在
	static bool Exists(const char* strPath);

	/// \brief 系统PATH目录
	static GsString PathFolder();

	/// \brief 增加搜索路径到系统PATH环境变量中
	static void AddSearchPath(const char* path);

	/// \brief 系统临时目录
	static GsString TemporaryFolder();

	/// \brief 工作目录
	static GsString WorkingFolder();
	/// \brief 设置工作目录，返回之前的工作目录
	static GsString  WorkingFolder(const char* folder);

	/// \brief 当前执行文件所在的文件路径
	static GsString ModuleFileName();

	/// \brief 合并两个路径字符串
	/// \param path1 第一个路径
	/// \param path2 第二个路径
    ///\return 包含合并的路径的字符串。如果指定的路径之一是零长度字符串，则该方法返回其他路径。如果 path2 包含绝对路径，则该方法返回 path2
	static GsString Combine(const char* path1,const char* path2);

	/// \brief 搜索Path获取一个路径存在的绝对路径
	/// \details 输入路径为绝对路径，如果该路径存在则返回，否则返回空
	/// \details 输入路径为相对路径，遍历PATH环境变量设置，组装第一个存在的路径返回，无法成功组装则返回空
	static GsString MakeExistFullPath(const char* path);

	/// \brief 从两个绝对路径计算相对路径
	/// \param from 绝对路径，从这个绝对路径计算
	/// \param to  绝对路径，计算该路径相对于from的相对路径
    ///\return 返回相对路径，如果输入路径无效则返回to
	static GsString PathRelativePathTo(const char* from,const char* to);
	
	/// \brief 判断一个路径是否是文件
	/// \param strPath 路径
	///\return 返回路径是否是文件路径
	static bool IsFile(const char* strPath);

	/// \brief 获取一个值，该值指示指定的路径字符串是包含绝对路径信息还是包含相对路径信息。
	/// \param path 要测试的路径
	///\return 如果 path 包含绝对路径，则为 true；否则为 false
	static bool IsPathRooted (const char* path);

	/// \brief 判断目录是否是根目录
	/// \return 如果path是根目录，则返回true；否则返回false
	static bool IsRootDirectory(const char* path);
};

class GsDir;

//// \brief 文件系统的文件
class GS_API GsFile
{
	GsString m_strPath;
public:
	/// \brief  默认函数
	GsFile();

	/// \brief 拷贝构造函数
	/// \brief rhs拷贝的对象
	GsFile(const GsFile& rhs);

	/// \brief 文件的完整路径构造对象
	/// \param strName 文件完整路径
	GsFile(const char* strName);
	virtual ~GsFile();
	
	
	/// \brief 获取文件的内部字符串路径
    ///\return 返回路径字符串指针
	const char* Path() const; 

	/// \brief 获取文件名称，不包含路径
    ///\return 返回文件的名称
	GsString Name(bool bHasExtension = true)const; 

	/// \brief 获取文件完整路径（UTF-8）
    ///\return 返回文件的完整路径
	GsString FullPath()const;

	/// \brief 获取文件是否存在
    ///\return 返回文件是否存在
	bool Exists()const;
	
	/// \brief 获取文件的大小
    ///\return 返回文件大小，字节
	unsigned long long Size()const;
	 
	/// \brief 判断文件是否为同一个文件
	/// \param rhs 其他文件
    ///\return 传入文件是否和本文件为同一个文件
	bool operator == (const GsFile& rhs) const;

	/// \brief 赋值
	/// \brief rhs其他文件
    ///\return 自身
	GsFile& operator = (const GsFile& rhs) ;

	/// \brief bool操作符 
	operator bool();

	/// \brief 赋值
	/// \param file 其他文件
    ///\return 自身
	GsFile& operator = (const char* file) ;

	/// \brief 赋值
	/// \param file 其他文件
	///\return 自身
	GsFile& operator = (const GsString& file);


	/// \brief 文件所在的目录
    ///\return 分返文件所在的目录
	GsDir Parent();


	/// \brief 文件的扩展名，(不带.)
    ///\return 返回文件的扩展名
	GsString Extension()const;
	
	/// \brief 改变文件路径的扩展名
	/// \details 注意并非修改文件的扩展名。
	/// \param strExt 新的扩展名，(不带.)
    /// \return 返回文件的扩展名
	GsString ChangeExtension(const char* strExt);

	/// \brief 将文件当作文本文件全部读取为字符串
	///\return 输出文件中读取的字符串
	GsString ReadAll() const;

	/// \brief 将字符串写入到文件中
	bool WriteAllText(const char* str) const;
	
	/// \brief 将文件当作二进制文件全部读取出来
	int ReadAllBytes(GsByteBuffer* buff)const;
	/// \brief 将文件当作二进制文件读取到缓冲区中
	unsigned long  long ReadAllBytes(unsigned char* buff, unsigned long  long nBuffLen)const;

	/// \brief 将文件当作文本文件读取所有的行
	GsVector<GsString> ReadAllLines()const;
	
	/// \brief 将文件当作文本文件读取所有的行
	int ReadAllLines(GsVector<GsString>& vec)const;
	                        
	/// \brief 将文件当二进制文件写入所有的数据
	bool WriteAllBytes(const unsigned char* bytes,int nSize)const;
	 
	/// \brief 将文件当作文本文件写入所有的行
	bool WriteAllLines(const GsVector<GsString>& vec)const;
	  
	/// \brief 将文件当作文本文件在末尾添加一行
	bool AppendLine(const char* line)const;

	/// \brief 将文件当作文本文件在末尾添加多行
	bool AppendLines(const GsVector<GsString>& vec)const;

	/// \brief 删除当前文件
	///\return 返回是否成功
	bool Delete();

	/// \brief 删除一个文件
	///\return 返回是否删除成功
	static bool Delete(const char* path);
	
	/// \brief 重命名当前文件
	///\return 返回是否成功
	bool ReName(const char* strNewFilePath);

	/// \brief 重命名一个文件
	///\return 返回是否重命名成功
	static bool ReName(const char* strOldFilePath,const char* strNewFilePath);
};

/// \brief 文件系统的目录
class GS_API GsDir
{
	GsString m_strPath;
public:
	/// \brief  默认函数
	GsDir();
	/// \brief 拷贝构造函数
	/// \param pDir 拷贝的对象
	GsDir(const GsDir& pDir);
	/// \brief 目录的完整路径构造对象
	/// \param strDirPath 目录完整路径
	GsDir(const char* strDirPath);
	virtual ~GsDir();
	/// \brief 获取目录的内部字符串路径
	///\return 返回目录字符串指针
	const char* Path() const;

	/// \brief 如果文件夹不存在则创建文件夹
	bool Create();

	/// \brief 目录的名称
    ///\return 返回目录的名称
	GsString Name() const; 

	/// \brief 目录的完整路径（UTF-8）
    ///\return 返回目录的完整路径
	GsString FullPath()const;

	/// \brief 目录是否存在
    ///\return 返回目录是否存在
	bool Exists() const;
	
	/// \brief 是否为根目录
    ///\return 返回目录是否是跟目录
	bool IsRoot()  const;


	/// \brief 返回父目录
    ///\return 返回父目录
	GsDir Parent() const;

	/// \brief 以回调遍历所有的文件
	void ForEachFiles(GsDelegate<bool(const char*,void*)>& onFileCallBack, void* context = NULL, const char* searchPattern = NULL);

	/// \brief 目录下所有的文件
	/// \param searchPattern 搜索条件，比如*.txt
    ///\return 返回目录下的所有文件对象
	GsVector<GsFile> Files(const char* searchPattern = NULL)const;


	/// \brief 目录下所有的文件
	/// \param files 保存文件对象的集合
    /// \return 返回保存集合的指针
	GsVector<GsFile>* Files(GsVector<GsFile>& files)const;

	/// \brief 获取子目录
    /// \return 返回子目录对象
	GsDir  SubDir(const char* subName)const;
	
	/// \brief 获取目录下的文件
    ///\return 目录下的文件
	GsFile  File(const char* fileName)const;
	
	/// \brief 以回调遍历所有的目录
	void ForEachDirs(GsDelegate<bool(const char*,void*)>& onFileCallBack,void* context = NULL);

	/// \brief 目录下所有的子目录
    /// \return 返回目录下的所有子目录对象
	GsVector<GsDir> Dirs()const;

	/// \brief 目录下所有的子目录
    ///\return 返回目录下的所有子目录对象
	GsVector<GsDir>* Dirs(GsVector<GsDir>& files)const;

	/// \brief 判断目录是否为同一个目录
	/// \brief rhs 其他目录
    /// \return 传入目录是否和本目录为同一个文件
	bool operator == (const GsDir& rhs) const;

	/// \brief 赋值
	/// \param rhs 对象
    ///\return 自身
	GsDir& operator = (const GsDir& rhs) ;
	
	/// \brief 赋值
	/// \brief strDirPath 对象
    ///\return 自身
	GsDir& operator = (const char* strDirPath) ;

	/// \brief 赋值
	/// \param strDirPath 对象
    /// \return 自身
	GsDir& operator = (const GsString& strDirPath) ;
	
	/// \brief bool操作符 
	operator bool();


	/// \brief 删除当前目录
	/// \param recursive 是否删除子目录
	/// \return 返回是否成功
	bool Delete(bool recursive =true );

	/// \brief 删除一个目录
	/// \return 返回是否删除成功
	static bool Delete(const char* path,bool recursive =true );
};
		

UTILITY_ENDNS