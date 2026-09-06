#include "stdafx.h"
#include "Data_Storage.h"

CData_Storage::CData_Storage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent{ pDevice , pContext }
{
}

CData_Storage::CData_Storage(const CData_Storage& rhs)
	:CComponent{ rhs }
	, m_StagePlayDatas{ rhs.m_StagePlayDatas }
{
}

HRESULT CData_Storage::Initialize_Prototype()
{
	m_StagePlayDatas = new vector<STAGE_SCORE_INFO*>;

	for (auto i = 0; i < StageNum; i++)
	{
		STAGE_SCORE_INFO* StageData = new STAGE_SCORE_INFO;
		ZeroMemory(&(*StageData), sizeof(STAGE_SCORE_INFO));

		m_StagePlayDatas->push_back(StageData);
	}
	

	return S_OK;
}

HRESULT CData_Storage::Initialize(void* pArg)
{
	return S_OK;
}

CData_Storage* CData_Storage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CData_Storage* pInstance = new CData_Storage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CData_Storage"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CData_Storage::Clone(void* pArg)
{
	CData_Storage* pInstance = new CData_Storage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CData_Storage"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CData_Storage::Free()
{
	__super::Free();

	if (!m_isCloned) {

		for (auto StageData : *m_StagePlayDatas)
			delete(StageData);

		delete(m_StagePlayDatas);
	}
}
