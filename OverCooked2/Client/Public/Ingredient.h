#pragma once

#include "Client_Defines.h"
#include "Belonging.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CIngredient final : public CBelonging
{
public:
	typedef struct tagIngredient_Desc : public BELONGING_DESC
	{
		INGREDIENT_TYPE eIngredientType = INGREDIENT_END;
	}INGREDIENT_DESC;

	enum CRATE_ANIMATION {
		ANIM_CUT1, ANIM_CUT2, ANIM_CUT3, ANIM_CUT4, ANIM_CUT5, ANIM_CUT6, ANIM_CUT7, ANIM_CUTTED, ANIM_NONCUT, ANIM_END
	};

private:
#define Default_Speed 20.f
	
private:
	CIngredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIngredient(const CIngredient& rhs);
	virtual ~CIngredient() = default;

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
	void Set_NextAnim();

	void Chop_Ingredient();

	void Set_Throwing(_float4 vThrowingDir);

	INGREDIENT_TYPE Get_IngredientType() {
		return m_eIngredientType;
	}
	COOKING_METHOD_TYPE Get_Method() {
		return m_eMethod;
	}

private:
	void Throwing_End() {
		m_IsThrow = false;
		m_fThrowingSpeed = Default_Speed;
	}
	void Set_IngredientType();

	void Update_ReactionType();
	HRESULT Change_Model();
	void Create_Chop_Effect();

private:
	CCollider* m_pColliderCom = { nullptr };
	INGREDIENT_TYPE m_eIngredientType = { INGREDIENT_END };
	COOKING_METHOD_TYPE m_eMethod = { METHOD_NONE };

	_bool m_IsAnimModel = { false };
	_bool m_IsChopped = { false };
	_uint m_iNonAnimChopCount = { 0 };

	/* for Throwing */
	_bool m_IsThrow = { false };
	_float4 m_vThrowingDir = {};
	_float m_fThrowingSpeed = { Default_Speed };

public:
	static CIngredient* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END