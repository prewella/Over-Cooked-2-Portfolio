#include "stdafx.h"
#include "UI_Notice_Font.h"

CUI_Notice_Font::CUI_Notice_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice,pContext }
{
}

CUI_Notice_Font::CUI_Notice_Font(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Notice_Font::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Notice_Font::Initialize(void* pArg)
{
	NOTICE_FONT_DESC* pDesc = (NOTICE_FONT_DESC*)pArg;
	m_pNoticeIdx = pDesc->pNoticeIdx;
	m_pIsRender = pDesc->pIsRender;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(15.f, 5.f);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.4f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_FRONT;

	return S_OK;
}

_int CUI_Notice_Font::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (*m_pIsRender)
	{
		if (m_vSize.x < 225.f)
		{
			m_vSize = _float2(m_vSize.x + 15.f, m_vSize.y + 5.f);
			m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
		}
	}

	return EVENT_NONE;
}

void CUI_Notice_Font::Late_Tick(_float fTimeDelta)
{
	if (*m_pIsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Notice_Font::Render()
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

HRESULT CUI_Notice_Font::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Notice_Fonts"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Notice_Font::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", *m_pNoticeIdx);

	return S_OK;
}

void CUI_Notice_Font::Change_Notice()
{
	m_vSize = _float2(15.f, 5.f);
	m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
}

CUI_Notice_Font* CUI_Notice_Font::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Notice_Font* pInstance = new CUI_Notice_Font(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Notice_Font"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Notice_Font::Clone(void* pArg)
{
	CUI_Notice_Font* pInstance = new CUI_Notice_Font(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Notice_Font"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Notice_Font::Free()
{
	__super::Free();
}
