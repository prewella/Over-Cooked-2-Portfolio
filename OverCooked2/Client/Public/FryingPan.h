#pragma once

#include "Client_Defines.h"
#include "Cooker.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CFryingPan final : public CCooker
{
private:
	CFryingPan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFryingPan(const CFryingPan& rhs);
	virtual ~CFryingPan() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CCollider* m_pColliderCom = { nullptr };

public:
	static CFryingPan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END