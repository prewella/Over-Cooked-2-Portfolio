#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Recipe_Ingredient final : public CUI
{
public:
	typedef struct tagUI_Recipe_Ingredient_Desc : public CUI::UI_DESC
	{
		INGREDIENT_TYPE eIngredient_type = INGREDIENT_NONE;
	}UI_RECIPE_INGREDIENT_DESC;

private:
	CUI_Recipe_Ingredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Recipe_Ingredient(const CUI& rhs);
	virtual ~CUI_Recipe_Ingredient() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	INGREDIENT_TYPE m_eIngredient_Type = INGREDIENT_NONE;

public:
	static CUI_Recipe_Ingredient* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END