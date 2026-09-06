#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)

class COnionKing final : public CModelObject
{
private:
	COnionKing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COnionKing(const COnionKing& rhs);
	virtual ~COnionKing() = default;

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
	static COnionKing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END