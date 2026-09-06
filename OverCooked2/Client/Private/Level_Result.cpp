#include "stdafx.h"
#include "Level_Result.h"

#include "Level_Loading.h"

#include "Data_Storage.h"

#include "Camera_UI.h"
#include "Camera_Free.h"
#include "UI_Result_Empty_Star.h"
#include "UI_Common_BG.h"

CLevel_Result::CLevel_Result(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Result::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_NPC()))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;

	m_pDataStorage = (CData_Storage*)m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Data_Storage"));

	STAGE_SCORE_INFO* pScoreInfo = m_pDataStorage->Get_PlayData(g_iStageIdx);

	m_iNumAddStarIdx = 0;
	switch (g_iStageIdx)
	{
	case 0:
		if (pScoreInfo->iTotalScore >= 20.f)
			m_iNumAddStarIdx = 1;
		if (pScoreInfo->iTotalScore >= 60.f)
			m_iNumAddStarIdx = 2;
		if (pScoreInfo->iTotalScore > 240.f)
			m_iNumAddStarIdx = 3;

		break;
	case 1:
		if (pScoreInfo->iTotalScore >= 140.f)
			m_iNumAddStarIdx = 1;
		if (pScoreInfo->iTotalScore >= 180)
			m_iNumAddStarIdx = 2;
		if (pScoreInfo->iTotalScore > 240.f)
			m_iNumAddStarIdx = 3;
		break;
	}

	m_pGameInstance->Play_BGM(L"BGM_Result.wav");

	/*g_iStageIdx++;
	g_iStageIdx = g_iStageIdx % 2;*/
	return S_OK;
}

void CLevel_Result::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Calc_Score_Render_Time(fTimeDelta);
	Add_Star(fTimeDelta);

	if (m_pGameInstance->Get_DIKeyState(DIK_RETURN) == EKeyState::DOWN)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MAIN_MENU))))
			return;
	}
}

