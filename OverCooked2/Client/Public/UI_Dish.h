#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Dish final : public CUI
{
public:
	typedef struct tagUI_Dish_Desc : public CUI::UI_DESC
	{
		wstring str_DishName = TEXT("");
	}UI_DISH_DESC;

private:
	CUI_Dish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Dish(const CUI& rhs);
	virtual ~CUI_Dish() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components(wstring strDishName);
	HRESULT Bind_ShaderResources();

public:
	static CUI_Dish* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END