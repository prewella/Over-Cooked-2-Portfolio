#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Common_Title final : public CUI
{
private:
	CUI_Common_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Common_Title(const CUI_Common_Title& rhs);
	virtual ~CUI_Common_Title() = default;

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
	static CUI_Common_Title* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END