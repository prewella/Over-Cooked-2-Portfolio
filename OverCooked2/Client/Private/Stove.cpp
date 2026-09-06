#include "stdafx.h"
#include "Stove.h"

#include "Stove_Flame.h"
#include "Ingredient.h"
#include "Cooker.h"

CStove::CStove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CStove::CStove(const CStove& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CStove::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStove::Initialize(void* pArg)
{
	STOVE_DESC* pDesc = (STOVE_DESC*)pArg;

	m_eInitCookerType = pDesc->eInitCookerType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_FlameObject()))
		return E_FAIL;

	if (m_eInitCookerType == COOKER_FRYINGPAN || m_eInitCookerType == COOKER_POT)
	{
		if (FAILED(Add_Cooker()))
			return E_FAIL;
	}

	m_eType = OBJECT_STOVE;

	return S_OK;
}

int CStove::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;
	m_IsFlameRender = false;
	
	if (nullptr != m_pBelonging)
	{
		Set_On_Belonging();
		if (((CCooker*)m_pBelonging)->IsHaveIngredient())
		{
			((CCooker*)m_pBelonging)->Cooking(fTimeDelta);
			m_IsFlameRender = true;
		}
	}

	m_pFlameObject->Tick(fTimeDelta);

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CStove::Late_Tick(_float fTimeDelta)
{
	m_pFlameObject->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CStove::Render()
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
			if (FAILED(m_pShaderCom->Begin(4)))
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

_bool CStove::Check_PutDown(class CBelonging* pBelonging)
{
	if (nullptr != m_pBelonging)
	{
		if (((CCooker*)m_pBelonging)->Check_PutDown(pBelonging))
			return true;
	}
	else if (BELONGING_COOKER == pBelonging->Get_Type() && 
		(dynamic_cast<CCooker*>(pBelonging)->Get_CookerType() == COOKER_FRYINGPAN 
			|| dynamic_cast<CCooker*>(pBelonging)->Get_CookerType() == COOKER_POT))
		return true;
	return false;
}

HRESULT CStove::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Stove"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStove::Add_FlameObject()
{
	CStove_Flame::STOVEFLAME_DESC	FlameDesc{};

	FlameDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	FlameDesc.pIsRender = &m_IsFlameRender;

	m_pFlameObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Stove_Flame"), &FlameDesc));
	if (nullptr == m_pFlameObject)
		return E_FAIL;

	return S_OK;
}

HRESULT CStove::Add_Cooker()
{
	CCooker::COOKER_DESC CookerDesc = {};
	CookerDesc.eCookerType = m_eInitCookerType;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pBelonging, LEVEL_GAMEPLAY, TEXT("Layer_Belongings"),
		TEXT("Prototype_GameObject_Cooker"), &CookerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStove::Add_Cooker(COOKER_TYPE eAddCooker)
{
	CCooker::COOKER_DESC CookerDesc = {};
	CookerDesc.eCookerType = eAddCooker;

	if (FAILED(m_pGameInstance->Add_Clone((CGameObject**)&m_pBelonging, LEVEL_GAMEPLAY, TEXT("Layer_Belongings"),
		TEXT("Prototype_GameObject_Cooker"), &CookerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStove::Bind_ShaderResources()
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

CStove* CStove::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStove* pInstance = new CStove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStove"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStove::Clone(void* pArg)
{
	CStove* pInstance = new CStove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStove"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStove::Free()
{
	__super::Free();

	Safe_Release(m_pFlameObject);
	Safe_Release(m_pColliderCom);
}
