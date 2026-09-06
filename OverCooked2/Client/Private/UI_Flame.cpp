#include "stdafx.h"
#include "UI_Flame.h"

CUI_Flame::CUI_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Flame::CUI_Flame(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Flame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Flame::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vSize = _float2(350.f, 350.f);
	m_vPos = _float2(120.f , 505.f );

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(22.5));

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_MIDDLE_FRONT;

	return S_OK;
}

_int CUI_Flame::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (m_IsRender)
	{
		fAnimSpeed += fTimeDelta;
		if (fAnimSpeed >= 0.025f)
		{
			fAnimSpeed = 0.f;
			m_iAnimIdx++;

			if (m_iAnimIdx >= 59)
			{
				m_iAnimIdx = 0;
			}
		}
	}
	else
	{
		fAnimSpeed = 0.f;
		m_iAnimIdx = 0;
	}

	return EVENT_NONE;
}

void CUI_Flame::Late_Tick(_float fTimeDelta)
{
	if (m_IsRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUI_Flame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Flame::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score_Flames"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Flame::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_uint iNumRow = 8, iNumCol = 8, iAnimIdx = (_uint)m_iAnimIdx;
	if (FAILED(m_pShaderCom->Bind_RawValue("iNumCol", &iNumCol, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("iNumRow", &iNumRow, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("iIdx", &iAnimIdx, sizeof(_uint))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

CUI_Flame* CUI_Flame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Flame* pInstance = new CUI_Flame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Flame"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Flame::Clone(void* pArg)
{
	CUI_Flame* pInstance = new CUI_Flame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Flame"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Flame::Free()
{
	__super::Free();
}
