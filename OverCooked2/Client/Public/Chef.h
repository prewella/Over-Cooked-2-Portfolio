#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)
class CCollider;
class CModelObject;
END

BEGIN(Client)
#define Dash_Time 0.2f

class CChef final : public CCharacter
{
private:
	CChef(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChef(const CChef& rhs);
	virtual ~CChef() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();

private:
	void	MoveControl(_float fTimeDelta);
	void	Dash(_float fTimeDelta);
	void	Throw(_float fTimeDelta);
	void	PutDown();
	void	PickUp();

	void	Reaction_With_Obj(_float fTimeDelta);
	void	Stop_Interacting();

	void	Holding_Object_Front();
	void	Take_Object_On_CollideObj();
	void	PutDown_Object_On_CollideObj();

	void	Is_Stun(_float fTimeDelta);

	void	Check_Collision_InteractObj();
	void	Check_Collision_BelongingObj();
	void	Check_Collision_WarningObj();

	_float4	Check_Collision_Restricted_Area(_float fTimeDelta);

private:
	// For Test
	_bool m_IsCollide = { true };

	_float m_fCreateInstance = { 0.f };

	/* For Stun */
	_bool m_IsStun = { false };
	_float m_fStunTime = { 0.f };

	_bool m_bState[STATE_END] = { false };

	CHEF_ANIMATION m_eAnim = ANIM_IDLE;
	INTERACTIVE_TOOL_TYPE m_eTool = TOOL_NONE;

	_float	fDashTime = Dash_Time;

	CCollider* m_pColliderCom = { nullptr };
	CCollider* m_pInteractColliderCom = { nullptr };

	class CInteractObj* m_pCollideInteractObj = { nullptr };
	class CBelonging*	m_pCollideBelongingObj = { nullptr };


	// 캐릭터가 들고 있는 오브젝트
	class CBelonging* m_pHoldingObject = { nullptr };

public:
	static CChef* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END