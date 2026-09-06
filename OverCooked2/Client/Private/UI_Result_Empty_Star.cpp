#include "stdafx.h"
#include "UI_Result_Empty_Star.h"

#include "UI_Common_Star.h"

CUI_Result_Empty_Star::CUI_Result_Empty_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Result_Empty_Star::CUI_Result_Empty_Star(const CUI_Result_Empty_Star& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Result_Empty_Star::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Result_Empty_Star::Initialize(void* pArg)
{
	UI_DESC* pDesc = (UI_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE_FRONT;

	return S_OK;
}

_int CUI_Result_Empty_Star::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CUI_Result_Empty_Star::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Result_Empty_Star::Render()
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

HRESULT CUI_Result_Empty_Star::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Empty_Star"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Result_Empty_Star::Bind_ShaderResources()
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

HRESULT CUI_Result_Empty_Star::Add_Star()
{
	CUI_Common_Star::UI_STAR_DESC UIStarDesc = {};
	UIStarDesc.vPos = m_vPos;
	UIStarDesc.vTargetSize = m_vSize;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_RESULT,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_Star"), &UIStarDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Result_Empty_Star* CUI_Result_Empty_Star::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Result_Empty_Star* pInstance = new CUI_Result_Empty_Star(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Result_Empty_Star"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Result_Empty_Star::Clone(void* pArg)
{
	CUI_Result_Empty_Star* pInstance = new CUI_Result_Empty_Star(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Result_Empty_Star"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Result_Empty_Star::Free()
{
	__super::Free();
}
