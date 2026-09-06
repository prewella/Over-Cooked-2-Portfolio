#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CWaste_Bin final : public CInteractObj
{
private:
	CWaste_Bin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaste_Bin(const CWaste_Bin& rhs);
	virtual ~CWaste_Bin() = default;

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

private:
	CCollider* m_pColliderCom = { nullptr };

public:
	static CWaste_Bin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END