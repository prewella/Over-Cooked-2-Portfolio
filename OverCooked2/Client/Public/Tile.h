#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)

class CTile final : public CModelObject
{
private:
	CTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTile(const CTile& rhs);
	virtual ~CTile() = default;

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
	static CTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END