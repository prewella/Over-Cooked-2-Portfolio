#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Exit final : public CUI
{
private:
	CUI_Exit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Exit(const CUI_Exit& rhs);
	virtual ~CUI_Exit() = default;

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
	RECT m_uiRect;

	_bool m_IsIntersect = { false };

public:
	static CUI_Exit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END