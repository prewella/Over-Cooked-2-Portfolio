#include "stdafx.h"
#include "Chef.h"

#include "Ingredient.h"
#include "Plate.h"
#include "Dirty_Plate.h"
#include "Cooker.h"

#include "Part_Body.h"
#include "Part_Hand.h"
#include "Part_Cooker.h"
#include "Part_Control_Mark.h"

#include "Table_Chop.h"
#include "Ingredient_Crate.h"
#include "Pass.h"
#include "Sink.h"

CChef::CChef(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCharacter{ pDevice, pContext }
{
}

CChef::CChef(const CChef& rhs)
	:CCharacter{ rhs }
{
}

HRESULT CChef::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChef::Initialize(void* pArg)
{
	GAMEOBJECT_DESC pDesc = {};
	pDesc.fSpeedPerSec = 7.5f;
	pDesc.fRotationPerSec = 5.f;

	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

int CChef::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (m_pGameInstance->Get_DIKeyState(DIK_V) == EKeyState::DOWN)
		m_IsCollide = !m_IsCollide;

	Is_Stun(fTimeDelta);

	MoveControl(fTimeDelta);

	Dash(fTimeDelta);

	if (nullptr == m_pHoldingObject)
		PickUp();
	else
		PutDown();
	Throw(fTimeDelta);

	Reaction_With_Obj(fTimeDelta);

	Holding_Object_Front();


	for (auto& Pair : m_PartObjects)
		Pair.second->Tick(fTimeDelta);

	m_pGameInstance->Add_Collision_Group(this);
	m_pInteractColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	return EVENT_NONE;
}

void CChef::Late_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_PartObjects)
		Pair.second->Late_Tick(fTimeDelta);

	Check_Collision_InteractObj();
	Check_Collision_BelongingObj();
	Check_Collision_WarningObj();


	//if (m_IsCollide)
	//{
	//	_vector vPushDir = XMLoadFloat4(&Check_Collision_Restricted_Area(fTimeDelta));
	//	if (!XMVector3Equal(vPushDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
	//	{
	//		m_pTransformCom->Move_To_SetDir(vPushDir, fTimeDelta * 0.25f, false);
	//		/*_vector vPushPos = vPos + (XMVector3Normalize(vPushDir) * fTimeDelta * 10.f);

	//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPushPos);*/
	//	}
	//}

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CChef::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pInteractColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CChef::Add_Components()
{
	/*CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.8f, 1.f, 0.8f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;*/

	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _float3(0.f, 0.4f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	CBounding_Sphere::BOUNDING_SPHERE_DESC		InteractColliderDesc{};

	/* 로컬상의 정보를 셋팅한다. */
	InteractColliderDesc.fRadius = 1.f;
	InteractColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_InteractCollider"), (CComponent**)&m_pInteractColliderCom, &InteractColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChef::Add_PartObjects()
{
	/* For.Part_Body */
	CPartObject* pPartBody = { nullptr };
	CPart_Body::BODY_DESC	BodyDesc{};

	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	BodyDesc.pState = &m_eAnim;
	BodyDesc.strModelTag = TEXT("Prototype_Component_Model_Chef_Mel_Body");

	pPartBody = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_Body"), &BodyDesc));
	if (nullptr == pPartBody)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Body"), pPartBody);

	CModel* pBodyModel = (CModel*)pPartBody->Get_ModelCom();

	/* For.Part_LHand */
	CPartObject* pPartLHand = { nullptr };
	CPart_Hand::HAND_DESC	LHandDesc{};


	LHandDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	LHandDesc.pSocket = pBodyModel->Get_BonePtr("LeftHand");
	LHandDesc.pIsGrip = &m_bState[STATE_GRIP_L];
	LHandDesc.strModelTags[0] = TEXT("Prototype_Component_Model_Chef_Mel_Hand_Open_L");
	LHandDesc.strModelTags[1] = TEXT("Prototype_Component_Model_Chef_Mel_Hand_Grip_L");

	pPartLHand = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_Hand"), &LHandDesc));
	if (nullptr == pPartLHand)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_LHand"), pPartLHand);

	/* For.Part_RHand */
	CPartObject* pPartRHand = { nullptr };
	CPart_Hand::HAND_DESC	RHandDesc{};

	RHandDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	RHandDesc.pSocket = pBodyModel->Get_BonePtr("HeldItem");
	RHandDesc.pIsGrip = &m_bState[STATE_GRIP_R];
	RHandDesc.strModelTags[0] = TEXT("Prototype_Component_Model_Chef_Mel_Hand_Open_R");
	RHandDesc.strModelTags[1] = TEXT("Prototype_Component_Model_Chef_Mel_Hand_Grip_R");

	pPartRHand = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_Hand"), &RHandDesc));
	if (nullptr == pPartRHand)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_RHand"), pPartRHand);

	/* For.Part_Cooker */
	CPartObject* pPartCooker = { nullptr };
	CPart_Cooker::COOKER_DESC	CookerDesc{};

	CookerDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	CookerDesc.pSocket = pBodyModel->Get_BonePtr("HeldItem");
	CookerDesc.pTool = &m_eTool;

	pPartCooker = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_Cooker"), &CookerDesc));
	if (nullptr == pPartCooker)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Cooker"), pPartCooker);

	/* For.Part_Control_Mark */
	CPartObject* pPartControlMark = { nullptr };
	CPart_Control_Mark::CONTROL_MARK_DESC	Control_MarkDesc{};

	Control_MarkDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
	Control_MarkDesc.pIsControl = &m_IsControl;
	Control_MarkDesc.pIsThrowing = &m_bState[STATE_THROW_HOLD];

	pPartControlMark = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_Control_Mark"), &Control_MarkDesc));
	if (nullptr == pPartControlMark)
		return E_FAIL;

	m_PartObjects.emplace(TEXT("Part_Control_Mark"), pPartControlMark);

	return S_OK;
}

void CChef::MoveControl(_float fTimeDelta)
{
	_vector vMoveDir = { 0.f,0.f ,0.f ,0.f };

	if (!m_IsStun && m_IsControl && !m_bState[STATE_THROW_HOLD] && !m_bState[STATE_DASH])
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) == EKeyState::PRESSING)
		{
			vMoveDir += {-1.f, 0.f, 0.f};
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) == EKeyState::PRESSING)
		{
			vMoveDir += {1.f, 0.f, 0.f};
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_UP) == EKeyState::PRESSING)
		{
			vMoveDir += { 0.f, 0.f, 1.f};
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) == EKeyState::PRESSING)
		{
			vMoveDir += {0.f, 0.f, -1.f};
		}
	}

	if (!XMVector3Equal(vMoveDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
	{
		_vector vPushDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		if (m_IsCollide)
			vPushDir = XMLoadFloat4(&Check_Collision_Restricted_Area(fTimeDelta));

		if (XMVector3Equal(vPushDir * -1.f, vMoveDir))
		{
			m_pTransformCom->Rotation_To_Dir(vMoveDir);
			m_eAnim = m_bState[STATE_HOLD] ? ANIM_IDLE_HOLDING : ANIM_IDLE;
			//m_eAnim = m_bState[STATE_HOLD] ? ANIM_WALK_HOLDING : ANIM_WALK;
		}
		else
		{
			m_pTransformCom->Move_To_SetDir(vMoveDir + vPushDir, fTimeDelta, false);
			m_pTransformCom->Rotation_To_Dir(vMoveDir);
			m_eAnim = m_bState[STATE_HOLD] ? ANIM_WALK_HOLDING : ANIM_WALK;

			m_pGameInstance->Play_Sound(L"Chef_Footstep_01.wav", CSound_Manager::CH_CHEF_STEP, 1.f);

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
		}
	}
	else if (m_eAnim == ANIM_WALK || m_eAnim == ANIM_WALK_HOLDING)
	{
		m_eAnim = m_bState[STATE_HOLD] ? ANIM_IDLE_HOLDING : ANIM_IDLE;
		m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_STEP);
	}

}

