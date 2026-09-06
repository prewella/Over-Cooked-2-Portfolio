#include "stdafx.h"
#include "Camera_UI.h"

CCamera_UI::CCamera_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCamera{ pDevice, pContext }
{
}

CCamera_UI::CCamera_UI(const CCamera_UI& rhs)
    : CCamera{ rhs }
{
}

HRESULT CCamera_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_UI::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CCamera_UI::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	__super::Bind_PipeLines();

	return EVENT_NONE;
}

void CCamera_UI::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_UI::Render()
{
	return S_OK;
}

CCamera_UI* CCamera_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_UI* pInstance = new CCamera_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_UI"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_UI::Clone(void* pArg)
{
	CCamera_UI* pInstance = new CCamera_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_UI"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_UI::Free()
{
	__super::Free();
}
