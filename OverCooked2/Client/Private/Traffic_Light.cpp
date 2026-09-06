#include "stdafx.h"
#include "Traffic_Light.h"

#include "Light.h"

#include "Automobile.h"

CTraffic_Light::CTraffic_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CModelObject{ pDevice, pContext }
{
}

CTraffic_Light::CTraffic_Light(const CTraffic_Light& rhs)
	:CModelObject{ rhs }
{
}

HRESULT CTraffic_Light::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTraffic_Light::Initialize(void* pArg)
{
	TRAFFIC_LIGHT_DESC* pDesc = (TRAFFIC_LIGHT_DESC*)pArg;

	for (_uint i = 0; i < 2; i++)
	{
		m_Automobile_PatrolPos.push_back(_float4(0.f, 0.f, 0.f, 1.f));
	}

	if (nullptr != pDesc)
	{
		m_Automobile_PatrolPos[0] = pDesc->vStartPos;
		m_Automobile_PatrolPos[1] = pDesc->vGoalPos;
		m_IsInitCroosAutomobile = pDesc->IsCroosAutomobile;
	}

	m_IsCroosAutomobile = m_IsInitCroosAutomobile;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_iSignalIdx = 0;
	m_fSignalTime = RedLight;

	Ready_Light();

	return S_OK;
}

_int CTraffic_Light::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_fSignalTime -= fTimeDelta;

	if (m_fSignalTime <= 0.f)
	{
		switch (m_iSignalIdx)
		{
		case 0:
			m_iSignalIdx = 2;
			m_fSignalTime = GreenLight;
			Cross_Automobile();
			m_IsCroosAutomobile = !m_IsCroosAutomobile;
			break;
		case 1:
			m_iSignalIdx = 0;
			m_fSignalTime = RedLight;
			break;
		case 2:
			m_iSignalIdx = 1;
			m_fSignalTime = YellowLight;
			break;
		}
		ReSetting_Light();
	}

	return EVENT_NONE;
}

void CTraffic_Light::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTraffic_Light::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CTraffic_Light::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Traffic_Light"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTraffic_Light::Bind_ShaderResources()
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

void CTraffic_Light::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_vector vLook= m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
	_vector vSignLightPos = vPos + (vLook * 0.2f);
	_float4 vSignPos;

	XMStoreFloat4(&vSignPos, vSignLightPos);

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(vSignPos.x, vSignPos.y + 2.45f, vSignPos.z, 1.f);
	LightDesc.fRange = 0.4f;

	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	m_pSignLight = CLight::Create(LightDesc);

	m_pGameInstance->Add_Light(m_pSignLight);

	// Create Ambient Light
	_vector vAmbientLightPos = vPos + (vLook * 2.f);
	_float4 vAmbientPos;

	XMStoreFloat4(&vAmbientPos, vAmbientLightPos);

	LightDesc.vPosition = _float4(vAmbientPos.x, vAmbientPos.y + 0.25f, vAmbientPos.z, 1.f);
	LightDesc.fRange = 2.f;
	m_pAmbientLight = CLight::Create(LightDesc);

	m_pGameInstance->Add_Light(m_pAmbientLight);


}

void CTraffic_Light::ReSetting_Light()
{
	LIGHT_DESC	SignLightDesc = *m_pSignLight->Get_LightDesc();
	LIGHT_DESC	AmbientLightDesc = *m_pAmbientLight->Get_LightDesc();

	_float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

	switch (m_iSignalIdx)
	{
	case 0:
		SignLightDesc.vPosition = _float4(SignLightDesc.vPosition.x, vPos.y + 2.45f, SignLightDesc.vPosition.z, 1.f);
		SignLightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
		AmbientLightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
		break;
	case 1:
		SignLightDesc.vPosition = _float4(SignLightDesc.vPosition.x, vPos.y + 2.15f, SignLightDesc.vPosition.z, 1.f);
		SignLightDesc.vDiffuse = _float4(1.f, 1.f, 0.f, 1.f);
		AmbientLightDesc.vDiffuse = _float4(1.f, 1.f, 0.f, 1.f);
		break;
	case 2:
		SignLightDesc.vPosition = _float4(SignLightDesc.vPosition.x, vPos.y + 1.85f, SignLightDesc.vPosition.z, 1.f);
		SignLightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
		AmbientLightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
		break;
	}

	m_pSignLight->ReSetting_LightDesc(SignLightDesc);
	m_pAmbientLight->ReSetting_LightDesc(AmbientLightDesc);
}

void CTraffic_Light::Cross_Automobile()
{
	if(XMVector3Equal(XMLoadFloat4(&m_Automobile_PatrolPos[0]), XMLoadFloat4(&m_Automobile_PatrolPos[1])) || !m_IsCroosAutomobile)
		return;

	CAutomobile::AUTOMOBILE_DESC pDesc = {};
	pDesc.vStartPos = m_Automobile_PatrolPos[0];
	pDesc.vGoalPos = m_Automobile_PatrolPos[1];

	m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_WarningObjects"), TEXT("Prototype_GameObject_Automobile"), &pDesc);
}

CTraffic_Light* CTraffic_Light::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTraffic_Light* pInstance = new CTraffic_Light(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTraffic_Light"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTraffic_Light::Clone(void* pArg)
{
	CTraffic_Light* pInstance = new CTraffic_Light(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTraffic_Light"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTraffic_Light::Free()
{
	__super::Free();

	Safe_Release(m_pSignLight);
	Safe_Release(m_pAmbientLight);
}