HRESULT CLevel_Result::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_ScoreFont();

	switch (g_iStageIdx)
	{
	case 0:
		m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
			TEXT("20 "), _float2(487.5f, 225.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
		m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
			TEXT("60 "), _float2(612.5f, 225.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
		m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
			TEXT("240 "), _float2(737.5f, 225.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);

		break;
	case 1:
		m_pGameInstance->Render_Font(TEXT("Font_Segoe32"),
			TEXT("140 "), _float2(300.f, 40.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
		m_pGameInstance->Render_Font(TEXT("Font_Segoe32"),
			TEXT("180 "), _float2(300.f, 40.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
		m_pGameInstance->Render_Font(TEXT("Font_Segoe32"),
			TEXT("240 "), _float2(300.f, 40.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
		break;
	}


	m_pGameInstance->Render_Font(TEXT("Font_Segoe32"),
		TEXT("STAGE ") + to_wstring(g_iStageIdx + 1),
		_float2(300.f, 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);


	return S_OK;
}


#define RenderTimeOrderNum 0.5f
#define RenderTimeOrderScore 0.75f
#define RenderTimeTip 2.f
#define RenderTimeTipScore 2.25f
#define RenderTimeTotalScore 3.5f

void CLevel_Result::Calc_Score_Render_Time(_float fTimeDelta)
{
	m_fRenderFontCounter += fTimeDelta;

	if (m_fRenderFontCounter >= RenderTimeOrderNum)
	{
		if (!m_IsRenderPassOrderNum)
		{
			m_pGameInstance->StopSound(CSound_Manager::CH_RESULT_SCORE);
			m_pGameInstance->Play_Sound(L"Results_Description_01.wav", CSound_Manager::CH_RESULT_SCORE, 1.f);
		}
		m_IsRenderPassOrderNum = true;


		if (m_fRenderFontCounter >= RenderTimeOrderScore)
		{
			if (!m_IsRenderPassOrderScore)
			{
				m_pGameInstance->StopSound(CSound_Manager::CH_RESULT_SCORE);
				m_pGameInstance->Play_Sound(L"Results_DescriptionScore.wav", CSound_Manager::CH_RESULT_SCORE, 1.f);
			}
			m_IsRenderPassOrderScore = true;
		}
	}

	if (m_fRenderFontCounter >= RenderTimeTip)
	{
		if (!m_IsRenderTip)
		{
			m_pGameInstance->StopSound(CSound_Manager::CH_RESULT_SCORE);
			m_pGameInstance->Play_Sound(L"Results_Description_02.wav", CSound_Manager::CH_RESULT_SCORE, 1.f);
		}
		m_IsRenderTip = true;

		if (m_fRenderFontCounter >= RenderTimeTipScore)
		{
			if (!m_IsRenderTipScore)
			{
				m_pGameInstance->StopSound(CSound_Manager::CH_RESULT_SCORE);
				m_pGameInstance->Play_Sound(L"Results_DescriptionScore.wav", CSound_Manager::CH_RESULT_SCORE, 1.f);
			}
			m_IsRenderTipScore = true;
		}
	}

	if (m_fRenderFontCounter >= RenderTimeTotalScore)
	{
		if (!m_IsRenderTotal)
		{
			m_pGameInstance->StopSound(CSound_Manager::CH_RESULT_SCORE);
			m_pGameInstance->Play_Sound(L"Results_TotalScore.wav", CSound_Manager::CH_RESULT_SCORE, 1.f);
			m_pGameInstance->StopSound(CSound_Manager::BGM);
			m_pGameInstance->Play_BGM(L"BGM_Results.wav");
		}
		m_IsRenderTotal = true;
		m_IsRenderTotal_Score = true;

	}
}

void CLevel_Result::Add_Star(_float fTimeDelta)
{
	if (m_iAddStarIdx < m_iNumAddStarIdx)
	{
		m_fAddStar_Counter += fTimeDelta;
		if (m_fAddStar_Counter >= 0.75f)
		{
			m_fAddStar_Counter = 0.f;

			m_iAddStarIdx++;

			auto pUI_Star = m_pResultStars.begin();

			for (_uint i = 0; i < m_iAddStarIdx - 1; i++)
				pUI_Star++;

			(*pUI_Star)->Add_Star();

			m_pGameInstance->StopSound(CSound_Manager::CH_RESULT_STAR);
			switch (m_iAddStarIdx)
			{
			case 0:
				m_pGameInstance->Play_Sound(L"Results_Star_01.wav", CSound_Manager::CH_RESULT_STAR, 1.f);
				break;
			case 1:
				m_pGameInstance->Play_Sound(L"Results_Star_02.wav", CSound_Manager::CH_RESULT_STAR, 1.f);
				break;
			case 2:
				m_pGameInstance->Play_Sound(L"Results_Star_03.wav", CSound_Manager::CH_RESULT_STAR, 1.f);
				break;
			}

		}
	}
}

void CLevel_Result::Render_ScoreFont()
{
	m_pGameInstance->Render_Font(TEXT("Font_Segoe32"),
		TEXT("STAGE ") + to_wstring(g_iStageIdx + 1),
		_float2(300.f, 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);

	STAGE_SCORE_INFO* pScoreInfo = m_pDataStorage->Get_PlayData(g_iStageIdx);

	_float vRenderScorePosX1 = 825.f;
	_float vRenderScorePosX2 = 775.f;


	if (m_IsRenderPassOrderNum)
	{
		m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
			TEXT("Orders Delivered x") + to_wstring(pScoreInfo->iNumPassedOrder),
			_float2(425.f, 275.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);

		if (m_IsRenderPassOrderScore)
		{
			if (pScoreInfo->iOrderScore / 100.f >= 1)
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iOrderScore),
					_float2(775.f, 275.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
			else if (pScoreInfo->iOrderScore / 10.f >= 1)
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iOrderScore),
					_float2(800.f, 275.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
			else
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iOrderScore),
					_float2(825.f, 275.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
		}
	}
	if (m_IsRenderTip)
	{
		m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
			TEXT("Tips"),
			_float2(425.f, 325.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
		if (m_IsRenderTipScore)
		{
			if (pScoreInfo->iTipScore / 100.f >= 1)
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iTipScore),
					_float2(775.f, 325.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
			else if (pScoreInfo->iTipScore / 10.f >= 1)
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iTipScore),
					_float2(800.f, 325.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
			else
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iTipScore),
					_float2(825.f, 325.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
		}
	}
	if (m_IsRenderTotal)
	{
		m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
			TEXT("TOTAL:"),
			_float2(425.f, 390.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
		if (m_IsRenderTotal_Score)
		{
			if (pScoreInfo->iTotalScore / 100.f >= 1)
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iTotalScore),
					_float2(775.f, 390.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
			else if (pScoreInfo->iTotalScore / 10.f >= 1)
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iTotalScore),
					_float2(800.f, 390.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
			else
			{
				m_pGameInstance->Render_Font(TEXT("Font_Segoe24"),
					to_wstring(pScoreInfo->iTotalScore),
					_float2(825.f, 390.f), XMVectorSet(0.61f, 0.59f, 0.54f, 1.f), 0.f);
			}
		}
	}
}

HRESULT CLevel_Result::Ready_Layer_Camera()
{
	CCamera_UI::CAMERA_DESC		CameraDesc{};

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, 0.f, -0.5f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_RESULT,
		TEXT("Layer_Camera"), TEXT("Prototype_GameObject_Camera_UI"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Result::Ready_Layer_NPC()
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_RESULT,
		TEXT("Layer_NPC"), TEXT("Prototype_GameObject_OnionKing"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Result::Ready_Layer_UI()
{
	CUI_Common_BG::COMMON_BG_DESC BGDesc = {};
	BGDesc.iLevelIdx = LEVEL_RESULT;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_RESULT,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_BG"), &BGDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_RESULT,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_Title"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_RESULT,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_Strip"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_RESULT,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Bill"))))
		return E_FAIL;

	for (_int i = 0; i < 3; i++)
	{
		CUI_Result_Empty_Star* pUI_ResultStar = nullptr;

		CUI::UI_DESC UIDesc = {};
		UIDesc.vSize = _float2(100.f, 100.f);
		UIDesc.vPos = _float2(g_iWinSizeX * 0.5f + (125.f * (i - 1)), g_iWinSizeY * 0.275f);

		if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pUI_ResultStar, LEVEL_RESULT,
			TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Empty_Star"), &UIDesc)))
			return E_FAIL;

		m_pResultStars.emplace_back(pUI_ResultStar);
	}

	return S_OK;
}

CLevel_Result* CLevel_Result::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Result* pInstance = new CLevel_Result(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Result"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Result::Free()
{
	Safe_Release(m_pDataStorage);

	for (auto UI_Star : m_pResultStars)
		Safe_Release(UI_Star);
	m_pResultStars.clear();
	m_pGameInstance->StopAll();

	__super::Free();
}
