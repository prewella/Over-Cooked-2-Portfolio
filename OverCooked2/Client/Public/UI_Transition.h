#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Transition final : public CUI
{
public:
	typedef struct tag_Transition_Desc : public UI_DESC {
		_uint iLevelIdx = { 0 };
	}TRANSITION_DESC;
private:
	CUI_Transition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Transition(const CUI_Transition& rhs);
	virtual ~CUI_Transition() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	void Set_EnterLevel();
	void Set_ExitLevel();

	_bool Enter_Finish() {
		return m_IsTransitionEnter_Finish;
	}
	_bool Exit_Finish() {
		return m_IsTransitionExit_Finish;
	}

private:
	_bool m_IsRender = { false };
	_bool m_IsEnterLevel = { false };
	_bool m_IsTransitionEnter_Finish = { false };
	_bool m_IsTransitionExit_Finish = { false };

public:
	static CUI_Transition* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END