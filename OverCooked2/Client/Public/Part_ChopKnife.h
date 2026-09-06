#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CPart_ChopKnife final : public CPartObject
{
public:
	typedef struct tagPartChopKnifeDesc : public CPartObject::PARTOBJECT_DESC
	{
		_bool* pIsRender = { nullptr };
	}CHOPKNIFE_DESC;

private:
	CPart_ChopKnife(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_ChopKnife(const CPart_ChopKnife& rhs);
	virtual ~CPart_ChopKnife() = default;

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
	_bool* m_pIsRender = { false };

public:
	static CPart_ChopKnife* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END