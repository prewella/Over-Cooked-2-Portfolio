#include "stdafx.h"
#include "..\Public\Camera_Free.h"

CCamera_Free::CCamera_Free(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera{ pDevice, pContext }
{

}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera{ rhs }
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	
	CAMERA_FREE_DESC*	pCameraFree = (CAMERA_FREE_DESC*)pArg;

	m_fMouseSensor = pCameraFree->fMouseSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CCamera_Free::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

#ifdef _DEBUG
	if (m_pGameInstance->Get_DIKeyState(DIK_A) == EKeyState::PRESSING)
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_D) == EKeyState::PRESSING)
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_W) == EKeyState::PRESSING)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}
	if (m_pGameInstance->Get_DIKeyState(DIK_S) == EKeyState::PRESSING)
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIMKS_WHEEL) == EKeyState::PRESSING)
	{
		_long	MouseMove = { 0 };

		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * m_fMouseSensor);
		}
		if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * m_fMouseSensor);
		}
	}
#endif // DEBUG

	
	__super::Bind_PipeLines();

	return EVENT_NONE;
}

void CCamera_Free::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

CCamera_Free * CCamera_Free::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Free*		pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CCamera_Free::Clone(void * pArg)
{
	CCamera_Free*		pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Free"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();

}
