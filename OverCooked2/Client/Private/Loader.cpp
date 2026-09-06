#include "stdafx.h"
#include "Loader.h"

#include <process.h>
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_UI.h"

#include "Tile.h"
#include "Prop.h"
#include "Empty_Collider.h"
#include "Traffic_Light.h"
#include "Automobile.h"
#include "Spone.h"

#include "BackGround.h"
#include "Terrain.h"

#include "Chef.h"

#include "Ingredient_Crate.h"
#include "Table_Chop.h"
#include "Part_ChopKnife.h"
#include "Waste_Bin.h"
#include "Plate_Return.h"
#include "Table.h"
#include "Stove.h"
#include "Stove_Flame.h"
#include "Sink.h"
#include "Part_Plate_Wash.h"
#include "Pass.h"
#include "Conveyor.h"
#include "Dispenser.h"

#include "Ingredient.h"
#include "Cooker.h"
#include "Soup.h"
#include "Plate.h"
#include "Plated_Ingredient.h"
#include "Dirty_Plate.h"
#include "Part_DirtyPlate.h"

#include "Part_Body.h"
#include "Part_Hand.h"
#include "Part_Weapon.h"
#include "Part_Cooker.h"
#include "Part_Control_Mark.h"

#include "Recipe_Manager.h"

#include "OnionKing.h"

/* For Instance */
#include "Instance_Blood.h"
#include "Instance_Vapor.h"
#include "Instance_Puff.h"
#include "Instance_RatHit.h"
#include "Instance_Hit_Star.h"

#pragma region UI Header

/* For UI */

/* For Common */
#include "UI_Common_BG.h"
#include "UI_Common_BG_Pattern.h"
#include "UI_Common_Title.h"
#include "UI_Common_Strip.h"
#include "UI_Common_Star.h"
#include "UI_Transition.h"
#include "UI_Curtain.h"

/* For Mainmenu */
#include "UI_Main_Logo.h"
#include "UI_Chef_Change_Header.h"
#include "UI_Stage_Select_Header.h"
#include "UI_Stage_Select_Manager.h"
#include "UI_Stage_Select_BG.h"
#include "UI_Stage_Select_Title.h"
#include "UI_Stage_Select_Preview.h"
#include "UI_Select_Stage.h"
#include "UI_Exit.h"


/* For Stage Load */
#include "UI_Stage_Level_Board.h"
#include "UI_Stage_Preview.h"
#include "UI_Stage_LoadingBar.h"
#include "UI_Stage_LoadingBar_Gauge.h"


/* For GamePlay */
#include "UI_Order.h"
#include "UI_Dish.h"
#include "UI_Recipe.h"
#include "UI_Recipe_Method.h"
#include "UI_Recipe_Ingredient.h"

#include "UI_Score.h"
#include "UI_Coin.h"
#include "UI_Flame.h"
#include "UI_Tip.h"

#include "UI_Timer.h"
#include "UI_Hourglass.h"
#include "UI_Timer_Banner.h"

#include "UI_Notice.h"
#include "UI_Notice_Font.h"


/* For Result */
#include "UI_Bill.h"
#include "UI_Result_Empty_Star.h"

#pragma endregion

//// for ifstream
//#include <fstream>
//#include <filesystem>

