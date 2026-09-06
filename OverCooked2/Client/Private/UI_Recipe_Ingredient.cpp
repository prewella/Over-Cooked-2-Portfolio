#include "stdafx.h"
#include "UI_Recipe_Ingredient.h"

CUI_Recipe_Ingredient::CUI_Recipe_Ingredient(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUI{ pDevice, pContext }
{
}

CUI_Recipe_Ingredient::CUI_Recipe_Ingredient(const CUI& rhs)
    :CUI{ rhs }
{
}

HRESULT CUI_Recipe_Ingredient::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Recipe_Ingredient::Initialize(void* pArg)
{
    UI_RECIPE_INGREDIENT_DESC* pDesc = (UI_RECIPE_INGREDIENT_DESC*)pArg;
    m_eIngredient_Type = pDesc->eIngredient_type;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Initialize_UI_Setting(g_iWinSizeX, g_iWinSizeY);

    m_iRenderOrder = (_uint)UI_ORDER::ORDER_FRONT;

    return S_OK;
}

_int CUI_Recipe_Ingredient::Tick(_float fTimeDelta)
{
    if (m_bDead)
        return EVENT_DEAD;

    return EVENT_NONE;
}

void CUI_Recipe_Ingredient::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI_Recipe_Ingredient::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Recipe_Ingredient::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recipe_Ingredients"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Recipe_Ingredient::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    _uint iNumRow = 5,iNumCol = 5, iIngredientIdx = (_uint)m_eIngredient_Type - 1;
    if (FAILED(m_pShaderCom->Bind_RawValue("iNumCol",&iNumCol, sizeof(_uint))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("iNumRow", &iNumRow, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("iIdx", &iIngredientIdx, sizeof(_uint))))
        return E_FAIL;

    m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

    return S_OK;
}

CUI_Recipe_Ingredient* CUI_Recipe_Ingredient::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Recipe_Ingredient* pInstance = new CUI_Recipe_Ingredient(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CUI_Recipe_Ingredient"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Recipe_Ingredient::Clone(void* pArg)
{
    CUI_Recipe_Ingredient* pInstance = new CUI_Recipe_Ingredient(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CUI_Recipe_Ingredient"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Recipe_Ingredient::Free()
{
    __super::Free();
}
