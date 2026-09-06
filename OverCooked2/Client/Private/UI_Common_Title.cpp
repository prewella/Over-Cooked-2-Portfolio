#include "stdafx.h"
#include "UI_Common_Title.h"

CUI_Common_Title::CUI_Common_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Common_Title::CUI_Common_Title(const CUI_Common_Title& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Common_Title::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Common_Title::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(800.f, 100.f);
	m_vPos = _float2(m_vSize.x * 0.45f, g_iWinSizeY * 0.1f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);


	m_iRenderOrder = (_uint)UI_ORDER::ORDER_FRONT;

	return S_OK;
}

_int CUI_Common_Title::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CUI_Common_Title::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Common_Title::Render()
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

HRESULT CUI_Common_Title::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Common_Title"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Common_Title::Bind_ShaderResources()
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

CUI_Common_Title* CUI_Common_Title::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Common_Title* pInstance = new CUI_Common_Title(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Common_Title"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Common_Title::Clone(void* pArg)
{
	CUI_Common_Title* pInstance = new CUI_Common_Title(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Common_Title"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Common_Title::Free()
{
	__super::Free();
}
