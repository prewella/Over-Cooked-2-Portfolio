#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CPass final : public CInteractObj
{
private:
	CPass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPass(const CPass& rhs);
	virtual ~CPass() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool Check_PutDown(class CBelonging* pBelonging) override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Light();

public:
	void Serving_Dish(list<RECIPE_DESC> Serving_Dish) {
		m_Served_Dish = Serving_Dish;
		m_IsReceive = true;
	}

	list<RECIPE_DESC> Get_Served_Dish() {
		return m_Served_Dish;
	}

	void Process_Order() {
		m_Served_Dish.clear();
		m_IsReceive = false;
	}

	_bool IsReceived() {
		return m_IsReceive;
	}

private:
	list<RECIPE_DESC> m_Served_Dish;
	CCollider* m_pColliderCom = { nullptr };
	_bool		m_IsReceive = { false };

	_float		m_fAccFalling = { 0.f };

public:
	static CPass* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END