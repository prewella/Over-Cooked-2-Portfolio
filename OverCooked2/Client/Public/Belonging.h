#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)

class CBelonging abstract : public CModelObject
{
public:
	typedef struct tagBelonging_Desc : public MODELOBJECT_DESC
	{
	}BELONGING_DESC;

protected:
	CBelonging(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBelonging(const CBelonging& rhs);
	virtual ~CBelonging() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	BELONGING_TYPE Get_Type() {
		return m_eBelongingType;
	}
	REACTION_ENABLE_TYPE Get_ReactionType() {
		return m_eReactionType;
	}

	void Set_Drop() {
		m_IsDrop = true;
	}

protected:
	void Drop(_float fTimeDelta);

protected:
	BELONGING_TYPE			m_eBelongingType = { BELONGING_END };
	REACTION_ENABLE_TYPE	m_eReactionType = { REACTION_END };
	_bool					m_IsDrop = { false };

public:
	virtual void Free() override;
};

END