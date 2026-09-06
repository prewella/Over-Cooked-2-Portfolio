#include "stdafx.h"
#include "UI_Tip.h"

CUI_Tip::CUI_Tip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Tip::CUI_Tip(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Tip::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Tip::Initialize(void* pArg)
{
	UI_TIP_DESC* pDesc = (UI_TIP_DESC*)pArg;

	m_pIsTips = pDesc->pIsTips;
	m_pTimes = pDesc->pTimes;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(150.f, 25.f);
	m_vPos = _float2(150.f, 690.f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE_LAST;

	return S_OK;
}

_int CUI_Tip::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (m_pGameInstance->Get_DIKeyState(DIK_T) == EKeyState::DOWN)
	{
		*m_pTimes += 1;
		if (*m_pTimes > 3)
		{
			*m_pTimes = 0;
		}
	}

	return EVENT_NONE;
}

void CUI_Tip::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Tip::Render()
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

	if (*m_pIsTips)
	{
		m_pGameInstance->Render_Font(TEXT("Font_Segoe19"), TEXT("TIP  x ") + to_wstring(*m_pTimes + 1), _float2(98.5f, 671.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);
		m_pGameInstance->Render_Font(TEXT("Font_Segoe18"), TEXT("TIP  x ") + to_wstring(*m_pTimes + 1), _float2(100.f, 673.f), XMVectorSet(0.2f, 0.345f, 0.41f, 1.f), 0.f);
	}

	return S_OK;
}

HRESULT CUI_Tip::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score_Tip_Banner"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Tip::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fRatioX = 224.f / (_float)m_vSize.x;
	_float fRatioY = 28.f / (_float)m_vSize.y;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioX", &fRatioX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioY", &fRatioY, sizeof(_float))))
		return E_FAIL;


	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", *m_pTimes);

	return S_OK;
}

CUI_Tip* CUI_Tip::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Tip* pInstance = new CUI_Tip(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Tip"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Tip::Clone(void* pArg)
{
	CUI_Tip* pInstance = new CUI_Tip(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Tip"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Tip::Free()
{
	__super::Free();
}
