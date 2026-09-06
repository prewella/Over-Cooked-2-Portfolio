#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Recipe final : public CUI
{
public:
	typedef struct tagUI_Recipe_Desc : public CUI::UI_DESC
	{
		RECIPE_DESC recipe_Desc = {};
		_float2 vStartPos = { 0.f,0.f };

	}UI_RECIPE_DESC;

private:
	CUI_Recipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Recipe(const CUI& rhs);
	virtual ~CUI_Recipe() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_UI_Recipes_Ingredient(INGREDIENT_TYPE eIngredient_type);
	HRESULT Add_UI_Recipes_Method(COOKING_METHOD_TYPE eMethod);
	HRESULT Bind_ShaderResources();
	
public:
	void RePosition_UI_Recipe();
	void Delete_UI_Recipe();

private:
	class CUI_Recipe_Ingredient* m_pUI_Recipe_Ingredient = { nullptr };
	class CUI_Recipe_Method* m_pUI_Recipe_Method = { nullptr };

	_float2 m_vStartPos = {};

public:
	static CUI_Recipe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END