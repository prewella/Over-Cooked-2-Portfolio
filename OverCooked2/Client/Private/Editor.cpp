#include "stdafx.h"

#include <commdlg.h> 
#include <ShObjIdl.h>

#include "Editor.h"
#include "GameInstance.h"

#include "BluePrint.h"

#include "Ingredient_Crate.h"
#include "Stove.h"
#include "Traffic_Light.h"
#include "Table.h"
#include "Dispenser.h"


CEditor::CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CTool{ pDevice, pContext }
{
}

HRESULT CEditor::Initialize()
{
	__super::Initialize(g_hWnd);

	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

void CEditor::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_bCallNewFrame = false;
	__super::Tick(fTimeDelta);
	m_bCallNewFrame = true;

	static _int iSettingTransformSelct = { 0 };	/* 위치, 회전, 크기 인덱스 :: Transform용 */
	static _int iSelectTileSetting = { 0 };		/* 라디오 버튼 인덱스 :: 타일용 */

	if (true == ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) ||
		true == ImGui::IsAnyItemHovered() ||
		true == ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
		m_bActiveMouse = false;
	else
		m_bActiveMouse = true;

	if (m_pGameInstance->Get_DIKeyState(DIK_TAB) == EKeyState::DOWN)
	{
		m_bShowIMGUI = !m_bShowIMGUI;
	}

	if (true == m_bShowIMGUI)
	{
		ImGui::Begin("Tool");
		// 최상위 탭

		Open_CreateObj_Tab();

		if (ImGui::CollapsingHeader("Save & Load Data"))
		{
			if (ImGui::Button("Load Data")) {
				if (FAILED(Open_File_To_Explorer()))
				{
					MSG_BOX(TEXT("로드 실패"));
				}
			}
			if (ImGui::Button("Save Data")) {
				if (FAILED(Create_File_To_Explorer()))
				{
					MSG_BOX(TEXT("저장 실패"));
				}
			}
		}

		ImGui::End();
	}

	Manage_BluePrintObj();

	

	if (nullptr != m_pSelectObj)
	{
		CTransform* pTransform = (CTransform*)m_pSelectObj->Get_Component(g_strTransformTag);
		_float4x4 worldMatrix = pTransform->Get_WorldFloat4x4();

		EditGuizmo(pTransform->Get_WorldFloat4x4_Ref());

		if (m_pGameInstance->Get_DIKeyState(DIK_DELETE) == EKeyState::DOWN)
		{
			m_pSelectObj->Set_Erase();
			Safe_Release(m_pSelectObj);
			m_pSelectObj = nullptr;
		}
	}

	Setting_SelectObj();

#endif
}

HRESULT CEditor::Render()
{
	if (m_bCallNewFrame)
	{
		__super::Render();
	}

	return S_OK;
}

void CEditor::BluePrintObj_Follow_Cursor()
{
	if (nullptr != m_pBluePrintObj)
	{
		const CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<const CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_EDITOR, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
		if (nullptr == pTerrainBuffer)
			return;

		const CTransform* pTerrainTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_EDITOR, TEXT("Layer_Terrain"), TEXT("Com_Transform")));
		if (nullptr == pTerrainTransform)
			return;

		_float fDist;
		CTransform* pTransform = (CTransform*)m_pBluePrintObj->Get_Component(g_strTransformTag);
		_float4 fPos = pTerrainBuffer->Compute_Picking(pTerrainTransform, fDist);
		if (fPos.x - (_float)((_int)fPos.x) < 0.25f)
			fPos.x = (_float)((_int)fPos.x);
		else if (fPos.x - (_float)((_int)fPos.x) < 0.5f)
			fPos.x = (_float)((_int)fPos.x) + 0.25f;
		else if (fPos.x - (_float)((_int)fPos.x) < 0.75f)
			fPos.x = (_float)((_int)fPos.x) + 0.5f;
		else
			fPos.x = (_float)((_int)fPos.x) + 0.75f;

		if (fPos.z - (_float)((_int)fPos.z) < 0.25f)
			fPos.z = (_float)((_int)fPos.z);
		else if (fPos.z - (_float)((_int)fPos.z) < 0.5f)
			fPos.z = (_float)((_int)fPos.z) + 0.25f;
		else if (fPos.z - (_float)((_int)fPos.z) < 0.75f)
			fPos.z = (_float)((_int)fPos.z) + 0.5f;
		else
			fPos.z = (_float)((_int)fPos.z) + 0.75f;

		pTransform->Set_State(CTransform::STATE_POSITION, fPos);
	}
}

void CEditor::Create_Object()
{
	CTransform* pTransform = (CTransform*)m_pBluePrintObj->Get_Component(g_strTransformTag);

	CModelObject::MODELOBJECT_DESC pDesc = {};
	pDesc.strModelTag = m_strModelTag;
	pDesc.bInitWorldMatrix = true;
	pDesc.InitWorldFloat4x4 = pTransform->Get_WorldFloat4x4();

	m_pGameInstance->Add_Clone(LEVEL_EDITOR, m_strLayer, m_strProtoTag, pTransform->Get_State_Float4(CTransform::STATE_POSITION), &pDesc);
}

