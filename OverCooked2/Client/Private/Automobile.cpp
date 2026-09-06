#include "stdafx.h"
#include "Automobile.h"

CAutomobile::CAutomobile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CModelObject{ pDevice,pContext }
{
}

CAutomobile::CAutomobile(const CAutomobile& rhs)
	:CModelObject{ rhs }
{
}

HRESULT CAutomobile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAutomobile::Initialize(void* pArg)
{
	AUTOMOBILE_DESC* pDesc = (AUTOMOBILE_DESC*)pArg;

	m_vGoalPos = pDesc->vGoalPos;
	pDesc->fSpeedPerSec = 10.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vStartPos);

	m_pGameInstance->Play_Sound(L"Automobile_Pass_01.wav", CSound_Manager::CH_AUTOMOBILE, 1.f);

	return S_OK;
}

_int CAutomobile::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_pTransformCom->Go_Straight(fTimeDelta);
	_vector vGoalDir = XMVector3Normalize(XMLoadFloat4(&m_vGoalPos) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	m_pTransformCom->Rotation_To_Dir(vGoalDir);

	m_fCreateInstance += fTimeDelta;
	if (m_fCreateInstance >= 0.125f * 0.25f)
	{
		_float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		WorldMatrix._42 += 0.5f;

		GAMEOBJECT_DESC pObjectDesc = {};
		pObjectDesc.bInitWorldMatrix = true;
		pObjectDesc.InitWorldFloat4x4 = WorldMatrix;

		m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Instance_Puff"), &pObjectDesc);
		m_fCreateInstance = 0.f;
	}


	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	if ( XMVectorGetX(XMVector3Length(vPos - XMLoadFloat4(&m_vGoalPos))) <= 0.25f)
	{
		m_bDead = true;
	}

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CAutomobile::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CAutomobile::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		if (i == 3)
		{
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		 }

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CAutomobile::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Automobile"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(2.f, 2.f, 3.f);
	ColliderDesc.vCenter = _float3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAutomobile::Bind_ShaderResources()
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

CAutomobile* CAutomobile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAutomobile* pInstance = new CAutomobile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAutomobile"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAutomobile::Clone(void* pArg)
{
	CAutomobile* pInstance = new CAutomobile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAutomobile"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAutomobile::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
