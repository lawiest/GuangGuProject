#pragma once

namespace geostar {
struct geo_object;
};

typedef unsigned long long id_type;

typedef const char* string_t;

/// \brief reader定义的回调函数 
/// \param user 用户定义的参数
/// \param i 要读取的第i个对象
/// \return 返回的结果
/// \details 这个回调函数，可以帮助用户实现geo_reader接口,参考geo_factory::new_reader
/// \details 参数i从1开始，如果i=0表示要释放geo_reader对象，用户代码应该释放user中的资源
typedef geostar::geo_object* (*reader_callback)(void* user,unsigned int i);

/// \brief writer定义的回调函数
/// \param user 用户定义的参数
/// \param obj 要写入的对象
/// \details 这个回调函数，可以帮助用户实现geo_writer接口,参考geo_factory::new_writer
typedef void (*writer_callback)(void* user,geostar::geo_object* obj);

/// \brief regist_open定义的回调函数
/// \param format 文件的格式
/// \param filename 文件名
/// \details 参考geo_factory::regist_open,geo_factory::open_reader,geo_factory::open_writer
// GeomathSE利用这个回调函数允许二次开发用户扩展open_reader和open_writer的能力
typedef geostar::geo_object* (*open_callback)(string_t format,string_t filename);

namespace geostar {

/// \brief 基础的对象接口
struct geo_object
{
    /// \brief 析构函数
    virtual ~geo_object(){};

    /// \brief 得到类型描述符
    virtual string_t type_info()const=0; 

    /// \brief 类型转换
    virtual void* type_cast(string_t)=0;         

    /// \brief 添加引用计数
    virtual int addref()=0;                         

    /// \brief 减少引用计数
    virtual int release()=0;                        
    
    /// \brief 得到属性
    /// \param i 属性的id
    /// \return 属性的值
    virtual int i4_get(int i)=0;                    

    /// \brief 设置属性
    /// \param i 属性的id
    /// \param v 属性的值
    /// \return 设置是否成功
    virtual bool i4_set(int i,int v)=0;             
};

/// \brief 读取对象的接口
struct geo_reader : geo_object
{
    /// \brief 读取下一个对象
    /// \return 如果成功返回对象的指针，否则返回0
    /// \details 如果返回0则表示已经读到的流的结尾
    virtual geo_object* read()=0;
};

/// \brief 写对象的接口
struct geo_writer : geo_object
{
    /// \brief 写入对象
    /// \param obj 要写入的对象
    virtual void write(geo_object* obj)=0;
};

/// \brief 同时支持读写的对象容器的接口
struct geo_reader2 : geo_reader,geo_writer
{	
	/// \brief 得到容器中对象的个数
	virtual unsigned int count()=0;

	/// \brief 得到当前读到的下标
	virtual unsigned int get_pos()=0;

	/// \brief 设置当前读到的下标
	virtual void set_pos(unsigned int i)=0;

	/// \brief 创建this的副本
    /// \return 返回的副本对象和this共享同一套数据，但不共享读下标。
    //  如果往副本中写入新对象，原件和副本都可以访问到新对象
	virtual geo_reader2* duplicate()=0; 
};

/// \brief 数据变换的接口
struct geo_trans : geo_object
{
    /// \brief 对obj进行变换
    /// \return 变换后的结果
    virtual geo_object* trans(geo_object* obj)=0;

    /// \\brief 设置变换参数
    /// \param name 参数名字
    /// \param para 参数的指针
	virtual bool setpara(string_t name,void* para)=0;
};

/// \brief 数据比较接口
struct geo_compare : geo_object
{
    /// \brief 比较两个对象的大小
    /// \return a<b返回-1；a=b返回0；a>b返回1
    virtual int compare(geo_object* a,geo_object* b)=0;
};

/// \brief 一维矩阵的接口 
/// \details 这是一个相对"简单"和"不安全"的接口
//本接口不限定每个子元素的类型必须一致，也就是说vector中第0个可以是int，而第1个可以是string_t，第2个可以是geo_object*
//每个元素的具体类型由接口的实现者和调用者自己约定，若约定不符则容易导致程序错误
//本接口约定的具体类型只有指针类型和值类型两种，指针类型目前只定义string_t,wstring_t,geo_object*三种，值类型调用者和实现者必须约定好数值的sizeof
//geo_vector一个突出的特点是约定get函数返回的对象外部程序不应该销毁，也就是说在已知get返回的是geo_object*的情况下，外部函数只需强制转换，使用完后无需调用release
struct geo_vector : geo_object
{
	/// \brief 矩阵中元素的个数
	virtual int count()=0;
	
	/// \brief 获取第i个元素
    /// \param i 要获取的下标 
	/// \param ptr 接受数据的外部指针，如果第i个元素是数值类型，那么给的ptr必须具有该数值类型sizeof的空间
	/// \return 第i个元素在geo_vector内部的地址或元素指针,
    // 如果第i个元素是指针类型，那么参数ptr将被忽略，返回值将得到元素指针，外部程序再转换为string_t或wstring_t或geo_object*
	// 如果返回值是geo_object*，由于不是geo_object*的形式，本设计约定外部程序没有必要release掉返回值
	virtual const void* get(int i,void* ptr)=0;
	
	/// \brief 修改第i个元素
    /// \param i 要修改的下标 
	/// \param ptr 传入数据的外部指针，调用者和实现者必须对其实际的类型有一致的约定
	// 如果第i个元素是数值类型，那么给的ptr必须具有该数值类型sizeof的空间
	// 如果第i个元素是指针类型，那么ptr必须是与之匹配的string_t,wstring_t或geo_object*
	virtual bool set(int i,const void* ptr)=0;

	/// \brief 添加一个新元素
	/// \param ptr 为传入数据的外部指针，调用者和实现者必须对其实际的类型有一致的约定
	/// \details 如果可以add的数据是数值类型，那么ptr必须具有该数值类型sizeof的空间
    /// \return 添加成功的对象
	virtual void* add(const void* ptr)=0;

    /// \brief 批量删除一组元素[i,i+n)
	/// \param i 要删除的第一个元素下标
    /// \param n 要删除的元素个数
    /// \return 是否删除成功
	virtual bool remove(int i,int n)=0;
};

/// \brief 单元格的元数据
struct geo_cell
{
    /// \brief 列名称
	string_t	name;		

    /// \brief 附加的描述信息
	string_t	discribe;

