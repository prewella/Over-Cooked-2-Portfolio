#include "stdafx.h"
#include "UI_Transition.h"

#include "UI_Curtain.h"

CUI_Transition::CUI_Transition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Transition::CUI_Transition(const CUI_Transition& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Transition::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Transition::Initialize(void* pArg)
{
	TRANSITION_DESC* pDesc = (TRANSITION_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	CUI_Curtain::CURTAIN_DESC pCurtainDesc = {};
	pCurtainDesc.pBlindSize = &m_vSize;
	pCurtainDesc.pIsRender = &m_IsRender;

	if (FAILED(m_pGameInstance->Add_Clone(pDesc->iLevelIdx,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Curtain"),&pCurtainDesc)))
		return E_FAIL;

	m_vSize = _float2(g_iWinSizeX, g_iWinSizeY);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_PRIORITY;

	return S_OK;
}

_int CUI_Transition::Tick(_float fTimeDelta)
{
	if (m_IsRender)
	{
		if (m_IsEnterLevel)
		{
			if (m_vSize.x < 2560.f)
				m_vSize = _float2(m_vSize.x * 1.125f, m_vSize.y * 1.125f);
			else
			{
				m_IsRender = false;
				m_IsTransitionEnter_Finish = true;
			}
		}
		else
		{
			if (m_vSize.x >= 16.f)
				m_vSize = _float2(m_vSize.x * 0.875f, m_vSize.y * 0.875f);
			else
			{
				m_IsRender = false;
				m_IsTransitionExit_Finish = true;
			}
		}
		m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
	}

	return EVENT_NONE;
}

void CUI_Transition::Late_Tick(_float fTimeDelta)
{
	if (m_IsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_AFTER_UI, this);
}

HRESULT CUI_Transition::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Transition::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Transitions"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Transition::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

void CUI_Transition::Set_EnterLevel()
{
	m_IsRender = true;
	m_IsEnterLevel = true;
	m_IsTransitionEnter_Finish = false;
	m_IsTransitionExit_Finish = false;
	m_vSize = _float2(16.f, 16.f);
	m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
	m_pGameInstance->StopSound(CSound_Manager::CH_LEVEL_TRANSITION);
	m_pGameInstance->Play_Sound(L"Level_Enter.wav", CSound_Manager::CH_LEVEL_TRANSITION, 1.f);
}

void CUI_Transition::Set_ExitLevel()
{
	m_IsRender = true;
	m_IsEnterLevel = false;
	m_IsTransitionEnter_Finish = false;
	m_IsTransitionExit_Finish = false;
	m_vSize = _float2(2560.f, 2560.f);
	m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
	m_pGameInstance->StopSound(CSound_Manager::CH_LEVEL_TRANSITION);
	m_pGameInstance->Play_Sound(L"Level_Out.wav", CSound_Manager::CH_LEVEL_TRANSITION, 1.f);
}

CUI_Transition* CUI_Transition::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Transition* pInstance = new CUI_Transition(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Transition"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Transition::Clone(void* pArg)
{
	CUI_Transition* pInstance = new CUI_Transition(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Transition"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Transition::Free()
{
	__super::Free();
}
