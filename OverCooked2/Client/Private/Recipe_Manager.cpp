#include "stdafx.h"
#include "Recipe_Manager.h"

#include "Recipe.h"

CRecipe_Manager::CRecipe_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent{ pDevice , pContext }
{
}

CRecipe_Manager::CRecipe_Manager(const CRecipe_Manager& rhs)
	:CComponent{ rhs }
	,m_pRecipes{rhs.m_pRecipes}
{
}

HRESULT CRecipe_Manager::Initialize_Prototype()
{
	m_pRecipes = new map<const wstring, class CRecipe*>;

	return S_OK;
}

HRESULT CRecipe_Manager::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CRecipe_Manager::Setting_Level_Recipe(_uint iLevelIdx)
{
	switch (iLevelIdx)
	{
	case 0:
		if (FAILED(Add_Level0_Recipe()))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(Add_Level1_Recipe()))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(Add_Level2_Recipe()))
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CRecipe_Manager::Add_Recipe(CRecipe* pRecipe)
{
	if (m_pRecipes->find(pRecipe->Get_RecipeName()) != m_pRecipes->end())
		return E_FAIL;

	m_pRecipes->emplace(pRecipe->Get_RecipeName(), pRecipe);
	return S_OK;
}

_bool CRecipe_Manager::Check_Add_Plated_Ingredient(RECIPE_DESC Plating_Ingredient, list<RECIPE_DESC> Plated_Ingredients)
{
	for (auto& Recipe : *m_pRecipes)
	{
		if (Recipe.second->Check_Add_Plated_Ingredient(Plating_Ingredient, Plated_Ingredients))
			return true;
	}

	return false;
}

wstring CRecipe_Manager::Check_Recipe(list<RECIPE_DESC> Plated_Ingredients)
{
	for (auto& Recipe : *m_pRecipes)
		if (Recipe.second->Check_Recipe(Plated_Ingredients))
			return Recipe.first;

	return TEXT("");
}

CRecipe* CRecipe_Manager::Get_RandomRecipe()
{
	_int iRandRecipeIdx = rand() % m_pRecipes->size();

	auto pRecipe = m_pRecipes->begin();

	for (_int i = 0; i < iRandRecipeIdx; i++)
		pRecipe++;

	return (*pRecipe).second;
}

HRESULT CRecipe_Manager::Add_Level0_Recipe()
{
	CRecipe* pRecipe = CRecipe::Create(TEXT("Fish_Sashimi"));
	RECIPE_DESC pDesc = {};
	pDesc.eIngredientType = INGREDIENT_FISH;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	pRecipe = CRecipe::Create(TEXT("Prawn_Sashimi"));
	pDesc.eIngredientType = INGREDIENT_PRAWN;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRecipe_Manager::Add_Level1_Recipe()
{
	CRecipe* pRecipe = CRecipe::Create(TEXT("Fish_Sashimi"));
	RECIPE_DESC pDesc = {};
	pDesc.eIngredientType = INGREDIENT_FISH;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	pRecipe = CRecipe::Create(TEXT("Prawn_Sashimi"));
	pDesc.eIngredientType = INGREDIENT_PRAWN;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	pRecipe = CRecipe::Create(TEXT("Fish_Sushi"));
	pDesc.eIngredientType = INGREDIENT_SEAWEED;
	pDesc.eMethod = METHOD_NONE;
	pRecipe->Add_Recipe_Ingredient(pDesc);
	pDesc.eIngredientType = INGREDIENT_RICE;
	pDesc.eMethod = METHOD_BOIL;
	pRecipe->Add_Recipe_Ingredient(pDesc);
	pDesc.eIngredientType = INGREDIENT_FISH;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	pRecipe = CRecipe::Create(TEXT("Cucumber_Sushi"));
	pDesc.eIngredientType = INGREDIENT_SEAWEED;
	pDesc.eMethod = METHOD_NONE;
	pRecipe->Add_Recipe_Ingredient(pDesc);
	pDesc.eIngredientType = INGREDIENT_RICE;
	pDesc.eMethod = METHOD_BOIL;
	pRecipe->Add_Recipe_Ingredient(pDesc);
	pDesc.eIngredientType = INGREDIENT_CUCUMBER;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	pRecipe = CRecipe::Create(TEXT("Fish_Cucumber_Sushi"));
	pDesc.eIngredientType = INGREDIENT_SEAWEED;
	pDesc.eMethod = METHOD_NONE;
	pRecipe->Add_Recipe_Ingredient(pDesc);
	pDesc.eIngredientType = INGREDIENT_RICE;
	pDesc.eMethod = METHOD_BOIL;
	pRecipe->Add_Recipe_Ingredient(pDesc);
	pDesc.eIngredientType = INGREDIENT_CUCUMBER;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);
	pDesc.eIngredientType = INGREDIENT_FISH;
	pDesc.eMethod = METHOD_CHOP;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRecipe_Manager::Add_Level2_Recipe()
{
	CRecipe* pRecipe = CRecipe::Create(TEXT("Mushroom_Pasta"));
	RECIPE_DESC pDesc = {};

	pDesc.eIngredientType = INGREDIENT_PASTA;
	pDesc.eMethod = METHOD_BOIL;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	pDesc.eIngredientType = INGREDIENT_MUSHROOM;
	pDesc.eMethod = METHOD_ROAST;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	pRecipe = CRecipe::Create(TEXT("Meat_Pasta"));
	pDesc.eIngredientType = INGREDIENT_PASTA;
	pDesc.eMethod = METHOD_BOIL;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	pDesc.eIngredientType = INGREDIENT_MEAT;
	pDesc.eMethod = METHOD_ROAST;
	pRecipe->Add_Recipe_Ingredient(pDesc);

	if (FAILED(Add_Recipe(pRecipe)))
		return E_FAIL;

	return S_OK;
}

CRecipe_Manager* CRecipe_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRecipe_Manager* pInstance = new CRecipe_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRecipe_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRecipe_Manager::Clone(void* pArg)
{
	CRecipe_Manager* pInstance = new CRecipe_Manager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRecipe_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRecipe_Manager::Free()
{
	__super::Free();

	if (!m_isCloned) {
		for (auto& Pair : *m_pRecipes)
			Safe_Release(Pair.second);

		m_pRecipes->clear();


		delete(m_pRecipes);
	}
}