    /// \brief 数据的类型 
    /// \details 目前只能是byte,string,wstring,object
	string_t	type;

    /// \brief 占用的字节数 
    /// \details 如果type等于string,wstring,object该值为0，如果type等于byte该值必须大于0
	int			size_of;
};

/// \brief GeomathSE内部实现的二维数组(表)的接口
struct geo_table : geo_object
{	
	/// \brief 创建一个表
    /// \param n,p [p,p+n)为列的元信息
	virtual bool create(int n,const geo_cell* p)=0;

	/// \brief 将this保存到文件
    /// \param filename 文件名
	virtual bool save(string_t filename)=0;

	/// \brief 从文件中装入表数据 
    /// \param filename 文件名
	virtual bool load(string_t filename)=0;

	/// \brief 得到表的附加属性
    /// \return 返回的geo_vector是一个string_t类型的一维数组
	virtual geo_vector* props()=0;

	/// \brief 得到表的列信息
    /// \return 返回的geo_vector是一个cell类型的一维数组
	virtual geo_vector* columns()=0;

    /// \brief 得到表的行信息
	/// \return 返回的geo_vector是一个object类型的一维数组 
    // 每个row对象都是geo_vector*类型的object，每个row的具体结构可以根据columns获知
	virtual geo_vector* rows()=0;

	/// \brief 根据id得到row对象
    /// \param id 行id
    /// \return 行对象
    virtual geo_vector* row_get(id_type id)=0;

    /// \brief 根据id新增row对象
    /// \param id 行id
    /// \return 行对象
    virtual geo_vector* row_add(id_type id)=0;

	/// \brief 批量删除row
    /// \param n,row_ids [row_ids,row_ids+n)为要删除的下标集合
    //  \return 成功删除对象的个数
	virtual int row_del(id_type id)=0;

    /// \brief 准备空间索引
    /// \param ipath 几何对象所在的列下标(ipath必须指定否则创建一定失败)
    /// \param ienv 外接矩形所在的列下标(ienv可以不指定(给-1))
    /// \return 是否创建成功
    virtual bool query_prepare(int ipath,int ienv)=0;

    /// \brief 空间查询
    /// \param mode 空间关系参见(spr_is_*)
    /// \param obj 几何对象
    /// \return 查询结果
    virtual geo_vector* query(geo_object* obj,int mode,double tol)=0;
};

/// \brief 显示GDI对象接口
/// \details 本设计约定graphics对象不需要支持多线程并发
struct geo_graphics : geo_object
{
	/// \brief 保存GDI状态
    //return 保存GDI状态的对象，通过grestor可以恢复GDI状态
	virtual geo_object* gsave()=0;

	/// \brief 恢复GDI状态
    /// \param gs gsave返回的对象
	virtual void grestor(geo_object* gs)=0;

	/// \brief 创建gdi对象
    /// \param paras 对象的描述信息例如 pen(r255,g0,b0) 表示要创建一个红色的笔
    /// \return 返回的gdi对象
	virtual geo_object* new_gdi(string_t paras)=0;

	/// \brief 设置gdi对象
    /// \param gdi new_gdi创建的对象
    /// \return 替换掉的老gdi对象
	virtual void set_gdi(geo_object* gdi)=0;

	/// \brief 画一条path
	/// \param mode mode&1表示要画线 mode&2表示要填充
	/// \param p 一个以-1为结尾的串，用来描述xy的结构 xy存放节点坐标
	//例如p={5,10,-1}表示xy描述了 两条线，第一条线有5个节点，第二条线有10个节点
    /// \param xy path的xy坐标
	virtual void draw_path(int mode,const int* p,const double* xy)=0;
	
    /// \brief 显示文字
	/// \param font 字体对象
    /// \param x,y,w,h 要显示的矩形[x,y,x+w,y+h]
    /// \param text 文字内容
	virtual void draw_text(geo_object* font,double x,double y,double w,double h,wchar_t* text)=0;
};

//符号分为原子符号和组合符号两种，客户程序绝不该直接使用原子符号，而是该通过geo_draw的new_symbol函数创建
//组合符号来显示对象，在符号的draw函数中有返回值，对组合符合而言此返回值永远为0，
//这个返回值的接口是为原子符号设计预留的

/// \brief 用来符号化显示geometry的对象接口
struct geo_symbol : geo_object
{
    /// \brief 准备显示
    /// \param dc graphics参数
	/// \param senv 附带的显示参数，当前只约定给出[s,x,y,w,h]5个值，今后可能添加,
	//[x,y,w,h)为显示窗口的矩形,s为一个象素的逻辑单位,也就是说将传入的path的x,y乘与s将得到变换前path的大小
    /// \details 在一个显示过程中，每个symbol对象的perpare只会调用一次，利用这次调研symbol对象可以提前准备相关的数据，为之后的多次run调用提高速度
	virtual void perpare(geo_graphics* dc,const double* senv)=0;

	/// \brief 执行该符号的动作
	virtual geo_object* run(geo_graphics* dc,geo_object* para)=0;

	/// \brief 将path显示到dc中
	void draw(geo_graphics* dc,geo_object* path){ run(dc,path); }
};

/// \brief 创建原子符号的接口
struct geo_sym_fac : geo_object
{
	/// \brief 符号脚本中的函数名
	virtual string_t name()=0;

	/// \brief 创建符号对象
	virtual geo_symbol* create(string_t* paras,string_t* dc,string_t* path)=0;
};

/// \brief GeomathSE显示框架暴露的接口
struct geo_draw : geo_object
{
	/// \brief 设置精度值
    /// \param value 新的精度值
    /// \return 老的精度值
	/// \details 精度值用来控制显示数据的压缩率,这个值越大则显示效果也粗燥，越小则越精细, 缺省为1
	virtual double precision(double value)=0;

	/// \brief 注册一个原子符号的类厂对象
	virtual void regist_sym_fac(geo_sym_fac* fac)=0;

	/// \brief 创建一个symbol对象
    /// \param type 对象的类型
    /// \param scrip 为此symbol的详细信息
    /// \return 符号对象
	virtual geo_symbol* new_symbol(string_t type,string_t scrip)=0;

    /// \brief 准备显示
	/// \param env env={x,y,w,h}为窗口的矩形
    /// \param tx,ty 逻辑坐标到屏幕坐标的变换参数,假如(x,y)为一个逻辑坐标那么它的屏幕坐标为 tx[0]*x+tx[1]*y+tx[2],ty[0]*x+ty[1]*y+ty[2]
	virtual void prepare(const double* env,const double* tx,const double* ty)=0;
	