void CChef::Dash(_float fTimeDelta)
{
	if (!m_IsStun && m_IsControl)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) == EKeyState::DOWN)
		{
			_vector vPushDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			if (m_IsCollide)
				vPushDir = XMLoadFloat4(&Check_Collision_Restricted_Area(fTimeDelta));

			if (XMVector3Equal(vPushDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
			{
				m_bState[STATE_DASH] = true;
				m_eAnim = m_bState[STATE_HOLD] ? ANIM_DASH_HOLDING : ANIM_DASH;

				m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_DASH);
				m_pGameInstance->Play_Sound(L"Chef_Dash_03.wav", CSound_Manager::CH_CHEF_DASH, 1.f);
			}
		}
	}

	if (m_bState[STATE_DASH])
	{
		_vector vRotationDir = { 0.f,0.f ,0.f ,0.f };

		if (!m_IsStun && m_IsControl) {
			if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) == EKeyState::PRESSING)
				vRotationDir += {-1.f, 0.f, 0.f};
			if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) == EKeyState::PRESSING)
				vRotationDir += {1.f, 0.f, 0.f};
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) == EKeyState::PRESSING)
				vRotationDir += { 0.f, 0.f, 1.f};
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) == EKeyState::PRESSING)
				vRotationDir += {0.f, 0.f, -1.f};
		}

		if (!XMVector3Equal(vRotationDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			m_pTransformCom->Rotation_To_Dir(vRotationDir);
		}

		_vector vPushDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		if (m_IsCollide)
			vPushDir = XMLoadFloat4(&Check_Collision_Restricted_Area(fTimeDelta));

		if (!XMVector3Equal(vPushDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			m_bState[STATE_DASH] = false;
			fDashTime = Dash_Time;
		}

		m_pTransformCom->Go_Straight(fTimeDelta * 2.f);
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

		if (fDashTime > 0)
			fDashTime -= fTimeDelta;
		else
		{
			m_bState[STATE_DASH] = false;
			fDashTime = Dash_Time;
		}
	}
	else if (m_eAnim == ANIM_DASH || m_eAnim == ANIM_DASH_HOLDING)
		m_eAnim = m_bState[STATE_HOLD] ? ANIM_IDLE_HOLDING : ANIM_IDLE;
}

