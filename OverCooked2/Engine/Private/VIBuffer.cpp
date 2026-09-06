#include "VIBuffer.h"

#include "GameInstance.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs) //헤헤호호
	: CComponent{ rhs }
	, m_pVB{ rhs.m_pVB }
	, m_pIB{ rhs.m_pIB }
	, m_BufferDesc{ rhs.m_BufferDesc }
	, m_InitialData{ rhs.m_InitialData }
	, m_iVertexStride{ rhs.m_iVertexStride }
	, m_iIndexStride{ rhs.m_iIndexStride }
	, m_iNumVertices{ rhs.m_iNumVertices }
	, m_iNumIndices{ rhs.m_iNumIndices }
	, m_iNumVertexBuffers{ rhs.m_iNumVertexBuffers }
	, m_eIndexFormat{ rhs.m_eIndexFormat }
	, m_ePrimitiveTopology{ rhs.m_ePrimitiveTopology }
	, m_pVerticesPos{ rhs.m_pVerticesPos }
	, m_pIndices{ rhs.m_pIndices }
	, m_IsCloned{ true }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 인덱스버퍼의 값을 이용하여 정점 버퍼의 정점들을 그려낸다. */
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,

	};

	_uint				iVertexStrides[] = {
		m_iVertexStride
	};

	_uint				iOffsets[] = {
		0
	};

	/* 정점버퍼들을 장치에 바인딩한다. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);

	/* 인덱스버퍼들을 장치에 바인딩한다. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 그릴때 어떤 형태로 정점들을 이어 그릴건지. */
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}


_float4 CVIBuffer::Compute_Picking(const CTransform* pTransform, _float& fDist) const
{
	_float3	fRayDir, fRayPos = {};

	m_pGameInstance->Transform_PickingToLocalSpace(pTransform, &fRayDir, &fRayPos);

	_vector vOut = XMVectorSet(-1.f, -1.f, -1.f, 1.f);


	for (_uint i = 0; i < m_iNumIndices / 3; i++)
	{
		/* 삼각형의 노말벡터를 구함*/
		_float3 fNormal = ComputeTriangleNormal(
			m_pVerticesPos[m_pIndices[i * 3]], 
			m_pVerticesPos[m_pIndices[i * 3 + 1]], 
			m_pVerticesPos[m_pIndices[i * 3 + 2]]);

		_vector vDot = XMVector3Dot(XMLoadFloat3(&fRayDir), XMLoadFloat3(&fNormal));

		if (XMVectorGetX(vDot) < 0.0f)
		{
			if (::TriangleTests::Intersects(XMLoadFloat3(&fRayPos), XMVector3Normalize(XMLoadFloat3(&fRayDir)), XMLoadFloat3(&m_pVerticesPos[m_pIndices[i * 3]]), XMLoadFloat3(&m_pVerticesPos[m_pIndices[i * 3 + 1]]), XMLoadFloat3(&m_pVerticesPos[m_pIndices[i * 3 + 2]]), fDist))
			{
				vOut = XMLoadFloat3(&fRayPos) + XMVector3Normalize(XMLoadFloat3(&fRayDir)) * fDist;
				goto exit;
			}
		}
	}
exit:
	_float4 vResult = { 0.f, 0.f, 0.f, 0.f };
	vOut = XMVector3TransformCoord(vOut, pTransform->Get_WorldMatrix());
	XMStoreFloat4(&vResult, vOut);

	return  vResult;
}

_float3 CVIBuffer::ComputeTriangleNormal(const _float3& v0, const _float3& v1, const _float3& v2) const
{
	_vector vectorAB = XMVectorSubtract(XMLoadFloat3(&v1), XMLoadFloat3(&v0));
	_vector vectorAC = XMVectorSubtract(XMLoadFloat3(&v2), XMLoadFloat3(&v0));

	_vector normalVector = XMVector3Cross(vectorAB, vectorAC);

	_float3 normal;
	XMStoreFloat3(&normal, XMVector3Normalize(normalVector));

	return normal;
}



HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** ppBuffer)
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_InitialData, ppBuffer);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	if (!m_IsCloned)
	{
		Safe_Delete_Array(m_pVerticesPos);
		Safe_Delete_Array(m_pIndices);
	}
}
