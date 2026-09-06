#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Chef_Change_Header final : public CUI
{
private:
	CUI_Chef_Change_Header(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Chef_Change_Header(const CUI_Chef_Change_Header& rhs);
	virtual ~CUI_Chef_Change_Header() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void Focus_Chef_Camera();

private:
	RECT m_uiRect;

	_bool m_IsIntersect = { false };
	_bool m_IsFocus_Chef = { false };

public:
	static CUI_Chef_Change_Header* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END