void CChef::Throw(_float fTimeDelta)
{
	if (nullptr == m_pHoldingObject || m_pHoldingObject->Get_Type() != BELONGING_INGREDIENT)
		return;

	if (!m_IsStun && m_IsControl)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) == EKeyState::DOWN)
		{
			m_bState[STATE_THROW_HOLD] = true;
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) == EKeyState::PRESSING)
		{
			_vector vLookDir = { 0.f,0.f ,0.f ,1.f };

			if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) == EKeyState::PRESSING)
			{
				vLookDir += {-1.f, 0.f, 0.f};
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) == EKeyState::PRESSING)
			{
				vLookDir += {1.f, 0.f, 0.f};
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_UP) == EKeyState::PRESSING)
			{
				vLookDir += { 0.f, 0.f, 1.f};
			}
			if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) == EKeyState::PRESSING)
			{
				vLookDir += {0.f, 0.f, -1.f};
			}

			if (!XMVector3Equal(vLookDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
			{
				m_pTransformCom->Rotation_To_Dir(vLookDir);
			}
		}
		if (m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) == EKeyState::UP)
		{
			m_bState[STATE_THROW_HOLD] = m_bState[STATE_HOLD] = false;
			((CIngredient*)m_pHoldingObject)->Set_Throwing(m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK));
			Safe_Release(m_pHoldingObject);
			m_pHoldingObject = nullptr;
			m_eAnim = ANIM_THROW;
		}
	}
	else
	{
		if (m_bState[STATE_THROW_HOLD])
		{
			m_bState[STATE_THROW_HOLD] = m_bState[STATE_HOLD] = false;
			((CIngredient*)m_pHoldingObject)->Set_Throwing(m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK));
			Safe_Release(m_pHoldingObject);
			m_pHoldingObject = nullptr;
			m_eAnim = ANIM_THROW;
		}
	}
}

void CChef::PutDown()
{
	if (nullptr == m_pHoldingObject || nullptr != m_pCollideInteractObj)
		return;

	if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
	{
		// 바닥에 내려놓음
		m_pHoldingObject->Set_Drop();
		Safe_Release(m_pHoldingObject);
		m_pHoldingObject = nullptr;

		m_bState[STATE_HOLD] = false;
		m_eAnim = ANIM_IDLE;

		m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_INTERACT);
		m_pGameInstance->Play_Sound(L"Chef_PutDown_01.wav", CSound_Manager::CH_CHEF_INTERACT, 1.f);
	}
}

void CChef::PickUp()
{
	if (nullptr != m_pHoldingObject || nullptr == m_pCollideBelongingObj || nullptr != m_pCollideInteractObj)
		return;

	if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
	{
		m_pHoldingObject = m_pCollideBelongingObj;
		Safe_AddRef(m_pHoldingObject);

		m_bState[STATE_HOLD] = true;
		m_eAnim = ANIM_IDLE_HOLDING;

		m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_INTERACT);
		m_pGameInstance->Play_Sound(L"Chef_PickUp_01.wav", CSound_Manager::CH_CHEF_INTERACT, 1.f);
	}
}

