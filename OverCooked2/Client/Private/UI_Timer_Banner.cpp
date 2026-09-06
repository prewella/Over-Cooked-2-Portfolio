#include "stdafx.h"
#include "UI_Timer_Banner.h"

CUI_Timer_Banner::CUI_Timer_Banner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUI{ pDevice, pContext }
{
}

CUI_Timer_Banner::CUI_Timer_Banner(const CUI& rhs)
    :CUI{ rhs }
{
}

HRESULT CUI_Timer_Banner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Timer_Banner::Initialize(void* pArg)
{
	UI_TIMER_BANNER_DESC* pDesc = (UI_TIMER_BANNER_DESC*)pArg;
	m_pRemain_Time = pDesc->pRemain_Time;
	m_pTotal_Time = pDesc->pTotal_Time;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_vSize = _float2(200.f, 23.f);
	m_vPos = _float2(1090.f, 687.5f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE;

	return S_OK;
}

_int CUI_Timer_Banner::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CUI_Timer_Banner::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Timer_Banner::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Timer_Banner::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timer_Banner"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Timer_Banner::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fPercent = *m_pRemain_Time / *m_pTotal_Time;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fPercent", &fPercent, sizeof(_float))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

CUI_Timer_Banner* CUI_Timer_Banner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Timer_Banner* pInstance = new CUI_Timer_Banner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Timer_Banner"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Timer_Banner::Clone(void* pArg)
{
	CUI_Timer_Banner* pInstance = new CUI_Timer_Banner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Timer_Banner"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Timer_Banner::Free()
{
	__super::Free();
}
