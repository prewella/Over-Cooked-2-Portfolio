#pragma once

#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3		vCenter;
	}BOUNDING_DESC;

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual void* Get_BoundingDesc() = 0;

public:
	virtual HRESULT Initialize(void* pBoundingDesc);
	virtual void Tick(_fmatrix WorldMatrix) = 0;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pBoundingDesc) = 0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif

public:
	_float3* Get_Points() {
		return m_vPoint;
	}

protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_bool					m_isCollision = { false };
	_float3					m_vPoint[8];

public:
	virtual void Free() override;
};

END