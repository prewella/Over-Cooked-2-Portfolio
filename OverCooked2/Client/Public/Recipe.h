#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CRecipe final : public CBase
{
private:
	CRecipe();
	CRecipe(const CRecipe& rhs);
	virtual ~CRecipe() = default;

public:
	HRESULT Initialize_Prototype(wstring strRecipeName);
	HRESULT Initialize();

public:
	HRESULT Add_Recipe_Ingredient(RECIPE_DESC Plated_Ingredient);
	_bool Check_Add_Plated_Ingredient(RECIPE_DESC Plating_Ingredient, list<RECIPE_DESC> Plated_Ingredients);
	_bool Check_Recipe(list<RECIPE_DESC> Plated_Ingredients);

	wstring Get_RecipeName() {
		return m_strRecipeName;
	}

	list<RECIPE_DESC>* Get_Recipes() {
		return &m_Recipes;
	}

private:
	wstring m_strRecipeName = TEXT("");
	list<RECIPE_DESC> m_Recipes;

public:
	static CRecipe* Create(wstring strRecipeName);
	CRecipe* Cloned();
	virtual void Free() override;
};

END