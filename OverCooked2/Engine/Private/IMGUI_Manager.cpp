#include "IMGUI_Manager.h"
#include "Tool.h"
#include "GameInstance.h"

CIMGUI_Manager::CIMGUI_Manager()
{
}

HRESULT CIMGUI_Manager::Initiailize(CTool* _pTool)
{
	if (nullptr == _pTool)
		return E_FAIL;

	m_pTool = _pTool;

	return S_OK;
}

void CIMGUI_Manager::Tick(_float fTimeDelta)
{
	if (nullptr != m_pTool)
		m_pTool->Tick(fTimeDelta);
}

HRESULT CIMGUI_Manager::Render()
{
	if (nullptr != m_pTool)
		m_pTool->Render();

	return S_OK;
}

CIMGUI_Manager* CIMGUI_Manager::Create(CTool* _pTool)
{
	CIMGUI_Manager* pInstance = new CIMGUI_Manager();

	if (FAILED(pInstance->Initiailize(_pTool)))
	{
		MSG_BOX(TEXT("Failed to Created : IMGUI Manager"));
		Safe_Release<CIMGUI_Manager*>(pInstance);
	}

	return pInstance;
}

void CIMGUI_Manager::Free()
{
	__super::Free();

	Safe_Release<CTool*>(m_pTool);
}
