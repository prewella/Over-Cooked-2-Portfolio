#include "stdafx.h"
#include "Empty_Collider.h"

CEmpty_Collider::CEmpty_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

CEmpty_Collider::CEmpty_Collider(const CEmpty_Collider& rhs)
	:CGameObject{ rhs }
{
}

HRESULT CEmpty_Collider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEmpty_Collider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

int CEmpty_Collider::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CEmpty_Collider::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CEmpty_Collider::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CEmpty_Collider::Add_Components()
{
	CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	_float3 vScaled = m_pTransformCom->Get_Scaled();

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CEmpty_Collider* CEmpty_Collider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEmpty_Collider* pInstance = new CEmpty_Collider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEmpty_Collider"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEmpty_Collider::Clone(void* pArg)
{
	CEmpty_Collider* pInstance = new CEmpty_Collider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEmpty_Collider"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEmpty_Collider::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
