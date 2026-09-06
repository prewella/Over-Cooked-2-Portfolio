#include "UI.h"
#include "Texture.h"
#include "VIBuffer.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{ pDevice, pContext }
{
}

CUI::CUI(const CUI& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	UI_DESC* pDesc = (UI_DESC*)pArg;

	if (nullptr != pDesc)
	{
		m_vPos = pDesc->vPos;
		m_vSize = pDesc->vSize;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CUI::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CUI::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::Initialize_UI_Setting(_uint iWinSizeX, _uint iWinSizeY)
{
	m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(
		m_vPos.x - iWinSizeX * 0.5f,
		-m_vPos.y + iWinSizeY * 0.5f,
		0.f,
		1.f
	));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)iWinSizeX, (_float)iWinSizeY, -10.f, 1.f));
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
