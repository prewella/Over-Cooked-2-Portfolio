#pragma once

#include "Client_Defines.h"
#include "Belonging.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CPlate final : public CBelonging
{
private:
	CPlate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlate(const CPlate& rhs);
	virtual ~CPlate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	_bool Check_Plating(CBelonging* pBelonging);
	_bool Plating_Ingredient(CBelonging* pBelonging);

	void Waste_Plated_Ingredient();
	list<RECIPE_DESC> Serving_Plate() {
		return m_PlatingDescs;
	}

private:
	HRESULT Add_Plate_Ingredient_Model(INGREDIENT_TYPE plated_IngredientType);

private:
	class CPlated_Ingredient* m_pPlatedIngredient = { nullptr };

	list<RECIPE_DESC> m_PlatingDescs;
	class CRecipe_Manager* m_pRecipe_Manager = { nullptr };

	CCollider* m_pColliderCom = { nullptr };

public:
	static CPlate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END