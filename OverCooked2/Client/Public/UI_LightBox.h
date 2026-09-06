#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_LightBox final : public CUI
{
private:
	CUI_LightBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LightBox(const CUI_LightBox& rhs);
	virtual ~CUI_LightBox() = default;

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
	static CUI_LightBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END