#include "stdafx.h"
#include "MainApp.h"

#include "Level_Loading.h"
#include "Data_Storage.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);	

	//D3D11_SAMPLER_DESC

	/*m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, 0x);*/
}


HRESULT CMainApp::Initialize()
{
	ENGINE_DESC		EngineDesc = {};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	/* 내 게임의 기초 초기화 과정을 거치자. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Begin_Draw(_float4(0.f, 0.f, 1.f, 1.f));

	m_pGameInstance->Draw();

#ifdef _DEBUG

	//m_pGameInstance->Render_Font(TEXT("Font_Default"), TEXT("Test"), _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

#endif

	m_pGameInstance->End_Draw();

	return	S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	// cmd창에 입력하여 
		// MakeSpriteFont "넥슨lv1고딕 Bold" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 140.spritefont
	
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Segoe19"), TEXT("../Bin/Resources/Fonts/Segoe19.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Segoe18"), TEXT("../Bin/Resources/Fonts/Segoe18.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Segoe24"), TEXT("../Bin/Resources/Fonts/Segoe24.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Segoe32"), TEXT("../Bin/Resources/Fonts/Segoe32.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Segoe36"), TEXT("../Bin/Resources/Fonts/Segoe36.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Segoe40"), TEXT("../Bin/Resources/Fonts/Segoe40.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/141.spriteFont"))))
		return E_FAIL;

	// MakeSpriteFont "맑은 고딕" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 140.spritefont
	// MakeSpriteFont "Segoe UI Black" /FontSize:18 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 Segoe18.spritefont
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID)
		return E_FAIL;

	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID));

	return	S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Data_Storage"),
		CData_Storage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return	S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
