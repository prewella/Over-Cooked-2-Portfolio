#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 다음레벨에 대한 자원을 로드한다. */

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CLoader final : public CBase
{
#define Scaling 0.00835f
#define ScalingMilliion 0.01f

private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Start();
	_bool isFinished() const {
		return m_isFinished;
	}
	void Output() {
		SetWindowText(g_hWnd, m_strLoadingText.c_str());
	}

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	HANDLE						m_hThread;
	CRITICAL_SECTION			m_Critical_Section;
	LEVEL						m_eNextLevelID = { LEVEL_END };
	wstring						m_strLoadingText;
	_bool						m_isFinished = { false };

	CGameInstance* m_pGameInstance = { nullptr };

private:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_MainMenu();
	HRESULT Loading_For_Stage_Load();
	HRESULT Loading_For_GamePlay();
	HRESULT Loading_For_Result();
	HRESULT Loading_For_Editor();

	HRESULT Create_UI_Common_ProtoTypes();

	HRESULT Create_UI_MainMenu_ProtoTypes();

	HRESULT Create_UI_StageLoad_ProtoTypes();


	HRESULT Create_Instance_VIBuffer_ProtoTypes();

	HRESULT Create_GamePlay_ProtoTypes();
	HRESULT Create_Chef_ProtoTypes();
	HRESULT Create_Gimmic_ProtoTypes();
	HRESULT Create_InteractObj_ProtoTypes();
	HRESULT Create_Belonging_ProtoTypes();
	HRESULT Create_Instance_ProtoTypes();
	HRESULT Create_UI_Gameplay_ProtoTypes();

	HRESULT Create_Result_ProtoTypes();
	HRESULT Create_UI_Result_ProtoTypes();

	HRESULT Load_Shaders();
	HRESULT Load_Collider();

	HRESULT Load_Models();
	
	HRESULT Load_Model_Design();
	HRESULT Load_Model_Tile();
	HRESULT Load_Model_Prop();
	HRESULT Load_Model_Gimmic();

	HRESULT Load_Model_Character();

	HRESULT Load_Model_Belonging();
	HRESULT Load_Model_Cooker();
	HRESULT Load_Model_Ingredient();
	HRESULT Load_Model_Plate();
	HRESULT Load_Model_Plating_Ingredient();

	HRESULT Load_Model_Single_Ingredient();
	HRESULT Load_Model_Sushi();
	HRESULT Load_Model_Pasta();

	HRESULT Load_Model_InteractObj();

	HRESULT Load_Model_NPC();


	HRESULT Load_Texture_GamePlay_Instance();

	HRESULT Load_Texture_Common_UI();
	HRESULT Load_Texture_Mainmenu_UI();
	HRESULT Load_Texture_Loading_UI();
	HRESULT Load_Texture_GamePlay_UI();
	HRESULT Load_Texture_Result_UI();


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END