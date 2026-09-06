#include "stdafx.h"
#include "FryingPan.h"

CFryingPan::CFryingPan(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CCooker{ pDevice ,pContext }
{
}

CFryingPan::CFryingPan(const CFryingPan& rhs)
    :CCooker{ rhs }
{
}

HRESULT CFryingPan::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFryingPan::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_eCookerType = COOKER_FRYINGPAN;

    return S_OK;
}

int CFryingPan::Tick(_float fTimeDelta)
{
    if (m_bDead)
        return EVENT_DEAD;

    m_pGameInstance->Add_Collision_Group(this);

    return EVENT_NONE;
}

void CFryingPan::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CFryingPan::Render()
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

HRESULT CFryingPan::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Plate"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

    ColliderDesc.fRadius = 0.25f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFryingPan::Bind_ShaderResources()
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

CFryingPan* CFryingPan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFryingPan* pInstance = new CFryingPan(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CFryingPan"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFryingPan::Clone(void* pArg)
{
    CFryingPan* pInstance = new CFryingPan(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CFryingPan"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFryingPan::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}