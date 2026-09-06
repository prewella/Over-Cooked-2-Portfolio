#include "stdafx.h"
#include "OnionKing.h"

COnionKing::COnionKing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CModelObject{pDevice, pContext}
{
}

COnionKing::COnionKing(const COnionKing& rhs)
	:CModelObject{rhs}
{
}

HRESULT COnionKing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COnionKing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(2, true);

	m_pTransformCom->Set_Scaled(0.175f, 0.175f, 0.175f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-0.3f, -0.075f, 0.f, 1.f));
	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP), XMConvertToRadians(-45));

	return S_OK;
}

_int COnionKing::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void COnionKing::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_MODEL_UI, this);
}

HRESULT COnionKing::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT COnionKing::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_OnionKing"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT COnionKing::Bind_ShaderResources()
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

COnionKing* COnionKing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COnionKing* pInstance = new COnionKing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : COnionKing"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COnionKing::Clone(void* pArg)
{
	COnionKing* pInstance = new COnionKing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COnionKing"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void COnionKing::Free()
{
	__super::Free();
}
