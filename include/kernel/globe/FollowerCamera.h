#pragma once
#include <utility.h>
#include <GsReference.h>
#include <Element.h>

GLOBE_NS

enum Modal
{
	//插值飞行，适用于跟新不频繁的对象
	interp,
	//跟随飞行，适用于跟随频繁的对象
	follow
};

class GS_API FollowerCamera :public GeoStar::Utility::GsRefObject
{
public:

	FollowerCamera(Modal eModel = follow);

	virtual ~FollowerCamera();
	//会跟随被绑定着飞行
	void Bind(Element* ptrElement);
	//解除绑定着飞行
	void Unbind(Element* ptrElement);
	//重新建立跟随
	void ReEstablish();
	//设置跟随模式
	void setModal(Modal eModel);

private:
	void Init();

	void OnPositionChange(KERNEL_NAME::GsRawPoint3D point);

	std::vector<KERNEL_NAME::GsRawPoint3D> m_vecPos;

	time_t m_preTime;

	GsReferencePtr m_ptrHandle;	
	ElementPtr m_ptrElement;
	Modal m_eModal;
};

GS_SMARTER_PTR(FollowerCamera);

GLOBE_ENDNS
