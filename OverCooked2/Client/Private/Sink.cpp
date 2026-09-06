#include "stdafx.h"
#include "Sink.h"

#include "Plate.h"
#include "Dirty_Plate.h"
#include "Cooker.h"

#include "Part_Plate_Wash.h"

CSink::CSink(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CInteractObj{ pDevice, pContext }
{
}

CSink::CSink(const CSink& rhs)
	:CInteractObj{ rhs }
{
}

HRESULT CSink::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSink::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	m_eType = OBJECT_SINK;

	return S_OK;
}

int CSink::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	if (m_pGameInstance->Get_DIKeyState(DIK_B) == EKeyState::DOWN)
	{
		Add_DirtyPlate(1);
	}

	if (m_pGameInstance->Get_DIKeyState(DIK_N) == EKeyState::DOWN)
	{
		Cleaning_Plate();
	}

	if (m_fSinkPercent >= 1.5f)
	{
		m_fSinkPercent = 0.f;
		Cleaning_Plate();
	}

	for (auto& plate : m_DirtyPlates)
		plate->Tick(fTimeDelta);

	m_pGameInstance->Add_Collision_Group(this);

	return EVENT_NONE;
}

void CSink::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	for (auto& plate : m_DirtyPlates)
		plate->Late_Tick(fTimeDelta);
}

HRESULT CSink::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		if (i == 1)
		{
			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;
		}
		else
		{
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
		}
		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

_bool CSink::Check_PutDown(CBelonging* pBelonging)
{
	_uint iNumCleanPlate = (_uint)m_CleanedPlates.size();

	switch (pBelonging->Get_Type())
	{
	case Client::BELONGING_INGREDIENT:
		if (0 >= iNumCleanPlate)
			return false;
		else if (((CPlate*)m_pBelonging)->Check_Plating(pBelonging))
			return true;
		break;
	case Client::BELONGING_PLATE:
		// 세척되어 있는 접시가 없으면 못놓음
		if (0 >= iNumCleanPlate)
			return false;
		// 있으면 놓을 수 있는지 체크
		else if (((CPlate*)m_pBelonging)->Check_Plating(pBelonging))
			return true;
		break;
	case Client::BELONGING_DIRTY_PLATE:
		return true;
		break;
	case Client::BELONGING_COOKER:
		if (0 >= iNumCleanPlate)
			return false;
		else if (((CPlate*)m_pBelonging)->Check_Plating(pBelonging))
			return true;
		break;

	}
	return false;
}

HRESULT CSink::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sink"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CBounding_OBB::BOUNDING_OBB_DESC				ColliderDesc{};

	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(2.f, 1.f, 1.f);
	ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSink::Add_PartObjects()
{
	for (_int i = 0; i < 3; i++)
	{
		CPart_Plate_Wash::PART_PLATE_WASH_DESC plate_WashDesc{};

		_matrix		TransformMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixRotationY(XMConvertToRadians(30.f * (i - 1) * (i + 1))) * XMMatrixTranslation(-0.75f + (0.2f * i), 0.35f, 0.f);

		plate_WashDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4_Ptr();
		XMStoreFloat4x4(&plate_WashDesc.vSettingMatrix, TransformMatrix);

		CPartObject* pPartObj = nullptr;

		pPartObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Part_Plate_Wash"), &plate_WashDesc));
		if (nullptr == pPartObj)
			return E_FAIL;

		m_DirtyPlates.emplace_back(pPartObj);
	}

	return S_OK;
}

