#pragma once

#include "Client_Defines.h"

#include "Tool.h"

BEGIN(Client)

class CEditor final : public CTool
{
private:
	CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEditor() = default;

public:
	HRESULT							Initialize();
	virtual void					Tick(_float fTimeDelta)		override;
	virtual HRESULT					Render()					override;

private:
	void							BluePrintObj_Follow_Cursor();
	void							Create_Object();
	void							Picking_Models();

	void							Open_CreateObj_Tab();
	void							Manage_BluePrintObj();
	void							Setting_SelectObj();


	/* For File Explorer*/
	HRESULT							Open_File_To_Explorer();
	HRESULT							Load_Data(wstring& strFilePath);
	HRESULT							Load_Layer_Data(ifstream& ifs);
	HRESULT							Load_Layer_FixModel_Data(ifstream& ifs);
	HRESULT							Load_Layer_Interact_Data(ifstream& ifs);
	HRESULT							Load_Layer_Collision_Data(ifstream& ifs);
	HRESULT							Load_Layer_Gimmic_Data(ifstream& ifs);

	HRESULT							Create_File_To_Explorer();
	HRESULT							Save_Data(wstring& strFilePath);
	HRESULT							Save_Layer_Data(ofstream& ofs, wstring strLayerName);
	HRESULT							Save_Layer_FixModel_Data(ofstream& ofs, wstring strLayerName);
	HRESULT							Save_Layer_Interact_Data(ofstream& ofs, wstring strLayerName);
	HRESULT							Save_Layer_Collision_Data(ofstream& ofs, wstring strLayerName);
	HRESULT							Save_Layer_Gimmic_Data(ofstream& ofs, wstring strLayerName);

	CGameObject*					Find_FloorObject(wstring strLayerTag, _int _iFindIdx);

private:
	_bool							m_bCallNewFrame = { false };

	_bool							m_bShowIMGUI = { false };
	_bool							m_bActiveMouse = { false };

	_int							m_iSelectTab = { 0 };
	_int							m_iSettingTab = { 0 };
	_int							m_iSelectListItem = { 0 };


	wstring							m_strLayer = { TEXT("") };
	wstring							m_strModelTag = { TEXT("") };
	wstring							m_strProtoTag = { TEXT("") };

	_float							m_fBluePrintYPos = 0.f;

	class CBluePrint*				m_pBluePrintObj = { nullptr };
	CGameObject*					m_pSelectObj = { nullptr };


public:
	static CEditor*					Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void					Free();
};

END