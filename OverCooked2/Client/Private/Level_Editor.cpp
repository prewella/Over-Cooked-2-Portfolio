#include "stdafx.h"
#include "Level_Editor.h"

#include "Camera_Free.h"
#include "Editor.h"

CLevel_Editor::CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Editor::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Initialize_IMGUI(CEditor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Editor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_Editor::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level_Editor"));

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Terrain(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Editor* CLevel_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Editor* pInstance = new CLevel_Editor(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Create : CLevel_Editor "));

		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Editor::Free()
{
	__super::Free();
}