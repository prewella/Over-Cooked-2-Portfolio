#include "stdafx.h"
#include "Level_GamePlay.h"

#include "Level_Loading.h"

#include "Data_Storage.h"

#include "Camera_Free.h"
#include "Character.h"

#include "Table.h"
#include "Table_Chop.h"
#include "Ingredient_Crate.h"
#include "Stove.h"
#include "Pass.h"
#include "Plate_Return.h"
#include "Dispenser.h"

#include "Plate.h"

#include "Recipe_Manager.h"
#include "Recipe.h"

#include "UI_Order_Manager.h"
#include "UI_Score.h"
#include "UI_Timer.h"
#include "UI_Notice.h"

#include "Traffic_Light.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize(void* pArg)
{
	GAMEPLAY_DESC* pDesc = (GAMEPLAY_DESC*)pArg;

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	wstring strDataPath;
	//strDataPath = TEXT("../Bin/Resources/Data/Level1-1.dat");
	switch (g_iStageIdx)
	{
	case 0:
		strDataPath = TEXT("../Bin/Resources/Data/Level0.dat");
		m_fTotalTime = 150.f;
		m_fRemainTime = 150.f;
		break;
	case 1:
		strDataPath = TEXT("../Bin/Resources/Data/Level1-2.dat");
		m_fTotalTime = 180.f;
		m_fRemainTime = 180.f;
		break;
	}

	if (FAILED(Load_Data(strDataPath)))
		return E_FAIL;

	m_pDataStorage = (CData_Storage*)m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Data_Storage"));
	m_pRecipe_Manager = (CRecipe_Manager*)m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Recipe_Manager"));
	m_pRecipe_Manager->Setting_Level_Recipe(g_iStageIdx);

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;

	if (FAILED(Ready_LandObject()))
		return E_FAIL;

	if (FAILED(Ready_UI()))
		return E_FAIL;

	//m_fTotalTime = GamePlayTime;
	//m_fRemainTime = GamePlayTime;

	//if (g_iStageIdx == 0)
	//{
	//	m_fTotalTime = 10.f;
	//	m_fRemainTime = 10.f;
	//}
	//else if (g_iStageIdx == 1)
	//{
	//	m_fTotalTime = 150.f;
	//	m_fRemainTime = 150.f;
	//}

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (m_pGameInstance->Get_DIKeyState(DIK_TAB) == EKeyState::DOWN)
	{
		m_CharacterList[m_iSelectCharacterIdx]->Set_Control(false);

		if (m_iSelectCharacterIdx < m_CharacterList.size() - 1)
			m_iSelectCharacterIdx++;
		else
			m_iSelectCharacterIdx = 0;

		m_CharacterList[m_iSelectCharacterIdx]->Set_Control(true);
	}

	if (m_pUI_Notice->IsNoticed() && m_pUI_Notice->Get_NoticeIdx() == 1) {
		if (!m_IsPlayBgm)
		{
			m_pGameInstance->Play_BGM(L"BGM_GamePlay.wav");
			m_IsPlayBgm = true;
		}

		if (m_fRemainTime > 0.f)
		{
			m_fRemainTime -= fTimeDelta;
			if (m_fRemainTime <= 0.f)
			{
				m_pUI_Notice->Notice_Render(2);
				m_pGameInstance->StopSound(CSound_Manager::CH_STAGE_TRANSITION);
				m_pGameInstance->Play_Sound(L"Times_Up.wav", CSound_Manager::CH_STAGE_TRANSITION, 1.f);
			}
		}

		Add_Order(fTimeDelta);

		Served_Plate();

		Add_Score();
	}

	if (m_pUI_Notice->IsNoticed() && m_pUI_Notice->Get_NoticeIdx() == 2)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_RESULT))))
			return;
	}

	
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Data(wstring& strFilePath)
{
	ifstream ifs;
	ifs.open(strFilePath);
	// 파일 읽고 객체들 생성 해줘야 함

	// 레이어 개수 저장해줘야 함;;
	_uint iNumLoadLayer = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLoadLayer), sizeof(_uint));

	for (_uint i = 0; i < iNumLoadLayer; i++)
	{
		switch ((Load_Layer)i)
		{
		case Client::LAYER_TILE:
		case Client::LAYER_PROP:
			Load_Layer_Data(ifs);
			break;
		case Client::LAYER_INTERACT:
			Load_Layer_Interact_Data(ifs);
			if (m_IsSinkExist)
				((CPlate_Return*)m_pPlateReturnObj)->IsExist_Sink(m_IsSinkExist);
			break;
		case Client::LAYER_COLLISION:
			Load_Layer_Collision_Data(ifs);
			break;
		case Client::LAYER_SPONE:
			Load_Layer_Chef_SponePos(ifs);
			break;
		case Client::LAYER_GIMMIC:
			Load_Layer_Gimmic_Data(ifs);
			break;
		}
	}

	ifs.close();

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Layer_Data(ifstream& ifs)
{
	_uint iNumLayerNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLayerNameLength), sizeof(_uint));
	wstring strLayerName = TEXT("");
	strLayerName.resize(iNumLayerNameLength);
	ifs.read(reinterpret_cast<_char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	_uint iListSize = 0;
	ifs.read(reinterpret_cast<_char*>(&iListSize), sizeof(_uint));

	for (_uint i = 0; i < iListSize; i++)
	{
		// 오브젝트 이름 읽기
		_uint iNumObjectNameLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumObjectNameLength), sizeof(_uint));

		wstring strObjectName = TEXT("");
		strObjectName.resize(iNumObjectNameLength);
		ifs.read(reinterpret_cast<_char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

		// 오브젝트 월드 행렬 읽기
		_float4x4 worldFloat4x4 = {};
		ifs.read(reinterpret_cast<_char*>(&worldFloat4x4), sizeof(_float4x4));

		_uint iNumModelTagLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumModelTagLength), sizeof(_uint));

		wstring strModelTag = TEXT("");
		strModelTag.resize(iNumModelTagLength);
		ifs.read(reinterpret_cast<_char*>(&strModelTag[0]), sizeof(wchar_t) * iNumModelTagLength);

		CModelObject::MODELOBJECT_DESC pDesc = {};
		pDesc.bInitWorldMatrix = true;
		pDesc.InitWorldFloat4x4 = worldFloat4x4;
		pDesc.strModelTag = strModelTag;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Layer_Interact_Data(ifstream& ifs)
{
	_uint iNumLayerNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLayerNameLength), sizeof(_uint));
	wstring strLayerName = TEXT("");
	strLayerName.resize(iNumLayerNameLength);
	ifs.read(reinterpret_cast<_char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	_uint iListSize = 0;
	ifs.read(reinterpret_cast<_char*>(&iListSize), sizeof(_uint));

	for (_uint i = 0; i < iListSize; i++)
	{
		// 오브젝트 이름 읽기
		_uint iNumObjectNameLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumObjectNameLength), sizeof(_uint));

		wstring strObjectName = TEXT("");
		strObjectName.resize(iNumObjectNameLength);
		ifs.read(reinterpret_cast<_char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

		// 오브젝트 월드 행렬 읽기
		_float4x4 worldFloat4x4 = {};
		ifs.read(reinterpret_cast<_char*>(&worldFloat4x4), sizeof(_float4x4));

		_uint iNumModelTagLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumModelTagLength), sizeof(_uint));

		wstring strModelTag = TEXT("");
		strModelTag.resize(iNumModelTagLength);
		ifs.read(reinterpret_cast<_char*>(&strModelTag[0]), sizeof(wchar_t) * iNumModelTagLength);

		_int iInteractType = 0;
		ifs.read(reinterpret_cast<_char*>(&iInteractType), sizeof(_int));

		switch ((INTERACTIVE_OBJECT_TYPE)iInteractType)
		{
		case Client::OBJECT_TABLE:
		{
			_bool IsCreatePlate = false;
			ifs.read(reinterpret_cast<_char*>(&IsCreatePlate), sizeof(_bool));

			CTable::TABLE_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.IsCreatePlate = IsCreatePlate;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		case Client::OBJECT_INGREDIENTCRATE:
		{
			_int iIngredientType = 0;
			ifs.read(reinterpret_cast<_char*>(&iIngredientType), sizeof(_int));

			CIngredient_Crate::INGREDIENTCRATE_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.eIngredientType = (INGREDIENT_TYPE)iIngredientType;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		case Client::OBJECT_STOVE:
		{
			_int iCookerType = 0;
			ifs.read(reinterpret_cast<_char*>(&iCookerType), sizeof(_int));

			CStove::STOVE_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.eInitCookerType = (COOKER_TYPE)iCookerType;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		case Client::OBJECT_FRYER:
			break;
		case Client::OBJECT_SINK:
		{
			CModelObject::MODELOBJECT_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;

			m_IsSinkExist = true;
		}
		break;
		case Client::OBJECT_PLATE_RETURN:
		{
			CModelObject::MODELOBJECT_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;

			if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pPlateReturnObj, LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		case Client::OBJECT_PASS:
		{
			CModelObject::MODELOBJECT_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;

			if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pPassObj, LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		case Client::OBJECT_DISPENSER:
		{
			_uint iNumIngredient = 0;
			ifs.read(reinterpret_cast<_char*>(&iNumIngredient), sizeof(_uint));

			INGREDIENT_TYPE eIngredient[2];

			for (_uint i = 0; i < iNumIngredient; i++)
			{
				ifs.read(reinterpret_cast<_char*>(&eIngredient[i]), sizeof(_int));
			}

			CDispenser::DISPENSER_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.iNumCreateIngredient = iNumIngredient;
			pDesc.eIngredientType[0] = eIngredient[0];
			pDesc.eIngredientType[1] = eIngredient[1];


			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		default:
		{
			CModelObject::MODELOBJECT_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		}

	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Layer_Collision_Data(ifstream& ifs)
{
	_uint iNumLayerNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLayerNameLength), sizeof(_uint));
	wstring strLayerName = TEXT("");
	strLayerName.resize(iNumLayerNameLength);
	ifs.read(reinterpret_cast<_char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	_uint iListSize = 0;
	ifs.read(reinterpret_cast<_char*>(&iListSize), sizeof(_uint));

	for (_uint i = 0; i < iListSize; i++)
	{
		// 오브젝트 이름 읽기
		_uint iNumObjectNameLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumObjectNameLength), sizeof(_uint));

		wstring strObjectName = TEXT("");
		strObjectName.resize(iNumObjectNameLength);
		ifs.read(reinterpret_cast<_char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

		// 오브젝트 월드 행렬 읽기
		_float4x4 worldFloat4x4 = {};
		ifs.read(reinterpret_cast<_char*>(&worldFloat4x4), sizeof(_float4x4));

		CGameObject::GAMEOBJECT_DESC pDesc = {};
		pDesc.bInitWorldMatrix = true;
		pDesc.InitWorldFloat4x4 = worldFloat4x4;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Layer_Chef_SponePos(ifstream& ifs)
{
	_uint iNumLayerNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLayerNameLength), sizeof(_uint));
	wstring strLayerName = TEXT("");
	strLayerName.resize(iNumLayerNameLength);
	ifs.read(reinterpret_cast<_char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	_uint iListSize = 0;
	ifs.read(reinterpret_cast<_char*>(&iListSize), sizeof(_uint));

	for (_uint i = 0; i < iListSize; i++)
	{
		// 오브젝트 이름 읽기
		_uint iNumObjectNameLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumObjectNameLength), sizeof(_uint));

		wstring strObjectName = TEXT("");
		strObjectName.resize(iNumObjectNameLength);
		ifs.read(reinterpret_cast<_char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

		// 오브젝트 월드 행렬 읽기
		_float4x4 worldFloat4x4 = {};
		ifs.read(reinterpret_cast<_char*>(&worldFloat4x4), sizeof(_float4x4));

		_float4 vChefSponePos;
		memcpy(&vChefSponePos, &worldFloat4x4.m[3], sizeof(_float4));

		m_ChefSpones.push_back(vChefSponePos);
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Layer_Gimmic_Data(ifstream& ifs)
{
	_uint iNumLayerNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNumLayerNameLength), sizeof(_uint));
	wstring strLayerName = TEXT("");
	strLayerName.resize(iNumLayerNameLength);
	ifs.read(reinterpret_cast<_char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	_uint iListSize = 0;
	ifs.read(reinterpret_cast<_char*>(&iListSize), sizeof(_uint));

	for (_uint i = 0; i < iListSize; i++)
	{
		// 오브젝트 이름 읽기
		_uint iNumObjectNameLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumObjectNameLength), sizeof(_uint));

		wstring strObjectName = TEXT("");
		strObjectName.resize(iNumObjectNameLength);
		ifs.read(reinterpret_cast<_char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

		// 오브젝트 월드 행렬 읽기
		_float4x4 worldFloat4x4 = {};
		ifs.read(reinterpret_cast<_char*>(&worldFloat4x4), sizeof(_float4x4));

		_uint iNumModelTagLength = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumModelTagLength), sizeof(_uint));

		wstring strModelTag = TEXT("");
		strModelTag.resize(iNumModelTagLength);
		ifs.read(reinterpret_cast<_char*>(&strModelTag[0]), sizeof(wchar_t) * iNumModelTagLength);

		_float4 vStartPos, vGoalPos;
		ifs.read(reinterpret_cast<_char*>(&vStartPos), sizeof(_float4));
		ifs.read(reinterpret_cast<_char*>(&vGoalPos), sizeof(_float4));

		_bool IsCrossAutomobile = false;
		ifs.read(reinterpret_cast<_char*>(&IsCrossAutomobile), sizeof(_bool));

		CTraffic_Light::TRAFFIC_LIGHT_DESC pDesc = {};
		pDesc.bInitWorldMatrix = true;
		pDesc.InitWorldFloat4x4 = worldFloat4x4;
		pDesc.strModelTag = strModelTag;
		pDesc.vStartPos = vStartPos;
		pDesc.vGoalPos = vGoalPos;
		pDesc.IsCroosAutomobile = IsCrossAutomobile;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, strObjectName, &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(0.85f, 0.85f, 0.85f, 1.f);
	LightDesc.vAmbient = _float4(0.75f, 0.75f, 0.75f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;

	switch (g_iStageIdx)
	{
	case 0:
		CameraDesc.vEye = _float4(21.f, 8.f, 6.f, 1.f);
		CameraDesc.vAt = _float4(21.f, 0.f, 12.5f, 1.f);
		break;
	case 1:
		CameraDesc.vEye = _float4(44.f, 10.5f, 10.f, 1.f);
		CameraDesc.vAt = _float4(44.f, 0.f, 15.f, 1.f);
		break;
	case 2:
		CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		break;
	}

	
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObject()
{
	if (FAILED(Ready_Layer_Character(TEXT("Layer_Character"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UI()
{
	CUI_Order_Manager::UI_ORDER_MANAGER_DESC UI_Order_Manager_Desc = {};
	UI_Order_Manager_Desc.pOrders = &m_Orders;

	m_pUI_Order_Manager = CUI_Order_Manager::Create(&UI_Order_Manager_Desc);


	CUI_Score::UI_SCORE_DESC UI_Score_Desc = {};
	UI_Score_Desc.pScore = &m_iScore;
	UI_Score_Desc.pIsAddScore = &m_IsAddScore;
	UI_Score_Desc.pTipTimes = &m_iTipTimes;
	UI_Score_Desc.pIsTips = &m_IsTips;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Score, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Score"), &UI_Score_Desc)))
		return E_FAIL;

	CUI_Timer::UI_TIMER_DESC UI_Timer_Desc = {};
	UI_Timer_Desc.pRemain_Time = &m_fRemainTime;
	UI_Timer_Desc.pTotal_Time = &m_fTotalTime;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Timer, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Timer"), &UI_Timer_Desc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Notice, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Notice"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Character(const wstring& strLayerTag)
{
	_int iChefSponeCnt = (_int)m_ChefSpones.size();

	if (iChefSponeCnt == 0)
	{
		for (_uint i = 0; i < 2; i++)
		{
			CCharacter* pCharacter = nullptr;
			if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pCharacter, LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Chef"))))
				return E_FAIL;

			CTransform* pTransform = (CTransform*)pCharacter->Get_Component(g_strTransformTag);
			pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 3 + 2.5f, 0.f, rand() % 3 + 2.5f, 1.f));
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180));
			m_CharacterList.emplace_back(pCharacter);
		}
	}
	else
	{
		for (auto& SponePos : m_ChefSpones)
		{
			CCharacter* pCharacter = nullptr;
			if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pCharacter, LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Chef"))))
				return E_FAIL;

			CTransform* pTransform = (CTransform*)pCharacter->Get_Component(g_strTransformTag);
			pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&SponePos));
			pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180));


			m_CharacterList.emplace_back(pCharacter);
		}
	}

	m_CharacterList[0]->Set_Control(true);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring& strLayerTag)
{
	return S_OK;
}

void CLevel_GamePlay::Add_Order(_float fTimeDelta)
{
	if (m_iNumOrders <= 0)
	{
		CRecipe* pRecipe = m_pRecipe_Manager->Get_RandomRecipe();
		m_pUI_Order_Manager->Add_Order(pRecipe);
		m_iNumOrders++;
	}
	else if (m_iNumOrders < 5)
	{
		m_fAddOrderTime += fTimeDelta;
		if (m_fAddOrderTime >= OrderTime)
		{
			m_fAddOrderTime = 0.f;
			CRecipe* pRecipe = m_pRecipe_Manager->Get_RandomRecipe();
			m_pUI_Order_Manager->Add_Order(pRecipe);
			m_iNumOrders++;
		}
	}

}

#define Score_Per_Ingredient 20.f
#define Max_Tip_Times 3

void CLevel_GamePlay::Served_Plate()
{
	if (nullptr != m_pPassObj)
	{
		if (((CPass*)m_pPassObj)->IsReceived())
		{
			wstring strServingDIshName = m_pRecipe_Manager->Check_Recipe(((CPass*)m_pPassObj)->Get_Served_Dish());

			if (TEXT("") != strServingDIshName)
			{
				_int iOrderIdx = 0;
				for (auto order = m_Orders.begin(); order != m_Orders.end(); order++, iOrderIdx++)
				{
					if (strServingDIshName == (*order)->Get_RecipeName())
					{
						m_pGameInstance->StopSound(CSound_Manager::CH_PASS);
						m_pGameInstance->Play_Sound(L"Delivery_Success.wav", CSound_Manager::CH_PASS, 1.f);
						STAGE_SCORE_INFO* StageInfoDesc = m_pDataStorage->Get_PlayData(g_iStageIdx);

						StageInfoDesc->iNumPassedOrder++;

						_uint iOrderScore = (_uint)((*order)->Get_Recipes()->size() * Score_Per_Ingredient);
						StageInfoDesc->iOrderScore += iOrderScore;

						_uint iTip = (m_iTipTimes + 1) * TipScore;
						StageInfoDesc->iTipScore += iTip;

						m_iAddScore += iTip + iOrderScore;

						StageInfoDesc->iTotalScore += m_iAddScore;

						m_IsAddScore = true;

						if (iOrderIdx == 0)
						{
							if (m_IsTips && m_iTipTimes < Max_Tip_Times)
							{
								m_iTipTimes++;
							}
							m_IsTips = true;
						}
						else {
							m_iTipTimes = 0;
							m_IsTips = false;
						}

						Safe_Release(*order);
						m_Orders.erase(order);
						m_pUI_Order_Manager->Process_Order(iOrderIdx);
						m_pUI_Score->Coin_Spinning();
						m_iNumOrders--;
						break;
					}
				}
				if (iOrderIdx >= m_Orders.size())
				{
					m_iTipTimes = 0;
					m_IsTips = false;
				}
			}

			if (TEXT("") == strServingDIshName)
			{
				m_iTipTimes = 0;
				m_IsTips = false;
			}

			((CPass*)m_pPassObj)->Process_Order();
			((CPlate_Return*)m_pPlateReturnObj)->Return_Plate();
		}
	}
}

void CLevel_GamePlay::Add_Score()
{
	if (m_iAddScore > 0)
	{
		m_iAddScore--;
		if (m_iAddScore <= 0)
			m_IsAddScore = false;
		m_iScore++;
	}
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_GamePlay"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	Safe_Release(m_pPassObj);
	Safe_Release(m_pPlateReturnObj);

	for (auto& pCharacter : m_CharacterList)
		Safe_Release(pCharacter);
	m_CharacterList.clear();

	for (auto& pOrder : m_Orders)
		Safe_Release(pOrder);
	m_Orders.clear();

	Safe_Release(m_pDataStorage);
	Safe_Release(m_pRecipe_Manager);
	Safe_Release(m_pUI_Order_Manager);
	Safe_Release(m_pUI_Score);
	Safe_Release(m_pUI_Timer);
	Safe_Release(m_pUI_Notice);

	m_pGameInstance->Clear_Lights();
	m_pGameInstance->StopAll();
	
	__super::Free();
}
