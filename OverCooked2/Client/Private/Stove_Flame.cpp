#include "stdafx.h"
#include "Stove_Flame.h"

CStove_Flame::CStove_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice,pContext }
{
}

CStove_Flame::CStove_Flame(const CStove_Flame& rhs)
	:CPartObject{ rhs }
{
}

HRESULT CStove_Flame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStove_Flame::Initialize(void* pArg)
{
	STOVEFLAME_DESC* pStoveFlameDesc = (STOVEFLAME_DESC*)pArg;

	//pPartObjectDesc->fSpeedPerSec = 10.f;
	//pPartObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_pIsRender = pStoveFlameDesc->pIsRender;

	if (FAILED(__super::Initialize(pStoveFlameDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.5f, 0.f, 1.f));

	return S_OK;
}

int CStove_Flame::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CStove_Flame::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	if (*m_pIsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CStove_Flame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(6)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CStove_Flame::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Stove_Flame"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStove_Flame::Bind_ShaderResources()
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

CStove_Flame* CStove_Flame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStove_Flame* pInstance = new CStove_Flame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStove_Flame"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStove_Flame::Clone(void* pArg)
{
	CStove_Flame* pInstance = new CStove_Flame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStove_Flame"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStove_Flame::Free()
{
	__super::Free();
}
