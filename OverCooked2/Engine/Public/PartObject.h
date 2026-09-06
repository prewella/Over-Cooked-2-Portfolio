#pragma once

#include "ModelObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CModelObject
{
public:
	typedef struct PartObjectDesc : public MODELOBJECT_DESC
	{
		const _float4x4* pParentMatrix;
	}PARTOBJECT_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float4x4				m_WorldMatrix = {};
	const _float4x4*		m_pParentMatrix = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END