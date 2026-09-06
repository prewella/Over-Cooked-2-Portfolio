#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Common_BG_Pattern final : public CUI
{
public:
	typedef struct tagUI_Common_BG_Pattern_Desc : public UI_DESC
	{
		wstring strPatternName = { TEXT("") };
	}UI_COMMON_BG_PATTERN_DESC;

private:
	CUI_Common_BG_Pattern(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Common_BG_Pattern(const CUI_Common_BG_Pattern& rhs);
	virtual ~CUI_Common_BG_Pattern() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components(wstring strPatternName);
	HRESULT Bind_ShaderResources();

private:
	_float m_fAccFalling = { 0.f };

public:
	static CUI_Common_BG_Pattern* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END