#pragma once

#include "Client_Defines.h"
#include "Component.h"

BEGIN(Client)

class CRecipe_Manager final : public CComponent
{
private:
	CRecipe_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRecipe_Manager(const CRecipe_Manager& rhs);
	virtual ~CRecipe_Manager() = default;

public:
	virtual HRESULT Initialize_Prototype() override; 
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Setting_Level_Recipe(_uint iLevelIdx);

	HRESULT Add_Recipe(class CRecipe* pRecipe);
	_bool Check_Add_Plated_Ingredient(RECIPE_DESC Plating_Ingredient, list<RECIPE_DESC> Plated_Ingredients);
	wstring Check_Recipe(list<RECIPE_DESC> Plated_Ingredients);

	class CRecipe* Get_RandomRecipe();

private:
	HRESULT Add_Level0_Recipe();
	HRESULT Add_Level1_Recipe();
	HRESULT Add_Level2_Recipe();

private:
	map<const wstring, class CRecipe*>*		m_pRecipes = {nullptr};

public:
	static CRecipe_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone (void* pArg) override;
	virtual void Free() override;
};

END