    /// \brief 显示一组path
    /// \param R 要显示的path的枚举器
    /// \param dc 显示的目标DC
    /// \param sym 显示方案的symbol对象
    /// \param tpath 如果R中读出来的不是path对象可以利用tpath将其变换成path对象，如果tpath为0则R中读取出来的必须是path对象
    /// \param tsym 如果tsym为0则所有path使用sym来表现，否则可用tsym为每个R中的对象指定不同的symbol
    /// \details 这个函数内部使用了多cpu并发、压缩等提速手段 显示性能优异
	virtual void draw_path(geo_reader* R,geo_graphics* dc,geo_symbol* sym,geo_trans* tpath,geo_trans* tsym)=0;

    /// \brief 显示一组path
    /// \param A 存放path的表容器
    /// \param path_cid path所在列的id
    /// \param env_cid env所在列的id
    /// \param dc 显示的目标DC
    /// \param sym 显示方案的symbol对象
    /// \param tsym 如果tsym为0则所有path使用sym来表现，否则可用tsym为每个R中的对象指定不同的symbol
	virtual void draw_path_2(geo_vector* A,int path_cid,int env_cid,geo_graphics* dc,geo_symbol* sym,geo_trans* tsym)=0;

    /// \brief 创建一个注记对象
	/// \param path 注记的几何对象 
    /// \param txt 注记的文本
	/// \param font 封装了注记的字体颜色等信息 
    /// \param mode 指定注记的排版方式
	/// \param w,h 字体的大小,当mode指定为可拆分的时候w是单个字的宽度，否则是整个文本的宽度
	virtual geo_object* new_text(geo_object* font,geo_object* path,const wchar_t* txt,int mode,int w,int h)=0;

	/// \brief 显示一组注记
    /// \param R 注记的数据流 read返回的对象必须是new_txt函数创建的对象
    /// \param dc 显示的目标DC
	virtual void draw_text(geo_reader* R,geo_graphics* dc)=0;

	/// \brief 显示注记
    /// \param A 存放path和文字的表容器
    /// \param path_cid path所在列的id
    /// \param env_cid env所在列的id
    /// \param dc 显示的目标DC
    /// \param ttxt 行对象到注记对象的转换器
	virtual void draw_text_2(geo_vector* A,int path_cid,int env_cid,geo_graphics* dc,geo_trans* ttxt)=0;
};

struct geo_top_check : geo_object
{
    /// \brief 设置容差
    /// \param tol 新的容差值
    /// \return 老的容差值
    virtual double tolerance(double tol)=0;

    /// \brief 单个对象检测
    /// \param p 要检测的几何对象
    /// \param W 检测结果的接收器
    /// \details 如果p中有重叠的线，自相交的点，输出到W
    virtual void check_1(geo_object* p,geo_writer* W)=0;

    /// \brief 点检测
    /// \param A 要检测的地物类集合
    /// \param W 检测结果的接收器
    /// \details 输出到W的一定是multi_path类型的点群M,M中所有子对象m都是在容差意义上相邻的点对象
    /// 每个m的属性继承自其来源的几何对象，如果M有n个字对象,那么它也一定有n个i4属性，每个属性指示
    /// 对应的点对象与其源自对象的关系 1表示起点，2表示终点，4表示节点，8表示线段内点,0表示源自对象为点
    /// 这个方法可以检测 ArcGIS中 必须大于容差,悬挂点，交点，伪节点等内容
    /// 出于效率的考虑写入W中的M和M中的子对象不会每次都构造新对象，所以W的使用者不能保存M的引用。
    virtual void check_point(geo_reader* A,geo_writer* W)=0;

    /// \brief 唯一性检测
    /// \param gdim 对象的几何维度，R中的所有对象都当作几何维度为gdim的对象看,不管它的实际维度是多少
    /// \param R 要检查的地物类集合
    /// \param W 检测结果的接收器
    /// \details 如果某几何对象在R中是唯一的，那么是正常对象不会输出到W中，如果它在R中出现0或多次那么
    ///         它将被当作错误输出到W，如果出现0次那么输出对象的属性长度为0，否则将合并所有与之重叠的R中的属性
    //          只有gdim=2的时候才可能产生0次出现的对象，这用来检测空隙
    virtual void unique(int gdim,geo_reader* R,geo_writer* W)=0;

    /// \brief 增加一个几何对象
    /// \details add进入的对象将参与query和split的函数
    virtual bool add(geo_object* obj)=0;

    /// \brief 指示add完成 为之后的query,split做好数据准备
    virtual void add_end()=0;

    /// \brief 空间关系查询检测
    /// \param spr 空间关系(参见spr_*等定义）
    /// \param obj 要检查的地物类
    /// \retual 返回所有满足spr_query_is(obj,add,spr)的add进入的对象几何
    /// \details 这个函数是可以并发的
    virtual geo_reader* query(int spr,geo_object* obj)=0;

    /// \brief 几何切割
    /// \param obj 要切割的对象
    /// \retual 返回obj被切割成的对象
    /// \details 这个函数是可以并发的
    virtual geo_reader* split(geo_object* obj)=0;
};


/// \brief 类厂对象的接口
struct geo_factory : geo_object
{
    /// \brief 得到几何对象的坐标维数
    /// \param p 要分析的几何对象
    /// \return 坐标维数 2表平面 3表立体
    virtual int dim_c(geo_object* p)=0;
  
    /// \brief 得到几何对象的几何维数
    /// \param p 要分析的几何对象
    /// \return 几何维数 0表示点 1表示线 2表示面
    virtual int dim_g(geo_object* p)=0;

    /// \brief 得到几何对象的子对象个数
    /// \param p 要分析的几何对象
    /// \return 子对象个数 如果p是简单对象返回0
    virtual int sub_n(geo_object* p)=0;

    /// \brief 得到几何对象的子对象
    /// \param p 要分析的几何对象
    /// \param i 子对象的下标
    /// \return 子对象 
    virtual geo_object* sub_at(geo_object* p,int i)=0;
    
    /// \brief 得到几何对象的点个数
    /// \param p 要分析的几何对象
    /// \return 点个数
    virtual int point_n(geo_object* p)=0;
    
