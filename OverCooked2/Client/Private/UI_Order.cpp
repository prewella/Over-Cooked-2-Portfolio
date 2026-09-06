#include "stdafx.h"
#include "UI_Order.h"

#include"UI_Dish.h"
#include "UI_Recipe.h"

#include "Recipe.h"

CUI_Order::CUI_Order(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Order::CUI_Order(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Order::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Order::Initialize(void* pArg)
{
	UI_ORDER_DESC* pDesc = (UI_ORDER_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_uint iNumRecipes = (_uint)pDesc->pRecipe->Get_Recipes()->size();

	if (iNumRecipes == 1)
		m_vSize = _float2(100.f, 60.f);
	else
		m_vSize = _float2(60.f * iNumRecipes, 60.f);

	m_vPos = _float2(pDesc->fStartPosX + (m_vSize.x * 0.5f), m_vSize.y * 0.5f);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_UI_Dish(pDesc->pRecipe)))
		return E_FAIL;
	if (FAILED(Add_UI_Recipes(pDesc->pRecipe)))
		return E_FAIL;

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_LAST;

	return S_OK;
}

_int CUI_Order::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CUI_Order::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Order::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Order::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Order_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Order::Add_UI_Recipes(CRecipe* pRecipe)
{
	for (auto recipe_Desc : *pRecipe->Get_Recipes())
	{
		CUI_Recipe* pUI_Recipe = nullptr;
		_float fSumSizeX = 0.f;

		for (auto UI_Recipe : m_UI_Recipes)
		{
			fSumSizeX += UI_Recipe->Get_Size().x + RecipeWhiteSpace;
		}

		CUI_Recipe::UI_RECIPE_DESC UI_Recipe_Desc = {};
		UI_Recipe_Desc.recipe_Desc = recipe_Desc;
		UI_Recipe_Desc.vStartPos = _float2((m_vPos.x - (m_vSize.x * 0.5f)) + RecipeWhiteSpace + fSumSizeX, m_vPos.y );

		if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pUI_Recipe, LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Recipe"), &UI_Recipe_Desc)))
			return E_FAIL;

		m_UI_Recipes.push_back(pUI_Recipe);
	}

	return S_OK;
}

HRESULT CUI_Order::Add_UI_Dish(CRecipe* pRecipe)
{
	CUI_Dish::UI_DISH_DESC UI_Dish_Desc = {};
	UI_Dish_Desc.vSize = _float2(50.f, 50.f);
	UI_Dish_Desc.vPos = _float2(m_vPos.x, m_vPos.y);
	UI_Dish_Desc.str_DishName = pRecipe->Get_RecipeName();

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Dish, LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Dish"), &UI_Dish_Desc)))
		return E_FAIL;

return S_OK;
}

HRESULT CUI_Order::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fRatioX = 64 / (_float)m_vSize.x ;
	_float fRatioY = 256 / (_float)m_vSize.y ;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioX", &fRatioX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioY", &fRatioY, sizeof(_float))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

void CUI_Order::RePosition_UI_Recipes()
{
	m_pUI_Dish->Set_Pos(g_iWinSizeX, g_iWinSizeY, m_vPos);

	_float fSumSizeX = RecipeWhiteSpace;

	for (auto UI_Recipe : m_UI_Recipes)
	{
		_float2 vUI_RecipeSize = UI_Recipe->Get_Size();
		_float2 vUI_RecipePos = UI_Recipe->Get_Position();
		UI_Recipe->Set_Pos(g_iWinSizeX, g_iWinSizeY, _float2(m_vPos.x - (m_vSize.x * 0.5f) + (vUI_RecipeSize.x * 0.5f) + fSumSizeX, vUI_RecipePos.y));
		UI_Recipe->RePosition_UI_Recipe();

		fSumSizeX += vUI_RecipeSize.x + RecipeWhiteSpace;
	}

}

void CUI_Order::Delete_UI_Order()
{
	m_bDead = true;
	m_pUI_Dish->Set_Erase();
	for (auto& pUI_Recipe : m_UI_Recipes )
		pUI_Recipe->Delete_UI_Recipe();
}

CUI_Order* CUI_Order::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Order* pInstance = new CUI_Order(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Order"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Order::Clone(void* pArg)
{
	CUI_Order* pInstance = new CUI_Order(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Order"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Order::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Dish);

	for (auto& pUI_Recipe : m_UI_Recipes)
		Safe_Release(pUI_Recipe);
	m_UI_Recipes.clear();
}
