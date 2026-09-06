#include "stdafx.h"
#include "Dispenser.h"

#include "Conveyor.h"
#include "Plate_Return.h"
#include "Stove.h"
#include "Ingredient.h"
#include "Cooker.h"
#include "Plate.h"

CDispenser::CDispenser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CDispenser::CDispenser(const CDispenser& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CDispenser::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDispenser::Initialize(void* pArg)
{
	DISPENSER_DESC* pDesc = (DISPENSER_DESC*)pArg;
	m_iNumIngredient = pDesc->iNumCreateIngredient;
	m_CreateIngredientTypes.resize(m_iNumIngredient);

	for (_uint i = 0; i < m_iNumIngredient; i++)
	{
		m_CreateIngredientTypes[i] = pDesc->eIngredientType[i];
	}


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_eType = OBJECT_DISPENSER;

	return S_OK;
}

#define SUPPLY_TIME 2.5f

int CDispenser::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	m_fCreateIngredientTime += fTimeDelta;
	if (m_fCreateIngredientTime >= SUPPLY_TIME)
	{
		m_fCreateIngredientTime = 0.f;
		Create_Ingredient();
	}

	Moving_Belonging();

	m_fAccFalling += fTimeDelta * 0.3f;

	m_pGameInstance->Add_Collision_Group(this);
	m_pForwardColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return EVENT_NONE;
}

void CDispenser::Late_Tick(_float fTimeDelta)
{
	Check_Collision_InteractObj();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CDispenser::Render()
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
			if (i == 1)
			{
				if (FAILED(m_pShaderCom->Begin(8)))
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
			if (i == 1)
			{
				if (FAILED(m_pShaderCom->Begin(7)))
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
	m_pForwardColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CDispenser::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Dispenser"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider2"), (CComponent**)&m_pForwardColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDispenser::Bind_ShaderResources()
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


void CDispenser::Moving_Belonging()
{
	if (nullptr == m_pBelonging)
		return;

	CTransform* pBelongingTransform = (CTransform*)m_pBelonging->Get_Component(g_strTransformTag);

	_vector vBelongingPosition = pBelongingTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vBelongingPosition += m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK) * 0.025f;

	pBelongingTransform->Set_State(CTransform::STATE_POSITION, vBelongingPosition);

	vBelongingPosition = XMVectorSetY(vBelongingPosition, 0.f);
	if (XMVectorGetX(XMVector3Length(vBelongingPosition - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION))) >= 0.5f)
		PassBelonging();
}

void CDispenser::Create_Ingredient()
{
	CIngredient::INGREDIENT_DESC IngredientDesc = {};
	if (!m_IsCreateIdx)
		IngredientDesc.eIngredientType = m_CreateIngredientTypes[0];
	else
		IngredientDesc.eIngredientType = m_CreateIngredientTypes[1];

	m_IsCreateIdx = !m_IsCreateIdx;

	m_pGameInstance->Add_Clone((CGameObject**)&m_pBelonging, LEVEL_GAMEPLAY, TEXT("Layer_Belongings"), TEXT("Prototype_GameObject_Ingredient"), &IngredientDesc);
	Set_On_Belonging();
}

