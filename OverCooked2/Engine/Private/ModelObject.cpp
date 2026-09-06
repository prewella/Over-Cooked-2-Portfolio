#include "ModelObject.h"
#include "Model.h"
#include "GameInstance.h" 


CModelObject::CModelObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}

CModelObject::CModelObject(const CModelObject& rhs)
    :CGameObject(rhs)
{
}

HRESULT CModelObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelObject::Initialize(void* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

_int CModelObject::Tick(_float fTimeDelta)
{
    return EVENT_NONE;
}

void CModelObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CModelObject::Render()
{
    return S_OK;
}

void CModelObject::Free()
{
    Safe_Release(m_pModelCom);

    __super::Free();
}