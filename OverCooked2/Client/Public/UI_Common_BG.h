#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Common_BG final : public CUI
{
public:
	typedef struct tag_Common_BG_Desc : public UI_DESC {
		_uint iLevelIdx = { 0 };
	}COMMON_BG_DESC;
private:
	CUI_Common_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Common_BG(const CUI_Common_BG& rhs);
	virtual ~CUI_Common_BG() = default;

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
	HRESULT Add_BG_Pattern(_uint iLevelIdx);

public:
	static CUI_Common_BG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END