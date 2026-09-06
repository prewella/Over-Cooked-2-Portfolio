#include "stdafx.h"
#include "Ingredient.h"

CIngredient::CIngredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBelonging{ pDevice ,pContext }
{
}

CIngredient::CIngredient(const CIngredient& rhs)
	:CBelonging{ rhs }
{
}

HRESULT CIngredient::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIngredient::Initialize(void* pArg)
{
	INGREDIENT_DESC* pDesc = (INGREDIENT_DESC*)pArg;

	m_eIngredientType = pDesc->eIngredientType;
	Set_IngredientType();

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (m_IsAnimModel)
		m_pModelCom->Set_Animation(ANIM_NONCUT, true);

	m_eBelongingType = BELONGING_INGREDIENT;

	return S_OK;
}

int CIngredient::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (m_IsThrow)
	{
		m_pTransformCom->Set_Speed(m_fThrowingSpeed);
		m_fThrowingSpeed -= 5 * fTimeDelta;
		if (m_fThrowingSpeed < 15.f)
		{
			m_fThrowingSpeed = 0;
			m_IsThrow = false;
		}
		m_pTransformCom->Move_To_SetDir_NonRot(XMLoadFloat4(&m_vThrowingDir), fTimeDelta);
	}

	Drop(fTimeDelta);


	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CIngredient::Late_Tick(_float fTimeDelta)
{
	if (m_IsAnimModel)
	{
		m_pModelCom->Play_Animation(fTimeDelta);

		if (m_pModelCom->Get_CurrentAnimationIdx() == ANIM_CUT7 && m_pModelCom->isFinished())
			m_pModelCom->Set_Animation(ANIM_CUTTED, true);
	}

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CIngredient::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		if (m_IsAnimModel)
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
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

void CIngredient::Set_NextAnim()
{
	if (m_pModelCom->Get_CurrentAnimationIdx() == ANIM_NONCUT)
	{
		m_pModelCom->Set_Animation(ANIM_CUT1, false);
	}
	else if (m_pModelCom->Get_CurrentAnimationIdx() < ANIM_CUTTED)
	{
		m_pModelCom->Set_NextAnimation(false);
	}
}

void CIngredient::Chop_Ingredient()
{
	if (m_IsAnimModel){
		if (m_pModelCom->isFinished())
		{
			Set_NextAnim();
			if (m_pModelCom->Get_CurrentAnimationIdx() == ANIM_CUT7)
			{
				m_IsChopped = true;
				Update_ReactionType();
			}
		}
	}
	else
	{
		if (m_iNonAnimChopCount < 10)
		{
			m_iNonAnimChopCount++;

			Create_Chop_Effect(); 
		}
		else
		{
			m_IsChopped = true;
			Update_ReactionType();
		}
	}
}

void CIngredient::Set_Throwing(_float4 vThrowingDir)
{
	m_IsThrow = true;
	// 생각해보면 캐릭터 회전에 따라 같이 회전하긴 하는데, 걍 직진하면 되지 않나?
	m_vThrowingDir = vThrowingDir;
}

void CIngredient::Set_IngredientType()
{
	switch (m_eIngredientType)
	{
	case Client::INGREDIENT_TOMATO:
	case Client::INGREDIENT_MUSHROOM:
	case Client::INGREDIENT_CUCUMBER:
		m_eReactionType = REACTION_CHOP_TABLE;
		m_IsAnimModel = true;
		break;
	case Client::INGREDIENT_MEAT:
	case Client::INGREDIENT_FISH:
	case Client::INGREDIENT_PRAWN:
		m_eReactionType = REACTION_CHOP_TABLE;
		break;
	case Client::INGREDIENT_RICE:
	case Client::INGREDIENT_PASTA:
		m_eReactionType = REACTION_BOIL;
		break;
	case Client::INGREDIENT_SEAWEED:
	default:
		m_eReactionType = REACTION_PLATE;
		break;
	}
}

void CIngredient::Update_ReactionType()
{
	switch (m_eIngredientType)
	{
	case Client::INGREDIENT_TOMATO:
	case Client::INGREDIENT_CUCUMBER:
		if (m_IsChopped)
		{
			m_eMethod = METHOD_CHOP;
			m_eReactionType = REACTION_PLATE;
		}
		break;
	case Client::INGREDIENT_MUSHROOM:
		if (m_IsChopped)
		{
			m_eMethod = METHOD_CHOP;
			m_eReactionType = REACTION_FRYINGPAN;
		}
		break;
	case Client::INGREDIENT_FISH:
	case Client::INGREDIENT_PRAWN:
		if (m_IsChopped)
		{
			m_eMethod = METHOD_CHOP;
			m_eReactionType = REACTION_PLATE;
			Change_Model();
		}
		break;
	case Client::INGREDIENT_MEAT:
		if (m_IsChopped)
		{
			m_eMethod = METHOD_CHOP;
			m_eReactionType = REACTION_FRYINGPAN;
			Change_Model();
		}
		break;
	default:
		break;
	}
}

HRESULT CIngredient::Change_Model()
{
	wstring strModelTag = TEXT("Prototype_Component_Model_Ingredient_");
	switch (m_eIngredientType)
	{
	case Client::INGREDIENT_MEAT:
		strModelTag += TEXT("Meat");
		break;
	case Client::INGREDIENT_FISH:
		strModelTag += TEXT("Fish");
		break;
	case Client::INGREDIENT_PRAWN:
		strModelTag += TEXT("Prawn");
		break;
	case Client::INGREDIENT_CUCUMBER:
		strModelTag += TEXT("Cucumber");
		break;
	}

	strModelTag += TEXT("_Sliced");

	//Prototype_Component_Model_Ingredient_Fish_Sliced
	Safe_Release(m_pModelCom);
	m_pModelCom = nullptr;

	auto pModelCom = m_Components.find(TEXT("Com_Model"));
	Safe_Release((*pModelCom).second);
	m_Components.erase(pModelCom);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

void CIngredient::Create_Chop_Effect()
{
	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WorldMatrix._42 += 0.125f;

	GAMEOBJECT_DESC pObjectDesc = {};
	pObjectDesc.bInitWorldMatrix = true;
	pObjectDesc.InitWorldFloat4x4 = WorldMatrix;

	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Instance_Hit_Star"), &pObjectDesc);
	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Instance_RatHit"), &pObjectDesc);
}

HRESULT CIngredient::Add_Components()
{
	if (m_IsAnimModel)
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	

	wstring strModelTag = TEXT("Prototype_Component_Model_Ingredient_");
	
	switch (m_eIngredientType)
	{
	case Client::INGREDIENT_TOMATO:
		strModelTag += TEXT("Tomato");
		break;
	case Client::INGREDIENT_SEAWEED:
		strModelTag += TEXT("Seaweed");
		break;
	case Client::INGREDIENT_MUSHROOM:
		strModelTag += TEXT("Mushroom");
		break;
	case Client::INGREDIENT_MEAT:
		strModelTag += TEXT("Meat");
		break;
	case Client::INGREDIENT_RICE:
		strModelTag += TEXT("Rice");
		break;
	case Client::INGREDIENT_FISH:
		strModelTag += TEXT("Fish");
		break;
	case Client::INGREDIENT_PASTA:
		strModelTag += TEXT("Pasta");
		break;
	case Client::INGREDIENT_PRAWN:
		strModelTag += TEXT("Prawn");
		break;
	case Client::INGREDIENT_CUCUMBER:
		strModelTag += TEXT("Cucumber");
		break;
	default:
		strModelTag += TEXT("Tomato");
		break;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
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

HRESULT CIngredient::Bind_ShaderResources()
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

CIngredient* CIngredient::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIngredient* pInstance = new CIngredient(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIngredient"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CIngredient::Clone(void* pArg)
{
	CIngredient* pInstance = new CIngredient(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CIngredient"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIngredient::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
