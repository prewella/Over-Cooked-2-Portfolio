#include "stdafx.h"
#include "Part_DirtyPlate.h"

CPart_DirtyPlate::CPart_DirtyPlate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice,pContext }
{
}

CPart_DirtyPlate::CPart_DirtyPlate(const CPart_DirtyPlate& rhs)
	:CPartObject{ rhs }
{
}

HRESULT CPart_DirtyPlate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPart_DirtyPlate::Initialize(void* pArg)
{
	PART_DIRTYPLATE_DESC* pPlateWasgDesc = (PART_DIRTYPLATE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pPlateWasgDesc->vSettingPos);

	return S_OK;
}

int CPart_DirtyPlate::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CPart_DirtyPlate::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPart_DirtyPlate::Render()
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

HRESULT CPart_DirtyPlate::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Dirty_Plate"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_DirtyPlate::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CPart_DirtyPlate* CPart_DirtyPlate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPart_DirtyPlate* pInstance = new CPart_DirtyPlate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_DirtyPlate"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPart_DirtyPlate::Clone(void* pArg)
{
	CPart_DirtyPlate* pInstance = new CPart_DirtyPlate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_DirtyPlate"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPart_DirtyPlate::Free()
{
	__super::Free();
}
