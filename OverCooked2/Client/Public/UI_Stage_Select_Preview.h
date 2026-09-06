#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Stage_Select_Preview final : public CUI
{
public:
	typedef struct Stage_Select_Preview_Desc : public CUI::UI_DESC {
		_uint iStage_Idx = { 0 };

	}STAGE_SELECT_PREVIEW_DESC;

private:
	CUI_Stage_Select_Preview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Stage_Select_Preview(const CUI_Stage_Select_Preview& rhs);
	virtual ~CUI_Stage_Select_Preview() = default;

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
	void Set_Render(_bool IsRender)
	{
		m_IsRender = IsRender;
	}

private:
	_bool m_IsRender = { false };
	_uint m_iStageIdx = { 0 };

public:
	static CUI_Stage_Select_Preview* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END