    /// \brief 得到点的内部数据
    /// \param p 要分析的几何对象
    /// \param i 点的下标
    /// \param vt vt=0返回x坐标,vt=1返回y坐标,vt=2返回z坐标vt=4返回曲线描述符
    /// \return 内部数据的地址
    /// \details 调用point_at(p,0,0)可以得到p的xy坐标的首地址，返回值实际是一个double* 格式是{x0,y0,x1,y1,……}
    // 如果p是3维对象要获取z值需要调用point_at(p,0,2)，返回值实际是一个double* 格式是{z0,z1,……}
    virtual const void* point_at(geo_object* p,int i,int vt)=0;
    
    /// \brief 创建简单几何对象
    /// \param gdim 几何维数
    /// \param pnum 点个数
    /// \param cdim 坐标维数
    /// \param xyz 坐标
    /// \param flag 是否引用传入的xyz坐标 0不引用 1引用
    /// \param vt_p 曲线描述符=0表示折线 vt_p[i]可以取值0,1,2,3 意义为0线段,1圆弧,2二次贝赛尔曲线,3三次贝赛尔曲线
    /// \return 返回的几何对象
    virtual geo_object* create_single(int gdim,int pnum,int cdim,const double* xyz,
        int flag,const void* vt_p)=0;

    /// \brief 创建复杂几何对象
    /// \param gdim 几何维数
    /// \param snum sub的个数
    /// \param subs 子对象的首地址 
    /// \param flag 保留参数传0
    /// \return 返回的几何对象
    virtual geo_object* create_multi(int gdim,int snum,geo_object** subs,int flag)=0;

    /// \brief 输出single path的信息
	/// \param single 要输出的path对象必须是一个single path
	/// \param cdim 输出的坐标维数
    /// \param gdim 输出几何维数
    /// \param pnum 输出点的个数
	/// \param xyz 输出(x,y,z)的值
    /// \param vt 输出曲线描述符
    /// \details xyz和vt外部都不能删除和更改
	virtual bool out_single(geo_object* single,int* cdim,int* gdim,int* pnum,double** xyz,char** vt)=0;

    /// \brief 创建oracle定义的几何对象
    /// \param i_n 解释串个数
    /// \param i_p 解释串首地址 [i_p,i_p+3*i_n)为解释串信息
    /// \param c_n 坐标串个数
    /// \param c_p 坐标串首地址[c_p,c_p+c_n)为坐标串信息
    /// \param c_dim 坐标串维数 2为平面对象 3为三维对象
    /// \param flag 是否引用传入的c_p坐标 0不引用 1引用
    /// \return 返回的几何对象
    virtual geo_object* ora_create(int i_n,const int *i_p,int c_n,int c_dim,const double *c_p,int flag)=0;

    /// \brief 得到oracle定义的解释串坐标串
    /// \param obj 几何对象
    /// \param i_n 解释串个数
    /// \param i_p 解释串首地址 [i_p,i_p+3*i_n)为解释串信息
    /// \param c_n 坐标串个数
    /// \param c_p 坐标串首地址[c_p,c_p+c_n)为坐标串信息
    /// \param c_dim 坐标串维数 2为平面对象 3为三维对象
    /// \details 返回的i_p,c_p指针由内部维护调用者不能删除.
    // 这个函数不能在多线程的环境下调用，否则容易引发内部维护内存错乱，如果每个线程有其独立的geo_factory对象则没有问题
    virtual bool ora_get_info(geo_object* obj,int* i_n,int** i_p,int* c_n,int* c_dim,double **c_p)=0;

    /// \brief 创建基于回调函数的geo_reader
    /// \param user 用户参数
    /// \param func 用户的回调函数
    /// \return 返回的geo_reader对象
    virtual geo_reader* new_reader(void* user,reader_callback func)=0;

    /// \brief 创建多个geo_reader的组合geo_reader 
    /// \param n,ppr [ppr,ppr+n)为要组合的geo_reader数组
    /// \return 返回的geo_reader对象
    virtual geo_reader* new_reader_n(int n,geostar::geo_reader** ppr)=0;

    /// \brief 创建一个过滤的geo_reader
    /// \param R 原geo_reader对象
    /// \param n,tt [tt,tt+n)为一组geo_trans对象 
    /// \return 包装后的reader对象 
    /// \details 假如a=R->read()是R中的某个对象，程序会用[tt,tt+n)中geo_trans执行a=tt[i]->trans()变换
    //  加入某个时刻trans()返回0，那么忽略这个a，继续读取R中的下一个对象，这样可以起到一个过滤的效果且不至于结束reader
    virtual geo_reader* new_reader_trans(geo_reader* R,int n,geo_trans** tt)=0;
    
    /// \brief 创建基于回调函数的geo_writer
    /// \param user 用户参数
    /// \param func 回调函数
    /// \return 返回的geo_writer对象
    virtual geo_writer* new_writer(void* user,writer_callback func)=0;

    /// \brief 打开一个geo_reader
    /// \param format 文件格式
    /// \param filename 文件名
    /// \return 返回的geo_reader对象
    virtual geo_reader* open_reader(string_t format,string_t filename)=0;

    /// \brief 打开一个geo_writer
    /// \param format 文件格式
    /// \param filename 文件名
    /// \return 返回的geo_writer对象
    virtual geo_writer* open_writer(string_t format,string_t filename)=0;
	
    /// \brief 创建一个geo_trans
    /// \param scrip trans的描述符
    /// \details
	//scrip="cut" 返回单个裁剪的trans 需要设置类型为面的"knife"参数,trans函数返回在knife内部的裁切结果
	//scrip="clip" 返回矩形裁剪的trans 此trans 需要设置"x0","y0","dx","dy","tx","ty" 六个参数
	//		clip trans返回的path一定是multi类型，multi中的每个子path一定有长度为2的i4属性
	//scrip="simplify_toint" 返回整数版的简化trans 此trans 需要设置 pricesion 参数
	//scrip="simplify_dauglas", 返回dauglas版的简化trans 此trans需要设置 tol 参数
	//scrip="parallel" 返回平行线的trans 此trans需要设置 radius 参数, 还可以设置narc参数(缺省值为6)
	//scrip="buffer" 返回缓冲区的trans 此trans需要设置 radius 参数, 还可以设置narc参数(缺省值为6)
    virtual geo_trans* create_trans(string_t scrip)=0;
	