void CChef::Reaction_With_Obj(_float fTimeDelta)
{
	if (nullptr == m_pCollideInteractObj)
	{
		Stop_Interacting();
		m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_INTERACT);
		return;
	}

	switch (m_pCollideInteractObj->Get_Type())
	{
	case Client::OBJECT_INGREDIENTCRATE:
		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
		{
			if (nullptr == m_pHoldingObject)
			{
				if (BELONGING_NONE == m_pCollideInteractObj->Get_HaveObjType())
				{
					m_pHoldingObject = ((CIngredient_Crate*)m_pCollideInteractObj)->Create_Ingredient();
					m_bState[STATE_HOLD] = true;
					m_eAnim = ANIM_IDLE_HOLDING;
					m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_INTERACT);
					m_pGameInstance->Play_Sound(L"Chef_PickUp_01.wav", CSound_Manager::CH_CHEF_INTERACT, 1.f);
				}
				else
				{
					Take_Object_On_CollideObj();
				}
			}
			else if (m_pCollideInteractObj->Check_PutDown(m_pHoldingObject))
				PutDown_Object_On_CollideObj();
		}
		break;

	case Client::OBJECT_CHOPTABLE:
	{
		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) == EKeyState::DOWN)
		{
			if (m_pCollideInteractObj->Check_Reaction())
			{
				m_bState[STATE_INTERACTION] = m_bState[STATE_GRIP_R] = true;
				m_eTool = TOOL_KNIFE;
				m_eAnim = ANIM_CHOP;
			}
		}
		if (m_bState[STATE_INTERACTION])
		{
			CPart_Body* pPart_Body = (CPart_Body*)m_PartObjects.find(TEXT("Part_Body"))->second;

			if (!m_pCollideInteractObj->Check_Reaction())
			{
				m_bState[STATE_INTERACTION] = m_bState[STATE_GRIP_R] = false;
				m_eTool = TOOL_NONE;
				m_eAnim = m_bState[STATE_HOLD] ? ANIM_IDLE_HOLDING : ANIM_IDLE;
			}
			else if (pPart_Body->Get_ModelCom()->isFinished())
			{
				// 테이블 위에 있는 재료 썰기
				((CTable_Chop*)m_pCollideInteractObj)->Chop_Ingredient();
				m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_CHOP);
				m_pGameInstance->Play_Sound(L"KnifeChop.wav", CSound_Manager::CH_CHEF_CHOP, 1.f);
			}
		}
		else
		{
			m_bState[STATE_GRIP_R] = false;
			m_eTool = TOOL_NONE;
			if (m_eAnim == ANIM_CHOP)
				m_eAnim = m_bState[STATE_HOLD] ? ANIM_IDLE_HOLDING : ANIM_IDLE;
		}
	}
	case Client::OBJECT_TABLE:
	case Client::OBJECT_STOVE:
	case Client::OBJECT_FRYER:
	case Client::OBJECT_CONVEYOR:
		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
		{
			if (nullptr == m_pHoldingObject)
				Take_Object_On_CollideObj();
			else if (m_pCollideInteractObj->Check_PutDown(m_pHoldingObject))
				PutDown_Object_On_CollideObj();
		}
		break;

	case Client::OBJECT_WASTE_BIN:
		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
		{
			if (nullptr != m_pHoldingObject && m_pCollideInteractObj->Check_PutDown(m_pHoldingObject))
			{
				switch (m_pHoldingObject->Get_Type())
				{
				case Client::BELONGING_INGREDIENT:
					Safe_Release(m_pHoldingObject);
					m_pHoldingObject->Set_Erase();
					m_pHoldingObject = nullptr;
					m_bState[STATE_HOLD] = false;
					m_eAnim = ANIM_IDLE;

					break;
				case Client::BELONGING_PLATE:
					((CPlate*)m_pHoldingObject)->Waste_Plated_Ingredient();
					break;
				case Client::BELONGING_COOKER:
					((CCooker*)m_pHoldingObject)->ThrowAway_Ingredient();
					break;
				}

				m_pGameInstance->Play_Sound(L"TrashCan.wav", CSound_Manager::CH_WASTEBIN, 1.f);
			}
		}
		break;

	case Client::OBJECT_PLATE_RETURN:
		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
		{
			if (nullptr == m_pHoldingObject)
				Take_Object_On_CollideObj();
			else if (m_pCollideInteractObj->Check_PutDown(m_pHoldingObject))
				PutDown_Object_On_CollideObj();
		}
		break;

	case Client::OBJECT_SINK:

		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_LCONTROL) == EKeyState::DOWN)
		{
			// 싱크대안에 접시가 있을때 상호작용
			if (m_pCollideInteractObj->Check_Reaction() &&
				((CSink*)m_pCollideInteractObj)->Check_In_ReactionArea(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION)))
			{
				m_bState[STATE_INTERACTION] = true;
				m_eAnim = ANIM_WASH;
			}
		}
		if (m_bState[STATE_INTERACTION])
		{
			if (!m_pCollideInteractObj->Check_Reaction() ||
				!((CSink*)m_pCollideInteractObj)->Check_In_ReactionArea(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION)))
			{
				m_bState[STATE_INTERACTION] = m_bState[STATE_GRIP_R] = false;
				m_eAnim = m_bState[STATE_HOLD] ? ANIM_IDLE_HOLDING : ANIM_IDLE;
				m_pGameInstance->StopSound(CSound_Manager::CH_SINK);
			}
			else
			{
				((CSink*)m_pCollideInteractObj)->Wash_Plate(fTimeDelta);
				m_eAnim = ANIM_WASH;

				CPart_Body* pPart_Body = (CPart_Body*)m_PartObjects.find(TEXT("Part_Body"))->second;
				if (m_eAnim == ANIM_WASH && pPart_Body->Get_ModelCom()->isFinished())
				{
					m_pGameInstance->StopSound(CSound_Manager::CH_SINK);
					m_pGameInstance->Play_Sound(L"Washing2.wav", CSound_Manager::CH_SINK, 1.f);
				}
			}
		}

		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
		{
			if (nullptr == m_pHoldingObject)
				Take_Object_On_CollideObj();
			else if (m_pHoldingObject->Get_Type() == BELONGING_DIRTY_PLATE)
			{
				((CSink*)m_pCollideInteractObj)->Add_DirtyPlate(m_pHoldingObject);
				Safe_Release(m_pHoldingObject);
				m_pHoldingObject->Set_Erase();
				m_pHoldingObject = nullptr;
				m_bState[STATE_HOLD] = false;
				m_eAnim = ANIM_IDLE;
				m_pGameInstance->StopSound(CSound_Manager::CH_SINK_PLATE);
				m_pGameInstance->Play_Sound(L"WashedPlate.wav", CSound_Manager::CH_SINK_PLATE, 1.f); 
			}
			else if (m_pCollideInteractObj->Check_PutDown(m_pHoldingObject))
				PutDown_Object_On_CollideObj();
		}

		break;

	case Client::OBJECT_PASS:

		if (nullptr == m_pHoldingObject)
			break;

		if (!m_IsStun && m_IsControl && m_pGameInstance->Get_DIKeyState(DIK_SPACE) == EKeyState::DOWN)
		{
			if (m_pCollideInteractObj->Check_PutDown(m_pHoldingObject))
			{
				((CPass*)m_pCollideInteractObj)->Serving_Dish(((CPlate*)m_pHoldingObject)->Serving_Plate());
				Safe_Release(m_pHoldingObject);
				m_pHoldingObject->Set_Erase();
				m_pHoldingObject = nullptr;
				m_bState[STATE_HOLD] = false;
				m_eAnim = ANIM_IDLE;
			}
		}
		break;
	}
}

