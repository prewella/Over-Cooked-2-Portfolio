#include "stdafx.h"
#include "Instance_RatHit.h"

CInstance_RatHit::CInstance_RatHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMy_Instance{ pDevice, pContext }
{
}

CInstance_RatHit::CInstance_RatHit(const CInstance_RatHit& rhs)
    : CMy_Instance{ rhs }
{
}

HRESULT CInstance_RatHit::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInstance_RatHit::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_fFade_Percent = 1.f;

    return S_OK;
}

_int CInstance_RatHit::Tick(_float fTimeDelta)
{
    if (m_bDead)
        return EVENT_DEAD;

    m_pVIBufferCom->Dwarf(fTimeDelta);

    m_fHitTextureTimer += fTimeDelta;
    if (m_fHitTextureTimer >= 0.125f)
    {
        m_IsNotHitTexture = true;
        if (m_fFade_Percent > 0.f)
            m_fFade_Percent -= fTimeDelta * 2.f;
        else {
            m_fFade_Percent = 0.f;
            m_bDead = true;
        }
    }
    return EVENT_NONE;
}

void CInstance_RatHit::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CInstance_RatHit::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(1)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CInstance_RatHit::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RatHits"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instance_RatHit"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInstance_RatHit::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fFade_Percent", &m_fFade_Percent, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_IsNotHitTexture)))
        return E_FAIL;

    return S_OK;
}

CInstance_RatHit* CInstance_RatHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInstance_RatHit* pInstance = new CInstance_RatHit(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CInstance_RatHit"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInstance_RatHit::Clone(void* pArg)
{
    CInstance_RatHit* pInstance = new CInstance_RatHit(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CInstance_RatHit"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInstance_RatHit::Free()
{
    __super::Free();
}
