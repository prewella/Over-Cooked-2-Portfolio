#include "stdafx.h"
#include "Soup.h"

CSoup::CSoup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice,pContext }
{
}

CSoup::CSoup(const CSoup& rhs)
	:CPartObject{ rhs }
{
}

HRESULT CSoup::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSoup::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(-0.025f, 0.375f, 0.05f, 1.f));

	return S_OK;
}

int CSoup::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CSoup::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CSoup::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CSoup::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Soup"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSoup::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4 vColor = _float4(1.f, 1.f, 0.8f, 0.8f);

	if (FAILED(m_pShaderCom->Bind_RawValue("vDiffuseColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CSoup* CSoup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSoup* pInstance = new CSoup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSoup"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSoup::Clone(void* pArg)
{
	CSoup* pInstance = new CSoup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSoup"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoup::Free()
{
	__super::Free();
}
