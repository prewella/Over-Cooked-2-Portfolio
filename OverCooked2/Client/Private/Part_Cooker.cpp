#include "stdafx.h"
#include "Part_Cooker.h"

#include "Bone.h"

CPart_Cooker::CPart_Cooker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPartObject{ pDevice, pContext }
{
}

CPart_Cooker::CPart_Cooker(const CPart_Cooker& rhs)
    : CPartObject{ rhs }
{
}

HRESULT CPart_Cooker::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPart_Cooker::Initialize(void* pArg)
{
    COOKER_DESC* pCookerDesc = (COOKER_DESC*)pArg;

    m_pSocket = pCookerDesc->pSocket;
    m_pTool = pCookerDesc->pTool;
    Safe_AddRef(m_pSocket);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components(pCookerDesc->strModelTag)))
        return E_FAIL;

    return S_OK;
}

int CPart_Cooker::Tick(_float fTimeDelta)
{
    return EVENT_NONE;
}

void CPart_Cooker::Late_Tick(_float fTimeDelta)
{
    _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocket->Get_CombinedTransformMatrix());

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

    if (*m_pTool != TOOL_NONE)
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    }
}

HRESULT CPart_Cooker::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
            return E_FAIL;

        /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CPart_Cooker::Add_Components(wstring strModelTag)
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Knife"),
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPart_Cooker::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CPart_Cooker* CPart_Cooker::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPart_Cooker* pInstance = new CPart_Cooker(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CPart_Cooker"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPart_Cooker::Clone(void* pArg)
{
    CPart_Cooker* pInstance = new CPart_Cooker(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CPart_Cooker"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPart_Cooker::Free()
{
    __super::Free();

    Safe_Release(m_pSocket);
}
