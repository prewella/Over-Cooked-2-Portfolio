#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Notice_Font final : public CUI
{
public:
	typedef struct tag_Notice_Font_Desc : public UI_DESC
	{
		_uint*	pNoticeIdx = { nullptr };
		_bool*	pIsRender = { nullptr };

	}NOTICE_FONT_DESC;

private:
	CUI_Notice_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Notice_Font(const CUI& rhs);
	virtual ~CUI_Notice_Font() = default;

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
	void Change_Notice();

private:
	_uint* m_pNoticeIdx = { nullptr };
	_bool* m_pIsRender = { nullptr };

public:
	static CUI_Notice_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END