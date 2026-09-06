#include "stdafx.h"
#include "UI_Dish.h"

CUI_Dish::CUI_Dish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice,pContext }
{
}

CUI_Dish::CUI_Dish(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Dish::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Dish::Initialize(void* pArg)
{
	UI_DISH_DESC* pDesc = (UI_DISH_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pDesc->str_DishName)))
		return E_FAIL;

	Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

	m_iRenderOrder = (_uint)UI_ORDER::ORDER_FRONT;

	return S_OK;
}

_int CUI_Dish::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CUI_Dish::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Dish::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Dish::Add_Components(wstring strDishName)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	wstring strUIDishName = TEXT("Prototype_Component_Texture_") + strDishName;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strUIDishName,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Dish::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

CUI_Dish* CUI_Dish::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Dish* pInstance = new CUI_Dish(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Dish"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Dish::Clone(void* pArg)
{
	CUI_Dish* pInstance = new CUI_Dish(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUI_Dish"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Dish::Free()
{
	__super::Free();
}
