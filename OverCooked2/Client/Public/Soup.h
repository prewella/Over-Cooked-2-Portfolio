#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CSoup final : public CPartObject
{
private:
	CSoup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSoup(const CSoup& rhs);
	virtual ~CSoup() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSoup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END