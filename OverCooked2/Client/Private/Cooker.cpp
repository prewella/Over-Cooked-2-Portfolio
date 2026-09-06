#include "stdafx.h"
#include "Cooker.h"

#include "Ingredient.h"

#include "PartObject.h"

CCooker::CCooker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBelonging{ pDevice, pContext }
{
}

CCooker::CCooker(const CCooker& rhs)
	:CBelonging{ rhs }
{
}

HRESULT CCooker::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCooker::Initialize(void* pArg)
{
	COOKER_DESC* pDesc = (COOKER_DESC*)pArg;
	m_eCookerType = pDesc->eCookerType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eBelongingType = BELONGING_COOKER;
	m_eReactionType = REACTION_PLATE;

	return S_OK;
}

int CCooker::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	Drop(fTimeDelta);

	if (nullptr != m_pCookingIngredient)
		m_pCookingIngredient->Tick(fTimeDelta);

	m_pGameInstance->Add_Collision_Group(this);

	if (m_IsCooked)
	{
		_float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		WorldMatrix._42 += 0.5f;

		GAMEOBJECT_DESC pObjectDesc = {};
		pObjectDesc.bInitWorldMatrix = true;
		pObjectDesc.InitWorldFloat4x4 = WorldMatrix;

		m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Instance_Vapor"), &pObjectDesc);


	}

	return EVENT_NONE;
}

void CCooker::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pCookingIngredient)
		m_pCookingIngredient->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CCooker::Render()
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

HRESULT CCooker::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	wstring strCookerModel = TEXT("");

	switch (m_eCookerType)
	{
	case Client::COOKER_FRYINGPAN:
		strCookerModel = TEXT("Prototype_Component_Model_FryingPan");
		break;
	case Client::COOKER_POT:
		strCookerModel = TEXT("Prototype_Component_Model_Pot");
		break;
	case Client::COOKER_FRYER_BASKET:
		strCookerModel = TEXT("Prototype_Component_Model_Fryer_Basket");
		break;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strCookerModel,
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

HRESULT CCooker::Add_CookingIngredient()
{
	switch (m_eCookerType)
	{
	case COOKER_FRYINGPAN:
		break;
	case COOKER_POT:
		CPartObject::PARTOBJECT_DESC	PartObjDesc{};
		PartObjDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();

		m_pCookingIngredient = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Soup"), &PartObjDesc));
		if (nullptr == m_pCookingIngredient)
			return E_FAIL;

		return S_OK;

		break;
	}

	return S_OK;
}

void CCooker::Delete_CookingIngredient()
{
	Safe_Release(m_pCookingIngredient);
	m_pCookingIngredient = nullptr;
}

HRESULT CCooker::Bind_ShaderResources()
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

_bool CCooker::Check_PutDown(CBelonging* pIngredient)
{
	switch (m_eCookerType)
	{
	case Client::COOKER_FRYINGPAN:
		if (pIngredient->Get_ReactionType() == REACTION_FRYINGPAN)
			return true;
		break;
	case Client::COOKER_POT:
		if (pIngredient->Get_ReactionType() == REACTION_BOIL)
			return true;
		break;
	case Client::COOKER_FRYER_BASKET:
		if (pIngredient->Get_ReactionType() == REACTION_FRYINGNET)
			return true;
		break;
	}
	return false;
}

_bool CCooker::PutIn_Ingredient(CBelonging* pIngredient)
{
	if (Check_PutDown(pIngredient))
	{
		m_eHaveIngredientType = (dynamic_cast<CIngredient*>(pIngredient))->Get_IngredientType();
		m_fCookingPercent = 0.f;
		m_IsCooked = false;
		pIngredient->Set_Erase();
		Add_CookingIngredient();
		return true;
	}
	return false;
}

void CCooker::ThrowAway_Ingredient()
{
	m_eHaveIngredientType = INGREDIENT_NONE;
	m_fCookingPercent = 0.f;
	m_IsCooked = false;
	Delete_CookingIngredient();
}

void CCooker::Cooking(_float fTimeDelta)
{
	m_fCookingPercent += fTimeDelta;
	if (m_fCookingPercent >= 1.5f)
	{
		m_IsCooked = true;

		/*	if (m_fCookingPercent >= 3.f)
			{
				m_IsCooked = false;
				m_IsOverCooked = true;
			}*/
	}
}

CCooker* CCooker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCooker* pInstance = new CCooker(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCooker"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCooker::Clone(void* pArg)
{
	CCooker* pInstance = new CCooker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCooker"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCooker::Free()
{
	__super::Free();

	Safe_Release(m_pCookingIngredient);
	Safe_Release(m_pColliderCom);
}
