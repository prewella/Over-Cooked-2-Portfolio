#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CPlated_Ingredient final : public CPartObject
{
public:
	typedef struct tagPlated_IngredientDesc : public CPartObject::PARTOBJECT_DESC
	{
		INGREDIENT_TYPE eIngredientType = { INGREDIENT_NONE };
	}PLATED_INGREDIENT_DESC;

private:
	CPlated_Ingredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlated_Ingredient(const CPlated_Ingredient& rhs);
	virtual ~CPlated_Ingredient() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components(INGREDIENT_TYPE eIngredient);
	HRESULT Bind_ShaderResources();

public:
	HRESULT Change_Model(list<RECIPE_DESC> PlatedIngredientDescs);

private:
	wstring Get_ModelName_Using_Ingredients(_uint iNumIngredient, _bool* IsPlatingIngredient);

public:
	static CPlated_Ingredient* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END