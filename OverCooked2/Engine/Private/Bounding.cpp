#include "Bounding.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CBounding::Initialize(void* pBoundingDesc)
{
	return S_OK;
}

#ifdef _DEBUG
HRESULT CBounding::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	return S_OK;
}
#endif

void CBounding::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