void CEditor::Picking_Models()
{
	_float fMinDist = 100.f;
	_float fTempDist = 0.f;

	CGameObject* pTempObj = nullptr;

	list<CGameObject*>* pTileList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Tiles"));
	if (nullptr != pTileList)
	{
		for (auto& Tile : *pTileList)
		{
			CTransform* pTransform = (CTransform*)Tile->Get_Component(g_strTransformTag);
			CModel* pModel = ((CModelObject*)Tile)->Get_ModelCom();
			pModel->Check_Picking(pTransform, fTempDist);

			if (0 < fTempDist && fMinDist > fTempDist)
				pTempObj = Tile;
		}
	}

	list<CGameObject*>* pPropList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Props"));
	if (nullptr != pPropList)
	{
		for (auto& Prop : *pPropList)
		{
			CTransform* pTransform = (CTransform*)Prop->Get_Component(g_strTransformTag);
			CModel* pModel = ((CModelObject*)Prop)->Get_ModelCom();
			pModel->Check_Picking(pTransform, fTempDist);

			if (0 < fTempDist && fMinDist > fTempDist)
				pTempObj = Prop;
		}
	}

	list<CGameObject*>* pInteractList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Interacts"));
	if (nullptr != pInteractList)
	{
		for (auto& Interact : *pInteractList)
		{
			CTransform* pTransform = (CTransform*)Interact->Get_Component(g_strTransformTag);
			CModel* pModel = ((CModelObject*)Interact)->Get_ModelCom();
			pModel->Check_Picking(pTransform, fTempDist);

			if (0 < fTempDist && fMinDist > fTempDist)
				pTempObj = Interact;
		}
	}

	list<CGameObject*>* pCharacterList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Characters"));
	if (nullptr != pCharacterList)
	{
		for (auto& Character : *pCharacterList)
		{
			CTransform* pTransform = (CTransform*)Character->Get_Component(g_strTransformTag);
			CModel* pModel = ((CModelObject*)Character)->Get_ModelCom();
			pModel->Check_Picking(pTransform, fTempDist);

			if (0 < fTempDist && fMinDist > fTempDist)
				pTempObj = Character;
		}
	}

	list<CGameObject*>* pSponeList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Spone"));
	if (nullptr != pSponeList)
	{
		for (auto& Spone : *pSponeList)
		{
			CTransform* pTransform = (CTransform*)Spone->Get_Component(g_strTransformTag);
			CModel* pModel = ((CModelObject*)Spone)->Get_ModelCom();
			pModel->Check_Picking(pTransform, fTempDist);

			if (0 < fTempDist && fMinDist > fTempDist)
				pTempObj = Spone;
		}
	}

	list<CGameObject*>* pGimmicList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Gimmics"));
	if (nullptr != pGimmicList)
	{
		for (auto& Gimmic : *pGimmicList)
		{
			CTransform* pTransform = (CTransform*)Gimmic->Get_Component(g_strTransformTag);
			CModel* pModel = ((CModelObject*)Gimmic)->Get_ModelCom();
			pModel->Check_Picking(pTransform, fTempDist);

			if (0 < fTempDist && fMinDist > fTempDist)
				pTempObj = Gimmic;
		}
	}


	// 피킹된 오브젝트가 있었다면
	if (nullptr != pTempObj)
	{
		if (nullptr == m_pSelectObj)
		{
			m_pSelectObj = pTempObj;
			Safe_AddRef(m_pSelectObj);
		}
		else if (nullptr != m_pSelectObj)
		{// 같은 객체일 경우 변동 X
			if (pTempObj == m_pSelectObj) {}
			else
			{
				Safe_Release(m_pSelectObj);
				m_pSelectObj = pTempObj;
				Safe_AddRef(m_pSelectObj);
			}
		}
	}
}

