#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CUI_Order_Manager final : public CBase
{
public:
	typedef struct tagUI_Order_Manager_Desc
	{
		list<class CRecipe*>* pOrders = { nullptr };
	}UI_ORDER_MANAGER_DESC;

private:
#define OrderWhiteSpace 10.f

private:
	CUI_Order_Manager();
	CUI_Order_Manager(CUI_Order_Manager& rhs);
	virtual ~CUI_Order_Manager() = default;

public:
	HRESULT Initialize(void* pArg) ;
	void	Tick(_float fTimeDelta);
	
public:
	void Add_Order(class CRecipe* pOrder);
	void Process_Order(_uint iOrderIdx);
	void TimeOut_Order();

private:
	HRESULT Add_UI_Order();
	void RePosition_UI_Orders();

private:
	CGameInstance* m_pGameInstance = { nullptr };

	list<class CRecipe*>* m_pOrders = { nullptr };
	list<class CUI_Order*> m_UI_Orders;

public:
	static CUI_Order_Manager* Create(void* pArg);
	virtual void Free() override;
};

END