#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public CBounding::BOUNDING_DESC
	{
		_float3 vSize;
	}BOUNDING_AABB_DESC;

private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_BoundingDesc() {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(void* pBoundingDesc) override;
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pBounding) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif

private:
	BoundingBox* m_pBoundingDesc_Original = { nullptr };
	BoundingBox* m_pBoundingDesc = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pBoundingDesc);
	virtual void Free() override;
};

END