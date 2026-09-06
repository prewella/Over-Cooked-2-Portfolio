#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Stage_Select_Manager final : public CUI
{
private:
	CUI_Stage_Select_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Stage_Select_Manager(const CUI_Stage_Select_Manager& rhs);
	virtual ~CUI_Stage_Select_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	HRESULT READY_Stage_Select_UI();

public:
	_bool IsSelected_Stage() {
		return m_IsSelected_Stage;
	}

	void Open_UI();
	void Close_UI();

private:
	class CUI_Stage_Select_BG* m_pUI_Select_BG = { nullptr };
	class CUI_Stage_Select_Title* m_pUI_Select_Title = { nullptr };
	vector<class CUI_Select_Stage*> m_pUI_Stage_Selects;
	vector<class CUI_Stage_Select_Preview*> m_pUI_Stage_Previews;

	_bool m_IsRender = { false };
	_bool m_IsSelected_Stage = { false };
	
public:
	static CUI_Stage_Select_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END