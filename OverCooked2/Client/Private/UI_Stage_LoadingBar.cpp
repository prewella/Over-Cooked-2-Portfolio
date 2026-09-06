#include "stdafx.h"
#include "UI_Stage_LoadingBar.h"

CUI_Stage_LoadingBar::CUI_Stage_LoadingBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Stage_LoadingBar::CUI_Stage_LoadingBar(const CUI_Stage_LoadingBar& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Stage_LoadingBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Stage_LoadingBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(1000.f, 50.f);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.9f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE_LOAD,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_LoadingBar_Gauge"))))
		return E_FAIL;

	return S_OK;
}

_int CUI_Stage_LoadingBar::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CUI_Stage_LoadingBar::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Stage_LoadingBar::Render()
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

HRESULT CUI_Stage_LoadingBar::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bar_BG"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Stage_LoadingBar::Bind_ShaderResources()
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


CUI_Stage_LoadingBar* CUI_Stage_LoadingBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Stage_LoadingBar* pInstance = new CUI_Stage_LoadingBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Stage_LoadingBar"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Stage_LoadingBar::Clone(void* pArg)
{
	CUI_Stage_LoadingBar* pInstance = new CUI_Stage_LoadingBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Stage_LoadingBar"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Stage_LoadingBar::Free()
{
	__super::Free();
}
