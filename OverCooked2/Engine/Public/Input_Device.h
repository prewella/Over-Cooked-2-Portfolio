#pragma once

#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

/* 키보드와 마우스를 초기화하고 현재 입력 장치의 상태를 받아옴 */

BEGIN(Engine)

class CInput_Device final : public CBase
{
public:
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	EKeyState Get_DIKeyState(_ubyte byKeyID) { return m_eKeyState[byKeyID]; }

	EKeyState Get_DIMouseState(MOUSEKEYSTATE eMouse) { return m_eMouseState[eMouse]; }

	_long Get_DIMouseMove(MOUSEMOVESTATE eMouseMoveState) { return *(((_long*)&m_tMouseState) + eMouseMoveState); }

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Tick();

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:
	_byte					m_byKeyState[256];	// 키보드에 있는 모든 키 값을 저장하기 위한 변수
	EKeyState				m_eKeyState[256];	// 해당 키의 상태 값 저장
	_bool					m_bPrePushKeyBoard[256];

	DIMOUSESTATE			m_tMouseState;
	EKeyState				m_eMouseState[MOUSEKEYSTATE::DIMKS_END];
	_bool					m_bPrePushMouse[MOUSEKEYSTATE::DIMKS_END];

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void Free() override;
};

END

#endif // InputDev_h__