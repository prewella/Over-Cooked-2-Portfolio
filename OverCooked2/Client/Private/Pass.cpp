#include "stdafx.h"
#include "Pass.h"

#include "Light.h"

#include "Plate.h"

CPass::CPass(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CPass::CPass(const CPass& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CPass::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPass::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Ready_Light();

	m_eType = OBJECT_PASS;

	return S_OK;
}

int CPass::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_fAccFalling += fTimeDelta * 0.3f;

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CPass::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPass::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (m_IsAdjacent)
		{
			if ( i == 2)
			{
				if (FAILED(m_pShaderCom->Begin(5)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(4)))
					return E_FAIL;
			}
		}
		else
		{
			if ( i == 2)
			{
				if (FAILED(m_pShaderCom->Begin(5)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(0)))
					return E_FAIL;
			}
		}

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

_bool CPass::Check_PutDown(CBelonging* pBelonging)
{
	if (pBelonging->Get_Type() == BELONGING_PLATE)
		return true;

	return false;
}

HRESULT CPass::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pass"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(2.f, 1.f, 1.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPass::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAccFalling", &m_fAccFalling, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPass::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_vector vUp = m_pTransformCom->Get_State_Vector(CTransform::STATE_UP);
	_vector vSignLightPos = vPos + (vUp * 0.25f);
	_float4 vSignPos;

	XMStoreFloat4(&vSignPos, vSignLightPos);

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(vSignPos.x, vSignPos.y , vSignPos.z, 1.f);
	LightDesc.fRange = 0.75f;

	LightDesc.vDiffuse = _float4(1.5f, 1.5f, 0.25f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

CPass* CPass::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPass* pInstance = new CPass(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPass"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPass::Clone(void* pArg)
{
	CPass* pInstance = new CPass(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPass"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPass::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
