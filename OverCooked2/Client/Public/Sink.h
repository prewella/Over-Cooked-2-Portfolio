#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CPartObject;
class CCollider;
END

BEGIN(Client)

class CSink final : public CInteractObj
{
private:
	CSink(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSink(const CSink& rhs);
	virtual ~CSink() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool Check_PutDown(class CBelonging* pBelonging) override;
	virtual void Take_HaveObj(_Out_ class CBelonging** pBelonging) override;

	virtual _bool Check_Reaction();

protected:
	virtual void Set_On_Belonging() override;

private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Bind_ShaderResources();

public:
	_bool Check_In_ReactionArea(_float4 vChefPos);

	void Add_DirtyPlate(CBelonging* pBelongnig);

	/* For Test */
	void Add_DirtyPlate(_int iNumDirty_Plate);
	void Wash_Plate(_float fTimeDelta);

private:
	void Add_CleanPlate();
	void Cleaning_Plate();

private:
	_int m_iNumDirtyPlate = { 0 };
	// 최대 3개까지 갯수에 따라 모델 렌더
	vector<class CPartObject*> m_DirtyPlates;

	// 상호작용 하여 닦인 접시
	list<class CBelonging*> m_CleanedPlates;

	_float m_fSinkPercent = { 0.f };

	CCollider* m_pColliderCom = { nullptr };

public:
	static CSink* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END