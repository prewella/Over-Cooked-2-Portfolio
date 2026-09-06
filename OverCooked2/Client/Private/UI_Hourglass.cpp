#include "stdafx.h"
#include "UI_Hourglass.h"

CUI_Hourglass::CUI_Hourglass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Hourglass::CUI_Hourglass(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Hourglass::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Hourglass::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(95.f * 0.8f, 112.f * 0.8f);
	m_vPos = _float2(1205.f, 660.f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_FRONT;

	return S_OK;
}

_int CUI_Hourglass::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;


	if (m_pGameInstance->Get_DIKeyState(DIK_R) == EKeyState::DOWN)
	{
		m_IsShake = !m_IsShake;
	}

	if (m_IsShake)
	{
		if (m_IsShakeLeft)
		{
			m_fRotationDegree += 6.f;
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fRotationDegree));
		}
		else
		{
			m_fRotationDegree -= 6.f;
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(m_fRotationDegree));
		}

		m_iRotationCnt++;

		if (m_iRotationCnt >= 3)
		{
			m_IsShakeLeft = !m_IsShakeLeft;
			m_iRotationCnt = 0;
		}
	}


	return EVENT_NONE;
}

void CUI_Hourglass::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Hourglass::Render()
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

HRESULT CUI_Hourglass::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timer_Hourglass"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Hourglass::Bind_ShaderResources()
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

CUI_Hourglass* CUI_Hourglass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Hourglass* pInstance = new CUI_Hourglass(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Hourglass"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Hourglass::Clone(void* pArg)
{
	CUI_Hourglass* pInstance = new CUI_Hourglass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Hourglass"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Hourglass::Free()
{
	__super::Free();
}
