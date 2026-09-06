#pragma once

#include "Client_Defines.h"
#include "My_Instance.h"

BEGIN(Client)

class CInstance_Hit_Star : public CMy_Instance
{
private:
	CInstance_Hit_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstance_Hit_Star(const CInstance_Hit_Star& rhs);
	virtual ~CInstance_Hit_Star() = default;

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
	static CInstance_Hit_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END