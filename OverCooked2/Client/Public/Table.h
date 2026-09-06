#pragma once

#include "Client_Defines.h"
#include "InteractObj.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CTable final : public CInteractObj
{
public:
	typedef struct tagTable_Desc : public MODELOBJECT_DESC
	{
		_bool IsCreatePlate = {false};
	}TABLE_DESC;

private:
	CTable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTable(const CTable& rhs);
	virtual ~CTable() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_Components(wstring strModelTag);
	HRESULT Add_Plate();
	HRESULT Bind_ShaderResources();

public:
	void Set_Create_Plate(_bool IsCreatePlate) {
		m_IsCreatePlate = IsCreatePlate;
	}
	_bool IsCreatePlate() {
		return m_IsCreatePlate;
	}

private:
	CCollider* m_pColliderCom = { nullptr };
	// 에디터 세팅 저장용
	_bool m_IsCreatePlate = { false };

public:
	static CTable* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END