#include "stdafx.h"
#include "UI_Order_Manager.h"
#include "GameInstance.h"

#include "UI_Order.h"
#include "Recipe.h"

CUI_Order_Manager::CUI_Order_Manager()
	:m_pGameInstance{CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
}

CUI_Order_Manager::CUI_Order_Manager(CUI_Order_Manager& rhs)
	:m_pGameInstance{ rhs.m_pGameInstance }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Order_Manager::Initialize(void* pArg)
{
	UI_ORDER_MANAGER_DESC* pDesc = (UI_ORDER_MANAGER_DESC*)pArg;
	m_pOrders = pDesc->pOrders;

    return S_OK;
}

void CUI_Order_Manager::Tick(_float fTimeDelta)
{
	TimeOut_Order();
}


void CUI_Order_Manager::Add_Order(CRecipe* pOrder)
{
	CRecipe* pRecipe = pOrder->Cloned();

	m_pOrders->emplace_back(pRecipe);

	Add_UI_Order();
}

void CUI_Order_Manager::Process_Order(_uint iOrderIdx)
{
	auto UI_OrderIter = m_UI_Orders.begin();

	for (_uint i = 0; i < iOrderIdx; i++)
		UI_OrderIter++;

	(*UI_OrderIter)->Delete_UI_Order();
	Safe_Release(*UI_OrderIter);
	m_UI_Orders.erase(UI_OrderIter);

	RePosition_UI_Orders();
}

void CUI_Order_Manager::TimeOut_Order()
{
	// 모든 요리의 제한시간이 같다면 맨 처음 주문서만 확인하여 빼낸다.
	if (0 < m_UI_Orders.size())
	{
		if ((*m_UI_Orders.begin())->IsTimeOver())
		{
			Safe_Release(*m_UI_Orders.begin());
			m_UI_Orders.pop_front();
			Safe_Release(*(m_pOrders->begin()));
			m_pOrders->pop_front();
		}
	}

	/*for (auto UIOrder : m_UI_Orders)
	{
		if (UIOrder->IsTimeOver()) {

		}
	}*/
}

HRESULT CUI_Order_Manager::Add_UI_Order()
{
	_float vSumSizeX = OrderWhiteSpace;

	for (auto& pUI_Order : m_UI_Orders)
	{
		vSumSizeX += pUI_Order->Get_Size().x + OrderWhiteSpace;
	}

	CUI_Order::UI_ORDER_DESC UI_Order_Desc = {};
	UI_Order_Desc.pRecipe = m_pOrders->back();
	UI_Order_Desc.fStartPosX = vSumSizeX;

	CUI_Order* pUI_Order = nullptr;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pUI_Order, LEVEL_GAMEPLAY, 
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Order"), &UI_Order_Desc)))
		return E_FAIL;

	m_UI_Orders.push_back(pUI_Order);

	return S_OK;
}

void CUI_Order_Manager::RePosition_UI_Orders()
{
	_float fSumUISizeX = OrderWhiteSpace;

	for (auto& ui_Order : m_UI_Orders)
	{
		_float2 vPos = ui_Order->Get_Position();
		_float2 vSize = ui_Order->Get_Size();
		ui_Order->Set_Pos(g_iWinSizeX, g_iWinSizeY, _float2((vSize.x * 0.5f) + fSumUISizeX, vPos.y));
		ui_Order->RePosition_UI_Recipes();

		fSumUISizeX += vSize.x + OrderWhiteSpace;
	}
}


CUI_Order_Manager* CUI_Order_Manager::Create(void* pArg)
{
	CUI_Order_Manager* pInstance = new CUI_Order_Manager();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Order_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Order_Manager::Free()
{
	Safe_Release(m_pGameInstance);

	for (auto& pUI_Order : m_UI_Orders)
		Safe_Release(pUI_Order);
	m_UI_Orders.clear();

}
