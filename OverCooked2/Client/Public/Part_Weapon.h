#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
END

BEGIN(Client)

class CPart_Weapon final : public CPartObject
{
public:
	typedef struct tagPartWeaponDesc: public CPartObject::PARTOBJECT_DESC
	{
		CBone* pSocket = { nullptr };
	}WEAPON_DESC;

private:
	CPart_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Weapon(const CPart_Weapon& rhs);
	virtual ~CPart_Weapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CBone* m_pSocket = { nullptr };

private:
	HRESULT Add_Components(wstring strModelTag);
	HRESULT Bind_ShaderResources();

public:
	static CPart_Weapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END