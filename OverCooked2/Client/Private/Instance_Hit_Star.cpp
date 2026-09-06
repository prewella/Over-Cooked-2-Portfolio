#include "stdafx.h"
#include "Instance_Hit_Star.h"

CInstance_Hit_Star::CInstance_Hit_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMy_Instance{ pDevice, pContext }
{
}

CInstance_Hit_Star::CInstance_Hit_Star(const CInstance_Hit_Star& rhs)
    : CMy_Instance{ rhs }
{
}

HRESULT CInstance_Hit_Star::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInstance_Hit_Star::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

_int CInstance_Hit_Star::Tick(_float fTimeDelta)
{
    if (m_bDead)
        return EVENT_DEAD;

    m_pVIBufferCom->Spread(fTimeDelta);

    return EVENT_NONE;
}

void CInstance_Hit_Star::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CInstance_Hit_Star::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInstance_Hit_Star::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Star"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_Hit_Star"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInstance_Hit_Star::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

CInstance_Hit_Star* CInstance_Hit_Star::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInstance_Hit_Star* pInstance = new CInstance_Hit_Star(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CInstance_Hit_Star"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInstance_Hit_Star::Clone(void* pArg)
{
    CInstance_Hit_Star* pInstance = new CInstance_Hit_Star(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CInstance_Hit_Star"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInstance_Hit_Star::Free()
{
    __super::Free();
}
