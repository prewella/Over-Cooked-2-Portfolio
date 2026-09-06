#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Recipe_Method final : public CUI
{
public:
	typedef struct tagUI_Recipe_Method_Desc : public CUI::UI_DESC
	{
		COOKING_METHOD_TYPE eMethod = METHOD_NONE;
	}UI_RECIPE_METHOD_DESC;
private:
	CUI_Recipe_Method(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Recipe_Method(const CUI& rhs);
	virtual ~CUI_Recipe_Method() = default;

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
	COOKING_METHOD_TYPE m_eMethod = METHOD_NONE;

public:
	static CUI_Recipe_Method* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END