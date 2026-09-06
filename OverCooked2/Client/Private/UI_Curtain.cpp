#include "stdafx.h"
#include "UI_Curtain.h"

CUI_Curtain::CUI_Curtain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Curtain::CUI_Curtain(const CUI_Curtain& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Curtain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Curtain::Initialize(void* pArg)
{
	CURTAIN_DESC* pDesc = (CURTAIN_DESC*)pArg;
	m_pBlindSize = pDesc->pBlindSize;
	m_pIsRender = pDesc->pIsRender;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(g_iWinSizeX, g_iWinSizeY);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_PRIORITY;

	return S_OK;
}

_int CUI_Curtain::Tick(_float fTimeDelta)
{

	return EVENT_NONE;
}

void CUI_Curtain::Late_Tick(_float fTimeDelta)
{
	if (*m_pIsRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_AFTER_UI, this);
	}
}

HRESULT CUI_Curtain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Curtain::Add_Components()
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

HRESULT CUI_Curtain::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float2 vBlindArea = _float2(1 - (m_pBlindSize->x / g_iWinSizeX),1- (m_pBlindSize->y / g_iWinSizeY));

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlindArea", &vBlindArea, sizeof(_float2))))
			return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

CUI_Curtain* CUI_Curtain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Curtain* pInstance = new CUI_Curtain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Curtain"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Curtain::Clone(void* pArg)
{
	CUI_Curtain* pInstance = new CUI_Curtain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Curtain"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Curtain::Free()
{
	__super::Free();
}
