#pragma once

#include "Client_Defines.h"
#include "Belonging.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CCooker final : public CBelonging
{
public:
	typedef struct tagCooker_Desc : public BELONGING_DESC
	{
		COOKER_TYPE eCookerType= COOKER_NONE;
	}COOKER_DESC;

protected:
	CCooker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCooker(const CCooker& rhs);
	virtual ~CCooker() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_CookingIngredient();
	void Delete_CookingIngredient();
	HRESULT Bind_ShaderResources();

public:
	_bool Check_PutDown(CBelonging* pIngredient);
	_bool PutIn_Ingredient(CBelonging* pIngredient);
	void ThrowAway_Ingredient();
	void Cooking(_float fTimeDelta);

	COOKER_TYPE Get_CookerType() {
		return m_eCookerType;
	}

	INGREDIENT_TYPE Get_HaveIngredient_Type() {
		return m_eHaveIngredientType;
	}

	_bool IsHaveIngredient() {
		if (m_eHaveIngredientType != INGREDIENT_NONE)
			return true;
		return false;
	}

	_bool IsCooked() {
		return m_IsCooked;
	}

protected:
	COOKER_TYPE m_eCookerType = { COOKER_NONE };
	INGREDIENT_TYPE m_eHaveIngredientType = { INGREDIENT_NONE };

	class CPartObject* m_pCookingIngredient = { nullptr };

	_float m_fCookingPercent = { 0.f };
	_bool m_IsCooked = { false };
	_bool m_IsOverCooked = { false };

	class CCollider* m_pColliderCom = { nullptr };

public:
	static CCooker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END