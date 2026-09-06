#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)

class CProp final : public CModelObject
{
private:
	CProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProp(const CProp& rhs);
	virtual ~CProp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_Components(wstring strModelTag);
	HRESULT Bind_ShaderResources();

public:
	static CProp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END