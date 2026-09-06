#include "stdafx.h"
#include "UI_Recipe.h"

#include "UI_Recipe_Ingredient.h"
#include "UI_Recipe_Method.h"

CUI_Recipe::CUI_Recipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Recipe::CUI_Recipe(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Recipe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Recipe::Initialize(void* pArg)
{
	UI_RECIPE_DESC* pDesc = (UI_RECIPE_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_vStartPos = pDesc->vStartPos;

	COOKING_METHOD_TYPE eMethod = pDesc->recipe_Desc.eMethod;

	if (eMethod == METHOD_NONE || eMethod == METHOD_CHOP)
	{
		m_vSize = _float2(50.f, 30.f + 40.f);
		m_vPos = _float2(m_vStartPos.x + (m_vSize.x * 0.5f), m_vStartPos.y + (m_vSize.y * 0.5f));
	}
	else
	{
		m_vSize = _float2(50.f, 30.f + 70.f);
		m_vPos = _float2(m_vStartPos.x + (m_vSize.x * 0.5f), m_vStartPos.y + (m_vSize.y * 0.5f));

		Add_UI_Recipes_Method(eMethod);
	}

	Add_UI_Recipes_Ingredient(pDesc->recipe_Desc.eIngredientType);

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_LAST;

	return S_OK;
}

_int CUI_Recipe::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CUI_Recipe::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Recipe::Render()
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

HRESULT CUI_Recipe::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recipe_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Recipe::Add_UI_Recipes_Ingredient(INGREDIENT_TYPE eIngredient_type)
{
	CUI_Recipe_Ingredient::UI_RECIPE_INGREDIENT_DESC UI_Ingredient_Desc = {};
	UI_Ingredient_Desc.eIngredient_type = eIngredient_type;
	UI_Ingredient_Desc.vSize = _float2(32.f, 32.f);
	UI_Ingredient_Desc.vPos = _float2(m_vPos.x, m_vStartPos.y + 30.f + (UI_Ingredient_Desc.vSize.y * 0.5f));

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Recipe_Ingredient, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Recipe_Ingredient"), &UI_Ingredient_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Recipe::Add_UI_Recipes_Method(COOKING_METHOD_TYPE eMethod)
{
	CUI_Recipe_Method::UI_RECIPE_METHOD_DESC UI_Method_Desc = {};
	UI_Method_Desc.eMethod = eMethod;
	UI_Method_Desc.vSize = _float2(32.f, 32.f);
	UI_Method_Desc.vPos = _float2(m_vPos.x, m_vStartPos.y + 30.f + ((UI_Method_Desc.vSize.y * 0.5f) * 3));


	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pUI_Recipe_Method, LEVEL_GAMEPLAY,
		TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Recipe_Method"), &UI_Method_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Recipe::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	_float fRatioX = 128 * 0.3f / (_float)m_vSize.x;
	_float fRatioY = 125 * 0.3f / (_float)m_vSize.y;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioX", &fRatioX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatioY", &fRatioY, sizeof(_float))))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

void CUI_Recipe::RePosition_UI_Recipe()
{
	if (nullptr != m_pUI_Recipe_Ingredient)
	{
		_float2 vUI_IngredientPos = m_pUI_Recipe_Ingredient->Get_Position();
		m_pUI_Recipe_Ingredient->Set_Pos(g_iWinSizeX, g_iWinSizeY, _float2(m_vPos.x, vUI_IngredientPos.y));
	}

	if (nullptr != m_pUI_Recipe_Method)
	{
		_float2 vUI_MethodPos = m_pUI_Recipe_Method->Get_Position();
		m_pUI_Recipe_Method->Set_Pos(g_iWinSizeX, g_iWinSizeY, _float2(m_vPos.x, vUI_MethodPos.y));
	}

}

void CUI_Recipe::Delete_UI_Recipe()
{
	m_bDead = true;
	if (nullptr != m_pUI_Recipe_Ingredient)
		m_pUI_Recipe_Ingredient->Set_Erase();
	if (nullptr != m_pUI_Recipe_Method)
		m_pUI_Recipe_Method->Set_Erase();
}

CUI_Recipe* CUI_Recipe::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Recipe* pInstance = new CUI_Recipe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Recipe"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Recipe::Clone(void* pArg)
{
	CUI_Recipe* pInstance = new CUI_Recipe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Recipe"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Recipe::Free()
{
	__super::Free();

	Safe_Release(m_pUI_Recipe_Ingredient);
	Safe_Release(m_pUI_Recipe_Method);
}
