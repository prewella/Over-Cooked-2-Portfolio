#include "Light_Manager.h"

#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

HRESULT CLight_Manager::Initialize()
{
    return S_OK;
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex)
{
    if (iIndex >= m_Lights.size())
        return nullptr;

    auto Iter = m_Lights.begin();

    for (_uint i = 0; i < iIndex; i++)
        Iter++;

    return (*Iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc)
{
    CLight* pLight = CLight::Create(LightDesc);
    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.emplace_back(pLight);

    return S_OK;
}

HRESULT CLight_Manager::Add_Light(CLight* pLight)
{
    if (nullptr == pLight)
        return E_FAIL;

    Safe_AddRef(pLight);
    m_Lights.emplace_back(pLight);

    return S_OK;
}

HRESULT CLight_Manager::Delete_Light(CLight* pDeleteLight)
{
    for (auto iter = m_Lights.begin(); iter == m_Lights.end(); iter++)
    {
        if (pDeleteLight == *iter)
        {
            m_Lights.erase(iter);
            return S_OK;
        }
    }

    return E_FAIL;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    for (auto& pLight : m_Lights)
        pLight->Render(pShader, pVIBuffer);

    return S_OK;
}

void CLight_Manager::Clear()
{
    for (auto& Light : m_Lights)
    {
        Safe_Release(Light);
    }
    m_Lights.clear();
}

CLight_Manager* CLight_Manager::Create()
{
    CLight_Manager* pInstance = new CLight_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CLight_Manager"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight_Manager::Free()
{
    for (auto& pLight : m_Lights)
        Safe_Release(pLight);
    m_Lights.clear();
}
