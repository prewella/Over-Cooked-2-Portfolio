#include "stdafx.h"
#include "Spone.h"

CSpone::CSpone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelObject{ pDevice, pContext }
{
}

CSpone::CSpone(const CSpone& rhs)
	: CModelObject{ rhs }
{
}

HRESULT CSpone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpone::Initialize(void* pArg)
{
	MODELOBJECT_DESC* pDesc = (MODELOBJECT_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

_int CSpone::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CSpone::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSpone::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CSpone::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Spone"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpone::Bind_ShaderResources()
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

CSpone* CSpone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpone* pInstance = new CSpone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSpone"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CSpone::Clone(void* pArg)
{
	CSpone* pInstance = new CSpone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSpone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpone::Free()
{
	__super::Free();
}
