#include "stdafx.h"
#include "InteractObj.h"

#include "Ingredient.h"
#include "Plate.h"
#include "Cooker.h"

CInteractObj::CInteractObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CModelObject{ pDevice , pContext }
{
}

CInteractObj::CInteractObj(const CInteractObj& rhs)
	:CModelObject{ rhs }
{
}

HRESULT CInteractObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractObj::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

int CInteractObj::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CInteractObj::Late_Tick(_float fTimeDelta)
{
}

HRESULT CInteractObj::Render() {
	return S_OK;
}

_bool CInteractObj::Check_PutDown(CBelonging* pBelonging)
{
	if (nullptr == m_pBelonging)
		return true;
	else
	{
		switch (m_pBelonging->Get_Type())
		{
		case Client::BELONGING_INGREDIENT:
			return false;
			break;
		case Client::BELONGING_PLATE:
			if (((CPlate*)m_pBelonging)->Check_Plating(pBelonging))
				return true;
			break;
		case Client::BELONGING_DIRTY_PLATE:
			return false;
			break;
		case Client::BELONGING_COOKER:
			if (((CCooker*)m_pBelonging)->Check_PutDown(pBelonging))
				return true;
			break;
		}
	}
	return false;
}

BELONGING_TYPE CInteractObj::Get_HaveObjType()
{
	if (nullptr == m_pBelonging)
		return BELONGING_NONE;

	return m_pBelonging->Get_Type();
}

BELONGING_TYPE CInteractObj::PutDown_Belonging(CBelonging* pBelonging)
{
	if (nullptr == m_pBelonging)
	{
		PutDown(pBelonging);
		return BELONGING_NONE;
	}

	switch (m_pBelonging->Get_Type())
	{
	case Client::BELONGING_INGREDIENT:
		return BELONGING_INGREDIENT;
		break;
	case Client::BELONGING_PLATE:
		if (((CPlate*)m_pBelonging)->Plating_Ingredient(pBelonging))
		{
			//pBelonging->Set_Erase();
			return BELONGING_PLATE;
		}
		break;

	case Client::BELONGING_DIRTY_PLATE:
		return BELONGING_DIRTY_PLATE;
		break;
	case Client::BELONGING_COOKER:
		if (((CCooker*)m_pBelonging)->PutIn_Ingredient(pBelonging))
			return BELONGING_COOKER;
		break;
	}
	return BELONGING_END;
}

_bool CInteractObj::Check_Reaction()
{
	return false;
}

void CInteractObj::Take_HaveObj(CBelonging** pBelonging)
{
	if (nullptr == m_pBelonging)
		return;

	*pBelonging = m_pBelonging;
	Safe_Release(m_pBelonging);
	m_pBelonging = nullptr;
}

void CInteractObj::PutDown(CBelonging* pObject) {
	m_pBelonging = pObject;
	Safe_AddRef(pObject);
	Set_On_Belonging();
}

_bool CInteractObj::PutIn_To_Cooker(CBelonging* pIngredient)
{
	if (m_pBelonging->Get_Type() != BELONGING_COOKER)
		return false;

	return ((CCooker*)m_pBelonging)->PutIn_Ingredient(pIngredient);
}

void CInteractObj::Set_On_Belonging()
{
	if (nullptr != m_pBelonging)
	{
		_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
		vPos += XMVectorSet(0.f, 0.5f, 0.f, 0.f);
		XMVectorSetW(vPos, 1.f);

		((CTransform*)m_pBelonging->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_POSITION, vPos);
	}
}

void CInteractObj::Free()
{
	__super::Free();

	Safe_Release(m_pBelonging);
}
