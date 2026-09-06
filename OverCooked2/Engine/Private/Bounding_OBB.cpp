#include "Bounding_OBB.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CBounding{pDevice, pContext}
{
}

HRESULT CBounding_OBB::Initialize(void* pBoundingDesc)
{
	BOUNDING_OBB_DESC* pDesc = (BOUNDING_OBB_DESC*)pBoundingDesc;

	_float4 vOrientation;
	XMStoreFloat4(&vOrientation, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

	m_pBoundingDesc_Original = new BoundingOrientedBox(pDesc->vCenter, _float3(pDesc->vSize.x * 0.5f, pDesc->vSize.y * 0.5f, pDesc->vSize.z * 0.5f), vOrientation);
	m_pBoundingDesc = new BoundingOrientedBox(*m_pBoundingDesc_Original);

	return S_OK;
}

void CBounding_OBB::Tick(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, WorldMatrix);

	m_pBoundingDesc->GetCorners(m_vPoint);
}

_bool CBounding_OBB::Intersect(CCollider::TYPE eType, CBounding* pBounding)
{
	void* pTargetBoundingDesc = pBounding->Get_BoundingDesc();

	m_isCollision = { false };

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isCollision = m_pBoundingDesc->Intersects(*((BoundingBox*)pTargetBoundingDesc));
		break;
	case CCollider::TYPE_OBB:
		m_isCollision = m_pBoundingDesc->Intersects(*((BoundingOrientedBox*)pTargetBoundingDesc));
		break;
	case CCollider::TYPE_SPHERE:
		m_isCollision = m_pBoundingDesc->Intersects(*((BoundingSphere*)pTargetBoundingDesc));
		break;
	}

	return m_isCollision;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, m_isCollision == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));


	return S_OK;
}
#endif

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CBounding_OBB"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pBoundingDesc_Original);
}
