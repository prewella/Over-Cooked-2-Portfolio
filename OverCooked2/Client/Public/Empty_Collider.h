#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CEmpty_Collider final : public CGameObject
{
private:
	CEmpty_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEmpty_Collider(const CEmpty_Collider& rhs);
	virtual ~CEmpty_Collider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();

private:
	CCollider* m_pColliderCom = { nullptr };

public:
	static CEmpty_Collider * Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END