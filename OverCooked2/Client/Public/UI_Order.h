#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Order final : public CUI
{
public:
	typedef struct tagUI_Order_Desc : public CUI::UI_DESC
	{
		class CRecipe* pRecipe = { nullptr };
		_float fStartPosX = { 0.f };
	}UI_ORDER_DESC;

private:
#define RecipeWhiteSpace 5.f

private:
	CUI_Order(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Order(const CUI& rhs);
	virtual ~CUI_Order() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_UI_Recipes(class CRecipe* pRecipe);
	HRESULT Add_UI_Dish(class CRecipe* pRecipe);
	HRESULT Bind_ShaderResources();

public:
	_bool IsTimeOver() {
		return m_IsTimeOver;
	}
	void RePosition_UI_Recipes();
	void Delete_UI_Order();

private:
	_bool m_IsTimeOver = { false };
	//_float m_fTImer = { 0.f };

	class CUI_Dish*			m_pUI_Dish = { nullptr };
	list<class CUI_Recipe*> m_UI_Recipes;

public:
	static CUI_Order* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END