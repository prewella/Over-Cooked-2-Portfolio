#include "stdafx.h"
#include "Prop.h"

CProp::CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelObject{ pDevice, pContext }
{
}

CProp::CProp(const CProp& rhs)
	: CModelObject{ rhs }
{

}

HRESULT CProp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProp::Initialize(void* pArg)
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

_int CProp::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CProp::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CProp::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CProp::Add_Components()
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

HRESULT CProp::Add_Components(wstring strModelTag)
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

HRESULT CProp::Bind_ShaderResources()
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

CProp* CProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProp* pInstance = new CProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProp"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CProp::Clone(void* pArg)
{
	CProp* pInstance = new CProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CProp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProp::Free()
{
	__super::Free();
}