	/// \brief 打开一个geo_reader2
    /// \param format 打开方式 只能是0，"a","r","ra" 四种
    /// \param filename 文件名 可以是null
    /// \return 返回的geo_reader2对象
    virtual geo_reader2* open_reader2(string_t format,string_t filename)=0;

    /// \brief 注册一个打开文件的函数
    /// \param func 打开文件的回调函数
    /// \param type 打开类型,type=0是打开reader,否则是打开writer
	virtual void regist_open(open_callback func,int type)=0;

	/// \brief 创建一个draw对象
	virtual geo_draw* new_draw()=0;

	/// \brief 创建一个geo_table对象
	virtual geo_table* new_table()=0;
	
    /// \brief 创建一个geo_top_check对象
    virtual geo_top_check* new_top_check()=0;
};

/// \brief 几何算法对象接口
struct geo_algorithm : geo_object
{
    /// \brief 设置和读取容差
    /// \param tol 新的容差值
    /// \return 老的容差值
    virtual double tolerance(double tol)=0;

    /// \brief 计算对象的数值属性
    /// \param obj 要计算的几何对象
    /// \param mode 要计算的属性类型
    /// \return 属性值
    /// \details mode可以指定 长度、面积、体积等等
    /// \参考常量 prop_value_length,prop_value_width,prop_value_height,prop_value_area
    virtual double prop_value(geo_object* obj,int mode)=0;

    /// \brief 计算对象的几何属性
    /// \param obj 要计算的几何对象
    /// \param mode 要计算的属性类型
    /// \return 属性值
    /// \details mode可以指定 clone 中心、重心、Label点 
    //外接矩形(正斜)、内接矩形(正斜)、内接外接(圆)、凸包 等等
    /// \参考常量  prop_geo_clone,prop_geo_clone_mem0,prop_geo_label,prop_geo_center,prop_geo_centroid
    //prop_geo_maxline,prop_geo_env,prop_geo_hull,prop_geo_env_nstd,prop_geo_env_circle,prop_geo_inner_rect
    //prop_geo_inner_square,prop_geo_inner_circle,prop_geo_simplify,prop_geo_boundary
    virtual geo_object* prop_geo(geo_object* obj,int mode)=0;

    /// \brief 计算对象的几何属性
	/// \param obj 要计算的几何对象
    /// \param mode 要计算的属性类型
    /// \param paras 返回的属性值
    /// \details mode 可以指定中心、重心、Label点 外接矩形 内接矩形 内接外接圆 等等
    /// \参考常量 prop_geo_label,prop_geo_center,prop_geo_centroid,prop_geo_maxline,
    //prop_geo_env,prop_geo_env_circle,prop_geo_inner_rect,prop_geo_inner_square,prop_geo_inner_circle
    virtual bool prop_geo_2(geo_object* obj,int mode,double* paras)=0;

	/// \brief 修改几何对象
    /// \param obj 要修改的几何对象
    /// \param mode 要修改的类型
    /// \param para 要修改的参数
	/// \details mode 可以指定 反向 去除重复点 折线压缩(dauglas) 调整方向(force_good_dir) 等等
    /// \参考常量 gmodify_remove_repeat,gmodify_dauglas,gmodify_reverse,gmodify_force_good_dir
    //gmodify_force_geo_close,force_geo_close
	virtual int gmodify(geo_object* obj,int mode,void* para)=0;

    /// \brief 返回点在线的左右边
    /// \param p 线对象
    /// \param x,y 点的坐标
    /// \return 如果(x,y)和p的距离小于容差返回0，左边返回1，右边返回2
	virtual int left_right(geo_object* p,double x,double y)=0;

    /// \brief 计算两个几何对象的间最近点
    /// \param a,b 要计算的两个几何对象
    /// \return 一个线段类型的几何对象,线段的第一个点落在a上第二个点落在b上
    virtual geo_object* nearest(geo_object* a,geo_object* b)=0;

    /// \brief 坐标转参数
    /// \param p p可以是点线面各种几何对象，也可以是一个索引对象
    /// \param xy 点坐标
    /// \param t 返回的参数，如果xy与p的距离大于容差则返回0，否则t将返回xy所在path_s的参数
    /// \return xy所在的简单对象,如果不为null则必与xy距离小于等于容差
    virtual geo_object* xy2t(geo_object* p,const double* xy,double* t)=0;

    /// \brief 参数转坐标
    /// \param s 几何对象,必须是一个简单线
    /// \param t s上的参数
    /// \param xy 返回的坐标点
    virtual bool t2xy(geo_object* s,double t,double* xy)=0;
	
    /// \brief 查询两个几何对象是否满足指定的空间关系
    /// \param a,b 要查询的两个几何对象(可以是索引对象)
    /// \param mode 空间关系模式(参考spr_is_***定义的模式)
    /// \return 是否满足mode的关系
    /// \参考常量 spr_is_disjion,spr_is_intersect,spr_is_cross,spr_is_equal,spr_is_equal_2,spr_is_touch,
    // spr_is_overlap,spr_is_contain,spr_is_within,spr_is_cross_l = 32,spr_no_touch,spr_can_touch
    virtual bool spr_query_is(geo_object* a,geo_object* b,int mode)=0;

    /// \brief 查询两个几何对象的空间关系spr
    /// \param a,b 要查询的两个几何对象(可以是索引对象)
    /// \return 返回值spr描述了a,b的空间关系，直接使用spr不是很方便 用户可以调用spr_is来测试spr是否满足某模式
    virtual int spr_query(geo_object* a,geo_object* b)=0;

    /// \brief 测试spr是否满足某mode给定的模式
	/// \param spr spr_query或者spr_query_n2n返回的值
	/// \param mode spr_is_***定义的模式
    /// \return 是否满足mode的关系
    /// \参考常量 spr_is_disjion,spr_is_intersect,spr_is_cross,spr_is_equal,spr_is_equal_2,spr_is_touch,
    // spr_is_overlap,spr_is_contain,spr_is_within,spr_is_cross_l = 32,spr_no_touch,spr_can_touch
	virtual bool spr_is(int spr,int mode)=0;
    
    /// \brief 查询两组数据空间关系
    /// \param P,Q 要查询的两组数据
    /// \param mode 保留的参数，传0
    /// \param R 空间关系的接受对象 假如a,b是P,Q中的对象,它们不相离,那么会向R写入一个对象c
    // c对象是一个纯i4属性对象其属性格式为[spr_query(a,b),a->i4(0),…,b->i4(0),…)
    virtual void spr_query_n2n(geo_reader* P,geo_reader* Q,geo_writer* R,int mode)=0;

