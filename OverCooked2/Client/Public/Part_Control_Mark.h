#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CPart_Control_Mark final : public CPartObject
{
public:
	typedef struct tagControlMarkDesc : public PARTOBJECT_DESC
	{
		_bool* pIsControl = { nullptr };
		_bool* pIsThrowing = { nullptr };
	}CONTROL_MARK_DESC;

private:
	CPart_Control_Mark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Control_Mark(const CPart_Control_Mark& rhs);
	virtual ~CPart_Control_Mark() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	vector<CModel*> m_pModelComs;

	_bool* m_pIsControl = { nullptr };
	_bool* m_pIsThrowing = { nullptr };

public:
	static CPart_Control_Mark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END