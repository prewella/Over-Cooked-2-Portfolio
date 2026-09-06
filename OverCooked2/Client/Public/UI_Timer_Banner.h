#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Timer_Banner final : public CUI
{
public:
	typedef struct tagUI_Timer_Banner_Desc : public CUI::UI_DESC
	{
		_float* pRemain_Time = { nullptr };
		_float* pTotal_Time = { nullptr };
	}UI_TIMER_BANNER_DESC;

private:
	CUI_Timer_Banner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Timer_Banner(const CUI& rhs);
	virtual ~CUI_Timer_Banner() = default;

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
	_float* m_pRemain_Time = { nullptr };
	_float* m_pTotal_Time = { nullptr };

public:
	static CUI_Timer_Banner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END