#ifdef _DEBUG
void CEditor::Open_CreateObj_Tab()
{
	if (ImGui::CollapsingHeader("Object List"))
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

		if (ImGui::BeginTabBar("Object_Tab", tab_bar_flags))
		{
			// 바닥 설치용 타일 메쉬들 모음 탭
			if (ImGui::BeginTabItem("Tiles"))
			{
				m_strLayer = TEXT("Layer_Tiles");

				static const char* szTiles[] = {
					"Black", "Checked", "Red_Carpet", "Road", "Road_Cross", "Road_Side", "Road_Stop",
					"Side_Step", "SideWalk", "Vent", "Road_Side_Corner", "Road_Centre", "Road_YellowBox"

				};

				if (ImGui::BeginCombo("Tile Combo", szTiles[m_iSelectTab]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(szTiles); n++)
					{
						if (ImGui::Selectable(szTiles[n]))
						{
							m_iSelectTab = n;
							m_strProtoTag = TEXT("Prototype_GameObject_Tile");

							switch (m_iSelectTab)
							{
							case 0:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Black");
								break;
							case 1:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Checked");
								break;
							case 2:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Red_Carpet");
								break;
							case 3:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Road");
								break;
							case 4:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Road_Cross");
								break;
							case 5:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Road_Side");
								break;
							case 6:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Road_Stop");
								break;
							case 7:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Side_Step");
								break;
							case 8:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_SideWalk");
								break;
							case 9:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Vent");
								break;
							case 10:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Road_Side_Corner");
								break;
							case 11:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Road_Centre");
								break;
							case 12:
								m_strModelTag = TEXT("Prototype_Component_Model_Tile_Road_YellowBox");
								break;
							}

							if (nullptr == m_pBluePrintObj)
							{
								CBluePrint::MODELOBJECT_DESC model_Desc = {};
								model_Desc.strModelTag = m_strModelTag;

								m_pGameInstance->Add_Clone((CGameObject**)&m_pBluePrintObj, LEVEL_EDITOR, TEXT("Layer_BluePrint"), TEXT("Prototype_GameObject_BluePrint"), &model_Desc);
							}
							else
							{
								m_pBluePrintObj->Set_Model(m_strModelTag);
							}
							m_pBluePrintObj->Set_Render(true);
						}
					}
					ImGui::EndCombo();
				}
				//ImGui::Checkbox(u8"Test Box", &IsWallSetting);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Props"))
			{
				m_strLayer = TEXT("Layer_Props");

				static const char* szProps[] = {
					"Bamboo", "Car", "Customer_Chair_Black", "Customer_Table_Black", "Grass", "Hydrant", "Market_Stall", "Market_Stall_Blank",
					"Orning_Corner_Long", "Orning_Corner_Short", "Orning_Straight_Long", "Orning_Straight_Short", "Plant_Table",
					"Restaurant_Chair", "Restaurant_Table", "Tree_CherryBlossom", "Wall_Corner", "Wall_End", "Wall_Inside", "Wall_Straight","Wall_Straight_Short", "Wall_Window", "Wall_Window_Small",
					"Traffic_Cone"
				
				};

				if (ImGui::BeginCombo("Prop Combo", szProps[m_iSelectTab]))
				{

					for (int n = 0; n < IM_ARRAYSIZE(szProps); n++)
					{
						if (ImGui::Selectable(szProps[n]))
						{
							m_iSelectTab = n;
							m_strProtoTag = TEXT("Prototype_GameObject_Prop");

							switch (m_iSelectTab)
							{
							case 0:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Bamboo");
								break;
							case 1:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Car");
								break;
							case 2:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Customer_Chair_Black");
								break;
							case 3:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Customer_Table_Black");
								break;
							case 4:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Grass");
								break;
							case 5:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Hydrant");
								break;
							case 6:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Market_Stall");
								break;
							case 7:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Market_Stall_Blank");
								break;
							case 8:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Orning_Corner_Long");
								break;
							case 9:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Orning_Corner_Short");
								break;
							case 10:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Orning_Straight_Long");
								break;
							case 11:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Orning_Straight_Short");
								break;
							case 12:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Plant_Table");
								break;
							case 13:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Restaurant_Chair");
								break;
							case 14:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Restaurant_Table");
								break;
							case 15:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Tree_CherryBlossom");
								break;
							case 16:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Wall_Corner");
								break;
							case 17:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Wall_End");
								break;
							case 18:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Wall_Inside");
								break;
							case 19:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Wall_Straight");
								break;
							case 20:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Wall_Straight_Short");
								break;
							case 21:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Wall_Window");
								break;
							case 22:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Wall_Window_Small");
								break;
							case 23:
								m_strModelTag = TEXT("Prototype_Component_Model_Prop_Traffic_Cone");
								break;
							}

							if (nullptr == m_pBluePrintObj)
							{
								CBluePrint::MODELOBJECT_DESC model_Desc = {};
								model_Desc.strModelTag = m_strModelTag;

								m_pGameInstance->Add_Clone((CGameObject**)&m_pBluePrintObj, LEVEL_EDITOR, TEXT("Layer_BluePrint"), TEXT("Prototype_GameObject_BluePrint"), &model_Desc);
							}
							else
							{
								m_pBluePrintObj->Set_Model(m_strModelTag);
							}
							m_pBluePrintObj->Set_Render(true);

						}
					}
					ImGui::EndCombo();
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Interacts"))
			{
				m_strLayer = TEXT("Layer_Interacts");

				const char* szInteracts[] = { "Table_Edge", "Table_NoEdge", "Table_Corner_Edge", "Table_Chop", "Ingredient_Crate",
				"Stove", "Sink" , "Waste_Bin" ,"Plate_Return", "Pass", "Conveyor", "Dispenser"
				};

				if (ImGui::BeginCombo("Interact Combo", szInteracts[m_iSelectTab]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(szInteracts); n++)
					{
						if (ImGui::Selectable(szInteracts[n]))
						{
							m_iSelectTab = n;

							switch (m_iSelectTab)
							{
							case 0:
								m_strModelTag = TEXT("Prototype_Component_Model_Table_Edge");
								m_strProtoTag = TEXT("Prototype_GameObject_Counter");
								break;
							case 1:
								m_strModelTag = TEXT("Prototype_Component_Model_Table_NoEdge");
								m_strProtoTag = TEXT("Prototype_GameObject_Counter");
								break;
							case 2:
								m_strModelTag = TEXT("Prototype_Component_Model_Table_Corner_Edge");
								m_strProtoTag = TEXT("Prototype_GameObject_Counter");
								break;
							case 3:
								m_strModelTag = TEXT("Prototype_Component_Model_Table_Chop");
								m_strProtoTag = TEXT("Prototype_GameObject_Table_Chop");
								break;
							case 4:
								m_strModelTag = TEXT("Prototype_Component_Model_Ingredient_Crate");
								m_strProtoTag = TEXT("Prototype_GameObject_Ingredient_Crate");
								break;
							case 5:
								m_strModelTag = TEXT("Prototype_Component_Model_Stove");
								m_strProtoTag = TEXT("Prototype_GameObject_GasStove");
								break;
							case 6:
								m_strModelTag = TEXT("Prototype_Component_Model_Sink");
								m_strProtoTag = TEXT("Prototype_GameObject_Sink");
								break;
							case 7:
								m_strModelTag = TEXT("Prototype_Component_Model_Waste_Bin");
								m_strProtoTag = TEXT("Prototype_GameObject_Waste_Bin");
								break;
							case 8:
								m_strModelTag = TEXT("Prototype_Component_Model_Plate_Return");
								m_strProtoTag = TEXT("Prototype_GameObject_Plate_Return");
								break;
							case 9:
								m_strModelTag = TEXT("Prototype_Component_Model_Pass");
								m_strProtoTag = TEXT("Prototype_GameObject_Pass");
								break;
							case 10:
								m_strModelTag = TEXT("Prototype_Component_Model_Conveyor");
								m_strProtoTag = TEXT("Prototype_GameObject_Conveyor");
								break;
							case 11:
								m_strModelTag = TEXT("Prototype_Component_Model_Dispenser");
								m_strProtoTag = TEXT("Prototype_GameObject_Dispenser");
								break;
							}

							if (nullptr == m_pBluePrintObj)
							{
								CBluePrint::MODELOBJECT_DESC model_Desc = {};
								model_Desc.strModelTag = m_strModelTag;

								m_pGameInstance->Add_Clone((CGameObject**)&m_pBluePrintObj, LEVEL_EDITOR, TEXT("Layer_BluePrint"), TEXT("Prototype_GameObject_BluePrint"), &model_Desc);
							}
							else
							{
								m_pBluePrintObj->Set_Model(m_strModelTag);
							}
							m_pBluePrintObj->Set_Render(true);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem("Gimmic"))
			{
				m_strLayer = TEXT("Layer_Gimmics");

				const char* szGimmics[] = { "Traffic_Light"
				};

				if (ImGui::BeginCombo("Gimmic Combo", szGimmics[m_iSelectTab]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(szGimmics); n++)
					{
						if (ImGui::Selectable(szGimmics[n]))
						{
							m_iSelectTab = n;

							switch (m_iSelectTab)
							{
							case 0:
								m_strModelTag = TEXT("Prototype_Component_Model_Traffic_Light");
								m_strProtoTag = TEXT("Prototype_GameObject_Traffic_Light");
								break;
							}

							if (nullptr == m_pBluePrintObj)
							{
								CBluePrint::MODELOBJECT_DESC model_Desc = {};
								model_Desc.strModelTag = m_strModelTag;

								m_pGameInstance->Add_Clone((CGameObject**)&m_pBluePrintObj, LEVEL_EDITOR, TEXT("Layer_BluePrint"), TEXT("Prototype_GameObject_BluePrint"), &model_Desc);
							}
							else
							{
								m_pBluePrintObj->Set_Model(m_strModelTag);
							}
							m_pBluePrintObj->Set_Render(true);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::EndTabItem();
			}
			
			ImGui::EndTabBar();
		}
	}

	if (ImGui::CollapsingHeader("EditorObj List"))
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

		if (ImGui::BeginTabBar("ColListBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("Collision"))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.75f, 0.75f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));

				_float3 vCurrentPos = { -1.f, -1.f, -1.f };

				if (ImGui::Button("Create", ImVec2(50.f, 30.f)))
				{
					m_pGameInstance->Add_Clone(LEVEL_EDITOR, TEXT("Layer_Collision"), TEXT("Prototype_GameObject_Empty_Collider"));

					if (nullptr != m_pSelectObj)
					{
						Safe_Release(m_pSelectObj);
						m_pSelectObj = nullptr;
					}
					list<CGameObject*>* pColLayerList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Collision"));

					if (nullptr != pColLayerList && pColLayerList->size() > 0)
					{
						m_pSelectObj = pColLayerList->back();
						Safe_AddRef(m_pSelectObj);
					}

				}
				ImGui::PopStyleColor(3);

				ImGui::PushItemWidth(300);

				if (ImGui::BeginListBox(""))
				{
					list<CGameObject*>* pCollisionList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Collision"));

					if (nullptr != pCollisionList)
					{
						_int iSize = (_int)pCollisionList->size();

						for (_int i = 0; i < iSize; i++)
						{
							string strName = "Collision";
							strName += std::to_string(i).c_str();

							if (ImGui::Selectable(strName.c_str(), m_iSelectListItem == i))
							{
								m_iSelectListItem = i;

								CGameObject* pObj = Find_FloorObject(TEXT("Layer_Collision"), m_iSelectListItem);

								if (nullptr != m_pSelectObj)
								{
									Safe_Release(m_pSelectObj);
									m_pSelectObj = nullptr;
									m_pSelectObj = pObj;
									Safe_AddRef(m_pSelectObj);
								}
								else
								{
									m_pSelectObj = pObj;
									Safe_AddRef(m_pSelectObj);
								}
							}
						}
					}
					ImGui::EndListBox();
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Spone"))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 0.5f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.75f, 0.75f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));

				_float3 vCurrentPos = { -1.f, -1.f, -1.f };

				if (ImGui::Button("Select", ImVec2(50.f, 30.f)))
				{
					m_strLayer = TEXT("Layer_Spone");
					m_strModelTag = TEXT("Prototype_Component_Model_Spone");
					m_strProtoTag = TEXT("Prototype_GameObject_ChefSpone");

					if (nullptr == m_pBluePrintObj)
					{
						CBluePrint::MODELOBJECT_DESC model_Desc = {};
						model_Desc.strModelTag = m_strModelTag;

						m_pGameInstance->Add_Clone((CGameObject**)&m_pBluePrintObj, LEVEL_EDITOR, TEXT("Layer_BluePrint"), TEXT("Prototype_GameObject_BluePrint"), &model_Desc);
					}
					else
					{
						m_pBluePrintObj->Set_Model(m_strModelTag);
					}
					m_pBluePrintObj->Set_Render(true);
				}
				ImGui::PopStyleColor(3);

				ImGui::PushItemWidth(300);

				if (ImGui::BeginListBox(""))
				{
					list<CGameObject*>* pCollisionList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, TEXT("Layer_Spone"));

					if (nullptr != pCollisionList)
					{
						_int iSize = (_int)pCollisionList->size();

						for (_int i = 0; i < iSize; i++)
						{
							string strName = "Spone";
							strName += std::to_string(i).c_str();

							if (ImGui::Selectable(strName.c_str(), m_iSelectListItem == i))
							{
								m_iSelectListItem = i;

								CGameObject* pObj = Find_FloorObject(TEXT("Layer_Spone"), m_iSelectListItem);

								if (nullptr != m_pSelectObj)
								{
									Safe_Release(m_pSelectObj);
									m_pSelectObj = nullptr;
									m_pSelectObj = pObj;
									Safe_AddRef(m_pSelectObj);
								}
								else
								{
									m_pSelectObj = pObj;
									Safe_AddRef(m_pSelectObj);
								}
							}
						}
					}
					ImGui::EndListBox();
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

}

void CEditor::Manage_BluePrintObj()
{
	if (nullptr != m_pBluePrintObj)
	{
		if (nullptr != m_pSelectObj)
		{
			Safe_Release(m_pSelectObj);
			m_pSelectObj = nullptr;
		}

		BluePrintObj_Follow_Cursor();

		_long	WheelMove = { 0 };

		if (WheelMove = m_pGameInstance->Get_DIMouseMove(MOUSEMOVESTATE::DIMMS_WHEEL))
		{
			if (WheelMove > 0)
			{
				m_fBluePrintYPos += 0.125f;
			}
			else if (WheelMove < 0 && m_fBluePrintYPos > 0)
			{
				m_fBluePrintYPos -= 0.125f;
			}
		}

		CTransform* pTransform = (CTransform*)m_pBluePrintObj->Get_Component(g_strTransformTag);
		_float4 fBluePrintPos = pTransform->Get_State_Float4(CTransform::STATE_POSITION);
		fBluePrintPos.y = m_fBluePrintYPos;
		pTransform->Set_State(CTransform::STATE_POSITION, fBluePrintPos);

		if (m_pGameInstance->Get_DIKeyState(DIK_Q) == EKeyState::DOWN)
		{
			pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-45), true);
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_E) == EKeyState::DOWN)
		{
			pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45), true);
		}

		if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIMKS_LBUTTON) == EKeyState::DOWN && m_bActiveMouse)
			Create_Object();

		if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIMKS_RBUTTON) == EKeyState::DOWN)
		{
			m_pBluePrintObj->Set_Erase(true);
			Safe_Release(m_pBluePrintObj);
			m_pBluePrintObj = nullptr;
			m_fBluePrintYPos = 0.f;
		}
	}
	else
	{
		if (m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIMKS_LBUTTON) == EKeyState::DOWN && m_bActiveMouse)
			Picking_Models();
	}
}

void CEditor::Setting_SelectObj()
{
	if (nullptr != m_pSelectObj)
	{
		if (nullptr != dynamic_cast<CTable*>(m_pSelectObj))
		{
			ImGui::Begin("Table_Setting");

			_bool IsCreatePlate = ((CTable*)m_pSelectObj)->IsCreatePlate();
			ImGui::Checkbox("Set_Create_Plate", &IsCreatePlate);
			((CTable*)m_pSelectObj)->Set_Create_Plate(IsCreatePlate);

			ImGui::End();
		}

		if (nullptr != dynamic_cast<CIngredient_Crate*>(m_pSelectObj))
		{
			ImGui::Begin("Ingredient_Setting");

			m_iSettingTab = ((CIngredient_Crate*)m_pSelectObj)->Get_IngredientType() - 1;

			//ImGui::Checkbox("Set_Ingredient", &IsCreatePlate);
			static const char* szIngredient[] = {
					"TOMATO", "ONION", "POTATO", "DOUGH", "SEAWEED",
					"MUSHROOM", "MEAT", "LETTUCE", "RICE", "FLOUR",
					"BUN", "FISH", "SAUSAGE", "UNKNOWN", "EGG",
					"CHICKEN", "TORTILLA", "CHEESE", "CARROT", "CHOCOLATE",
					"HONEY", "PASTA", "PRAWN", "CUCUMBER"
			};

			if (ImGui::BeginCombo("Ingredient Combo", szIngredient[m_iSettingTab]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(szIngredient); n++)
				{
					if (ImGui::Selectable(szIngredient[n]))
					{
						m_iSettingTab = n;
						_int iIngredientIdx = m_iSettingTab + 1;
						((CIngredient_Crate*)m_pSelectObj)->Set_Ingredient((INGREDIENT_TYPE)iIngredientIdx);
					}
				}
				ImGui::EndCombo();
			}
			ImGui::End();
		}

		if (nullptr != dynamic_cast<CStove*>(m_pSelectObj))
		{
			ImGui::Begin("Stove_Setting");

			//ImGui::Checkbox("Set_Ingredient", &IsCreatePlate);
			static const char* szCooker[] = {
					"Frying_Pan", "Pot"
			};

			COOKER_TYPE eCookerType = ((CStove*)m_pSelectObj)->Get_InitCookerType();
			if (eCookerType == COOKER_FRYINGPAN)
				m_iSettingTab = 0;
			else if (eCookerType == COOKER_POT)
				m_iSettingTab = 1;
			

			if (ImGui::BeginCombo("Cooker Combo", szCooker[m_iSettingTab]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(szCooker); n++)
				{
					if (ImGui::Selectable(szCooker[n]))
					{
						m_iSettingTab = n;

						switch (m_iSettingTab)
						{
						case 0:
							((CStove*)m_pSelectObj)->Set_InitCookerType(COOKER_FRYINGPAN);
							break;
						case 1:
							((CStove*)m_pSelectObj)->Set_InitCookerType(COOKER_POT);
							break;
						}
					}
				}
				ImGui::EndCombo();
			}

			ImGui::End();
		}

		if (nullptr != dynamic_cast<CTraffic_Light*>(m_pSelectObj))
		{
			ImGui::Begin("Traffic_Light_Setting");

			_float4 vStartPos = ((CTraffic_Light*)m_pSelectObj)->Get_PatrolPos(0);
			_float4 vGoalPos = ((CTraffic_Light*)m_pSelectObj)->Get_PatrolPos(1);

			ImGui::InputFloat4("Start Position", &vStartPos.x);
			ImGui::InputFloat4("Goal Position", &vGoalPos.x);

			((CTraffic_Light*)m_pSelectObj)->Set_PatrolPos(0, vStartPos);
			((CTraffic_Light*)m_pSelectObj)->Set_PatrolPos(1, vGoalPos);

			_bool IsCrossAutomobile = ((CTraffic_Light*)m_pSelectObj)->Get_IsCrossAutomobile();
			ImGui::Checkbox("Cross_Automobile", &IsCrossAutomobile);
			((CTraffic_Light*)m_pSelectObj)->IsCrossAutomobile(IsCrossAutomobile);


			ImGui::End();
		}

		if (nullptr != dynamic_cast<CDispenser*>(m_pSelectObj))
		{
			ImGui::Begin("Dispenser_Setting");

			_int iNumIngredient = ((CDispenser*)m_pSelectObj)->Get_NumIngredient();
			ImGui::InputInt("NumIngredinetTypes", &iNumIngredient);
			((CDispenser*)m_pSelectObj)->Set_NumIngredient(iNumIngredient);

			//ImGui::Checkbox("Set_Ingredient", &IsCreatePlate);
			static const char* szIngredient[] = {
					"None","TOMATO", "ONION", "POTATO", "DOUGH", "SEAWEED",
					"MUSHROOM", "MEAT", "LETTUCE", "RICE", "FLOUR",
					"BUN", "FISH", "SAUSAGE", "UNKNOWN", "EGG",
					"CHICKEN", "TORTILLA", "CHEESE", "CARROT", "CHOCOLATE",
					"HONEY", "PASTA", "PRAWN", "CUCUMBER"
			};

			_uint iSettingIdx;

			if (iNumIngredient >= 1)
			{
				iSettingIdx = ((CDispenser*)m_pSelectObj)->Get_CreateIngredientType(0);
				//m_iSettingTab = ((CDispenser*)m_pSelectObj)->Get_CreateIngredientType(i) - 1;

				if (ImGui::BeginCombo("Ingredient Combo0", szIngredient[iSettingIdx]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(szIngredient); n++)
					{
						if (ImGui::Selectable(szIngredient[n]))
						{
							iSettingIdx = n;
							_int iIngredientIdx = iSettingIdx;
							((CDispenser*)m_pSelectObj)->Set_CreateIngredient_Type(0, (INGREDIENT_TYPE)iIngredientIdx);
						}
					}
					ImGui::EndCombo();
				}
			}

			if (iNumIngredient >= 2)
			{
				iSettingIdx = ((CDispenser*)m_pSelectObj)->Get_CreateIngredientType(1);
				//m_iSettingTab = ((CDispenser*)m_pSelectObj)->Get_CreateIngredientType(i) - 1;

				if (ImGui::BeginCombo("Ingredient Combo1", szIngredient[iSettingIdx]))
				{
					for (int n = 0; n < IM_ARRAYSIZE(szIngredient); n++)
					{
						if (ImGui::Selectable(szIngredient[n]))
						{
							iSettingIdx = n;
							_int iIngredientIdx = iSettingIdx;
							((CDispenser*)m_pSelectObj)->Set_CreateIngredient_Type(1, (INGREDIENT_TYPE)iIngredientIdx);
						}
					}
					ImGui::EndCombo();
				}
			}
			

			//for (_int i = 0; i < iNumIngredient; i++)
			//{
			//	_uint iSettingIdx = ((CDispenser*)m_pSelectObj)->Get_CreateIngredientType(i);
			//	//m_iSettingTab = ((CDispenser*)m_pSelectObj)->Get_CreateIngredientType(i) - 1;

			//	if (ImGui::BeginCombo("Ingredient Combo", szIngredient[iSettingIdx]))
			//	{
			//		for (int n = 0; n < IM_ARRAYSIZE(szIngredient); n++)
			//		{
			//			if (ImGui::Selectable(szIngredient[n]))
			//			{
			//				iSettingIdx = n;
			//				_int iIngredientIdx = iSettingIdx;
			//				((CDispenser*)m_pSelectObj)->Set_CreateIngredient_Type(i, (INGREDIENT_TYPE)iIngredientIdx);
			//			}
			//		}
			//		ImGui::EndCombo();
			//	}
			//}
			
			ImGui::End();
		}
	}
}

HRESULT CEditor::Open_File_To_Explorer()
{
	TCHAR fileName[MAX_PATH] = L"";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	//L"Image Files\0*.png;*.jpg;*.bmp\0Text Files\0*.txt\0All Files\0*.*\0";
	ofn.lpstrFilter = L"All\0*.*\0Data\0*.dat\0"; // 필터를 사용하려면 필요에 따라 수정
	ofn.nFilterIndex = 2;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == TRUE) {
		wstring strFilePath(&fileName[0]);
		if (FAILED(Load_Data(strFilePath)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEditor::Load_Data(wstring& strFilePath)
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
			break;
		case Client::LAYER_COLLISION:
			Load_Layer_Collision_Data(ifs);
			break;
		case Client::LAYER_SPONE:
			Load_Layer_FixModel_Data(ifs);
			break;
		case Client::LAYER_GIMMIC:
			Load_Layer_Gimmic_Data(ifs);
			break;
		}
	}

	ifs.close();

	return S_OK;
}

HRESULT CEditor::Load_Layer_Data(ifstream& ifs)
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

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CEditor::Load_Layer_FixModel_Data(ifstream& ifs)
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

		CModelObject::MODELOBJECT_DESC pDesc = {};
		pDesc.bInitWorldMatrix = true;
		pDesc.InitWorldFloat4x4 = worldFloat4x4;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CEditor::Load_Layer_Interact_Data(ifstream& ifs)
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
		case Client::OBJECT_NONE:
			return S_OK;
			break;
		case Client::OBJECT_TABLE:
		{
			_bool IsCreatePlate = false;
			ifs.read(reinterpret_cast<_char*>(&IsCreatePlate), sizeof(_bool));

			CTable::TABLE_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.IsCreatePlate = IsCreatePlate;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
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

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
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

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
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

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		default:
		{
			CModelObject::MODELOBJECT_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		break;
		}

		/*
		if (iInteractType == OBJECT_INGREDIENTCRATE)
		{
			_int iIngredientType = 0;
			ifs.read(reinterpret_cast<_char*>(&iIngredientType), sizeof(_int));

			CIngredient_Crate::INGREDIENTCRATE_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.eIngredientType = (INGREDIENT_TYPE)iIngredientType;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		else if (iInteractType == OBJECT_STOVE)
		{
			_int iCookerType = 0;
			ifs.read(reinterpret_cast<_char*>(&iCookerType), sizeof(_int));

			CStove::STOVE_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.eInitCookerType = (COOKER_TYPE)iCookerType;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		else if (iInteractType == OBJECT_TABLE)
		{
			_bool IsCreatePlate = false;
			ifs.read(reinterpret_cast<_char*>(&IsCreatePlate), sizeof(_bool));

			CTable::TABLE_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;
			pDesc.IsCreatePlate = IsCreatePlate;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}
		else
		{
			CModelObject::MODELOBJECT_DESC pDesc = {};
			pDesc.bInitWorldMatrix = true;
			pDesc.InitWorldFloat4x4 = worldFloat4x4;
			pDesc.strModelTag = strModelTag;

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
				return E_FAIL;
		}*/
	}
	return S_OK;
}