    /// \brief 创建空间索引对象
    /// \param obj 输入的几何对象
    /// \param mode 保留参数传0
    /// \return 空间索引对象
    /// \details 利用空间索引对象可以提高很多空间分析算法的性能。假设a是一个几何对象,sa是a的索引对象，那么对任何几何对象b
    // 函数spr_query(a,b)和spr_query(sa,b)的返回结果都是一样的，而后者的执行速度要远由于前者。具有同样效果的函数还有
    // spr_query_is,gcombine,gsplit等等。
    virtual geo_object* spi_create(geo_object* obj,int mode)=0;

    /// \brief 两个几何对象的组合运算
    /// \param a,b 参与计算的两个几何对象,可以是索引对象
    /// \param mode 指定组合方式 交、并、差、对称差
    /// \return 组合结果的几何对象
    /// \参考常量 gcombine_union,gcombine_intersect,gcombine_diffrence,gcombine_sym_diffrence
    virtual geo_object* gcombine(geo_object* a,geo_object* b,int mode)=0;
    
    /// \brief 几何切割
    /// \param K 切割的刀对象 可以是几何对象，也可以是索引对象
    /// \param x 被切割的对象 可以是几何对象，也可以是一个geo_reader
    /// \param W 接受切割结果的流
    virtual void gsplit(geo_object* K,geo_object* x,geo_writer* W)=0;

    /// \brief 多面对象合并 
    /// \param R 要并的几何对象流 函数只处理面忽略R中的线、点
    /// \param W 接受结果的流 写入W的每个对象都是单外圈的面
    virtual void n_union(geo_reader* R,geo_writer* W)=0;

    /// \brief 检查单个对象的几何正确性
    /// \param obj 要检查的几何对象
    /// \param mode  指定检查内容 可以是 自相交 线段重叠 方向正确 是否封闭 mode是可以位组合的
	/// \return 返回0表示没有错误，否则返回错误情况
    /// \参考常量 gcheck_1_e_simple,gcheck_1_e_close_2,gcheck_1_e_close,gcheck_1_e_dir
    //gcheck_1_e_cross,gcheck_1_e_lap,gcheck_1_e_repeat
    virtual int gcheck_1(geo_object* obj,int mode)=0;

	/// \brief 检查悬挂点、容错点、伪节点、内交点,孤立线，重复线
    /// \param R 要检查的几何对象，或者几何对象流
    /// \param W 接收检查结果的流对象，函数会往W中写入指示错误位置的 点和线
    /// \param mode 检查方式 mode是可以位组合的
    /// \details 参考常量 gc2_pt_repeat,gc2_pt_isolate,gc2_pt_endp_x,gc2_pt_tol_x,gc2_pt_nodal,
    //gc2_pt_cross,gc2_l_1,gc2_l_n,gc2_a_0,gc2_a_1,gc2_a_n
	virtual void gcheck_2(geo_object* R,geo_writer* W,int mode)=0;

    /// \brief 构造点、线的缓冲区
    /// \param R 输入的几何对象或者对象流
    /// \param radius 缓冲区半径
    /// \param narc 指定圆弧内插成线段的数目和端点的衔接方式 narc/1000 指定端点的衔接方式 0圆头 1平头 2方头 
    // narc%1000指定整圆内插的线段数目 999表示尖角内插
    /// \param W 接受缓冲区的数据流
    /// \details R 中的所有对象面面对象都当作线看待。
    virtual void line_buffer(geo_object* R,double radius,int narc,geo_writer* W)=0;

    /// \brief 数据清理 
    /// \param R 输入的几何对象或者几何对象数据流
    /// \param W 输出的几何对象数据流 输出到W的几何对象都是线对象且都不自相交、互相之间不相交
    virtual void line_clean(geo_object* R,geo_writer* W)=0;

    /// \brief 构圈构面
    /// \param R 输入的几何对象数据流
    /// \param mode 构造方式 mode&1表示R中的每条线都要当作正反两条来使用 
    // mode&2表示R中的线已经是圈了,不需要再构圈了 mode&4表示只构圈不构面，否则要构面
    /// \param W 输出的几何对象数据流
    virtual void build(geo_reader* R,geo_writer* W,int mode)=0;

	/// \brief 图层的叠置
    /// \param A,B 参与叠置的两个图层
    /// \param W 接受叠置结果的数据流
    /// \param mode 叠置的配置参数  mode的最低2位表示输出对象的几何维数
	//         mode&4表示输出对象要带A图层的关系 mode&8表示输出对象要带B图层的关系 
    //         mode&16表示输出对象在一个图层中可以带多个关系
    //         mode&1024表示A、B两个图层是干净的 
    /// \param min_width 狭长面的平均宽度 一个面其面积除于长度称之为平均宽度，如果平均宽度小于min_width那么这个面将会融合到附近的面中，而不会输出
    /// \details A、B两个图层可以是线也可以是面，但在一个图层中必须只有一种类型
	virtual void overlay(geo_reader* A,geo_reader* B,geo_writer* W,double min_width,int mode)=0;
	
	/// \brief 创建point_pool对象
    /// \param R 输入的几何对象或者对象流
    /// \param mode 创建方式
    /// \return point_pool对象此对象可以用来修正path的点坐标
    /// \参考常量 pp_ipt_create,pp_ipt_trans,pp_use_line
	virtual geo_trans* point_pool(geo_object* R,int mode)=0;

	/// \brief 创建line_pool对象
    /// \details 此函数暂时没用
	virtual geo_trans* line_pool(geo_object* R,int mode)=0;

	/// \brief 根据属性对path排序
    /// \param A 要排序的数据流
    /// \param cmp 排序规则 如果为0则使用默认的字典排序规则
    /// \return 排序后的数据流
	virtual geo_reader* sort_by_prop(geo_reader* A,geo_compare* cmp)=0;

	/// \brief 在一个对象数组中执行空间查询
    /// \param A 要查询的数据表
    /// \param path_cid path数据所在的列下标
    /// \param env_cid 外接矩形数据所在的列下标 -1 表示不存在
    /// \param geo 要查询的几何对象
    /// \param mode 查询模式 spr_is_*系列
    /// \return 查询结果
	virtual geo_vector* geo_query(geo_vector* A,int path_cid,int env_cid,geo_object* geo,int mode)=0;

