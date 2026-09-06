#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Notice final : public CUI
{
#define NoticeRenderTime 1.5f

private:
	CUI_Notice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Notice(const CUI& rhs);
	virtual ~CUI_Notice() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_Font_UI();
	HRESULT Bind_ShaderResources();

public:
	void Notice_Render(_uint iNoticeIdx);

	_uint Get_NoticeIdx() {
		return m_iNoticeIdx;
	}

	_bool IsNoticed() {
		return m_IsNoticed;
	}

private:
	_uint	m_iNoticeIdx = { 0 };
	_float	m_fRenderTimer = { 0.f };

	_bool	m_IsRender = { true };
	_bool	m_IsNoticed = { false };

	class CUI_Notice_Font* m_pUI_Notice_Font = { nullptr };

public:
	static CUI_Notice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END