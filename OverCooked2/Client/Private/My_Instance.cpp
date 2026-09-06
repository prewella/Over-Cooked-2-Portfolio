#include "stdafx.h"
#include "My_Instance.h"

#include "VIBuffer_Instance.h"

CMy_Instance::CMy_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{ pDevice, pContext }
{
}

CMy_Instance::CMy_Instance(const CMy_Instance& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CMy_Instance::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMy_Instance::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

_int CMy_Instance::Tick(_float fTimeDelta)
{
    return EVENT_NONE;
}

void CMy_Instance::Late_Tick(_float fTimeDelta)
{   
}

HRESULT CMy_Instance::Render()
{
    return S_OK;
}

HRESULT CMy_Instance::Add_Components()
{
    return S_OK;
}

HRESULT CMy_Instance::Bind_ShaderResources()
{
    return S_OK;
}

void CMy_Instance::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
}
