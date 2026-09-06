#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Flame final : public CUI
{
private:
	CUI_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Flame(const CUI& rhs);
	virtual ~CUI_Flame() = default;

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
	void Render_UI(_bool IsRender) {
		m_IsRender = IsRender;
	}

private:
	_bool m_IsRender = { false };

	_uint m_iAnimIdx = { 0 };

	_float fAnimSpeed = { 0.f };

public:
	static CUI_Flame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END