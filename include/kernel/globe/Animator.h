#pragma once
#include "utility.h"
#include "geometry.h"
/*****************动画模型封装的类*******************/
GLOBE_NS

//
enum ActionState
{
	Start,
	Stop
};
//

class GS_API Animator:public Utility::GsRefObject
{
public:
	Animator();
	virtual ~Animator();
};
GS_SMARTER_PTR(Animator);
/// \brief 浮点动画过程
class GS_API FloatAnimator :public Animator
{
public:
	FloatAnimator(const std::vector<float>& values, float startLife, float endLife, bool mode);
	FloatAnimator(const std::vector<float>& values);
	virtual ~FloatAnimator();
	//获取当前值的过程
	virtual float GetValue(float a);
	std::vector<float>& GetFloats();
	float GetStartLife();
	float GetEndLife();
protected:
	float m_StartLife;
	float m_EndLife;
	bool m_Mode;
	std::vector<float> m_Values;
	float m_DurLife;
};

GS_SMARTER_PTR(FloatAnimator);
//路径动画插值的过程

class GS_API DynamicPathAnimator :public FloatAnimator
{
public:
	DynamicPathAnimator();
	DynamicPathAnimator(const std::vector<float>& values, float startLife, float endLife);
	~DynamicPathAnimator();
	//
	virtual GeoStar::Kernel::GsRawPoint& GetRawPoint(float a);
	void AddDynamic(const GeoStar::Kernel::GsRawPoint & point);//动态添加目标点
	virtual GeoStar::Kernel::GsRawPoint&  CurRawPoint();
	 //轨迹当前指向的角度
	virtual float CurAngle();
protected:
	void initalPath();
protected:
	std::vector<KERNEL_NAME::GsRawPoint> m_PathPoints;
	std::vector<double> m_SubLen;//每段的效果情况
	std::vector<float> m_Angles;//当前角度
	double m_PathLen;//路径长度
	int m_Index;//
	GeoStar::Kernel::GsRawPoint m_CurPoint;
};
GS_SMARTER_PTR(DynamicPathAnimator);
//
/// \brief 动画执行者，指定时间或是否循环播放的过程
class GS_API AnimatorAction:public Utility::GsRefObject
{
public:
	//动画化执行时长单位毫秒，循环播放
	AnimatorAction(int durtion, bool isLoop);
	AnimatorAction(int durtion, int loops, bool isLoop=true);
	~AnimatorAction();
	/// \brief 传入没帧的时间信息的过程
	virtual float Update(int64_t time);//动画时间传入
	// 开始
	void Start();
	// 停止
	void Stop();
	// 是否开始
	bool isStart();
	int GetDurtion();
	bool IsLoop();
	void reSet();
	// 动画事件的过程
	UTILITY_NAME::GsDelegate<void(ActionState)> OnAnimatorEvent;
protected:
	virtual void OnUpdate(float a) = 0;
protected:
	bool m_isStarted;
	int64_t m_AniTime;
	float m_Durtion;
	bool m_isLoop;
	int m_Loops;

};
GS_SMARTER_PTR(AnimatorAction);
// 线的样式
class GS_API LineAnimatorAction :public AnimatorAction
{
public:
	LineAnimatorAction(int durtion, bool isLoop);
	LineAnimatorAction(int durtion, int loops, bool isLoop = true);
	~LineAnimatorAction();
	// 贴图纹理动画
	FloatAnimator *getTextCoord();
	// 设置纹理动画
	void setTextCoord(FloatAnimator* animator);
	//透明动画
	FloatAnimator* getAphla();
	// 设置透明动画
	void setAphla(FloatAnimator* animator);
	// 获取线宽缩放动画
	FloatAnimator* getWidthScale();
	// 设置线宽缩放动画
	void setWidthScale(FloatAnimator* animator);
	FloatAnimator* getPrevice();
	void setPrevice(FloatAnimator* animator);
protected:
	virtual void OnUpdate(float a)override;
protected:
	FloatAnimatorPtr m_TextCoordAnimator;
	FloatAnimatorPtr m_ptrAphlaAnimator;//线透明动画
	FloatAnimatorPtr m_ptrLineScaleAnimator;//线宽动画
	FloatAnimatorPtr m_ptrPreviceAnimator;// 显示线的百分比
};

GS_SMARTER_PTR(LineAnimatorAction);
//
class GS_API PointAnimatorAction :public AnimatorAction
{
public:
	PointAnimatorAction(int durtion, bool isLoop);
	PointAnimatorAction(int durtion, int loops, bool isLoop = true);
	~PointAnimatorAction();
	//透明动画
	FloatAnimator* getAphla();
	// 设置透明动画
	void setAphla(FloatAnimator* animator);
	// 获取缩放动画
	FloatAnimator* getScale();
	// 设置缩放动画
	void setScale(FloatAnimator* animator);
	// 旋转动画
	FloatAnimator* getRota();
	//设置旋转动画
	void setRota(FloatAnimator* animator);
protected:
	virtual void OnUpdate(float a)override;
protected:
	FloatAnimatorPtr m_ptrRotaAnimator;//旋转动画
	FloatAnimatorPtr m_ptrAphlaAnimator;//线透明动画
	FloatAnimatorPtr m_ptrScaleAnimator;//线宽动画
};

GS_SMARTER_PTR(PointAnimatorAction);

GLOBE_ENDNS