#include "stdafx.h"
#include "Part_Control_Mark.h"

CPart_Control_Mark::CPart_Control_Mark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CPart_Control_Mark::CPart_Control_Mark(const CPart_Control_Mark& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CPart_Control_Mark::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPart_Control_Mark::Initialize(void* pArg)
{
	CONTROL_MARK_DESC* pDesc = (CONTROL_MARK_DESC*)pArg;

	m_pIsControl = pDesc->pIsControl;
	m_pIsThrowing = pDesc->pIsThrowing;

	m_pModelComs.resize(2);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

_int CPart_Control_Mark::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CPart_Control_Mark::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	if (*m_pIsControl)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPart_Control_Mark::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelComs[*m_pIsThrowing]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[*m_pIsThrowing]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		m_pModelComs[*m_pIsThrowing]->Render(i);
	}

	return S_OK;
}

HRESULT CPart_Control_Mark::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Control_Mark"),
		TEXT("Com_Model0"), (CComponent**)&m_pModelComs[0])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Aim_Control_Mark"),
		TEXT("Com_Model1"), (CComponent**)&m_pModelComs[1])))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Control_Mark::Bind_ShaderResources()
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

CPart_Control_Mark* CPart_Control_Mark::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPart_Control_Mark* pInstance = new CPart_Control_Mark(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_Control_Mark"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPart_Control_Mark::Clone(void* pArg)
{
	CPart_Control_Mark* pInstance = new CPart_Control_Mark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_Control_Mark"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPart_Control_Mark::Free()
{
	__super::Free();

	for (auto& pModel : m_pModelComs)
	{
		Safe_Release(pModel);
	}

}
