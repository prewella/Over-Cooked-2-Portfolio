#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Common_Star final : public CUI
{
public:
	typedef struct tagUI_Star_Desc : public UI_DESC {
		_float2 vTargetSize = { 0.f, 0.f };

	}UI_STAR_DESC;

private:
	CUI_Common_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Common_Star(const CUI_Common_Star& rhs);
	virtual ~CUI_Common_Star() = default;

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
	_float2 m_vTargetSize = { 0.f,0.f };
	_bool m_IsGrowing = { false };

public:
	static CUI_Common_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END