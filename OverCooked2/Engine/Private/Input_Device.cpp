#include "Input_Device.h"

CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Tick()
{
	/* 키보드와 마우스가 어떤 입력 상태를 가지고 있는지를 저장 */
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	/* KeyBoard KeyStateUpdate*/
	for (int iKey = 0; iKey < 256; ++iKey)
	{
		if (m_byKeyState[iKey] & 0x80)
		{
			if (m_bPrePushKeyBoard[iKey])
			{
				m_eKeyState[iKey] = PRESSING;
			}
			else
			{
				m_eKeyState[iKey] = DOWN;
			}
			m_bPrePushKeyBoard[iKey] = true;
		}
		else
		{
			if (m_bPrePushKeyBoard[iKey])
			{
				m_eKeyState[iKey] = UP;
			}
			else
			{
				m_eKeyState[iKey] = NONE;
			}
			m_bPrePushKeyBoard[iKey] = false;
		}
	}

	/* KeyBoard KeyStateUpdate*/
	for (int iKey = 0; iKey < MOUSEKEYSTATE::DIMKS_END; ++iKey)
	{
		if (m_tMouseState.rgbButtons[iKey] & 0x80)
		{
			if (m_bPrePushMouse[iKey])
			{
				m_eMouseState[iKey] = PRESSING;
			}
			else
			{
				m_eMouseState[iKey] = DOWN;
			}
			m_bPrePushMouse[iKey] = true;
		}
		else
		{
			if (m_bPrePushMouse[iKey])
			{
				m_eMouseState[iKey] = UP;
			}
			else
			{
				m_eMouseState[iKey] = NONE;
			}
			m_bPrePushMouse[iKey] = false;
		}
	}
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Create : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
