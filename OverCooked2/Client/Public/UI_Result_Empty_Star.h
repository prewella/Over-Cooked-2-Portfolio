#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Result_Empty_Star final : public CUI
{
private:
	CUI_Result_Empty_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Result_Empty_Star(const CUI_Result_Empty_Star& rhs);
	virtual ~CUI_Result_Empty_Star() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	HRESULT Add_Star();

public:
	static CUI_Result_Empty_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END