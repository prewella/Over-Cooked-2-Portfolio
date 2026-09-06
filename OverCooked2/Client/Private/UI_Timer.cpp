#include "stdafx.h"
#include "UI_Timer.h"

#include "UI_Hourglass.h"
#include "UI_Timer_Banner.h"

CUI_Timer::CUI_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Timer::CUI_Timer(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Timer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Timer::Initialize(void* pArg)
{
	UI_TIMER_DESC* pDesc = (UI_TIMER_DESC*)pArg;
	m_pRemain_Time = pDesc->pRemain_Time;
	m_pTotal_Time = pDesc->pTotal_Time;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(230.f, 80.f);
	m_vPos = _float2(1100, 670.f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	Add_Timer_UI();

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_LAST;

	return S_OK;
}

_int CUI_Timer::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	// 남은 시간이 30초 이하라면
	if (*m_pRemain_Time <= 30.f)
	{
		m_fNowReaminTime = (_uint)*m_pRemain_Time;

		// 모래시계를 흔듦
		m_pUI_Hourglass->Shake_Hourglass();
		if (m_fNowReaminTime < m_fPreReaminTime)
		{
			m_IsTimeChange = true;
			m_pGameInstance->StopSound(CSound_Manager::CH_TIMER);
			m_pGameInstance->Play_Sound(L"Timer_Beep.wav", CSound_Manager::CH_TIMER, 1.f);
		}
	}

	if (m_IsTimeChange)
	{
		m_TimerAlarm += fTimeDelta;
		if (m_TimerAlarm > 0.25f) 
		{
			m_TimerAlarm = 0.f;
			m_IsTimeChange = false;
		}
	}
	

	m_fPreReaminTime = m_fNowReaminTime;

	return EVENT_NONE;
}

void CUI_Timer::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Timer::Render()
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

	wstring strReaminMinute = TEXT("0") + to_wstring((_uint)(*m_pRemain_Time / 60));

	_uint iRemainSecond = (_uint)*m_pRemain_Time % 60;
	wstring strRemainSecond = TEXT("");

	if (iRemainSecond < 10)
		strRemainSecond = TEXT("0") + to_wstring(iRemainSecond);
	else
		strRemainSecond = to_wstring(iRemainSecond);

	if (m_IsTimeChange)
	{
		m_pGameInstance->Render_Font(TEXT("Font_Segoe40"), strReaminMinute + TEXT(" : ") + strRemainSecond, _float2(1005.f, 620.f), XMVectorSet(0.59f, 0.295f, 0.f, 1.f), 0.f);
	}
	else
	{
		m_pGameInstance->Render_Font(TEXT("Font_Segoe32"), strReaminMinute + TEXT(" : ") + strRemainSecond, _float2(1025.f, 625.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);
	}

	return S_OK;
}

HRESULT CUI_Timer::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timer_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Timer::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fRatioX = 41.5f / (_float)m_vSize.x;
	_float fRatioY = 22.2f / (_float)m_vSize.y;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioX", &fRatioX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioY", &fRatioY, sizeof(_float))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

HRESULT CUI_Timer::Add_Timer_UI()
{
	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Hourglass, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Hourglass"))))
		return E_FAIL;

	CUI_Timer_Banner::UI_TIMER_BANNER_DESC UI_Timer_Banner_Desc = {};
	UI_Timer_Banner_Desc.pRemain_Time = m_pRemain_Time;
	UI_Timer_Banner_Desc.pTotal_Time = m_pTotal_Time;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Timer_Banner, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Timer_Banner"), &UI_Timer_Banner_Desc)))
		return E_FAIL;

	return S_OK;
}

CUI_Timer* CUI_Timer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Timer* pInstance = new CUI_Timer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Timer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Timer::Clone(void* pArg)
{
	CUI_Timer* pInstance = new CUI_Timer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Timer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Timer::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Hourglass);
	Safe_Release(m_pUI_Timer_Banner);
}
