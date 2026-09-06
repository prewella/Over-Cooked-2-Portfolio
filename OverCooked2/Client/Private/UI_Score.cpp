#include "stdafx.h"
#include "UI_Score.h"

#include "UI_Coin.h"
#include "UI_Flame.h"
#include "UI_Tip.h"

CUI_Score::CUI_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Score::CUI_Score(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Score::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Score::Initialize(void* pArg)
{
	UI_SCORE_DESC* pDesc = (UI_SCORE_DESC*)pArg;
	m_pScore = pDesc->pScore;
	m_pIsAddScore = pDesc->pIsAddScore;
	m_pIsTips = pDesc->pIsTips;
	m_pTipTimes = pDesc->pTipTimes;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(180.f, 80.f);
	m_vPos = _float2(150.f, 670.f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	Add_Score_UI();

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_LAST;

	return S_OK;
}

_int CUI_Score::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;
	 
	if (*m_pTipTimes >= 3)
	{
		m_pUI_Flame->Render_UI(true);
	}
	else
	{
		m_pUI_Flame->Render_UI(false);
	}

	//m_pUI_Flame->Render_UI(true);

	return EVENT_NONE;
}

void CUI_Score::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Score::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (*m_pIsAddScore)
		m_pGameInstance->Render_Font(TEXT("Font_Segoe32"), to_wstring(*m_pScore), _float2(120.f, 625.f), XMVectorSet(0.f, 1.f, 0.f, 1.f), 0.f);
	else
		m_pGameInstance->Render_Font(TEXT("Font_Segoe32"), to_wstring(*m_pScore) , _float2(120.f, 625.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);

	return S_OK;
}

HRESULT CUI_Score::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Score::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fRatioX = 275.f / (_float)m_vSize.x;
	_float fRatioY = 111.f / (_float)m_vSize.y;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioX", &fRatioX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioY", &fRatioY, sizeof(_float))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

void CUI_Score::Coin_Spinning()
{
	m_pUI_Coin->Anim_Start();
}

HRESULT CUI_Score::Add_Score_UI()
{
	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Coin, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Coin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Flame, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Flame"))))
		return E_FAIL;

	CUI_Tip::UI_TIP_DESC UI_Tip_Desc = {};
	UI_Tip_Desc.pTimes = m_pTipTimes;
	UI_Tip_Desc.pIsTips = m_pIsTips;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Tip, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Tip"), &UI_Tip_Desc)))
		return E_FAIL;

	return S_OK;
}

CUI_Score* CUI_Score::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Score* pInstance = new CUI_Score(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Score"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Score::Clone(void* pArg)
{
	CUI_Score* pInstance = new CUI_Score(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Score"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Score::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Coin);
	Safe_Release(m_pUI_Flame);
	Safe_Release(m_pUI_Tip);
}