void CChef::Stop_Interacting()
{
	if (m_bState[STATE_INTERACTION])
	{
		m_bState[STATE_INTERACTION] = m_bState[STATE_GRIP_R] = false;
		m_eTool = TOOL_NONE;
		m_eAnim = ANIM_IDLE;
	}
}

void CChef::Holding_Object_Front()
{
	if (nullptr == m_pHoldingObject)
		return;

	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);

	vPos += (vLook * 0.75f) + XMVectorSet(0.f, 0.75f, 0.f, 0.f);
	XMVectorSetW(vPos, 1.f);

	CTransform* pHoldObjTrans = (CTransform*)m_pHoldingObject->Get_Component(g_strTransformTag);

	pHoldObjTrans->Set_State(CTransform::STATE_POSITION, vPos);
	pHoldObjTrans->Rotation_To_Dir(vLook);
}

void CChef::Take_Object_On_CollideObj()
{
	m_pCollideInteractObj->Take_HaveObj(&m_pHoldingObject);
	if (nullptr != m_pHoldingObject)
	{
		Safe_AddRef(m_pHoldingObject);
		m_bState[STATE_HOLD] = true;
		m_eAnim = ANIM_IDLE_HOLDING;
		m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_INTERACT);
		m_pGameInstance->Play_Sound(L"Chef_PickUp_01.wav", CSound_Manager::CH_CHEF_INTERACT, 1.f);
	}
}

