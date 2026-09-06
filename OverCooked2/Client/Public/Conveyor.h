#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CConveyor final : public CInteractObj
{
private:
	CConveyor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConveyor(const CConveyor& rhs);
	virtual ~CConveyor() = default;

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
	_bool	ReceiveBelonging(class CBelonging* pPassedBelonging, _float4 vPassDir);

private:
	void	Moving_Belonging();

	void	PassBelonging();

	void	Check_Collision_InteractObj();

private:
	_bool m_IsReceived = { false };
	_float4 m_vReceivedDir = {};

	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pForwardColliderCom = { nullptr };
	CInteractObj* m_pCollideObj = { nullptr };

	_float		m_fAccFalling = { 0.f };

public:
	static CConveyor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END