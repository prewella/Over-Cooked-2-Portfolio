#include "stdafx.h"
#include "Belonging.h"

CBelonging::CBelonging(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CModelObject{ pDevice , pContext }
{
}

CBelonging::CBelonging(const CBelonging& rhs)
    :CModelObject{ rhs }
{
}

HRESULT CBelonging::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBelonging::Initialize(void* pArg)
{
    __super::Initialize(pArg);
    return S_OK;
}

int CBelonging::Tick(_float fTimeDelta)
{
    return EVENT_NONE;
}

void CBelonging::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBelonging::Render()
{
    return S_OK;
}

void CBelonging::Drop(_float fTimeDelta)
{
    if (!m_IsDrop)
        return;

    _vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    if (XMVectorGetY(vPos) > 0.f)
    {
        vPos -= {0.f, fTimeDelta * 1.5f, 0.f};

        if (XMVectorGetY(vPos) <= 0.f)
        {
            m_IsDrop = false;
            vPos = { XMVectorGetX(vPos), 0.f, XMVectorGetZ(vPos), XMVectorGetW(vPos) };
        }

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
    }
}

void CBelonging::Free()
{
    __super::Free();
}