#include "stdafx.h"
#include "Level_StageLoad.h"

#include"Level_Loading.h"
#include "GameInstance.h"
#include "UI_Transition.h"
#include "UI_Common_BG.h"

CLevel_StageLoad::CLevel_StageLoad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_StageLoad::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    m_pUI_Transition->Set_EnterLevel();

    //m_pGameInstance->Play_BGM(L"LevelReady.wav");

    return S_OK;
}

void CLevel_StageLoad::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);


    fLoadingPercent += fTimeDelta;

    if (m_pUI_Transition->Enter_Finish())
    {
        if (fLoadingPercent >= 3.f)
        {
            m_pUI_Transition->Set_ExitLevel();
        }
    }
    if (m_pUI_Transition->Exit_Finish())
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
            return;
    }
}

HRESULT CLevel_StageLoad::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (nullptr != m_pUI_Transition)
    {
        m_pGameInstance->Render_Font(TEXT("Font_Segoe32"),
            TEXT("STAGE LOADING "),
            _float2(200.f, 40.f), XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f);
    }
   
    return S_OK;
}

HRESULT CLevel_StageLoad::Ready_Layer_UI()
{
    CUI_Common_BG::COMMON_BG_DESC BGDesc = {};
    BGDesc.iLevelIdx = LEVEL_STAGE_LOAD;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE_LOAD,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_BG"), &BGDesc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE_LOAD,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_Title"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE_LOAD,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_Strip"))))
        return E_FAIL;

   if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE_LOAD,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_Level_Board"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE_LOAD,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_Preview"))))
        return E_FAIL;
        
    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_STAGE_LOAD,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_LoadingBar"))))
        return E_FAIL;

    CUI_Transition::TRANSITION_DESC pTransitionDesc = {};
    pTransitionDesc.iLevelIdx = (_uint)LEVEL_STAGE_LOAD;

    if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Transition, LEVEL_STAGE_LOAD,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Transition"), &pTransitionDesc)))
        return E_FAIL;

    return S_OK;
}

CLevel_StageLoad* CLevel_StageLoad::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_StageLoad* pInstance = new CLevel_StageLoad(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Created : CLevel_StageLoad"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_StageLoad::Free()
{
    __super::Free();

    Safe_Release(m_pUI_Transition);
}
