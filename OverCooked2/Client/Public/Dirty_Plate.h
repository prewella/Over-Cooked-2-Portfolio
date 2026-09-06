#pragma once

#include "Client_Defines.h"
#include "Belonging.h"

BEGIN(Engine)
class CPartObject;
class CCollider;
END

BEGIN(Client)

class CDirty_Plate final : public CBelonging
{
private:
	CDirty_Plate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDirty_Plate(const CDirty_Plate& rhs);
	virtual ~CDirty_Plate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();

public:
	_int Get_Plate_StackCnt() {
		return (_int)m_StackPlates.size();
	}

	void Stack_Plate();

private:
	list<class CPartObject*> m_StackPlates;

	CCollider* m_pColliderCom = { nullptr };

public:
	static CDirty_Plate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END