#include "stdafx.h"
#include "UI_Notice.h"

#include "UI_Notice_Font.h"

CUI_Notice::CUI_Notice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice,pContext }
{
}

CUI_Notice::CUI_Notice(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Notice::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Notice::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(30.f, 10.f);
	m_vPos = _float2(g_iWinSizeX * 0.5f, g_iWinSizeY * 0.4f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	Add_Font_UI();

	m_pGameInstance->StopSound(CSound_Manager::CH_STAGE_TRANSITION);
	m_pGameInstance->Play_Sound(L"Stage_Ready.wav", CSound_Manager::CH_STAGE_TRANSITION, 1.f);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_LAST;

	return S_OK;
}

_int CUI_Notice::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (m_IsRender)
	{
		m_fRenderTimer += fTimeDelta;
		if (m_vSize.x < 450.f)
		{
			m_vSize = _float2(m_vSize.x + 30.f, m_vSize.y + 10.f);
			m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
		}
		if (m_fRenderTimer >= NoticeRenderTime)
		{
			m_fRenderTimer = 0.f;
			m_IsRender = false;
			m_IsNoticed = true;

			if (m_iNoticeIdx == 0)
			{
				Notice_Render(1);
				m_pGameInstance->StopSound(CSound_Manager::CH_STAGE_TRANSITION);
				m_pGameInstance->Play_Sound(L"Stage_Start.wav", CSound_Manager::CH_STAGE_TRANSITION, 1.f);
			}
		}
	}

	return EVENT_NONE;
}

void CUI_Notice::Late_Tick(_float fTimeDelta)
{
	if (m_IsRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Notice::Render()
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

HRESULT CUI_Notice::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Notices"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Notice::Add_Font_UI()
{
	CUI_Notice_Font::NOTICE_FONT_DESC pDesc = {};
	pDesc.pNoticeIdx = &m_iNoticeIdx;
	pDesc.pIsRender = &m_IsRender;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Notice_Font, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Notice_Font"), &pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Notice::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iNoticeIdx);

	return S_OK;
}

void CUI_Notice::Notice_Render(_uint iNoticeIdx)
{
	m_iNoticeIdx = iNoticeIdx;
	m_fRenderTimer = 0.f;
	m_IsRender = true;
	m_IsNoticed = false;
	m_vSize = _float2(30.f, 10.f);
	m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);

	m_pUI_Notice_Font->Change_Notice();
}

CUI_Notice* CUI_Notice::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Notice* pInstance = new CUI_Notice(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Notice"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Notice::Clone(void* pArg)
{
	CUI_Notice* pInstance = new CUI_Notice(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Notice"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Notice::Free()
{
	Safe_Release(m_pUI_Notice_Font);

	__super::Free();
}
