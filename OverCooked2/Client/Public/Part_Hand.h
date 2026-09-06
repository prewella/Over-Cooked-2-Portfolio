#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBone;
class CModel;
END

BEGIN(Client)

class CPart_Hand final : public CPartObject
{
public:
	typedef struct tagPartHandDesc : public CPartObject::PARTOBJECT_DESC
	{
		_bool* pIsGrip = { nullptr };
		CBone* pSocket = { nullptr };
		wstring strModelTags[2];
	}HAND_DESC;

private:
	CPart_Hand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Hand(const CPart_Hand& rhs);
	virtual ~CPart_Hand() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CBone*	m_pSocket = { nullptr };
	vector<CModel*> m_pModelComs;
	_bool*	m_pIsGrip = { nullptr };

private:
	HRESULT Add_Components(wstring* strModelTags);
	HRESULT Bind_ShaderResources();

public:
	static CPart_Hand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END