    /// \brief 保拓扑的简化
    /// \param n 要简化的数据个数
    /// \param pp 要简化的几何对象首指针[pp,pp+n)为所有要简化的对象
    /// \param env 要简化的区间，如果env=0则所有点都可以被简化，否则只简化在env内部的点 env的数据结构为(x0,y0,x1,y1)
    /// \param tol 简化容差
    /// \details 程序只会简化[pp,pp+n)内部的点，不会改变任何一个对象的属性
    virtual void top_reserve_simplify(int n,geo_object** pp,double tol,double* env)=0;

    /// \brief 根据参数区间返回一段子path
    /// \param p 输入的几何对象
    /// \param t0,t1 子对象在p上的参数区间
    virtual geo_object* t2path(geo_object* p,double t0,double t1)=0;
	
    /// \brief 文字布局
    /// \param p 输入的几何对象 
    /// \param env 是窗口的可视范围(可以为null) 数据格式为[x,y,x1,y1]
    /// \param font 是字体大小(可以为null) 数据格式为[w，h]
    /// \param n 文字的个数
    /// \param xy 输出的每个文字的左下角坐标
    virtual bool layout(geo_object* p,const double* env,const double* font,int n,double* xy)=0;

    /// \brief 线合并
    /// \param p 输入的几何对象 
    /// \param tol 如果两条线的端点距离小于tol合并
    /// \param len 对于短于len的悬挂线清除
    /// \return 合并后的path_m或path_s
    virtual geo_object* line_combine(geo_object* p,double tol,double len)=0;

    /// \brief 线合并 在交叉路口按照最少拐弯的方式连接
    /// \param p 输入的几何对象 
    /// \return 合并后的path_m或path_s
    virtual geo_object* line_combine2(geo_object* p)=0;

    /// \brief 线消除重复
    /// \param p 输入的几何对象 
    /// \param tol 如果两条线的距离小于tol_line合并
    /// \return 合并后的path_m或path_s
    virtual geo_object* line_unique(geo_object* p,double tol)=0;
};

};

/* 预留的接口设计
namespace geostar {

struct geo_keymap : geo_object
{
    virtual int keylen()=0;
    virtual geo_object* get(const void* key)=0;
};

struct galgorithm2_ex : geo_algorithm
{
    //线构面
    //mode指定 线构圈 圈构面 线构面
    virtual geo_object* build_by_point(geo_keymap* db,int n,const void* keys,const double* envs,double x,double y)=0;
}; } */

