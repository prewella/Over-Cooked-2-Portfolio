#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CDispenser final : public CInteractObj
{
public:
	typedef struct tagDispenserDesc : public CModelObject::MODELOBJECT_DESC
	{
		_uint iNumCreateIngredient = { 0 };

		INGREDIENT_TYPE eIngredientType[2];

	}DISPENSER_DESC;

private:
	CDispenser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDispenser(const CDispenser& rhs);
	virtual ~CDispenser() = default;

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
	void Set_NumIngredient(_uint iNumIngredient) {
		m_iNumIngredient = iNumIngredient;
		m_CreateIngredientTypes.resize(m_iNumIngredient);
	}

	_uint Get_NumIngredient() {
		return m_iNumIngredient;
	}

	void Set_CreateIngredient_Type(_uint iIngredientIdx, INGREDIENT_TYPE eIngredientType) {
		m_CreateIngredientTypes[iIngredientIdx] = eIngredientType;
	}

	INGREDIENT_TYPE Get_CreateIngredientType(_uint iIdx) {
		return m_CreateIngredientTypes[iIdx];
	}

private:
	void	Moving_Belonging();
	void	Create_Ingredient();
	void	PassBelonging();

	void	Check_Collision_InteractObj();

private:
	_uint m_iNumIngredient = { 0 };
	vector<INGREDIENT_TYPE> m_CreateIngredientTypes;

	_float m_fCreateIngredientTime = { 0.f };
	_bool m_IsCreateIdx = { false };
	 
	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pForwardColliderCom = { nullptr };
	CInteractObj* m_pCollideObj = { nullptr };

	_float		m_fAccFalling = { 0.f };

public:
	static CDispenser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END