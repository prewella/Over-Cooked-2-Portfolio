#include "stdafx.h"
#include "Plate.h"

#include "Recipe_Manager.h"

#include "Ingredient.h"
#include "Cooker.h"

#include "Plated_Ingredient.h"

CPlate::CPlate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBelonging{ pDevice ,pContext }
{
}

CPlate::CPlate(const CPlate& rhs)
	:CBelonging{ rhs }
{
}

HRESULT CPlate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlate::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pRecipe_Manager = (CRecipe_Manager*)m_pGameInstance->Clone_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Recipe_Manager"));

	m_eBelongingType = BELONGING_PLATE;
	m_eReactionType = REACTION_PLATE;

	return S_OK;
}

int CPlate::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	Drop(fTimeDelta);

	if (nullptr != m_pPlatedIngredient)
		m_pPlatedIngredient->Tick(fTimeDelta);

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CPlate::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	if (nullptr != m_pPlatedIngredient)
		m_pPlatedIngredient->Late_Tick(fTimeDelta);
}

HRESULT CPlate::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CPlate::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plate"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlate::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

_bool CPlate::Check_Plating(CBelonging* pBelonging)
{
	if (pBelonging->Get_ReactionType() != REACTION_PLATE)
		return false;

	RECIPE_DESC Recipe_Desc = {};

	switch (pBelonging->Get_Type())
	{
	case Client::BELONGING_INGREDIENT:
		Recipe_Desc.eIngredientType = ((CIngredient*)pBelonging)->Get_IngredientType();
		// 컷팅 여부 판단
		Recipe_Desc.eMethod = ((CIngredient*)pBelonging)->Get_Method();
		break;
	case Client::BELONGING_PLATE:
		break;
	case Client::BELONGING_DIRTY_PLATE:
		return false;
		break;
	case Client::BELONGING_COOKER:
		CCooker* pCooker = (CCooker*)pBelonging;
		if (!pCooker->IsCooked() || !pCooker->IsHaveIngredient())
			return false;

		Recipe_Desc.eIngredientType = pCooker->Get_HaveIngredient_Type();
		switch (pCooker->Get_CookerType())
		{
		case Client::COOKER_FRYINGPAN:
			Recipe_Desc.eMethod = METHOD_ROAST;
			break;
		case Client::COOKER_POT:
			Recipe_Desc.eMethod = METHOD_BOIL;
			break;
		case Client::COOKER_FRYER_BASKET:
			Recipe_Desc.eMethod = METHOD_FRYED;
			break;
		}
		break;
	}

	if (m_pRecipe_Manager->Check_Add_Plated_Ingredient(Recipe_Desc, m_PlatingDescs))
		return true;

	return false;
}

_bool CPlate::Plating_Ingredient(CBelonging* pBelonging)
{
	RECIPE_DESC Recipe_Desc = {};

	switch (pBelonging->Get_Type())
	{
	case Client::BELONGING_INGREDIENT:
		Recipe_Desc.eIngredientType = ((CIngredient*)pBelonging)->Get_IngredientType();
		// 컷팅 여부 판단
		Recipe_Desc.eMethod = ((CIngredient*)pBelonging)->Get_Method();
		break;
	case Client::BELONGING_PLATE:
		break;
	case Client::BELONGING_COOKER:
		CCooker* pCooker = (CCooker*)pBelonging;
		if (!pCooker->IsCooked() || !pCooker->IsHaveIngredient())
			return false;

		Recipe_Desc.eIngredientType = pCooker->Get_HaveIngredient_Type();
		switch (pCooker->Get_CookerType())
		{
		case Client::COOKER_FRYINGPAN:
			Recipe_Desc.eMethod = METHOD_ROAST;
			break;
		case Client::COOKER_POT:
			Recipe_Desc.eMethod = METHOD_BOIL;
			break;
		case Client::COOKER_FRYER_BASKET:
			Recipe_Desc.eMethod = METHOD_FRYED;
			break;
		}
		break;
	}

	m_PlatingDescs.push_back(Recipe_Desc);
	m_pGameInstance->StopSound(CSound_Manager::CH_PLATE);
	m_pGameInstance->Play_Sound(L"Chef_Add_Ingredient_01.wav", CSound_Manager::CH_PLATE, 1.f);
	if (!FAILED(Add_Plate_Ingredient_Model(Recipe_Desc.eIngredientType)))
		return true;

	return false;
}

void CPlate::Waste_Plated_Ingredient()
{
	Safe_Release(m_pPlatedIngredient);
	m_pPlatedIngredient = nullptr;

	m_PlatingDescs.clear();
}

HRESULT CPlate::Add_Plate_Ingredient_Model(INGREDIENT_TYPE plated_IngredientType)
{
	if (nullptr == m_pPlatedIngredient)
	{
		// 이거 모델 말고 그냥 파트 오브젝트 파생 클래스 만들어서 추가해서 관리해주는게 더 편하겠는데?
		CPlated_Ingredient::PLATED_INGREDIENT_DESC	Plated_IngredientDesc = {};

		Plated_IngredientDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
		Plated_IngredientDesc.eIngredientType = plated_IngredientType;

		m_pPlatedIngredient = dynamic_cast<CPlated_Ingredient*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Plated_Ingredient"), &Plated_IngredientDesc));
		if (nullptr == m_pPlatedIngredient)
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pPlatedIngredient->Change_Model(m_PlatingDescs)))
			return E_FAIL;
	}

	return S_OK;
}

CPlate* CPlate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlate* pInstance = new CPlate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlate"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlate::Clone(void* pArg)
{
	CPlate* pInstance = new CPlate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlate"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlate::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pPlatedIngredient);
	Safe_Release(m_pRecipe_Manager);
}
