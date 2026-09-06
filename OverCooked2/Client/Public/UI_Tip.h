#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Tip final : public CUI
{
public:
	typedef struct tagUI_Tip_Desc : public CUI::UI_DESC
	{
		_bool* pIsTips = { nullptr };
		_uint* pTimes = { nullptr };
	}UI_TIP_DESC;

private:
	CUI_Tip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Tip(const CUI& rhs);
	virtual ~CUI_Tip() = default;

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
	_bool* m_pIsTips = { nullptr };
	_uint* m_pTimes = { nullptr };

public:
	static CUI_Tip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END