//各种常量
namespace geostar {

/// \brief 取消的属性
/// \details geo_algorithm类中有一些耗时的函数，如overlay,query_n2n,line_buffer等等，
// 假如W是这些函数中的一个geo_object类型的参数(可以是输入参数，也可以是输出参数），如果耗时函数在调用下面的is_cancel(W)返回true后都将终止函数的任务
// bool is_cancel(geo_object* g){ return g->i4_get(prop_cancel)!=0; }
const int prop_cancel = -100;	

/// \brief 设置时针规则的全局变量
/// \code geo_algorithm* g; ……; g->i4_set(ga_i4_clockwise,value);
/// \endcode
/// \details value=0表示逆时针规则，value=1表示顺时针规则，确省为0
const int ga_i4_clockwise = 0;

/// \brief 设置圆内插线段数目的全局变量
/// \code geo_algorithm* g; ……; g->i4_set(ga_i4_narc,value);
/// \endcode
/// \details value 为圆内插线段的数目 缺省为24
const int ga_i4_narc = 1;

/// \brief 设置缓存大小的全局变量
/// \code geo_algorithm* g; ……; g->i4_set(ga_i4_max_pt_in_mem,value);
/// \endcode
/// \details value为缓存点的个数，换算成字节数位16*value,缺省为8M，也就是128M的内存
// 这个值一定不能设置太小，否则会引发用文件缓存严重影响性能，外部把虚拟内存调大一些会比设小这个变量好很多
const int ga_i4_max_pt_in_mem = 2; 

/// \brief 几何对象的几何维度属性
/// \code geo_object* g; ……; g->i4_get(geometry_i4_gdim); g->i4_set(geometry_i4_gdim,gdim);
/// \endcode
// 设置或者得到的值为几何对象的几何维度，0,1,2分别表示点、线、面
const int geometry_i4_gdim = -2;

/// \brief 对象的属性长度
/// \code geo_object* g; ……; g->i4_get(geometry_i4_prop_len); g->i4_set(geometry_i4_prop_len,len);
/// \endcode
// 设置或者得到的值为对象的属性长度
const int geometry_i4_prop_len = -1;

/// \brief 计算几何对象的长度 (prop_value的mode类型)
const int prop_value_length = 11;

/// \brief 计算几何对象的宽度 (prop_value的mode类型)
const int prop_value_width =  21;

/// \brief 计算几何对象的高度 (prop_value的mode类型)
const int prop_value_height = 31;

/// \brief 计算几何对象的面积 (prop_value的mode类型)
const int prop_value_area =	  12;		

/// \brief 得到几何对象的克隆 (prop_geo的mode类型)
const int prop_geo_clone =   0;

/// \brief 得到几何对象的克隆 (prop_geo的mode类型)
/// \details 只对没有真实内存的path克隆,有内存的返回自身
const int prop_geo_clone_mem0 = 1;		

/// \brief 得到几何对象的标志点 (prop_geo,prop_geo2的mode类型)
const int prop_geo_label =   10;

/// \brief 得到几何对象的中心 (prop_geo,prop_geo2的mode类型)
const int prop_geo_center =  20;		

/// \brief 得到几何对象的重心 (prop_geo,prop_geo2的mode类型)
const int prop_geo_centroid = 30;	

/// \brief 得到几何对象的最长线 (prop_geo,prop_geo2的mode类型)
const int prop_geo_maxline = 11;		

/// \brief 得到几何对象的外接矩形 (prop_geo,prop_geo2的mode类型)
const int prop_geo_env =	 12;		

/// \brief 得到几何对象的凸壳 (prop_geo的mode类型)
const int prop_geo_hull = 22;			

/// \brief 得到几何对象的斜外接矩形 (prop_geo的mode类型)
const int prop_geo_env_nstd = 32;

/// \brief 得到几何对象的外接圆 (prop_geo,prop_geo2的mode类型)
const int prop_geo_env_circle = 42;		

/// \brief 得到几何对象的内接矩形 (prop_geo,prop_geo2的mode类型)
const int prop_geo_inner_rect = 52;	

/// \brief 得到几何对象的内接正方形 (prop_geo,prop_geo2的mode类型)
const int prop_geo_inner_square = 62;

/// \brief 得到几何对象的内接圆 (prop_geo,prop_geo2的mode类型)
const int prop_geo_inner_circle = 72;

/// \brief 得到几何对象的简化对象 (prop_geo的mode类型)
const int prop_geo_simplify = 13;		

/// \brief 得到几何对象的边界对象 (prop_geo的mode类型)
const int prop_geo_boundary = 23;

/// \brief 删除重复的点 (gmodify的mode类型)
/// \details gmodify的另一个参数para为距离平方
const int gmodify_remove_repeat = 1;	

/// \brief 道格拉斯压缩 (gmodify的mode类型)
/// \details gmodify的另一个参数para为距离
const int gmodify_dauglas = 2;			

/// \brief 反向 (gmodify的mode类型)
/// \details gmodify的另一个参数para为null
const int gmodify_reverse = 3;

/// \brief force_good_dir (gmodify的mode类型)
/// \details gmodify的另一个参数para为null
const int gmodify_force_good_dir = 4;

/// \brief force_geo_close (gmodify的mode类型)
/// \details gmodify的另一个参数para为null
const int gmodify_force_geo_close = 5;

/// \brief 面求并 (gcombine的mode类型)
const int gcombine_union = 1;

/// \brief 面求交 (gcombine的mode类型)
const int gcombine_intersect = 2;

/// \brief 面求差 (gcombine的mode类型)
const int gcombine_diffrence = 3;

/// \brief 面求对称差 (gcombine的mode类型)
const int gcombine_sym_diffrence = 4;

/// \brief 是否是简单对象 (gcheck_1的mode类型)
const int gcheck_1_e_simple = 0;

/// \brief 强封闭检查 (gcheck_1的mode类型)
const int gcheck_1_e_close_2 = 1;

/// \brief 封闭检查 (gcheck_1的mode类型)
const int gcheck_1_e_close = 2;			

/// \brief 方向是否正确 (gcheck_1的mode类型)
const int gcheck_1_e_dir = 4;			

/// \brief 自相交 (gcheck_1的mode类型)
const int gcheck_1_e_cross = 8;			

/// \brief 线重叠 (gcheck_1的mode类型)
const int gcheck_1_e_lap = 16;			

/// \brief 点重叠 (gcheck_1的mode类型)
const int gcheck_1_e_repeat = 32;       

/// \brief 重复点 (gcheck_2的mode类型)
const int gc2_pt_repeat = 0x01;

/// \brief 悬挂点 (gcheck_2的mode类型)
const int gc2_pt_isolate = 0x02;	

/// \brief 伪端点 (gcheck_2的mode类型)
const int gc2_pt_endp_x = 0x04;		

/// \brief 容错点 在容差范围内但不精确相同 (gcheck_2的mode类型)
const int gc2_pt_tol_x = 0x08;		

/// \brief 相交的节点 一个必须是节点 (gcheck_2的mode类型)
const int gc2_pt_nodal = 0x10;		

/// \brief 相穿点 (gcheck_2的mode类型)
const int gc2_pt_cross = 0x20;		

/// \brief 孤立线 (gcheck_2的mode类型)
const int gc2_l_1 = 0x100;			

/// \brief 重叠线 (gcheck_2的mode类型)
const int gc2_l_n = 0x200;			

/// \brief 镂空的面 (gcheck_2的mode类型)
const int gc2_a_0 = 0x1000;			

/// \brief 只被一个包含的面 (gcheck_2的mode类型)
const int gc2_a_1 = 0x2000;			

/// \brief 被多个包含的面 (gcheck_2的mode类型)
const int gc2_a_n = 0x4000;			

/// \brief 是否相离 （spr_is的模式)
const int spr_is_disjion = 1;

/// \brief 是否相交 （spr_is的模式)
const int spr_is_intersect = 2;

/// \brief 是否相穿 （spr_is的模式)
const int spr_is_cross = 3;

/// \brief 是否大致相等(线段可以拆分) （spr_is的模式)
const int spr_is_equal = 4;   

/// \brief 是否精确相等(线段不可以拆分) （spr_is的模式)
const int spr_is_equal_2 = 5; 

/// \brief 是否相触 （spr_is的模式)
/// \details 点在线的端点；点是面的节点或边界线上点；线与线仅相交于端点且不是相穿；
// 线与面在节点处相交且不相穿不在面内部；面与面在节点处相交且不重叠
const int spr_is_touch = 6; 

/// \brief 是否重叠 （spr_is的模式)
/// \details 相同维度之间才存在重叠
const int spr_is_overlap = 7; 

/// \brief 是否包含 （spr_is的模式)
const int spr_is_contain = 8;

/// \brief 是否被包含 （spr_is的模式)
const int spr_is_within = 16;

/// \brief 是否在线的语义下相穿 （spr_is的模式)
const int spr_is_cross_l = 32;

/// \brief 边界相触表没有 （spr_is的模式)
/// \details 当两个几何对象的边界相交的时候认为它们没有is关系
const int spr_no_touch = 0x10000000;  

/// \brief 边界相触表不影响 （spr_is的模式)
/// \details 当两个几何对象的边界相交的时候不影响它们的is关系
const int spr_can_touch = 0x20000000; 

/// \brief point_pool函数模式
/// \details point_pool函数的mode&pp_ipt_create时对输入的线段间的交点也会计算出来
const int pp_ipt_create = 0x01; 

/// \brief point_pool函数模式
/// \details point_pool函数的mode&pp_ipt_trans时返回的geo_trans在trans时候会在线段间内插节点
const int pp_ipt_trans = 0x02;  

/// \brief point_pool函数模式
/// \details point_pool函数的mode&pp_use_line时返回的geo_trans在trans时候会对点进行线段意义上的贴靠
const int pp_use_line = 0x04;  

/// \brief geo_draw::new_text的mode类型
//details 显示文本不能压盖对象
const int draw_txt_nocover = 1;

/// \brief geo_draw::new_text的mode类型
//details 显示文本可以拆分成单个字
const int draw_txt_split = 2;   

};

