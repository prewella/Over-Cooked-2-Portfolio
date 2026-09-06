#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 1. 레벨에 필요한 객체들을 생성한다. */
/* 2. 레벨을 반복적으로 갱신하여 화면에 보여준다. */

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
#define GamePlayTime	150.f
#define OrderTime		5.f
#define TipScore		8

public:
	typedef struct Gameplay_Desc {
		list<class CCharacter*>* CharacterList = { nullptr };
	}GAMEPLAY_DESC;

private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT	Load_Data(wstring& strFilePath);
	HRESULT	Load_Layer_Data(ifstream& ifs);
	HRESULT	Load_Layer_Interact_Data(ifstream& ifs);
	HRESULT	Load_Layer_Collision_Data(ifstream& ifs);
	HRESULT	Load_Layer_Chef_SponePos(ifstream& ifs);
	HRESULT	Load_Layer_Gimmic_Data(ifstream& ifs);

	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_LandObject();
	HRESULT Ready_UI();

	HRESULT Ready_Layer_Character(const wstring& strLayerTag);

	HRESULT Ready_Layer_Effect(const wstring& strLayerTag);

	void Add_Order(_float fTimeDelta);
	void Served_Plate();
	void Add_Score();

private:
	class CData_Storage* m_pDataStorage = { nullptr };

	_bool m_IsSinkExist = { false };
	_bool m_IsPlayBgm = { false };

	class CInteractObj* m_pPassObj = { nullptr };
	class CInteractObj* m_pPlateReturnObj = { nullptr };

	vector<_float4>				m_ChefSpones;
	vector<class CCharacter*>	m_CharacterList;
	_uint						m_iSelectCharacterIdx = { 0 };

	/* With UI */
	/* For Order */
	_uint						m_iNumOrders = { 0 };
	_float						m_fAddOrderTime = { 0.f };
	list<class CRecipe*>		m_Orders;

	class CRecipe_Manager*		m_pRecipe_Manager = { nullptr };
	class CUI_Order_Manager*	m_pUI_Order_Manager = { nullptr };

	/* For Score */
	_uint						m_iScore = { 0 };
	_uint						m_iAddScore = { 0 };
	_bool						m_IsAddScore = { false };
	_bool						m_IsTips = { false };
	_uint						m_iTipTimes = { 0 };

	class CUI_Score*			m_pUI_Score = { nullptr };

	/* For Timer */
	_float						m_fRemainTime = { 0.f };
	_float						m_fTotalTime = { 0.f };
	class CUI_Timer*			m_pUI_Timer = { nullptr };

	/* For Notice */
	class CUI_Notice*			m_pUI_Notice = { nullptr };

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END