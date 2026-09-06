#include "stdafx.h"
#include "Table_Chop.h"

#include "Part_ChopKnife.h"

#include "Ingredient.h"
#include "Cooker.h"
#include "Plate.h"

CTable_Chop::CTable_Chop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CTable_Chop::CTable_Chop(const CTable_Chop& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CTable_Chop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTable_Chop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_eType = OBJECT_CHOPTABLE;

	return S_OK;
}

int CTable_Chop::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (nullptr == m_pBelonging)
		m_IsKnifeRender = true;
	else
		m_IsKnifeRender = false;

	m_pPartKnife->Tick(fTimeDelta);

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CTable_Chop::Late_Tick(_float fTimeDelta)
{
	m_pPartKnife->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CTable_Chop::Render()
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

HRESULT CTable_Chop::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Table_Chop"),
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

HRESULT CTable_Chop::Add_PartObjects()
{
	CPart_ChopKnife::CHOPKNIFE_DESC	KnifeDesc{};

	KnifeDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	KnifeDesc.pIsRender = &m_IsKnifeRender;

	m_pPartKnife = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_ChopKnife"), &KnifeDesc));
	if (nullptr == m_pPartKnife)
		return E_FAIL;

	return S_OK;
}

HRESULT CTable_Chop::Bind_ShaderResources()
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


void CTable_Chop::Chop_Ingredient()
{
	if (Check_Reaction())
	{
		CIngredient* pIngredient = (CIngredient*)m_pBelonging;
		pIngredient->Chop_Ingredient();
	}
}

_bool CTable_Chop::Check_Reaction()
{
	if (m_pBelonging == nullptr)
		return false;

	if (m_pBelonging->Get_Type() == BELONGING_INGREDIENT && ((CIngredient*)m_pBelonging)->Get_ReactionType() == REACTION_CHOP_TABLE)
	{
		if (((CIngredient*)m_pBelonging)->Get_ModelCom()->Get_CurrentAnimationIdx() < CIngredient::ANIM_CUTTED)
			return true;
		else if (((CIngredient*)m_pBelonging)->Get_ModelCom()->Get_CurrentAnimationIdx() == CIngredient::ANIM_NONCUT)
			return true;
	}

	return false;
}

CTable_Chop* CTable_Chop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTable_Chop* pInstance = new CTable_Chop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTable_Chop"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTable_Chop::Clone(void* pArg)
{
	CTable_Chop* pInstance = new CTable_Chop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTable_Chop"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTable_Chop::Free()
{
	__super::Free();

	Safe_Release(m_pPartKnife);

	Safe_Release(m_pColliderCom);
}