void CDispenser::PassBelonging()
{
	if (nullptr != m_pCollideObj)
	{
		if (m_pCollideObj->Get_Type() == OBJECT_CONVEYOR)
		{
			_float4 vLook;
			XMStoreFloat4(&vLook, m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
			if (((CConveyor*)m_pCollideObj)->ReceiveBelonging(m_pBelonging, vLook))
			{
				Safe_Release(m_pBelonging);
				m_pBelonging = nullptr;
			}
			else
			{
				m_pBelonging->Set_Drop();
				Safe_Release(m_pBelonging);
				m_pBelonging = nullptr;
			}

		}
		else if (m_pCollideObj->Get_Type() == OBJECT_WASTE_BIN)
		{
			if (m_pCollideObj->Check_PutDown(m_pBelonging))
			{
				switch (m_pBelonging->Get_Type())
				{
				case Client::BELONGING_INGREDIENT:
					Safe_Release(m_pBelonging);
					m_pBelonging->Set_Erase();
					m_pBelonging = nullptr;

					break;
				case Client::BELONGING_PLATE:
				{
					list<CGameObject*>* pInteractList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_Interacts"));
					if (nullptr == pInteractList)
						return;
					for (auto& InteractObj : *pInteractList)
					{
						if (((CInteractObj*)InteractObj)->Get_Type() == OBJECT_PLATE_RETURN)
						{
							((CPlate_Return*)InteractObj)->Return_Plate();
							Safe_Release(m_pBelonging);
							m_pBelonging->Set_Erase();
							m_pBelonging = nullptr;
						}
					}
				}
				break;
				case Client::BELONGING_COOKER:
				{
					list<CGameObject*>* pInteractList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_Interacts"));
					if (nullptr == pInteractList)
						return;
					for (auto& InteractObj : *pInteractList)
					{
						if (((CInteractObj*)InteractObj)->Get_Type() == OBJECT_STOVE && BELONGING_NONE == ((CInteractObj*)InteractObj)->Get_HaveObjType())
						{
							((CStove*)InteractObj)->Add_Cooker(((CCooker*)m_pBelonging)->Get_CookerType());
							Safe_Release(m_pBelonging);
							m_pBelonging->Set_Erase();
							m_pBelonging = nullptr;
							break;
						}
					}
				}
				break;
				}
			}
		}
		else
		{
			switch (m_pCollideObj->PutDown_Belonging(m_pBelonging))
			{
				// 상호작용한 오브젝트가 들고 있던게 없음 -> 그냥 놓음
			case Client::BELONGING_NONE:
				Safe_Release(m_pBelonging);
				m_pBelonging = nullptr;
				break;
			case Client::BELONGING_PLATE:
				switch (m_pBelonging->Get_Type())
				{
				case Client::BELONGING_INGREDIENT:
					m_pBelonging->Set_Erase();
					Safe_Release(m_pBelonging);
					m_pBelonging = nullptr;
					break;
				case Client::BELONGING_PLATE:
					break;
				case Client::BELONGING_COOKER:
					((CCooker*)m_pBelonging)->ThrowAway_Ingredient();
					break;
				}
				break;
			case Client::BELONGING_COOKER:
				// 재료만 상호작용 했을 것이므로
				m_pBelonging->Set_Erase();
				Safe_Release(m_pBelonging);
				m_pBelonging = nullptr;
				break;
			case Client::BELONGING_INGREDIENT:
			case Client::BELONGING_DIRTY_PLATE:
			case Client::BELONGING_END:
				// 아무일도 없음
				break;
			}
		}
	}
	else
	{
		m_pBelonging->Set_Drop();
		Safe_Release(m_pBelonging);
		m_pBelonging = nullptr;
	}
}

void CDispenser::Check_Collision_InteractObj()
{
	_float fMinDis = 3.f;
	CGameObject* pIntersectObj = nullptr;

	list<CGameObject*>* pInteractList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_Interacts"));
	if (nullptr == pInteractList)
		return;

	for (auto& InteractObj : *pInteractList)
	{
		if (InteractObj == this)
			continue;

		CCollider* pCollider = (CCollider*)InteractObj->Get_Component(TEXT("Com_Collider"));
		if (nullptr == pCollider)
			continue;

		if (m_pForwardColliderCom->Intersect(pCollider))
		{
			_vector vColPos = ((CTransform*)InteractObj->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
			_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
			_float fDis = XMVectorGetX(XMVector3Length(vColPos - vPos));

			if (fMinDis > fDis)
			{
				fMinDis = fDis;
				pIntersectObj = InteractObj;
			}
		}
	}

	if (nullptr != pIntersectObj)
	{
		if (m_pCollideObj != pIntersectObj)
		{
			if (nullptr != m_pCollideObj)
				Safe_Release(m_pCollideObj);

			m_pCollideObj = (CInteractObj*)pIntersectObj;
			Safe_AddRef(m_pCollideObj);
		}
	}
	else
	{
		if (nullptr != m_pCollideObj)
			Safe_Release(m_pCollideObj);

		m_pCollideObj = (CInteractObj*)pIntersectObj;
		Safe_AddRef(m_pCollideObj);
	}
}

CDispenser* CDispenser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDispenser* pInstance = new CDispenser(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDispenser"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDispenser::Clone(void* pArg)
{
	CDispenser* pInstance = new CDispenser(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDispenser"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDispenser::Free()
{
	__super::Free();

	Safe_Release(m_pForwardColliderCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pCollideObj);
}
