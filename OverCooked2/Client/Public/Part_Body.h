#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CPart_Body final : public CPartObject
{
public:
	typedef struct tagPartBodyDesc: public CPartObject::PARTOBJECT_DESC
	{
		const CHEF_ANIMATION*	pState;
	}BODY_DESC;

private:
	CPart_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Body(const CPart_Body& rhs);
	virtual ~CPart_Body() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Anim_IsFinished() {
		return m_pModelCom->isFinished();
	}
	_bool Anim_IsLoop() {
		return m_IsLoopAnim;
	}


private:
	const CHEF_ANIMATION*	m_pState;

	_bool					m_IsLoopAnim = { false };

private:
	HRESULT Add_Components(wstring strModelTag);
	HRESULT Bind_ShaderResources();

public:
	static CPart_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END