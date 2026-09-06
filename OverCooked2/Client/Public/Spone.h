#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)

class CSpone final : public CModelObject
{
private:
	CSpone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpone(const CSpone& rhs);
	virtual ~CSpone() = default;

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
	static CSpone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END