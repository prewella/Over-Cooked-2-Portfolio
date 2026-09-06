#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Curtain final : public CUI
{
public:
	typedef struct tagCurtainDesc : public CUI::UI_DESC {
		_float2* pBlindSize = { nullptr };
		_bool* pIsRender = { nullptr };
 	}CURTAIN_DESC;

private:
	CUI_Curtain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Curtain(const CUI_Curtain& rhs);
	virtual ~CUI_Curtain() = default;

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
	_float2* m_pBlindSize = { nullptr };
	_bool* m_pIsRender = { nullptr };

public:
	static CUI_Curtain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END