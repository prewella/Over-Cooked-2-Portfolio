#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CUI_Hourglass final : public CUI
{
private:
	CUI_Hourglass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Hourglass(const CUI& rhs);
	virtual ~CUI_Hourglass() = default;

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
	void	Shake_Hourglass(_bool IsShake = true) {
		m_IsShake = IsShake;
	}

private:
	_bool	m_IsShake = { false };
	_bool	m_IsShakeLeft = { false };
	_int	m_iRotationCnt = { 0 };

	_float  m_fRotationDegree = { 0.f };

public:
	static CUI_Hourglass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END