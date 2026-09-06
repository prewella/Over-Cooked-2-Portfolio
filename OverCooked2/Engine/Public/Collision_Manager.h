#pragma once

#include "Base.h"

/* 1. 화면에 그려져야할 객체들만 그려지는 순서대로 보관하는 클래스이다.*/
/* 2. 보관하고 있는 순서대로 객체들의 Draw콜(렌더함수를호출한다.)을 수행한다.*/

BEGIN(Engine)

class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;
public:
	HRESULT Initialize();
	void Update_Collision();
	HRESULT Add_Collision_Group(class CGameObject* pCollisionObject);

private:
	list<class CGameObject*> m_CollisionObjects;

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};

END