// Editor
#include "Editor.h"
#include "BluePrint.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	/* 로더에게 지정된 레벨을 준비해라*/
	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Start()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	/* 스레드 생성. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };


	switch (m_eNextLevelID)
	{
	case Client::LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	case Client::LEVEL_MAIN_MENU:
		hr = Loading_For_MainMenu();
		break;
	case Client::LEVEL_STAGE_LOAD:
		hr = Loading_For_Stage_Load();
		break;
	case Client::LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	case Client::LEVEL_RESULT:
		hr = Loading_For_Result();
		break;
	case Client::LEVEL_EDITOR:
		hr = Loading_For_Editor();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");
	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Title/Overcooked_Title.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile0.jpg")))))
		return E_FAIL;

	Load_Texture_GamePlay_Instance();

	Load_Texture_Common_UI();
	Load_Texture_Mainmenu_UI();
	Load_Texture_Loading_UI();
	Load_Texture_GamePlay_UI();
	Load_Texture_Result_UI();

	m_strLoadingText = TEXT("컴포넌트를(을) 로딩 중 입니다.");
	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100))))
		return E_FAIL;

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	Load_Models();

	m_strLoadingText = TEXT("콜라이더를(을) 로딩 중 입니다.");
	Load_Collider();

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");
	Load_Shaders();

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MainMenu()
{
	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");
	Create_UI_Common_ProtoTypes();
	Create_UI_MainMenu_ProtoTypes();

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;

}

HRESULT CLoader::Loading_For_Stage_Load()
{
	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");
	Create_UI_Common_ProtoTypes();
	Create_UI_StageLoad_ProtoTypes();

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("컴포넌트를(을) 로딩 중 입니다.");
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Recipe_Manager"),
		CRecipe_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Create_Instance_VIBuffer_ProtoTypes();

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");

	Create_GamePlay_ProtoTypes();

	Create_Chef_ProtoTypes();

	Create_Gimmic_ProtoTypes();

	Create_InteractObj_ProtoTypes();

	Create_Belonging_ProtoTypes();
	
	Create_Instance_ProtoTypes();

	Create_UI_Gameplay_ProtoTypes();

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Result()
{
	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체의 원형를(을) 로딩 중 입니다.");
	Create_Result_ProtoTypes();
	Create_UI_Common_ProtoTypes();
	Create_UI_Result_ProtoTypes();

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Editor()
{
	m_strLoadingText = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("컴포넌트를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("모델를(을) 로딩 중 입니다.");
	_matrix		TransformMatrix = XMMatrixIdentity();
	TransformMatrix = XMMatrixScaling(ScalingMilliion, ScalingMilliion, ScalingMilliion);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Spone"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Spone", TransformMatrix))))
		return E_FAIL;

	m_strLoadingText = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strLoadingText = TEXT("객체를(을) 로딩 중 입니다.");
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tile */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tile"),
		CTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Prop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Prop"),
		CProp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Prop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Empty_Collider"),
		CEmpty_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BluePrint */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BluePrint"),
		CBluePrint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Spone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChefSpone"),
		CSpone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Create_InteractObj_ProtoTypes();
	Create_Gimmic_ProtoTypes();
	Create_Belonging_ProtoTypes();

	m_strLoadingText = TEXT("로딩이 완료되었습니다.");

	m_strLoadingText = TEXT("툴 매니저 생성이 완료되었습니다.");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Create_UI_Common_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Common_BG"),
		CUI_Common_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Common_BG_Pattern"),
		CUI_Common_BG_Pattern::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Common_Title"),
		CUI_Common_Title::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Common_Strip"),
		CUI_Common_Strip::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Common_Star"),
		CUI_Common_Star::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Transition"),
		CUI_Transition::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Curtain"),
		CUI_Curtain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_UI_MainMenu_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Main_Logo"),
		CUI_Main_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Chef_Change_Header"),
		CUI_Chef_Change_Header::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_Select_Header"),
		CUI_Stage_Select_Header::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_Select_Manager"),
		CUI_Stage_Select_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_Select_BG"),
		CUI_Stage_Select_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_Select_Title"),
		CUI_Stage_Select_Title::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_Select_Preview"),
		CUI_Stage_Select_Preview::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Select_Stage"),
		CUI_Select_Stage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Exit"),
		CUI_Exit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_UI_StageLoad_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_Level_Board"),
		CUI_Stage_Level_Board::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_Preview"),
		CUI_Stage_Preview::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_LoadingBar"),
		CUI_Stage_LoadingBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Stage_LoadingBar_Gauge"),
		CUI_Stage_LoadingBar_Gauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_Instance_VIBuffer_ProtoTypes()
{
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};

	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vPivot = InstanceDesc.vCenter;
	InstanceDesc.vRange = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vMinScale = _float3(0.125f, 0.125f, 0.125f);
	InstanceDesc.vMaxScale = _float3(0.125f, 0.125f, 0.125f);
	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vLifeTime = _float2(2.5f, 2.5f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vSpeed = _float2(2.f, 4.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Vapor"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vPivot = InstanceDesc.vCenter;
	InstanceDesc.vRange = _float3(0.5f, 0, 0.5f);
	InstanceDesc.vMinScale = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vMaxScale = _float3(1.f, 1.f, 1.f);
	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vLifeTime = _float2(10.f, 10.f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vSpeed = _float2(0.25f, 0.5f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Puff"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vPivot = InstanceDesc.vCenter;
	InstanceDesc.vRange = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vMinScale = _float3(1.f, 1.f, 1.f);
	InstanceDesc.vMaxScale = _float3(1.f, 1.f, 1.f);
	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vLifeTime = _float2(10.f, 10.f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vSpeed = _float2(0.25f, 0.5f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_RatHit"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vPivot = InstanceDesc.vCenter;
	InstanceDesc.vRange = _float3(0.25f, 0.125f, 0.25f);
	InstanceDesc.vMinScale = _float3(0.125f, 0.125f, 0.125f);
	InstanceDesc.vMaxScale = _float3(0.125f, 0.125f, 0.125f);
	InstanceDesc.iNumInstance = 25;
	InstanceDesc.vLifeTime = _float2(0.125f, 0.25f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vSpeed = _float2(2.5f, 5.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Hit_Star"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	/*InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vPivot = InstanceDesc.vCenter;
	InstanceDesc.vRange = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vMinScale = _float3(0.125f, 0.125f, 0.125f);
	InstanceDesc.vMaxScale = _float3(0.125f, 0.125f, 0.125f);
	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vLifeTime = _float2(10.f, 10.f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vSpeed = _float2(2.5f, 5.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Blood"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;*/

	InstanceDesc.vPivot = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vCenter = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vRange = _float3(0.f, 0.f, 0.f);
	InstanceDesc.vMinScale = _float3(0.5f, 0.5f, 0.5f);
	InstanceDesc.vMaxScale = _float3(0.5f, 0.5f, 0.5f);
	InstanceDesc.iNumInstance = 1;
	InstanceDesc.vLifeTime = _float2(10.f, 10.f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vSpeed = _float2(2.5f, 5.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Blood"),
		CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_GamePlay_ProtoTypes()
{
	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tile */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tile"),
		CTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Prop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Prop"),
		CProp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Prop */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Empty_Collider"),
		CEmpty_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_Chef_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chef"),
		CChef::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Body"),
		CPart_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Hand */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Hand"),
		CPart_Hand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Cooker */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Cooker"),
		CPart_Cooker::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Control_Mark */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Control_Mark"),
		CPart_Control_Mark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_Gimmic_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Traffic_Light"),
		CTraffic_Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Automobile"),
		CAutomobile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_InteractObj_ProtoTypes()
{
	/* For.Prototype_GameObject_Counter */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Counter"),
		CTable::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Table_Chop*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Table_Chop"),
		CTable_Chop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Part_ChopKnife*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_ChopKnife"),
		CPart_ChopKnife::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Ingredient_Crate*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ingredient_Crate"),
		CIngredient_Crate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Stove*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GasStove"),
		CStove::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stove_Flame"),
		CStove_Flame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Waste_Bin"),
		CWaste_Bin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sink"),
		CSink::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Plate_Wash"),
		CPart_Plate_Wash::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Plate_Return"),
		CPlate_Return::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pass"),
		CPass::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Conveyor"),
		CConveyor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dispenser"),
		CDispenser::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_Belonging_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ingredient"),
		CIngredient::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cooker"),
		CCooker::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Soup"),
		CSoup::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Plate"),
		CPlate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Plated_Ingredient"),
		CPlated_Ingredient::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dirty_Plate"),
		CDirty_Plate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_DirtyPlate"),
		CPart_DirtyPlate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_Instance_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Instance_Blood"),
		CInstance_Blood::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Instance_Vapor"),
		CInstance_Vapor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Instance_Puff"),
		CInstance_Puff::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Instance_RatHit"),
		CInstance_RatHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Instance_Hit_Star"),
		CInstance_Hit_Star::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_UI_Gameplay_ProtoTypes()
{
	/* For UI Order */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Order"),
		CUI_Order::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Dish"),
		CUI_Dish::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Recipe"),
		CUI_Recipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Recipe_Method"),
		CUI_Recipe_Method::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Recipe_Ingredient"),
		CUI_Recipe_Ingredient::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For UI Score */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Score"),
		CUI_Score::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Coin"),
		CUI_Coin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Flame"),
		CUI_Flame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Tip"),
		CUI_Tip::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For UI Timer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Timer"),
		CUI_Timer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Hourglass"),
		CUI_Hourglass::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Timer_Banner"),
		CUI_Timer_Banner::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For UI Notice */

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Notice"),
		CUI_Notice::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Notice_Font"),
		CUI_Notice_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLoader::Create_Result_ProtoTypes()
{
	///* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_UI"),
		CCamera_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OnionKing"),
		COnionKing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_UI_Result_ProtoTypes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Bill"),
		CUI_Bill::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Empty_Star"),
		CUI_Result_Empty_Star::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Load_Shaders()
{
	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Models()
{
	if (FAILED(Load_Model_Design()))
		return E_FAIL;
	if (FAILED(Load_Model_Character()))
		return E_FAIL;
	if (FAILED(Load_Model_Belonging()))
		return E_FAIL;
	if (FAILED(Load_Model_InteractObj()))
		return E_FAIL;
	if (FAILED(Load_Model_NPC()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Design()
{
	if (FAILED(Load_Model_Tile()))
		return E_FAIL;
	if (FAILED(Load_Model_Prop()))
		return E_FAIL;
	if (FAILED(Load_Model_Gimmic()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Tile()
{
	_matrix		TransformMatrix = XMMatrixIdentity();
	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Black"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Black", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Checked"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Checked", TransformMatrix))))
		return E_FAIL;
	/*
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_RedCarpet"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Design/Tile/RedCarpet", TransformMatrix))))
		return E_FAIL;
	*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Road"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Road", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Road_Cross"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Road_Cross", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Road_Side"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Road_Side", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Road_Side_Corner"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Road_Side_Corner", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Road_Stop"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Road_Stop", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Road_YellowBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Road_YellowBox", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Road_Centre"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Road_Centre", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Side_Step"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Side_Step", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_SideWalk"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/SideWalk", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tile_Vent"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Tile/Vent", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Prop()
{
	_matrix		TransformMatrix = XMMatrixIdentity();
	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Bamboo"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Bamboo", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Car"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Car", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Customer_Chair_Black"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Customer_Chair_Black", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Customer_Table_Black"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Customer_Table_Black", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Grass"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Grass", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Hydrant"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Hydrant", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Market_Stall"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Market_Stall", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Market_Stall_Blank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Market_Stall_Blank", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Orning_Corner_Long"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Orning_Corner_Long", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Orning_Corner_Short"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Orning_Corner_Short", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Orning_Straight_Long"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Orning_Straight_Long", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Orning_Straight_Short"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Orning_Straight_Short", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Plant_Table"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Plant_Table", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Restaurant_Chair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Restaurant_Chair", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Restaurant_Table"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Restaurant_Table", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Tree_CherryBlossom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Tree_CherryBlossom", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Wall_Corner"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Wall_Corner", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Wall_End"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Wall_End", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Wall_Inside"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Wall_Inside", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Wall_Straight"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Wall_Straight", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Wall_Straight_Short"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Wall_Straight_Short", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Wall_Window"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Wall_Window", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Wall_Window_Small"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Wall_Window_Small", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Prop_Traffic_Cone"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Prop/Traffic_Cone", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Gimmic()
{
	_matrix		TransformMatrix = XMMatrixIdentity();
	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Traffic_Light"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Gimmic/Traffic_Light", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Automobile"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Design/Gimmic/Traffic_Light/Automobile", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Character()
{
	_matrix		TransformMatrix = XMMatrixIdentity();

	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For Model Chef_Mel*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chef_Mel_Body"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Character/Chef_Mel/Body", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chef_Mel_Hand_Grip_L"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Chef_Mel/Hand/Grip_L", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chef_Mel_Hand_Grip_R"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Chef_Mel/Hand/Grip_R", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chef_Mel_Hand_Open_L"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Chef_Mel/Hand/Open_L", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chef_Mel_Hand_Open_R"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Chef_Mel/Hand/Open_R", TransformMatrix))))
		return E_FAIL;

	/* For Model Cookers*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Knife"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Cooker/Knife", TransformMatrix))))
		return E_FAIL;

	/* For Model Control_Mark*/
	TransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Control_Mark"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Indicator/Idle_Indicator", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Aim_Control_Mark"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Character/Indicator/Aim_Indicator", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Belonging()
{
	if (FAILED(Load_Model_Cooker()))
		return E_FAIL;
	if (FAILED(Load_Model_Ingredient()))
		return E_FAIL;
	if (FAILED(Load_Model_Plate()))
		return E_FAIL;
	if (FAILED(Load_Model_Plating_Ingredient()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Cooker()
{
	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FryingPan"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Cooker/FryingPan", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pot"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Cooker/Pot", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Soup"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Cooker/Pot/Soup", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Ingredient()
{
	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling) * XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Tomato"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Belonging/Ingredients/Tomato", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Cucumber"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Belonging/Ingredients/Cucumber", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Mushroom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Belonging/Ingredients/Mushroom", TransformMatrix))))
		return E_FAIL;

	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Fish"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Fish", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Fish_Sliced"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Fish_Sliced", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Meat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Meat", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Meat_Fried"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Meat_Fried", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Meat_Sliced"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Meat_Sliced", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Prawn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Prawn", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Prawn_Sliced"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Prawn_Sliced", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Rice"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Rice", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Seaweed"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Ingredients/Seaweed", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Plate()
{
	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plated_Tomato"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Tomato", TransformMatrix))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Dirty_Plate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Dirty_Plate", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Plating_Ingredient()
{
	if (FAILED(Load_Model_Single_Ingredient()))
		return E_FAIL;
	if (FAILED(Load_Model_Sushi()))
		return E_FAIL;
	if (FAILED(Load_Model_Pasta()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Single_Ingredient()
{
	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Seaweed"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Seaweed", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Rice"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Rice", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Fish"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Fish", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Cucumber"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Cucumber", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Prawn"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Prawn", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Pasta"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Pasta", TransformMatrix))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Meat"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Meat", TransformMatrix))))
	//	return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Mushroom"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Mushroom", TransformMatrix))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLoader::Load_Model_Sushi()
{
	_matrix		TransformMatrix = XMMatrixIdentity();
	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);

	/* 2 Ingredients*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Seaweed_Rice"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Seaweed_Rice", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Seaweed_Fish"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Seaweed_Fish", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Seaweed_Cucumber"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Seaweed_Cucumber", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Rice_Fish"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Rice_Fish", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Rice_Cucumber"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Rice_Cucumber", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Fish_Cucumber"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Fish_Cucumber", TransformMatrix))))
		return E_FAIL;

	/* 3 Ingredients*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Fish_Sushi"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Fish_Sushi", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Cucumber_Sushi"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Cucumber_Sushi", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Seaweed_Fish_Cucumber"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Seaweed_Fish_Cucumber", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Rice_Fish_Cucumber"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Rice_Fish_Cucumber", TransformMatrix))))
		return E_FAIL;

	/* 4 Ingredients*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Fish_Cucumber_Sushi"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Fish_Cucumber_sushi", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_Pasta()
{
	_matrix		TransformMatrix = XMMatrixIdentity();
	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Meat_Pasta"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Meat_Pasta", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plating_Mushroom_Pasta"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Belonging/Plate/Plate/Plated_Ingredient/Mushroom_Pasta", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_InteractObj()
{
	_matrix		TransformMatrix = XMMatrixIdentity();

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Crate"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/Ingredient_Crate", TransformMatrix))))
		return E_FAIL;


	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling);
	// Tables
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Table_Edge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Table_Edge", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Table_NoEdge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Table_NoEdge", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Table_Corner_Edge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Table_Corner_Edge", TransformMatrix))))
		return E_FAIL;
	// Table_Chop
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Table_Chop"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Table_Chop", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chop_Knife"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Table_Chop/Knife", TransformMatrix))))
		return E_FAIL;

	// Stove
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Stove"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Stove", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Stove_Flame"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Stove/Stove_Flame", TransformMatrix))))
		return E_FAIL;

	// Sink
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sink"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Sink", TransformMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plate_Wash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Sink/Plate_Wash", TransformMatrix))))
		return E_FAIL;

	// Plate_Return
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plate_Return"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Plate_Return", TransformMatrix))))
		return E_FAIL;

	// Waste_Bin
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Waste_Bin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Waste_Bin", TransformMatrix))))
		return E_FAIL;

	// Pass
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pass"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Pass", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Conveyor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Conveyor", TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Dispenser"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/InteractObj/Dispenser", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Model_NPC()
{
	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix = XMMatrixScaling(Scaling, Scaling, Scaling) * XMMatrixRotationX(XMConvertToRadians(90)) * XMMatrixRotationY(XMConvertToRadians(180));
	// OnionKing
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_OnionKing"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/NPC/OnionKing", TransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Texture_GamePlay_Instance()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Instance/Blood.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Vapor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Instance/Vapor.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Puff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Instance/Puff.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RatHits"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Instance/RatHit%d.png"),2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Instance/Hit_Star.png")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Texture_Common_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Common_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Common/Common_BG.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pattern_Gray"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Common/BG_Pattern_Gray.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pattern_Blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Common/BG_Pattern_Blue.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Common_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Common/Common_Title.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Strip"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Common/Strip.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gold_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Common/Gold_Star.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Transitions"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Common/Transition%d.png"), 2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Texture_Mainmenu_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu/Logo.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage_Select_Headers"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu/Stage_Select_Header%d.png"), 2))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage_Select_Boards"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu/Stage_Select_Board%d.png"), 2))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chef_Change_Headers"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu/Chef_Change_Header%d.png"), 2))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Exit_Headers"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu/Exit_Header%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage_Select_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu/Stage_Select_BG.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stage_Select_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MainMenu/Stage_Select_Title.png")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Texture_Loading_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Board"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/Board.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Preview_Levels"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/Preview_Level%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bar_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading_Bar_BG.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Bar_Gauge"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading_Bar_Gauge.png")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Texture_GamePlay_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Order_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Order_Background.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Prawn_Sashimi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Dish/Prawn_Sashimi.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fish_Sashimi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Dish/Fish_Sashimi.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cucumber_Sushi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Dish/Cucumber_Sushi.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fish_Sushi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Dish/Fish_Sushi.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fish_Cucumber_Sushi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Dish/Fish_Cucumber_Sushi.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Meat_Pasta"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Dish/Meat_Pasta.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mushroom_Pasta"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Dish/Mushroom_Pasta.png")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recipe_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Recipe/Recipe_Background.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recipe_Ingredients"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Recipe/Ingredients.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recipe_Methods"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Order/Recipe/Methods.png")))))
		return E_FAIL;


	/* For Score */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Score/Score_BackGround.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score_Coins"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Score/Coins.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score_Flames"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Score/Flames.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score_Tip_Banner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Score/Tip_Banner_%d.png"), 4))))
		return E_FAIL;

	/* For Timer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timer_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Timer/Timer_BackGround.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timer_Hourglass"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Timer/Hourglass.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Timer_Banner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Timer/Timer_Banner.png")))))
		return E_FAIL;

	/* For Notice */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Notices"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Notice/Notice_%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Notice_Fonts"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Notice/Notice_Font_%d.png"), 3))))
		return E_FAIL;

	return E_NOTIMPL;
}

HRESULT CLoader::Load_Texture_Result_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bill"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Result/Bill.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Empty_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Result/Empty_Star.png")))))
		return E_FAIL;

	return E_NOTIMPL;
}

HRESULT CLoader::Load_Collider()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	CoUninitialize();

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}