HRESULT CSink::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4 vColor =  _float4(0.f, 0.85f, 1.f, 0.75f);

	if (FAILED(m_pShaderCom->Bind_RawValue("vDiffuseColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

_bool CSink::Check_In_ReactionArea(_float4 vChefPos)
{
	_vector vRight = m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT);

	_vector vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_vector vDir = (XMLoadFloat4(&vChefPos) - vPos);

	// 좌측에 있을경우
	if (0 > XMVectorGetX(XMVector3Dot(vRight, vDir)))
		// 상호작용 가능
		return true;

	return false;
}

void CSink::Add_DirtyPlate(CBelonging* pBelongnig)
{
	m_iNumDirtyPlate += ((CDirty_Plate*)pBelongnig)->Get_Plate_StackCnt();

	for (_int i = 0; i < m_DirtyPlates.size(); i++)
	{
		if (i < m_iNumDirtyPlate)
			((CPart_Plate_Wash*)m_DirtyPlates[i])->IsRender(true);
		else
			((CPart_Plate_Wash*)m_DirtyPlates[i])->IsRender(false);
	}
}

void CSink::Add_DirtyPlate(_int iNumDirty_Plate)
{
	m_iNumDirtyPlate += iNumDirty_Plate;

	for (_int i = 0; i < m_DirtyPlates.size(); i++)
	{
		if (i < m_iNumDirtyPlate)
			((CPart_Plate_Wash*)m_DirtyPlates[i])->IsRender(true);
		else
			((CPart_Plate_Wash*)m_DirtyPlates[i])->IsRender(false);
	}
}

void CSink::Wash_Plate(_float fTimeDelta)
{
	m_fSinkPercent += fTimeDelta;
}

void CSink::Take_HaveObj(CBelonging** pBelonging)
{
	if (nullptr == m_pBelonging)
		return;

	*pBelonging = m_pBelonging;
	Safe_Release(m_pBelonging);

	m_CleanedPlates.pop_back();
	if (0 < m_CleanedPlates.size())
		m_pBelonging = m_CleanedPlates.back();
	else
		m_pBelonging = nullptr;
}

_bool CSink::Check_Reaction()
{
	if (0 < m_iNumDirtyPlate)
		return true;

	return false;
}

void CSink::Add_CleanPlate()
{
	CBelonging* pPlate = nullptr;

	if (!FAILED(m_pGameInstance->Add_Clone((CGameObject**)&pPlate, LEVEL_GAMEPLAY, TEXT("Layer_Belongings"), TEXT("Prototype_GameObject_Plate"))))
	{
		m_CleanedPlates.emplace_back(pPlate);
		m_pBelonging = pPlate;
	}

	Set_On_Belonging();
}

void CSink::Set_On_Belonging()
{
	int i = 0;
	_vector vRight = m_pTransformCom->Get_State_Vector(CTransform::STATE_RIGHT);
	vRight = XMVectorSetW(vRight, 0.f);
	for (auto& plate : m_CleanedPlates)
	{
		CTransform* pTransform = (CTransform*)plate->Get_Component(g_strTransformTag);

		_vector vSetOnPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
		vSetOnPos += vRight * 0.5f + XMVectorSet(0.f, 0.45f + (0.125f * i++), 0.f, 0.f);

		((CTransform*)plate->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_POSITION, vSetOnPos);
	}
}

void CSink::Cleaning_Plate()
{
	if (0 >= m_iNumDirtyPlate)
		return;

	m_iNumDirtyPlate--;
	Add_CleanPlate();
	m_pGameInstance->StopSound(CSound_Manager::CH_SINK_PLATE);
	m_pGameInstance->Play_Sound(L"WashedPlate.wav", CSound_Manager::CH_SINK_PLATE, 1.f);

	for (_int i = 0; i < 3; i++)
	{
		if (i < m_iNumDirtyPlate)
			((CPart_Plate_Wash*)m_DirtyPlates[i])->IsRender(true);
		else
			((CPart_Plate_Wash*)m_DirtyPlates[i])->IsRender(false);
	}

}

CSink* CSink::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSink* pInstance = new CSink(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSink"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSink::Clone(void* pArg)
{
	CSink* pInstance = new CSink(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSink"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSink::Free()
{
	__super::Free();

	for (auto& plate : m_CleanedPlates)
		Safe_Release(plate);
	m_CleanedPlates.clear();

	for (auto& plate : m_DirtyPlates)
		Safe_Release(plate);
	m_DirtyPlates.clear();

	Safe_Release(m_pColliderCom);
}
