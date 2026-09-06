#include "stdafx.h"
#include "Table.h"

#include "Plate.h"
#include "Cooker.h"

CTable::CTable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CTable::CTable(const CTable& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CTable::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTable::Initialize(void* pArg)
{
	TABLE_DESC* pDesc = (TABLE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pDesc)
	{
		if (pDesc->strModelTag != TEXT(""))
		{
			if (FAILED(Add_Components(pDesc->strModelTag)))
				return E_FAIL;
		}
		else
			if (FAILED(Add_Components()))
				return E_FAIL;
	}
	else
		if (FAILED(Add_Components()))
			return E_FAIL;

	if (pDesc->IsCreatePlate)
		Add_Plate();

	m_eType = OBJECT_TABLE;

	return S_OK;
}

int CTable::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CTable::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTable::Render()
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

HRESULT CTable::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Table_NoEdge"),
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

HRESULT CTable::Add_Components(wstring strModelTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
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

HRESULT CTable::Add_Plate()
{
	CBelonging* pPlate = nullptr;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pPlate, LEVEL_GAMEPLAY, TEXT("Layer_Belongings"), TEXT("Prototype_GameObject_Plate"))))
		return E_FAIL;

	m_pBelonging = pPlate;
	Set_On_Belonging();

	return S_OK;
}

HRESULT CTable::Bind_ShaderResources()
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

CTable* CTable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTable* pInstance = new CTable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTable"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTable::Clone(void* pArg)
{
	CTable* pInstance = new CTable(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTable"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTable::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
