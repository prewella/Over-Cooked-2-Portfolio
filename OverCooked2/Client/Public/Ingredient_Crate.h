#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CIngredient_Crate final : public CInteractObj
{
public:
	typedef struct tagIngredientCrateDesc : public MODELOBJECT_DESC
	{
		INGREDIENT_TYPE eIngredientType = INGREDIENT_NONE;
	}INGREDIENTCRATE_DESC;

private:
	enum CRATE_ANIMATION {
		ANIM_IDLE, ANIM_OPEN, ANIM_END
	};

private:
	CIngredient_Crate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIngredient_Crate(const CIngredient_Crate& rhs);
	virtual ~CIngredient_Crate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_OpenAnim() {
		m_pModelCom->Set_Animation(ANIM_OPEN, false);
	}

	void Set_Ingredient(INGREDIENT_TYPE eIngredient_Type) {
		m_eIngredientType = eIngredient_Type;
	}

	INGREDIENT_TYPE Get_IngredientType() {
		return m_eIngredientType;
	}

	class CBelonging* Create_Ingredient();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CCollider* m_pColliderCom = { nullptr };
	INGREDIENT_TYPE m_eIngredientType = { INGREDIENT_END };

public:
	static CIngredient_Crate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END