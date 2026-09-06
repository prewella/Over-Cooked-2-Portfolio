#include "stdafx.h"
#include "Level_MainMenu.h"

#include"Level_Loading.h"
#include "GameInstance.h"

#include "UI_Common_BG.h"
#include "UI_Transition.h"
#include "UI_Stage_Select_Header.h"
#include "UI_Stage_Select_Manager.h"

CLevel_MainMenu::CLevel_MainMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_MainMenu::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI()))
        return E_FAIL;

    m_pGameInstance->Play_BGM(L"BGM_Lobby.wav");

    return S_OK;
}

void CLevel_MainMenu::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_pUI_Stage_Select_Header->Is_Stage_Select_Open())
    {
        if (m_pGameInstance->Get_DIKeyState(DIK_ESCAPE) == EKeyState::DOWN)
        {
            m_pUI_Stage_Select_Header->Close_Stage_Select();
            m_pUI_Stage_Select_Manager->Close_UI();
        }
        m_pUI_Stage_Select_Manager->Open_UI();

        if (m_pUI_Stage_Select_Manager->IsSelected_Stage() )
        {
            if (!m_IsSelected_Stage)
                m_pUI_Transition->Set_ExitLevel();
            m_IsSelected_Stage = true;
        }
    }

    if (m_pUI_Transition->Exit_Finish())
    {
        if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE_LOAD))))
            return;
    }
}

HRESULT CLevel_MainMenu::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetWindowText(g_hWnd, TEXT("메인메뉴입니다."));

    return S_OK;
}

HRESULT CLevel_MainMenu::Load_Data(const wstring& strLayerTag)
{
    return E_NOTIMPL;
}

HRESULT CLevel_MainMenu::Ready_Layer_UI()
{
    CUI_Common_BG::COMMON_BG_DESC BGDesc = {};
    BGDesc.iLevelIdx = LEVEL_MAIN_MENU;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MAIN_MENU,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Common_BG"), &BGDesc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MAIN_MENU,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Main_Logo"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Stage_Select_Header, LEVEL_MAIN_MENU,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_Select_Header"))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Stage_Select_Manager, LEVEL_MAIN_MENU,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Stage_Select_Manager"))))
        return E_FAIL;

    CUI_Transition::TRANSITION_DESC pTransitionDesc = {};
    pTransitionDesc.iLevelIdx = (_uint)LEVEL_MAIN_MENU;

    if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Transition, LEVEL_MAIN_MENU,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Transition"), &pTransitionDesc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Clone(LEVEL_MAIN_MENU,
        TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Exit"))))
        return E_FAIL;

    return S_OK;
}

CLevel_MainMenu* CLevel_MainMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_MainMenu* pInstance = new CLevel_MainMenu(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Created : CLevel_MainMenu"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_MainMenu::Free()
{
    __super::Free();

    Safe_Release(m_pUI_Stage_Select_Header);
    Safe_Release(m_pUI_Stage_Select_Manager);
    Safe_Release(m_pUI_Transition);

    m_pGameInstance->StopAll();
}