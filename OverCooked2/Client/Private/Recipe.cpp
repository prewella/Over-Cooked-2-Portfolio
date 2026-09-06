#include "stdafx.h"
#include "Recipe.h"

CRecipe::CRecipe()
{
}

CRecipe::CRecipe(const CRecipe& rhs)
	:m_strRecipeName{rhs.m_strRecipeName}
	,m_Recipes{rhs.m_Recipes }
{
	/*for (auto& recipes : rhs.m_Recipes)
	{
		m_Recipes.push_back(recipes);
	}*/
}

HRESULT CRecipe::Initialize_Prototype(wstring strRecipeName)
{
	m_strRecipeName = strRecipeName;

	return S_OK;
}

HRESULT CRecipe::Initialize()
{
	return S_OK;
}

HRESULT CRecipe::Add_Recipe_Ingredient(RECIPE_DESC Plated_Ingredient)
{
	if (Plated_Ingredient.eIngredientType == INGREDIENT_END)
		return E_FAIL;

	m_Recipes.push_back(Plated_Ingredient);

	return S_OK;
}

_bool CRecipe::Check_Add_Plated_Ingredient(RECIPE_DESC add_Recipe, list<RECIPE_DESC> Recipes)
{
	// 이미 놓여져 있는 재료가 레시피 재료수와 같으면 x
	if (Recipes.size() == m_Recipes.size())
		return false;

	// 추가하려는 재료가 이미 놓여져있던 재료중에 있다면 x
	for (auto& Plated_Ingredient : Recipes)
	{
		if (Plated_Ingredient.eIngredientType == add_Recipe.eIngredientType)
			return false;
	}

	for (auto& Recipe : m_Recipes)
	{
		if (add_Recipe.eIngredientType == Recipe.eIngredientType
			&& add_Recipe.eMethod == Recipe.eMethod)
			return true;
	}

	return false;
}

_bool CRecipe::Check_Recipe(list<RECIPE_DESC> Recipes)
{
	if (Recipes.size() != m_Recipes.size())
		return false;

	_bool IsSameRecipe = false;
	for (auto& Recipe_Ingredient : m_Recipes)
	{
		for (auto& Plated_Ingredient : Recipes)
		{
			if (Plated_Ingredient.eIngredientType == Recipe_Ingredient.eIngredientType
				&& Plated_Ingredient.eMethod == Recipe_Ingredient.eMethod)
			{
				IsSameRecipe = true;
				break;
			}
			else
			{
				IsSameRecipe = false;
			}
		}
	}

	return IsSameRecipe;
}

CRecipe* CRecipe::Create(wstring strRecipeName)
{
	CRecipe* pInstance = new CRecipe();

	if (FAILED(pInstance->Initialize_Prototype(strRecipeName)))
	{
		MSG_BOX(TEXT("Failed To Created : CRecipe"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CRecipe* CRecipe::Cloned()
{
	CRecipe* pInstance = new CRecipe(*this);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRecipe"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRecipe::Free()
{
}
