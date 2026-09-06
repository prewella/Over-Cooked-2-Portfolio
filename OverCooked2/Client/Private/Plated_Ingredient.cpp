#include "stdafx.h"
#include "Plated_Ingredient.h"

CPlated_Ingredient::CPlated_Ingredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice,pContext }
{
}

CPlated_Ingredient::CPlated_Ingredient(const CPlated_Ingredient& rhs)
	:CPartObject{ rhs }
{
}

HRESULT CPlated_Ingredient::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlated_Ingredient::Initialize(void* pArg)
{
	PLATED_INGREDIENT_DESC* pDesc = (PLATED_INGREDIENT_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pDesc->eIngredientType)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.125f, 0.f, 1.f));

	return S_OK;
}

int CPlated_Ingredient::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CPlated_Ingredient::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlated_Ingredient::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlated_Ingredient::Add_Components(INGREDIENT_TYPE eIngredientType)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	wstring strModelName = TEXT("Prototype_Component_Model_Plating_");

	switch (eIngredientType)
	{
	case Client::INGREDIENT_SEAWEED:
		strModelName += TEXT("Seaweed");
		break;
	case Client::INGREDIENT_MUSHROOM:
		strModelName += TEXT("Mushroom");
		break;
	case Client::INGREDIENT_MEAT:
		strModelName += TEXT("Meat");
		break;
	case Client::INGREDIENT_RICE:
		strModelName += TEXT("Rice");
		break;
	case Client::INGREDIENT_FISH:
		strModelName += TEXT("Fish");
		break;
	case Client::INGREDIENT_PASTA:
		strModelName += TEXT("Pasta");
		break;
	case Client::INGREDIENT_PRAWN:
		strModelName += TEXT("Prawn");
		break;
	case Client::INGREDIENT_CUCUMBER:
		strModelName += TEXT("Cucumber");
		break;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelName,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlated_Ingredient::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlated_Ingredient::Change_Model(list<RECIPE_DESC> PlatedIngredientDescs)
{
	wstring strModelTag = TEXT("Prototype_Component_Model_Plating_");

	_uint iNumIngredient = (_uint)PlatedIngredientDescs.size();
	_bool IsPlatedIngredient[INGREDIENT_END] = { false };

	for (auto IngredientDesc : PlatedIngredientDescs)
	{
		IsPlatedIngredient[IngredientDesc.eIngredientType] = true;
	}

	strModelTag += Get_ModelName_Using_Ingredients(iNumIngredient, IsPlatedIngredient);
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

wstring CPlated_Ingredient::Get_ModelName_Using_Ingredients(_uint iNumIngredient, _bool* IsPlatingIngredient)
{
	wstring strIngredientsName = TEXT("");

	switch (iNumIngredient)
	{
	case 2:
		if (IsPlatingIngredient[INGREDIENT_SEAWEED])
		{
			if (IsPlatingIngredient[INGREDIENT_RICE])
				strIngredientsName = TEXT("Seaweed_Rice"); 
			else if (IsPlatingIngredient[INGREDIENT_FISH])
				strIngredientsName = TEXT("Seaweed_Fish");
			else if (IsPlatingIngredient[INGREDIENT_CUCUMBER])
				strIngredientsName = TEXT("Seaweed_Cucumber");
		}
		if (IsPlatingIngredient[INGREDIENT_RICE])
		{
			if (IsPlatingIngredient[INGREDIENT_FISH])
				strIngredientsName = TEXT("Rice_Fish");
			else if (IsPlatingIngredient[INGREDIENT_CUCUMBER])
				strIngredientsName = TEXT("Rice_Cucumber");
		}
		if (IsPlatingIngredient[INGREDIENT_FISH] && IsPlatingIngredient[INGREDIENT_CUCUMBER])
		{
			strIngredientsName = TEXT("Fish_Cucumber");
		}

		if (IsPlatingIngredient[INGREDIENT_PASTA])
		{
			if (IsPlatingIngredient[INGREDIENT_MEAT])
				strIngredientsName = TEXT("Meat_Pasta");
			else if (IsPlatingIngredient[INGREDIENT_MUSHROOM])
				strIngredientsName = TEXT("Mushroom_Pasta");
		}

		break;
	case 3:
		if (IsPlatingIngredient[INGREDIENT_SEAWEED] && IsPlatingIngredient[INGREDIENT_RICE])
		{
			if (IsPlatingIngredient[INGREDIENT_FISH])
				strIngredientsName = TEXT("Fish_Sushi");
			else if (IsPlatingIngredient[INGREDIENT_CUCUMBER])
				strIngredientsName = TEXT("Cucumber_Sushi");
		}
		if (IsPlatingIngredient[INGREDIENT_SEAWEED] && IsPlatingIngredient[INGREDIENT_FISH] && IsPlatingIngredient[INGREDIENT_CUCUMBER])
		{
			strIngredientsName = TEXT("Seaweed_Fish_Cucumber");
		}
		if (IsPlatingIngredient[INGREDIENT_RICE] && IsPlatingIngredient[INGREDIENT_FISH] && IsPlatingIngredient[INGREDIENT_CUCUMBER])
		{
			strIngredientsName = TEXT("Rice_Fish_Cucumber");
		}

		break;
	case 4:
		if (IsPlatingIngredient[INGREDIENT_SEAWEED] && IsPlatingIngredient[INGREDIENT_FISH] && IsPlatingIngredient[INGREDIENT_CUCUMBER] && IsPlatingIngredient[INGREDIENT_RICE])
		{
			strIngredientsName = TEXT("Fish_Cucumber_Sushi");
		}
		break;
	}
	return strIngredientsName;
}

CPlated_Ingredient* CPlated_Ingredient::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlated_Ingredient* pInstance = new CPlated_Ingredient(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlated_Ingredient"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlated_Ingredient::Clone(void* pArg)
{
	CPlated_Ingredient* pInstance = new CPlated_Ingredient(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlated_Ingredient"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlated_Ingredient::Free()
{
	__super::Free();
}
