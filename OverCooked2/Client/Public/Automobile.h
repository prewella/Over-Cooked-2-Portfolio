#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CAutomobile final : public CModelObject
{
public:
	typedef struct tagAutomoblic_Desc : public MODELOBJECT_DESC
	{
		_float4 vStartPos = { 0.f,0.f ,0.f ,0.f };
		_float4 vGoalPos = { 0.f,0.f ,0.f ,0.f };
	}AUTOMOBILE_DESC;

private:
	CAutomobile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAutomobile(const CAutomobile& rhs);
	virtual ~CAutomobile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_float4 m_vGoalPos = { 0.f,0.f ,0.f ,0.f };
	_float m_fCreateInstance = { 0.f };

	CCollider* m_pColliderCom = { nullptr };

public:
	static CAutomobile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
