#include "Animator.h"

GLOBE_NS

Animator::Animator()
{
}

Animator::~Animator()
{
}


FloatAnimator::FloatAnimator(const std::vector<float>& values, float startLife, float endLife, bool mode):
	m_Values(values),
	m_Mode(mode),
	m_StartLife(startLife),
	m_EndLife(endLife)
{
m_DurLife = endLife - startLife;
}

FloatAnimator::FloatAnimator(const std::vector<float>& values) :FloatAnimator(values, 0, 1, false)
{
}

FloatAnimator::~FloatAnimator()
{
}

float FloatAnimator::GetValue(float a)
{
	if (a <= m_StartLife)
		return m_Values[0];
	if (a >= m_EndLife)
		return m_Values[m_Values.size() - 1];
	float delayP = (a - m_StartLife) / m_DurLife;
	float fIndex = delayP*(m_Values.size() - 1);
	int index = fIndex;
	return m_Values[index] + (m_Values[index + 1] - m_Values[index])*(fIndex - index);
}

std::vector<float>& FloatAnimator::GetFloats()
{
	// TODO: 在此处插入 return 语句
	return m_Values;
}

float FloatAnimator::GetStartLife()
{
	return m_StartLife;
}

float FloatAnimator::GetEndLife()
{
	return m_EndLife;
}

// 动画过程
AnimatorAction::AnimatorAction(int durtion, bool isLoop)
{
	m_isStarted = true;
	m_AniTime = 0;
	m_Durtion = durtion;
	m_Loops = 1;
	m_isLoop = isLoop;
}

AnimatorAction::AnimatorAction(int durtion, int loops, bool isLoop)
{
	m_isStarted = true;
	m_AniTime = 0;
	m_Durtion = durtion;
	m_Loops = loops;
	m_isLoop = isLoop;
}

AnimatorAction::~AnimatorAction()
{
}

float AnimatorAction::Update(int64_t time)
{
	if (!m_isStarted)
		return 0.f;
	if (m_AniTime == 0)
	{
		m_AniTime = time;
	}
	float f = (time - m_AniTime) / m_Durtion;
	int loop = (int)f;
	float a = f - loop;
	if (!m_isLoop && f > m_Loops)
	{
		a = 1;
		Stop();
	}
	OnUpdate(a);
	return a;
}

void AnimatorAction::Start()
{
	if (m_isStarted)
		return;
	m_isStarted = true;
	m_AniTime = 0;
}

void AnimatorAction::Stop()
{
	if (!m_isStarted)
		return;
	m_isStarted = false;
	m_AniTime = 0;
	OnAnimatorEvent(ActionState::Stop);
}

bool AnimatorAction::isStart()
{
	return m_isStarted;
}

int AnimatorAction::GetDurtion()
{
	return m_Durtion;
}

bool AnimatorAction::IsLoop()
{
	return m_isLoop;
}

void AnimatorAction::reSet()
{
	m_isStarted = true;
	m_AniTime = 0;
}

LineAnimatorAction::LineAnimatorAction(int durtion, bool isLoop):
	AnimatorAction(durtion,isLoop)
{
}

LineAnimatorAction::LineAnimatorAction(int durtion, int loops, bool isLoop)
	:AnimatorAction(durtion,loops,isLoop)
{
}

LineAnimatorAction::~LineAnimatorAction()
{
}

FloatAnimator * LineAnimatorAction::getTextCoord()
{
	return m_TextCoordAnimator;
}

void LineAnimatorAction::setTextCoord(FloatAnimator * animator)
{
	m_TextCoordAnimator = animator;
}

FloatAnimator * LineAnimatorAction::getAphla()
{
	return m_ptrAphlaAnimator;
}

void LineAnimatorAction::setAphla(FloatAnimator * animator)
{
	m_ptrAphlaAnimator = animator;
}

FloatAnimator * LineAnimatorAction::getWidthScale()
{
	return m_ptrLineScaleAnimator;
}

void LineAnimatorAction::setWidthScale(FloatAnimator * animator)
{
	m_ptrLineScaleAnimator = animator;
}

FloatAnimator * LineAnimatorAction::getPrevice()
{
	return m_ptrPreviceAnimator;
}

void LineAnimatorAction::setPrevice(FloatAnimator * animator)
{
	m_ptrPreviceAnimator = animator;
}

void LineAnimatorAction::OnUpdate(float a)
{
}
/*----------------------------------------------------------------------------------------------------
*点类型的动画设置
-------------------------------------------------------------------------------------------------------*/

