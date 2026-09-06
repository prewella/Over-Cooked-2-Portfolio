#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CPartObject;
END

BEGIN(Client)

class CCharacter abstract : public CGameObject
{
protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Control(_bool IsControl) {
		m_IsControl = IsControl;
	};

protected:
	map<const wstring, CPartObject*>	m_PartObjects;
	_bool								m_IsControl = { false };

public:
	virtual void Free() override;
};

END