void CChef::PutDown_Object_On_CollideObj()
{
	switch (m_pCollideInteractObj->PutDown_Belonging(m_pHoldingObject))
	{
		// 상호작용한 오브젝트가 들고 있던게 없음 -> 그냥 놓음
	case Client::BELONGING_NONE:
		Safe_Release(m_pHoldingObject);
		m_pHoldingObject = nullptr;
		m_bState[STATE_HOLD] = false;
		m_eAnim = ANIM_IDLE;
		m_pGameInstance->StopSound(CSound_Manager::CH_CHEF_INTERACT);
		m_pGameInstance->Play_Sound(L"Chef_PutDown_01.wav", CSound_Manager::CH_CHEF_INTERACT, 1.f);
		break;
	case Client::BELONGING_PLATE:
		switch (m_pHoldingObject->Get_Type())
		{
		case Client::BELONGING_INGREDIENT:
			m_pHoldingObject->Set_Erase();
			Safe_Release(m_pHoldingObject);
			m_pHoldingObject = nullptr;
			m_bState[STATE_HOLD] = false;
			m_eAnim = ANIM_IDLE;
			break;
		case Client::BELONGING_PLATE:
			break;
		case Client::BELONGING_COOKER:
			((CCooker*)m_pHoldingObject)->ThrowAway_Ingredient();
			break;
		}
		break;
	case Client::BELONGING_COOKER:
		// 재료만 상호작용 했을 것이므로
		m_pHoldingObject->Set_Erase();
		Safe_Release(m_pHoldingObject);
		m_pHoldingObject = nullptr;
		m_bState[STATE_HOLD] = false;
		m_eAnim = ANIM_IDLE;
		break;
	case Client::BELONGING_INGREDIENT:
	case Client::BELONGING_DIRTY_PLATE:
	case Client::BELONGING_END:
		// 아무일도 없음
		break;
	}
}

void CChef::Is_Stun(_float fTimeDelta)
{
	if (!m_IsStun)
		return;

	m_fStunTime += fTimeDelta;

	if (m_fStunTime >= 1.5f)
	{
		CPart_Body* pPart_Body = (CPart_Body*)m_PartObjects.find(TEXT("Part_Body"))->second;

		if (m_eAnim == ANIM_STAND && pPart_Body->Get_ModelCom()->isFinished())
		{
			m_IsStun = false;
		}

		if (m_eAnim == ANIM_DEATH && pPart_Body->Get_ModelCom()->isFinished())
		{
			m_eAnim = ANIM_STAND;
		}
	}
}

void CChef::Check_Collision_InteractObj()
{
	_float fMinDis = 5.f;
	CGameObject* pIntersectObj = nullptr;

	list<CGameObject*>* pInteractList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_Interacts"));
	if (nullptr == pInteractList)
		return;
	for (auto& InteractObj : *pInteractList)
	{
		CCollider* pCollider = (CCollider*)InteractObj->Get_Component(TEXT("Com_Collider"));
		if (nullptr == pCollider)
		{
			continue;
		}

		if (m_pInteractColliderCom->Intersect(pCollider))
		{
			_vector vColPos = ((CTransform*)InteractObj->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
			_vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
			_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) + (vLook * 0.25f);
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
		if (m_pCollideInteractObj != pIntersectObj)
		{
			if (nullptr != m_pCollideInteractObj) {
				m_pCollideInteractObj->IsAdjacent(false);
				Safe_Release(m_pCollideInteractObj);
			}

			m_pCollideInteractObj = (CInteractObj*)pIntersectObj;
			m_pCollideInteractObj->IsAdjacent(true);
			Safe_AddRef(m_pCollideInteractObj);
			Stop_Interacting();
		}
	}
	else
	{
		if (nullptr != m_pCollideInteractObj)
		{
			m_pCollideInteractObj->IsAdjacent(false);
			Safe_Release(m_pCollideInteractObj);
		}
		m_pCollideInteractObj = (CInteractObj*)pIntersectObj;
		Safe_AddRef(m_pCollideInteractObj);
	}
}

void CChef::Check_Collision_BelongingObj()
{
	_float fMinDis = 5.f;
	CGameObject* pIntersectObj = nullptr;

	list<CGameObject*>* pBelongingList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_Belongings"));

	if (nullptr == pBelongingList)
		return;

	for (auto& BelongingObj : *pBelongingList)
	{
		CCollider* pCollider = (CCollider*)BelongingObj->Get_Component(TEXT("Com_Collider"));
		if (nullptr == pCollider)
		{
			continue;
		}

		if (m_pInteractColliderCom->Intersect(pCollider))
		{
			_vector vColPos = ((CTransform*)BelongingObj->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
			_vector vLook = m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK);
			_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) + (vLook * 0.25f);
			_float fDis = XMVectorGetX(XMVector3Length(vColPos - vPos));

			if (fMinDis > fDis)
			{
				fMinDis = fDis;
				pIntersectObj = BelongingObj;
			}
		}
	}

	if (nullptr != pIntersectObj)
	{
		if (m_pCollideBelongingObj != pIntersectObj)
		{
			if (nullptr != m_pCollideBelongingObj)
				Safe_Release(m_pCollideBelongingObj);

			m_pCollideBelongingObj = (CBelonging*)pIntersectObj;
			Safe_AddRef(m_pCollideBelongingObj);
		}
	}
	else
	{
		if (nullptr != m_pCollideBelongingObj)
			Safe_Release(m_pCollideBelongingObj);

		m_pCollideBelongingObj = (CBelonging*)pIntersectObj;
		Safe_AddRef(m_pCollideBelongingObj);
	}
}

