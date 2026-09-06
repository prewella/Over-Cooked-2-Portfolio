#pragma once

#include "Client_Defines.h"
#include "My_Instance.h"

BEGIN(Client)

class CInstance_RatHit : public CMy_Instance
{
private:
	CInstance_RatHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_RatHit(const CInstance_RatHit& rhs);
	virtual ~CInstance_RatHit() = default;

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
	_bool m_IsNotHitTexture = { false };
	_float m_fHitTextureTimer = { 0.f };
	_float m_fFade_Percent = { 0.f };

public:
	static CInstance_RatHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END