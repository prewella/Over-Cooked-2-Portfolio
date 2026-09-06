#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Select_Stage final : public CUI
{
public:
	typedef struct Select_Stage_Desc : public CUI::UI_DESC {
		_uint iStage_Idx = { 0 };

	}SELECT_STAGE_DESC;

private:
	CUI_Select_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Select_Stage(const CUI_Select_Stage& rhs);
	virtual ~CUI_Select_Stage() = default;

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
	_bool IsOpen_Stage() {
		return m_IsOpen_Stage;
	}

	void Set_Render(_bool IsRender)
	{
		m_IsRender = IsRender;
	}



private:
	_bool m_IsRender = { false };

	RECT m_uiRect;

	_bool m_IsIntersect = { false };
	_uint m_iStageIdx = { 0 };

	_bool m_IsOpen_Stage = { false };

public:
	static CUI_Select_Stage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END