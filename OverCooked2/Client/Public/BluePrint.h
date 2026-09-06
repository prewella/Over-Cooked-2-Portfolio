#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Client)

class CBluePrint final : public CModelObject
{
private:
	CBluePrint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBluePrint(const CBluePrint& rhs);
	virtual ~CBluePrint() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Set_Model(wstring strModelTag);
	void Set_Render(_bool bRender = true) {
		m_bRender = bRender;
	}

private:
	HRESULT Add_Components(wstring strModelTag);
	HRESULT Bind_ShaderResources();

private:
	_bool m_bRender = { false };

public:
	static CBluePrint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END