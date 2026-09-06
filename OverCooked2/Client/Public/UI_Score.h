#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Score final : public CUI
{
public:
	typedef struct tagUI_Score_Desc : public CUI::UI_DESC
	{
		_uint* pScore = { nullptr };
		_bool* pIsAddScore = { nullptr };
		_bool* pIsTips = { nullptr };
		_uint* pTipTimes = { nullptr };
	}UI_SCORE_DESC;

private:
	CUI_Score(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Score(const CUI& rhs);
	virtual ~CUI_Score() = default;

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
	void Coin_Spinning();

private:
	HRESULT Add_Score_UI();

private:
	_uint* m_pScore = {nullptr};
	_bool* m_pIsAddScore = { nullptr };
	_bool* m_pIsTips = { nullptr };
	_uint* m_pTipTimes = { nullptr };

	class CUI_Coin*		m_pUI_Coin = { nullptr };
	class CUI_Flame*	m_pUI_Flame = { nullptr };
	class CUI_Tip*		m_pUI_Tip = { nullptr };

public:
	static CUI_Score* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END