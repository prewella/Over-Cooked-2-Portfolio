#include "stdafx.h"
#include "UI_Stage_Select_Manager.h"

#include "UI_Stage_Select_BG.h"
#include "UI_Stage_Select_Title.h"
#include "UI_Select_Stage.h"
#include "UI_Stage_Select_Preview.h"

CUI_Stage_Select_Manager::CUI_Stage_Select_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Stage_Select_Manager::CUI_Stage_Select_Manager(const CUI_Stage_Select_Manager& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Stage_Select_Manager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Stage_Select_Manager::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	READY_Stage_Select_UI();

	m_vSize = _float2(g_iWinSizeX, g_iWinSizeY);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_LAST;

	return S_OK;
}

_int CUI_Stage_Select_Manager::Tick(_float fTimeDelta)
{
	for (auto& pUI_Stage_Select : m_pUI_Stage_Selects)
	{
		if (pUI_Stage_Select->IsOpen_Stage())
		{
			m_IsSelected_Stage = true;
		}
	}
	return EVENT_NONE;
}

void CUI_Stage_Select_Manager::Late_Tick(_float fTimeDelta)
{
	if (m_IsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_AFTER_UI, this);
}

HRESULT CUI_Stage_Select_Manager::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Stage_Select_Manager::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Common_BG"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Stage_Select_Manager::Bind_ShaderResources()
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

HRESULT CUI_Stage_Select_Manager::READY_Stage_Select_UI()
{
	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Select_BG, LEVEL_MAIN_MENU,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_Select_BG"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Select_Title, LEVEL_MAIN_MENU,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_Select_Title"))))
		return E_FAIL;

	for (_uint i = 0; i < 2; i++)
	{
		CUI_Select_Stage* pUI_SelectStage = nullptr;

		CUI_Select_Stage::SELECT_STAGE_DESC pStageDesc = {};
		pStageDesc.iStage_Idx = i;

		if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pUI_SelectStage, LEVEL_MAIN_MENU,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Select_Stage"), &pStageDesc)))
			return E_FAIL;

		m_pUI_Stage_Selects.push_back(pUI_SelectStage);


		CUI_Stage_Select_Preview* pUI_Stage_Preview = nullptr;
		CUI_Stage_Select_Preview::STAGE_SELECT_PREVIEW_DESC pPreviewDesc = {};
		pPreviewDesc.iStage_Idx = i;

		if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pUI_Stage_Preview, LEVEL_MAIN_MENU,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_Select_Preview"), &pPreviewDesc)))
			return E_FAIL;

		m_pUI_Stage_Previews.push_back(pUI_Stage_Preview);
	}

	return S_OK;
}

void CUI_Stage_Select_Manager::Open_UI()
{
	m_IsRender = true;
	m_pUI_Select_BG->Set_Render(true);
	m_pUI_Select_Title->Set_Render(true);

	for (auto& pUI_Stage_Select : m_pUI_Stage_Selects)
		pUI_Stage_Select->Set_Render(true);

	for (auto& pUI_Stage_Preview : m_pUI_Stage_Previews)
		pUI_Stage_Preview->Set_Render(true);
}

void CUI_Stage_Select_Manager::Close_UI()
{
	m_IsRender = false;
	m_IsRender = false;
	m_pUI_Select_BG->Set_Render(false);
	m_pUI_Select_Title->Set_Render(false);

	for (auto& pUI_Stage_Select : m_pUI_Stage_Selects)
		pUI_Stage_Select->Set_Render(false);

	for (auto& pUI_Stage_Preview : m_pUI_Stage_Previews)
		pUI_Stage_Preview->Set_Render(false);
}

CUI_Stage_Select_Manager* CUI_Stage_Select_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Stage_Select_Manager* pInstance = new CUI_Stage_Select_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Stage_Select_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Stage_Select_Manager::Clone(void* pArg)
{
	CUI_Stage_Select_Manager* pInstance = new CUI_Stage_Select_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Stage_Select_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Stage_Select_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Select_BG);
	Safe_Release(m_pUI_Select_Title);

	for (auto& pUI_Stage_Select : m_pUI_Stage_Selects)
		Safe_Release(pUI_Stage_Select);
	m_pUI_Stage_Selects.clear();

	for (auto& pUI_Stage_Preview : m_pUI_Stage_Previews)
		Safe_Release(pUI_Stage_Preview);
	m_pUI_Stage_Previews.clear();

}
