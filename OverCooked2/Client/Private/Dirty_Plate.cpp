#include "stdafx.h"
#include "Dirty_Plate.h"

#include "Part_DirtyPlate.h"

CDirty_Plate::CDirty_Plate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CBelonging{ pDevice ,pContext }
{
}

CDirty_Plate::CDirty_Plate(const CDirty_Plate& rhs)
    :CBelonging{ rhs }
{
}

HRESULT CDirty_Plate::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDirty_Plate::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    Stack_Plate();

    m_eBelongingType = BELONGING_DIRTY_PLATE;
    m_eReactionType = REACTION_END;

    return S_OK;
}

int CDirty_Plate::Tick(_float fTimeDelta)
{
    if (m_bDead)
        return EVENT_DEAD;

    Drop(fTimeDelta);

    for (auto& plate : m_StackPlates)
        plate->Tick(fTimeDelta);

    m_pGameInstance->Add_Collision_Group(this);

    return EVENT_NONE;
}

void CDirty_Plate::Late_Tick(_float fTimeDelta)
{
    for (auto& plate : m_StackPlates)
        plate->Late_Tick(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CDirty_Plate::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
#endif

    return S_OK;
}

HRESULT CDirty_Plate::Add_Components()
{
    /* Com_Collider */
    CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

    ColliderDesc.fRadius = 0.5f;
    ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;
    
    return S_OK;
}

void CDirty_Plate::Stack_Plate()
{
    CPart_DirtyPlate::PART_DIRTYPLATE_DESC dirtyPlate_Desc{};

    _int iNumStack = (_int)m_StackPlates.size();
    dirtyPlate_Desc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
    dirtyPlate_Desc.vSettingPos = _float4(0.f, 0.125f * iNumStack, 0.f, 1.f);

    CPartObject* pPartObj = nullptr;

    pPartObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_DirtyPlate"), &dirtyPlate_Desc));
    if (nullptr == pPartObj)
        return;

    m_StackPlates.emplace_back(pPartObj);
}

CDirty_Plate* CDirty_Plate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDirty_Plate* pInstance = new CDirty_Plate(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CDirty_Plate"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDirty_Plate::Clone(void* pArg)
{
    CDirty_Plate* pInstance = new CDirty_Plate(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Cloned : CDirty_Plate"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDirty_Plate::Free()
{
    __super::Free();

    for (auto& plate : m_StackPlates)
        Safe_Release(plate);
    m_StackPlates.clear();

    Safe_Release(m_pColliderCom);
}
