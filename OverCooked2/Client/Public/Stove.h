#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
class CPartObject;
END

BEGIN(Client)

class CStove final : public CInteractObj
{
public:
	typedef struct tagStoveDesc : public MODELOBJECT_DESC
	{
		COOKER_TYPE eInitCookerType = { COOKER_END };
	}STOVE_DESC;

private:
	CStove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStove(const CStove& rhs);
	virtual ~CStove() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool Check_PutDown(class CBelonging* pBelonging) override;

private:
	HRESULT Add_Components();
	HRESULT Add_FlameObject();
	HRESULT Add_Cooker();
	 
	HRESULT Bind_ShaderResources();

public:
	HRESULT Add_Cooker(COOKER_TYPE eAddCooker);
	COOKER_TYPE Get_InitCookerType() {
		return m_eInitCookerType;
	}

	void Set_InitCookerType(COOKER_TYPE eInitCookerType) {
		m_eInitCookerType = eInitCookerType;
	}

private:
	_bool		m_IsFlameRender = { false };
	class CPartObject* m_pFlameObject = { nullptr };
	CCollider*	m_pColliderCom = { nullptr };

	COOKER_TYPE m_eInitCookerType = { COOKER_END };

public:
	static CStove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END