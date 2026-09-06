#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CPart_DirtyPlate final : public CPartObject
{
public:
	typedef struct tag_Part_DirtyPlate_Desc : public CPartObject::PARTOBJECT_DESC
	{
		_float4 vSettingPos = {};
	}PART_DIRTYPLATE_DESC;

private:
	CPart_DirtyPlate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_DirtyPlate(const CPart_DirtyPlate& rhs);
	virtual ~CPart_DirtyPlate() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPart_DirtyPlate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END