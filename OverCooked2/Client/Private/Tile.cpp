#include "stdafx.h"
#include "Tile.h"

CTile::CTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelObject{ pDevice, pContext }
{
}

CTile::CTile(const CTile& rhs)
	: CModelObject{ rhs }
{
}

HRESULT CTile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTile::Initialize(void* pArg)
{
	MODELOBJECT_DESC* pDesc = (MODELOBJECT_DESC*)pArg;

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

	return S_OK;
}

_int CTile::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CTile::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTile::Render()
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

HRESULT CTile::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile0"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTile::Add_Components(wstring strModelTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTile::Bind_ShaderResources()
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

CTile* CTile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTile* pInstance = new CTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTile"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTile::Clone(void* pArg)
{
	CTile* pInstance = new CTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTile"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTile::Free()
{
	__super::Free();
}
