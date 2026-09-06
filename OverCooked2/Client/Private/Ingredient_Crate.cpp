#include "stdafx.h"
#include "Ingredient_Crate.h"

#include "Ingredient.h"
#include "Cooker.h"
#include "Plate.h"

CIngredient_Crate::CIngredient_Crate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CIngredient_Crate::CIngredient_Crate(const CIngredient_Crate& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CIngredient_Crate::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIngredient_Crate::Initialize(void* pArg)
{
	INGREDIENTCRATE_DESC* pDesc = (INGREDIENTCRATE_DESC*)pArg;

	if (pDesc->eIngredientType < INGREDIENT_NONE)
		m_eIngredientType = INGREDIENT_TOMATO;
	else
		m_eIngredientType = pDesc->eIngredientType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eType = OBJECT_INGREDIENTCRATE;

	return S_OK;
}

int CIngredient_Crate::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (m_pModelCom->Get_CurrentAnimationIdx() == ANIM_OPEN && m_pModelCom->isFinished())
	{
		m_pModelCom->Set_Animation(ANIM_IDLE, true);
	}

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CIngredient_Crate::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CIngredient_Crate::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (i == 1)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			m_pModelCom->Render(i);
		}
		else
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (m_IsAdjacent)
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
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

CBelonging* CIngredient_Crate::Create_Ingredient()
{
	m_pModelCom->Set_Animation(ANIM_OPEN, false);
	CIngredient* pIngredient = nullptr;
	CIngredient::INGREDIENT_DESC IngredientDesc = {};
	IngredientDesc.eIngredientType = m_eIngredientType;

	m_pGameInstance->Add_Clone((CGameObject**)&pIngredient, LEVEL_GAMEPLAY, TEXT("Layer_Belongings"), TEXT("Prototype_GameObject_Ingredient"), &IngredientDesc);

	return pIngredient;
}

HRESULT CIngredient_Crate::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ingredient_Crate"),
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

HRESULT CIngredient_Crate::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint iNumRow = 5, iNumCol = 5, iIngredientIdx = (_uint)m_eIngredientType - 1;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumCol", &iNumCol, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumRow", &iNumRow, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIngredientIdx, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

CIngredient_Crate* CIngredient_Crate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIngredient_Crate* pInstance = new CIngredient_Crate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTable"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CIngredient_Crate::Clone(void* pArg)
{
	CIngredient_Crate* pInstance = new CIngredient_Crate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTable"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIngredient_Crate::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