void CChef::Check_Collision_WarningObj()
{
	list<CGameObject*>* pWarningObjList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_WarningObjects"));

	if (nullptr == pWarningObjList)
		return;

	for (auto& WarningObj : *pWarningObjList)
	{
		CCollider* pCollider = (CCollider*)WarningObj->Get_Component(TEXT("Com_Collider"));
		if (nullptr == pCollider)
		{
			continue;
		}

		if (m_pColliderCom->Intersect(pCollider))
		{
			m_eAnim = ANIM_DEATH;
			m_IsStun = true;
			m_fStunTime = 0.f;
			m_pGameInstance->Play_Sound(L"Angry_Horn.wav", CSound_Manager::CH_CRASH, 1.f);
			m_pGameInstance->Play_Sound(L"Chef_Slip_01.wav", CSound_Manager::CH_CHEF_SLIP, 1.f);

			_float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			WorldMatrix._42 += 0.01f;

			GAMEOBJECT_DESC pObjectDesc = {};
			pObjectDesc.bInitWorldMatrix = true;
			pObjectDesc.InitWorldFloat4x4 = WorldMatrix;

			m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Instance_Blood"), &pObjectDesc);
			

			if (nullptr != m_pHoldingObject)
			{
				// 바닥에 내려놓음
				m_pHoldingObject->Set_Drop();
				Safe_Release(m_pHoldingObject);
				m_pHoldingObject = nullptr;

				m_bState[STATE_HOLD] = false;
			}
		}
	}
}

