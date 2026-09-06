#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Stage_Select_Header final : public CUI
{
private:
	CUI_Stage_Select_Header(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Stage_Select_Header(const CUI_Stage_Select_Header& rhs);
	virtual ~CUI_Stage_Select_Header() = default;

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
	void Close_Stage_Select() {
		m_IsOpened_StageSelect = false;
	}

	_bool Is_Stage_Select_Open() {
		return m_IsOpened_StageSelect;
	}

private:
	RECT m_uiRect;

	_bool m_IsIntersect = { false };
	_bool m_IsOpened_StageSelect = { false };

public:
	static CUI_Stage_Select_Header* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END