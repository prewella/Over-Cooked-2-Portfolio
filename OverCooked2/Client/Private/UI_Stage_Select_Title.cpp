#include "stdafx.h"
#include "UI_Stage_Select_Title.h"

CUI_Stage_Select_Title::CUI_Stage_Select_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Stage_Select_Title::CUI_Stage_Select_Title(const CUI_Stage_Select_Title& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Stage_Select_Title::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Stage_Select_Title::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(g_iWinSizeX * 0.5, g_iWinSizeY * 0.15f);
	m_vPos = _float2(g_iWinSizeX * 0.225f, g_iWinSizeY * 0.325f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE;

	return S_OK;
}

_int CUI_Stage_Select_Title::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CUI_Stage_Select_Title::Late_Tick(_float fTimeDelta)
{
	if (m_IsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_AFTER_UI, this);
}

HRESULT CUI_Stage_Select_Title::Render()
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

	m_pGameInstance->Render_Font(TEXT("Font_Segoe32"),
		TEXT("SELECT STAGE"), _float2(m_vPos.x - (m_vSize.x * 0.125f), m_vPos.y - (m_vSize.y * 0.375f)), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);


	return S_OK;
}

HRESULT CUI_Stage_Select_Title::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage_Select_Title"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Stage_Select_Title::Bind_ShaderResources()
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

CUI_Stage_Select_Title* CUI_Stage_Select_Title::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Stage_Select_Title* pInstance = new CUI_Stage_Select_Title(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Stage_Select_Title"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Stage_Select_Title::Clone(void* pArg)
{
	CUI_Stage_Select_Title* pInstance = new CUI_Stage_Select_Title(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Stage_Select_Title"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Stage_Select_Title::Free()
{
	__super::Free();
}
