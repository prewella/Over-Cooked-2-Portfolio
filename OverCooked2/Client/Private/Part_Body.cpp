#include "stdafx.h"
#include "Part_Body.h"

#include "Character.h"

CPart_Body::CPart_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CPart_Body::CPart_Body(const CPart_Body& rhs)
    : CPartObject{ rhs }
{
}

HRESULT CPart_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPart_Body::Initialize(void* pArg)
{
	BODY_DESC* pPartObjectDesc = (BODY_DESC*)pArg;

	//pPartObjectDesc->fSpeedPerSec = 10.f;
	//pPartObjectDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	m_pState = pPartObjectDesc->pState;

	if (FAILED(__super::Initialize(pPartObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components(pPartObjectDesc->strModelTag)))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, true);

    return S_OK;
}

_int CPart_Body::Tick(_float fTimeDelta)
{
	// 애니메이션 제어
	/*if (*m_pState & CHARACTERSTATE::STATE_IDLE)
	{
		m_pModelCom->Set_Animation(3, true);
	}

	else if (*m_pState & CHARACTERSTATE::STATE_MOVE)
	{
		m_pModelCom->Set_Animation(4, true);
	}*/

	switch (*m_pState)
	{
	case Client::ANIM_BINDPOSE:
	case Client::ANIM_CHOP:
	case Client::ANIM_DASH:
	case Client::ANIM_DASH_HOLDING:
	case Client::ANIM_FRY:
	case Client::ANIM_IDLE:
	case Client::ANIM_IDLE_HOLDING:
	case Client::ANIM_MIX:
	case Client::ANIM_POUR:
	case Client::ANIM_ROLL:
	case Client::ANIM_WALK:
	case Client::ANIM_WALK_HOLDING:
	case Client::ANIM_WASH:
		m_IsLoopAnim = true;
		break;
	case Client::ANIM_THROW:
	case Client::ANIM_CATCH:
	case Client::ANIM_DEATH:
	case Client::ANIM_STAND:
		m_IsLoopAnim = false;
		break;
	}

	m_pModelCom->Set_Animation(*m_pState, m_IsLoopAnim);

	//if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
	//{
	//	m_pModelCom->Set_NextAnimation(true);
	//}

    return EVENT_NONE;
}

void CPart_Body::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta * 1.5f);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPart_Body::Render()
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

HRESULT CPart_Body::Add_Components(wstring strModelTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Body::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CPart_Body* CPart_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPart_Body* pInstance = new CPart_Body(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_Body"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPart_Body::Clone(void* pArg)
{
	CPart_Body* pInstance = new CPart_Body(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_Body"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPart_Body::Free()
{
	__super::Free();
}
