#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CPlate_Return final : public CInteractObj
{
private:
	CPlate_Return(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlate_Return(const CPlate_Return& rhs);
	virtual ~CPlate_Return() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool Check_PutDown(class CBelonging* pBelonging) override;
	virtual void	Take_HaveObj(_Out_ class CBelonging** pBelonging);
	virtual	void	Set_On_Belonging();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	void IsExist_Sink(_bool IsExist_Sink) {
		m_IsExist_Sink = IsExist_Sink;
	}

	void Return_Plate();

private:
	_bool m_IsExist_Sink = { false };

	list<class CBelonging*> m_CleanPlates;

	CCollider* m_pColliderCom = { nullptr };

public:
	static CPlate_Return* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END