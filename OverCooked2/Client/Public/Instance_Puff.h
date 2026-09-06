#pragma once

#include "Client_Defines.h"
#include "My_Instance.h"

BEGIN(Client)

class CInstance_Puff : public CMy_Instance
{
private:
	CInstance_Puff(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Puff(const CInstance_Puff& rhs);
	virtual ~CInstance_Puff() = default;

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
	static CInstance_Puff* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END