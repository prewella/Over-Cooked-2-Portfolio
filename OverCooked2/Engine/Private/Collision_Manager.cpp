#include "Collision_Manager.h"
#include "GameObject.h"

#include "Collider.h"

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Initialize()
{
	return S_OK;
}

void CCollision_Manager::Update_Collision()
{
	for (auto CollisionObj : m_CollisionObjects)
	{
		CTransform* pTrnasform = (CTransform*)CollisionObj->Get_Component(g_strTransformTag);
		((CCollider*)CollisionObj->Get_Component(TEXT("Com_Collider")))->Tick(pTrnasform->Get_WorldMatrix());
		Safe_Release(CollisionObj);
	}

	m_CollisionObjects.clear();
}

HRESULT CCollision_Manager::Add_Collision_Group(CGameObject* pCollisionObject)
{
	m_CollisionObjects.push_back(pCollisionObject);
	Safe_AddRef(pCollisionObject);

	return S_OK;
}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CCollision_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	__super::Free();

	for (auto& pCollsionObj : m_CollisionObjects)
		Safe_Release(pCollsionObj);
	m_CollisionObjects.clear();
}
