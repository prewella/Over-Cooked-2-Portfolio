#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CPart_Plate_Wash final : public CPartObject
{
public:
	typedef struct tag_PartPlate_WashDesc : public CPartObject::PARTOBJECT_DESC
	{
		_float4x4 vSettingMatrix = {};
	}PART_PLATE_WASH_DESC;

private:
	CPart_Plate_Wash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Plate_Wash(const CPart_Plate_Wash& rhs);
	virtual ~CPart_Plate_Wash() = default;

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
	void IsRender(_bool IsRender) {
		m_IsRender = IsRender;
	}

private:
	_bool m_IsRender = { false };

public:
	static CPart_Plate_Wash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END