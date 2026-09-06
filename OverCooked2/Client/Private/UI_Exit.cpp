#include "stdafx.h"
#include "UI_Exit.h"

CUI_Exit::CUI_Exit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Exit::CUI_Exit(const CUI_Exit& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Exit::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Exit::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(300.f, 100.f);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.75f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_uiRect = { (_long)(m_vPos.x - (m_vSize.x * 0.5f)),(_long)(m_vPos.y - (m_vSize.y * 0.5f)),
		(_long)(m_vPos.x + (m_vSize.x * 0.5f)), (_long)(m_vPos.y + (m_vSize.y * 0.5f)) };  // 좌상단과 우하단 좌표로 사각형 정의

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_FRONT;

	return S_OK;
}

_int CUI_Exit::Tick(_float fTimeDelta)
{
	POINT vCursor;
	GetCursorPos(&vCursor);
	ScreenToClient(g_hWnd, &vCursor);

	if (PtInRect(&m_uiRect, vCursor)) {
		m_IsIntersect = true;
		if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON) == EKeyState::DOWN)
		{

		}
	}
	else
	{
		m_IsIntersect = false;
	}

	return EVENT_NONE;
}

void CUI_Exit::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Exit::Render()
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
		TEXT("Exit"), _float2(m_vPos.x - (m_vSize.x * 0.15f), m_vPos.y - (m_vSize.y * 0.375f)), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);

	return S_OK;
}

HRESULT CUI_Exit::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Exit_Headers"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Exit::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_IsIntersect);

	return S_OK;
}

CUI_Exit* CUI_Exit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Exit* pInstance = new CUI_Exit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Exit"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Exit::Clone(void* pArg)
{
	CUI_Exit* pInstance = new CUI_Exit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Exit"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Exit::Free()
{
	__super::Free();
}
