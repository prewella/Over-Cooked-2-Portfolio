#include "stdafx.h"
#include "UI_Stage_Select_Preview.h"

CUI_Stage_Select_Preview::CUI_Stage_Select_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Stage_Select_Preview::CUI_Stage_Select_Preview(const CUI_Stage_Select_Preview& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Stage_Select_Preview::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Stage_Select_Preview::Initialize(void* pArg)
{
	STAGE_SELECT_PREVIEW_DESC* pDesc = (STAGE_SELECT_PREVIEW_DESC*)pArg;
	m_iStageIdx = pDesc->iStage_Idx;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(125.f, 90.f);
	m_vPos = _float2(g_iWinSizeX * 0.5f + (150.f * ((m_iStageIdx * 2.f) - 1.f)) - 2.5f, g_iWinSizeY * 0.52f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_FRONT;

	return S_OK;
}

_int CUI_Stage_Select_Preview::Tick(_float fTimeDelta)
{
	
	return EVENT_NONE;
}

void CUI_Stage_Select_Preview::Late_Tick(_float fTimeDelta)
{
	if (m_IsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_AFTER_UI, this);
}

HRESULT CUI_Stage_Select_Preview::Render()
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

HRESULT CUI_Stage_Select_Preview::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Preview_Levels"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Stage_Select_Preview::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iStageIdx);

	return S_OK;
}

CUI_Stage_Select_Preview* CUI_Stage_Select_Preview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Stage_Select_Preview* pInstance = new CUI_Stage_Select_Preview(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Stage_Select_Preview"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Stage_Select_Preview::Clone(void* pArg)
{
	CUI_Stage_Select_Preview* pInstance = new CUI_Stage_Select_Preview(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Stage_Select_BG"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Stage_Select_Preview::Free()
{
	__super::Free();
}
