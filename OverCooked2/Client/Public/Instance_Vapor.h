#pragma once

#include "Client_Defines.h"
#include "My_Instance.h"

BEGIN(Client)

class CInstance_Vapor : public CMy_Instance
{
private:
	CInstance_Vapor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Vapor(const CInstance_Vapor& rhs);
	virtual ~CInstance_Vapor() = default;

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
	_float m_fFade_Percent = { 0.f };

public:
	static CInstance_Vapor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END