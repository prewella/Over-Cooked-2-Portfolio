#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Timer final : public CUI
{
public:
	typedef struct tagUI_Timer_Desc : public CUI::UI_DESC
	{
		_float* pRemain_Time = { nullptr };
		_float* pTotal_Time = { nullptr };
	}UI_TIMER_DESC;

private:
	CUI_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Timer(const CUI& rhs);
	virtual ~CUI_Timer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	HRESULT Add_Timer_UI();

private:
	_float* m_pRemain_Time = { nullptr };
	_float* m_pTotal_Time = { nullptr };

	_uint m_fPreReaminTime = { 0 };
	_uint m_fNowReaminTime = { 0 };
	_bool m_IsTimeChange = { false };

	_float m_TimerAlarm = { 0.f };

	class CUI_Hourglass* m_pUI_Hourglass = { nullptr };
	class CUI_Timer_Banner* m_pUI_Timer_Banner = { nullptr };

public:
	static CUI_Timer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END