PointAnimatorAction::PointAnimatorAction(int durtion, bool isLoop):AnimatorAction(durtion,isLoop)
{
}

PointAnimatorAction::PointAnimatorAction(int durtion, int loops, bool isLoop):AnimatorAction(durtion,loops,isLoop)
{
}

PointAnimatorAction::~PointAnimatorAction()
{
}

FloatAnimator * PointAnimatorAction::getAphla()
{
	return m_ptrAphlaAnimator;
}

void PointAnimatorAction::setAphla(FloatAnimator * animator)
{
	m_ptrAphlaAnimator = animator;
}

FloatAnimator * PointAnimatorAction::getScale()
{
	return m_ptrScaleAnimator;
}

void PointAnimatorAction::setScale(FloatAnimator * animator)
{
	m_ptrScaleAnimator = animator;
}

FloatAnimator * PointAnimatorAction::getRota()
{
	return m_ptrRotaAnimator;
}

void PointAnimatorAction::setRota(FloatAnimator * animator)
{
	m_ptrRotaAnimator = animator;
}

void PointAnimatorAction::OnUpdate(float a)
{
}

/***********************************-------------------------*******************************************
* 动态点位插值动画
********************************************************************************************************/

DynamicPathAnimator::DynamicPathAnimator():
	DynamicPathAnimator({0,1},0,1)
{
}

DynamicPathAnimator::DynamicPathAnimator(const std::vector<float>& values, float startLife, float endLife)
	:FloatAnimator(values,startLife,endLife,true)
{
	m_CurPoint = KERNEL_NAME::GsRawPoint(0, 0);

}

DynamicPathAnimator::~DynamicPathAnimator()
{

}

GeoStar::Kernel::GsRawPoint & DynamicPathAnimator::GetRawPoint(float a)
{
	// TODO: 在此处插入 return 语句
	if (m_PathPoints.size() == 0)
		return m_CurPoint;
	if (m_PathPoints.size() == 1)
	{
		m_CurPoint = m_PathPoints[0];
		return m_CurPoint;
	}
	//计算
	float f = GetValue(a);
	if (f <= 0)
	{
		m_Index = 0;
		m_CurPoint = m_PathPoints[m_Index];
		return m_CurPoint;
	}
	if (f >= 1)
	{
		m_Index = m_PathPoints.size() - 1;
		m_CurPoint = m_PathPoints[m_Index];
		return m_CurPoint;
	}
	//
	int index = 0;
	double sumLen = 0;
	double curLen = f*m_PathLen;
	while (index < m_SubLen.size())
	{
		sumLen += m_SubLen[index];
		if (sumLen > curLen)
		{
			double xp = (curLen + m_SubLen[index] - sumLen) / m_SubLen[index];
			GeoStar::Kernel::GsRawPoint &p1 = m_PathPoints[index];
			GeoStar::Kernel::GsRawPoint &p2 = m_PathPoints[index + 1];
			m_CurPoint.X = p1.X + xp*(p2.X - p1.X);
			m_CurPoint.Y = p1.Y + xp*(p2.Y - p1.Y);
			m_Index = index;
			return m_CurPoint;
		}
		index++;
	}
	return m_PathPoints[0];
}

void DynamicPathAnimator::AddDynamic(const GeoStar::Kernel::GsRawPoint & point)
{
	if (m_PathPoints.size() == 0)
	{
		m_CurPoint = point;
		m_PathPoints.push_back(point);
	}else
	if (m_PathPoints.size() ==1)
	{
		m_PathPoints[0] = m_CurPoint;
		m_PathPoints.push_back(point);
	}
	else {
		m_PathPoints[0] = m_CurPoint;
		m_PathPoints[1] = point;
	}
	initalPath();
}

GeoStar::Kernel::GsRawPoint & DynamicPathAnimator::CurRawPoint()
{
	// TODO: 在此处插入 return 语句
	return m_CurPoint;
}

float DynamicPathAnimator::CurAngle()
{
	return 0.0f;
}

void DynamicPathAnimator::initalPath()
{
	m_PathLen = 0;
	m_SubLen.resize(m_PathPoints.size() - 1);
//	m_Angles.resize(m_PathPoints.size());
	for (int i = 0, size = m_SubLen.size(); i < size; i++)
	{
		m_SubLen[i] = std::sqrt(m_PathPoints[i].Distance2(m_PathPoints[i + 1]));
		m_PathLen += m_SubLen[i];
	}
//	m_Angles[m_PathPoints.size() - 1] = m_Angles[m_PathPoints.size() - 2];
	m_Index = 0;
}
GLOBE_ENDNS