HRESULT CEditor::Load_Layer_Collision_Data(ifstream& ifs)
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

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CEditor::Load_Layer_Gimmic_Data(ifstream& ifs)
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

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_EDITOR, strLayerName, strObjectName, &pDesc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CEditor::Create_File_To_Explorer()
{
	TCHAR fileName[MAX_PATH] = L"*.dat";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"All Files\0*.*\0Data\0*.dat\0";
	ofn.nFilterIndex = 2;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileName(&ofn)) {

		wstring strFilePath(&fileName[0]);
		// 경로 읽음
		// 위에서 받아온 경로로 파일 입출력(fstream) 하기
		if (FAILED(Save_Data(strFilePath)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEditor::Save_Data(wstring& strFilePath)
{
	ofstream ofs;
	ofs.open(strFilePath);
	// 오브젝트 레이어 돌면서 객체들 저장
	// 타일 레이어 저장
	_uint iNumSaveLayer = LAYER_END;
	ofs.write(reinterpret_cast<const _char*>(&iNumSaveLayer), sizeof(_uint));
	Save_Layer_Data(ofs, TEXT("Layer_Tiles"));

	Save_Layer_Data(ofs, TEXT("Layer_Props"));

	Save_Layer_Interact_Data(ofs, TEXT("Layer_Interacts"));

	Save_Layer_Collision_Data(ofs, TEXT("Layer_Collision"));

	Save_Layer_FixModel_Data(ofs, TEXT("Layer_Spone"));

	Save_Layer_Gimmic_Data(ofs, TEXT("Layer_Gimmics"));

	ofs.close();

	return S_OK;
}

HRESULT CEditor::Save_Layer_Data(ofstream& ofs, wstring strLayerName)
{
	_uint iNumLayerNameLength = (_uint)strLayerName.size();
	ofs.write(reinterpret_cast<const _char*>(&iNumLayerNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	list<CGameObject*>* pObjList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, strLayerName);
	if (nullptr != pObjList)
	{
		_uint iListSize = (_uint)pObjList->size();
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
		for (auto& Obj : *pObjList)										/* 0번 층 부터 iter를 받아서 시작함 */
		{
			// 오브젝트 이름 저장
			wstring strObjectName = Obj->Get_PrototypeName();
			_uint iNumObjectNameLength = (_uint)strObjectName.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumObjectNameLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

			// 오브젝트 월드 행렬 저장
			CTransform* pTransform = dynamic_cast<CTransform*>(Obj->Get_Component(g_strTransformTag));
			_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
			ofs.write(reinterpret_cast<const _char*>(&WorldMatrix), sizeof(_float4x4));

			wstring strModelTag = ((CModel*)((CModelObject*)Obj)->Get_ModelCom())->Get_ProtoTag();
			_uint iNumModelTagLength = (_uint)strModelTag.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumModelTagLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strModelTag[0]), sizeof(wchar_t) * iNumModelTagLength);
		}
	}
	else
	{
		_uint iListSize = 0;
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
	}

	return S_OK;
}

HRESULT CEditor::Save_Layer_FixModel_Data(ofstream& ofs, wstring strLayerName)
{
	_uint iNumLayerNameLength = (_uint)strLayerName.size();
	ofs.write(reinterpret_cast<const _char*>(&iNumLayerNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	list<CGameObject*>* pObjList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, strLayerName);
	if (nullptr != pObjList)
	{
		_uint iListSize = (_uint)pObjList->size();
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
		for (auto& Obj : *pObjList)										/* 0번 층 부터 iter를 받아서 시작함 */
		{
			// 오브젝트 이름 저장
			wstring strObjectName = Obj->Get_PrototypeName();
			_uint iNumObjectNameLength = (_uint)strObjectName.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumObjectNameLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

			// 오브젝트 월드 행렬 저장
			CTransform* pTransform = dynamic_cast<CTransform*>(Obj->Get_Component(g_strTransformTag));
			_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
			ofs.write(reinterpret_cast<const _char*>(&WorldMatrix), sizeof(_float4x4));
		}
	}
	else
	{
		_uint iListSize = 0;
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
	}

	return S_OK;
}

HRESULT CEditor::Save_Layer_Interact_Data(ofstream& ofs, wstring strLayerName)
{
	_uint iNumLayerNameLength = (_uint)strLayerName.size();
	ofs.write(reinterpret_cast<const _char*>(&iNumLayerNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	list<CGameObject*>* pObjList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, strLayerName);
	if (nullptr != pObjList)
	{
		_uint iListSize = (_uint)pObjList->size();
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
		for (auto& pObj : *pObjList)										/* 0번 층 부터 iter를 받아서 시작함 */
		{
			// 오브젝트 이름 저장
			wstring strObjectName = pObj->Get_PrototypeName();
			_uint iNumObjectNameLength = (_uint)strObjectName.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumObjectNameLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

			// 오브젝트 월드 행렬 저장
			CTransform* pTransform = dynamic_cast<CTransform*>(pObj->Get_Component(g_strTransformTag));
			_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
			ofs.write(reinterpret_cast<const _char*>(&WorldMatrix), sizeof(_float4x4));

			wstring strModelTag = ((CModel*)((CModelObject*)pObj)->Get_ModelCom())->Get_ProtoTag();
			_uint iNumModelTagLength = (_uint)strModelTag.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumModelTagLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strModelTag[0]), sizeof(wchar_t) * iNumModelTagLength);

			_int iInteractType = ((CInteractObj*)pObj)->Get_Type();
			ofs.write(reinterpret_cast<const _char*>(&iInteractType), sizeof(_int));

			if (iInteractType == OBJECT_INGREDIENTCRATE)
			{
				_int iIngredientType = (_int)((CIngredient_Crate*)pObj)->Get_IngredientType();
				ofs.write(reinterpret_cast<const _char*>(&iIngredientType), sizeof(_int));
			}
			else if (iInteractType == OBJECT_STOVE)
			{
				_int iCookerIdx = (_int)((CStove*)pObj)->Get_InitCookerType();
				ofs.write(reinterpret_cast<const _char*>(&iCookerIdx), sizeof(_int));
			}
			else if (iInteractType == OBJECT_TABLE)
			{
				_bool IsCreatePlate = ((CTable*)pObj)->IsCreatePlate();
				ofs.write(reinterpret_cast<const _char*>(&IsCreatePlate), sizeof(_bool));
			}
			else if (iInteractType == OBJECT_DISPENSER)
			{
				_uint iNumCreateIngredient = ((CDispenser*)pObj)->Get_NumIngredient();
				ofs.write(reinterpret_cast<const _char*>(&iNumCreateIngredient), sizeof(_uint));

				for (_uint i = 0; i < iNumCreateIngredient; i++)
				{	
					_int iIngredientType = (_int)((CDispenser*)pObj)->Get_CreateIngredientType(i);
					ofs.write(reinterpret_cast<const _char*>(&iIngredientType), sizeof(_int));
				}
			}
		}
	}
	else
	{
		_uint iListSize = 0;
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
	}

	return S_OK;
}

HRESULT CEditor::Save_Layer_Gimmic_Data(ofstream& ofs, wstring strLayerName)
{
	_uint iNumLayerNameLength = (_uint)strLayerName.size();
	ofs.write(reinterpret_cast<const _char*>(&iNumLayerNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	list<CGameObject*>* pObjList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, strLayerName);
	if (nullptr != pObjList)
	{
		_uint iListSize = (_uint)pObjList->size();
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
		for (auto& Obj : *pObjList)										/* 0번 층 부터 iter를 받아서 시작함 */
		{
			// 오브젝트 이름 저장
			wstring strObjectName = Obj->Get_PrototypeName();
			_uint iNumObjectNameLength = (_uint)strObjectName.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumObjectNameLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

			// 오브젝트 월드 행렬 저장
			CTransform* pTransform = dynamic_cast<CTransform*>(Obj->Get_Component(g_strTransformTag));
			_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
			ofs.write(reinterpret_cast<const _char*>(&WorldMatrix), sizeof(_float4x4));

			wstring strModelTag = ((CModel*)((CModelObject*)Obj)->Get_ModelCom())->Get_ProtoTag();
			_uint iNumModelTagLength = (_uint)strModelTag.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumModelTagLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strModelTag[0]), sizeof(wchar_t) * iNumModelTagLength);

			_float4 vStartPos = ((CTraffic_Light*)Obj)->Get_PatrolPos(0);
			ofs.write(reinterpret_cast<const _char*>(&vStartPos), sizeof(_float4));
			_float4 vGoalPos = ((CTraffic_Light*)Obj)->Get_PatrolPos(1);
			ofs.write(reinterpret_cast<const _char*>(&vGoalPos), sizeof(_float4));

			_bool IsCrossAutomobile = ((CTraffic_Light*)Obj)->Get_IsCrossAutomobile();
			ofs.write(reinterpret_cast<const _char*>(&IsCrossAutomobile), sizeof(_bool));
		}
	}
	else
	{
		_uint iListSize = 0;
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
	}

	return S_OK;
}

HRESULT CEditor::Save_Layer_Collision_Data(ofstream& ofs, wstring strLayerName)
{
	_uint iNumLayerNameLength = (_uint)strLayerName.size();
	ofs.write(reinterpret_cast<const _char*>(&iNumLayerNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&strLayerName[0]), sizeof(wchar_t) * iNumLayerNameLength);

	list<CGameObject*>* pObjList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, strLayerName);
	if (nullptr != pObjList)
	{
		_uint iListSize = (_uint)pObjList->size();
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
		for (auto& Obj : *pObjList)										/* 0번 층 부터 iter를 받아서 시작함 */
		{
			// 오브젝트 이름 저장
			wstring strObjectName = Obj->Get_PrototypeName();
			_uint iNumObjectNameLength = (_uint)strObjectName.size();
			ofs.write(reinterpret_cast<const _char*>(&iNumObjectNameLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&strObjectName[0]), sizeof(wchar_t) * iNumObjectNameLength);

			// 오브젝트 월드 행렬 저장
			CTransform* pTransform = dynamic_cast<CTransform*>(Obj->Get_Component(g_strTransformTag));
			_float4x4 WorldMatrix = pTransform->Get_WorldFloat4x4();
			ofs.write(reinterpret_cast<const _char*>(&WorldMatrix), sizeof(_float4x4));
		}
	}
	else
	{
		_uint iListSize = 0;
		ofs.write(reinterpret_cast<const _char*>(&iListSize), sizeof(_uint));
	}

	return S_OK;
}

CGameObject* CEditor::Find_FloorObject(wstring strLayerTag, _int _iFindIdx)
{
	list<CGameObject*>* pLayerList = m_pGameInstance->Get_LayerList(LEVEL_EDITOR, strLayerTag);

	if (nullptr == pLayerList)
		return nullptr;

	if (-1 == _iFindIdx || _iFindIdx >= pLayerList->size())
		return nullptr;

	auto iter = pLayerList->begin();

	for (_int i = 0; i < _iFindIdx; ++i)
	{
		++iter;
	}

	return *iter;
}
#endif

CEditor* CEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEditor* pInstance = new CEditor(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CEditor"));
		Safe_Release<CEditor*>(pInstance);
	}

	return pInstance;
}

void CEditor::Free()
{
	Safe_Release(m_pSelectObj);
	Safe_Release(m_pBluePrintObj);

	__super::Free();

}