_float4 CChef::Check_Collision_Restricted_Area(_float fTimeDelta)
{
	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	_vector vPushDir = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	list<CGameObject*>* pInteractList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_Interacts"));
	if (nullptr != pInteractList)
	{
		for (auto& InteractObj : *pInteractList)
		{
			CCollider* pCollider = (CCollider*)InteractObj->Get_Component(TEXT("Com_Collider"));
			if (nullptr == pCollider)
				continue;

			if (m_pColliderCom->Intersect(pCollider))
			{
				CTransform* pColTransform = (CTransform*)InteractObj->Get_Component(g_strTransformTag);
				_vector vColCenter = pColTransform->Get_State_Vector(CTransform::STATE_POSITION);
				_vector vDir = XMVector3Normalize(vPos - vColCenter);

				_vector vWorldFront = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				_vector vWorldRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);

				_float3 vScale = pColTransform->Get_Scaled();

				_vector vToPointDir[4] = {
					XMVector3Normalize(XMVectorSet(-vScale.x, 0.f, vScale.z, 0.f)),
					XMVector3Normalize(XMVectorSet(vScale.x, 0.f, vScale.z, 0.f)),
					XMVector3Normalize(XMVectorSet(vScale.x, 0.f, -vScale.z, 0.f)),
					XMVector3Normalize(XMVectorSet(-vScale.x, 0.f, -vScale.z, 0.f))
				};

				// 위
				if (XMVectorGetX(XMVector3Dot(vWorldFront, vDir)) > 0.f)
				{
					// 우측
					if (XMVectorGetX(XMVector3Dot(vWorldRight, vDir)) > 0.f)
					{
						if (XMVectorGetX(XMVector3Dot(vWorldFront, vToPointDir[1])) < XMVectorGetX(XMVector3Dot(vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
						else
							vPushDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
					}
					// 좌측
					else
					{
						if (XMVectorGetX(XMVector3Dot(vWorldFront, vToPointDir[0])) < XMVectorGetX(XMVector3Dot(vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
						else
							vPushDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
					}
				}
				// 아래
				else
				{
					// 우측
					if (XMVectorGetX(XMVector3Dot(vWorldRight, vDir)) > 0.f)
					{
						if (XMVectorGetX(XMVector3Dot(-vWorldFront, vToPointDir[2])) < XMVectorGetX(XMVector3Dot(-vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
						else
							vPushDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
					}
					// 좌측
					else
					{
						if (XMVectorGetX(XMVector3Dot(-vWorldFront, vToPointDir[3])) < XMVectorGetX(XMVector3Dot(-vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
						else
							vPushDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
					}
				}
			}
		}
	}


	list<CGameObject*>* pRestrictedList = m_pGameInstance->Get_LayerList(LEVEL_GAMEPLAY, TEXT("Layer_Collision"));
	if (nullptr != pRestrictedList)
	{
		for (auto& RestrictedObj : *pRestrictedList)
		{
			CCollider* pCollider = (CCollider*)RestrictedObj->Get_Component(TEXT("Com_Collider"));
			if (nullptr == pCollider)
				continue;

			if (m_pColliderCom->Intersect(pCollider))
			{
				CTransform* pColTransform = (CTransform*)RestrictedObj->Get_Component(g_strTransformTag);
				_vector vColCenter = pColTransform->Get_State_Vector(CTransform::STATE_POSITION);
				_vector vDir = XMVector3Normalize(vPos - vColCenter);

				_vector vWorldFront = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				_vector vWorldRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);

				_float3 vScale = pColTransform->Get_Scaled();

				_vector vToPointDir[4] = {
					XMVector3Normalize(XMVectorSet(-vScale.x, 0.f, vScale.z, 0.f)),
					XMVector3Normalize(XMVectorSet(vScale.x, 0.f, vScale.z, 0.f)),
					XMVector3Normalize(XMVectorSet(vScale.x, 0.f, -vScale.z, 0.f)),
					XMVector3Normalize(XMVectorSet(-vScale.x, 0.f, -vScale.z, 0.f))
				};

				// 위
				if (XMVectorGetX(XMVector3Dot(vWorldFront, vDir)) > 0.f)
				{
					// 우측
					if (XMVectorGetX(XMVector3Dot(vWorldRight, vDir)) > 0.f)
					{
						if (XMVectorGetX(XMVector3Dot(vWorldFront, vToPointDir[1])) < XMVectorGetX(XMVector3Dot(vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
						else
							vPushDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
					}
					// 좌측
					else
					{
						if (XMVectorGetX(XMVector3Dot(vWorldFront, vToPointDir[0])) < XMVectorGetX(XMVector3Dot(vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, 1.f, 0.f);
						else
							vPushDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
					}
				}
				// 아래
				else
				{
					// 우측
					if (XMVectorGetX(XMVector3Dot(vWorldRight, vDir)) > 0.f)
					{
						if (XMVectorGetX(XMVector3Dot(-vWorldFront, vToPointDir[2])) < XMVectorGetX(XMVector3Dot(-vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
						else
							vPushDir += XMVectorSet(1.f, 0.f, 0.f, 0.f);
					}
					// 좌측
					else
					{
						if (XMVectorGetX(XMVector3Dot(-vWorldFront, vToPointDir[3])) < XMVectorGetX(XMVector3Dot(-vWorldFront, vDir)))
							vPushDir += XMVectorSet(0.f, 0.f, -1.f, 0.f);
						else
							vPushDir += XMVectorSet(-1.f, 0.f, 0.f, 0.f);
					}
				}
			}
		}
	}

	XMVECTOR vMin = XMVectorSet(-1.0f, -1.0f, -1.0f, -1.0f);
	XMVECTOR vMax = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	vPushDir = XMVectorClamp(vPushDir, vMin, vMax);

	_float4 vResultPushDir;
	XMStoreFloat4(&vResultPushDir, vPushDir);
	return vResultPushDir;


}

CChef* CChef::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChef* pInstance = new CChef(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChef"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChef::Clone(void* pArg)
{
	CChef* pInstance = new CChef(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CChef"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChef::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pInteractColliderCom);
	Safe_Release(m_pCollideInteractObj);
	Safe_Release(m_pCollideBelongingObj);
	Safe_Release(m_pHoldingObject);
}
