#include "stdafx.h"
#include "Waste_Bin.h"

CWaste_Bin::CWaste_Bin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CWaste_Bin::CWaste_Bin(const CWaste_Bin& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CWaste_Bin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWaste_Bin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eType = OBJECT_WASTE_BIN;

	return S_OK;
}

int CWaste_Bin::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CWaste_Bin::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CWaste_Bin::Render()
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

_bool CWaste_Bin::Check_PutDown(CBelonging* pBelonging)
{
	// 접시면, 위에 있는 음식만 없애는데, 이건 플레이어쪽에서 하는게 낳겠져?
	return true;
}

HRESULT CWaste_Bin::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Waste_Bin"),
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

HRESULT CWaste_Bin::Bind_ShaderResources()
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

CWaste_Bin* CWaste_Bin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaste_Bin* pInstance = new CWaste_Bin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWaste_Bin"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWaste_Bin::Clone(void* pArg)
{
	CWaste_Bin* pInstance = new CWaste_Bin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CWaste_Bin"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaste_Bin::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
