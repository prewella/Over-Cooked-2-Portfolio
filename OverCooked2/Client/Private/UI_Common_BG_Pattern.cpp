#include "stdafx.h"
#include "UI_Common_BG_Pattern.h"

CUI_Common_BG_Pattern::CUI_Common_BG_Pattern(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Common_BG_Pattern::CUI_Common_BG_Pattern(const CUI_Common_BG_Pattern& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Common_BG_Pattern::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Common_BG_Pattern::Initialize(void* pArg)
{
	UI_COMMON_BG_PATTERN_DESC* pDesc = (UI_COMMON_BG_PATTERN_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pDesc->strPatternName)))
		return E_FAIL;

	m_vSize = _float2(g_iWinSizeX, g_iWinSizeY);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE_LAST;

	return S_OK;
}

_int CUI_Common_BG_Pattern::Tick(_float fTimeDelta)
{
	m_fAccFalling += fTimeDelta * fTimeDelta * 2.5f;

	return EVENT_NONE;
}

void CUI_Common_BG_Pattern::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Common_BG_Pattern::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Common_BG_Pattern::Add_Components(wstring strPatternName)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_") + strPatternName,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Common_BG_Pattern::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccFalling", &m_fAccFalling, sizeof(_float))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

CUI_Common_BG_Pattern* CUI_Common_BG_Pattern::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Common_BG_Pattern* pInstance = new CUI_Common_BG_Pattern(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Common_BG_Pattern"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Common_BG_Pattern::Clone(void* pArg)
{
	CUI_Common_BG_Pattern* pInstance = new CUI_Common_BG_Pattern(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Common_BG_Pattern"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Common_BG_Pattern::Free()
{
	__super::Free();
}
