#include "stdafx.h"
#include "UI_Select_Stage.h"

CUI_Select_Stage::CUI_Select_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Select_Stage::CUI_Select_Stage(const CUI_Select_Stage& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Select_Stage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Select_Stage::Initialize(void* pArg)
{
	SELECT_STAGE_DESC* pDesc = (SELECT_STAGE_DESC*)pArg;
	m_iStageIdx = pDesc->iStage_Idx;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(175.f, 175.f);

	m_vPos = _float2(g_iWinSizeX * 0.5f + (150.f * ((m_iStageIdx * 2.f)-1.f)), g_iWinSizeY * 0.5f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE_FRONT;

	return S_OK;
}

_int CUI_Select_Stage::Tick(_float fTimeDelta)
{
	m_uiRect = { (_long)(m_vPos.x - (m_vSize.x * 0.5f)),(_long)(m_vPos.y - (m_vSize.y * 0.5f)),
		(_long)(m_vPos.x + (m_vSize.x * 0.5f)), (_long)(m_vPos.y + (m_vSize.y * 0.5f)) };  // 좌상단과 우하단 좌표로 사각형 정의

	POINT vCursor;
	GetCursorPos(&vCursor);
	ScreenToClient(g_hWnd, &vCursor);

	if (m_IsRender)
	{
		if (PtInRect(&m_uiRect, vCursor)) {
			if (!m_IsIntersect)
			{
				m_pGameInstance->StopSound(CSound_Manager::CH_UI_HIGHLIGHT);
				m_pGameInstance->Play_Sound(L"UI_Highlight.wav", CSound_Manager::CH_UI_HIGHLIGHT, 1.f);
			}
			m_IsIntersect = true;

			if (m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON) == EKeyState::DOWN)
			{
				g_iStageIdx = m_iStageIdx;
				m_IsOpen_Stage = true;
				m_pGameInstance->StopSound(CSound_Manager::CH_UI_START);
				m_pGameInstance->Play_Sound(L"UI_PressStart.wav", CSound_Manager::CH_UI_START, 0.5f);
			}
		}
		else
		{
			m_IsIntersect = false;
		}
	}
	
	return EVENT_NONE;
}

void CUI_Select_Stage::Late_Tick(_float fTimeDelta)
{
	if (m_IsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_AFTER_UI, this);
}

HRESULT CUI_Select_Stage::Render()
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

	m_pGameInstance->Render_Font(TEXT("Font_Segoe18"),
		TEXT("STAGE ") + to_wstring(m_iStageIdx + 1), _float2(m_vPos.x - (m_vSize.x * 0.325f), m_vPos.y - (m_vSize.y * 0.4f)), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);


	return S_OK;
}

HRESULT CUI_Select_Stage::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage_Select_Boards"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Select_Stage::Bind_ShaderResources()
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

CUI_Select_Stage* CUI_Select_Stage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Select_Stage* pInstance = new CUI_Select_Stage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Select_Stage"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Select_Stage::Clone(void* pArg)
{
	CUI_Select_Stage* pInstance = new CUI_Select_Stage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Select_Stage"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Select_Stage::Free()
{
	__super::Free();
}
