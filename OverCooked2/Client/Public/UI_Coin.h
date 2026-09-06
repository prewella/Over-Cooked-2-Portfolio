#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Coin final : public CUI
{
public:
	typedef struct tagUI_Coin_Desc : public CUI::UI_DESC
	{
	}UI_COIN_DESC;

private:
	CUI_Coin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Coin(const CUI& rhs);
	virtual ~CUI_Coin() = default;

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
	void Anim_Start() {
		m_IsAnim = true;
	}

private:
	_bool m_IsAnim = { false };

	_uint m_iAnimIdx = { 0 };

	_float fAnimSpeed = { 0.f };

public:
	static CUI_Coin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END