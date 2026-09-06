#include "stdafx.h"
#include "Plate_Return.h"

#include "Plate.h"
#include "Dirty_Plate.h"

CPlate_Return::CPlate_Return(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CPlate_Return::CPlate_Return(const CPlate_Return& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CPlate_Return::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlate_Return::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eType = OBJECT_PLATE_RETURN;

	return S_OK;
}

int CPlate_Return::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CPlate_Return::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlate_Return::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (m_IsAdjacent)
		{
			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

_bool CPlate_Return::Check_PutDown(CBelonging* pBelonging)
{
	// 아무것도 못 놔유
	if (nullptr == m_pBelonging)
		return false;
	else
	{
		switch (m_pBelonging->Get_Type())
		{
		case Client::BELONGING_INGREDIENT:
			return false;
			break;
		case Client::BELONGING_PLATE:
			if (((CPlate*)m_pBelonging)->Check_Plating(pBelonging))
				return true;
			break;
		case Client::BELONGING_DIRTY_PLATE:
			return false;
			break;
		}
	}
	return false;
}

void CPlate_Return::Take_HaveObj(CBelonging** pBelonging)
{
	if (nullptr == m_pBelonging)
		return;

	*pBelonging = m_pBelonging;
	Safe_Release(m_pBelonging);

	if (m_IsExist_Sink)
	{
		m_pBelonging = nullptr;
	}
	else
	{
		Safe_Release(m_CleanPlates.back());
		m_CleanPlates.pop_back();

		if (0 < m_CleanPlates.size())
		{
			m_pBelonging = m_CleanPlates.back();
			Safe_AddRef(m_pBelonging);
		}
		else
			m_pBelonging = nullptr;
	}
}

void CPlate_Return::Set_On_Belonging()
{
	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	if (m_IsExist_Sink)
		vPos += XMVectorSet(0.f, 0.4f, 0.f, 0.f);
	else
		vPos += XMVectorSet(0.f, 0.4f + (0.125f * (m_CleanPlates.size() - 1)), 0.f, 0.f);
	XMVectorSetW(vPos, 1.f);

	((CTransform*)m_pBelonging->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_POSITION, vPos);

}

HRESULT CPlate_Return::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plate_Return"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlate_Return::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CPlate_Return::Return_Plate()
{
	if (m_IsExist_Sink) { 
		// 위에 없었으면 생성
		if (nullptr == m_pBelonging)
		{
			m_pGameInstance->Add_Clone((CGameObject**)&m_pBelonging, LEVEL_GAMEPLAY, 
				TEXT("Layer_Belongings"), TEXT("Prototype_GameObject_Dirty_Plate"));
			Set_On_Belonging();
		}
		// 있으면 dirty_plate의 part오브젝트 추가
		else
		{
			((CDirty_Plate*)m_pBelonging)->Stack_Plate();
		}
	}
	else
	{
		CBelonging* pPlate = nullptr;
		m_pGameInstance->Add_Clone((CGameObject**)&pPlate, LEVEL_GAMEPLAY, 
			TEXT("Layer_Belongings"), TEXT("Prototype_GameObject_Plate"));

		if (nullptr != pPlate)
		{
			m_CleanPlates.push_back(pPlate);

			Safe_Release(m_pBelonging);
			m_pBelonging = m_CleanPlates.back();
			Safe_AddRef(m_pBelonging);

			Set_On_Belonging();
		}
	}
}

CPlate_Return* CPlate_Return::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlate_Return* pInstance = new CPlate_Return(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlate_Return"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlate_Return::Clone(void* pArg)
{
	CPlate_Return* pInstance = new CPlate_Return(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlate_Return"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlate_Return::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	for (auto& plate : m_CleanPlates)
		Safe_Release(plate);

	m_CleanPlates.clear();
}