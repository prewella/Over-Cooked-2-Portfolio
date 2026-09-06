#include "stdafx.h"
#include "UI_Main_Logo.h"

CUI_Main_Logo::CUI_Main_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Main_Logo::CUI_Main_Logo(const CUI_Main_Logo& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Main_Logo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Main_Logo::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.375f);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.25f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);


	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE_LAST;

	return S_OK;
}

_int CUI_Main_Logo::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CUI_Main_Logo::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Main_Logo::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Main_Logo::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Main_Logo::Bind_ShaderResources()
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

CUI_Main_Logo* CUI_Main_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Main_Logo* pInstance = new CUI_Main_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Main_Logo"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Main_Logo::Clone(void* pArg)
{
	CUI_Main_Logo* pInstance = new CUI_Main_Logo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Main_Logo"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Main_Logo::Free()
{
	__super::Free();
}
