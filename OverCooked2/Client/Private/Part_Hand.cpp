#include "stdafx.h"
#include "Part_Hand.h"

#include "Bone.h"

CPart_Hand::CPart_Hand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPartObject{ pDevice, pContext }
{
}

CPart_Hand::CPart_Hand(const CPart_Hand& rhs)
	:CPartObject{ rhs }
{
}

HRESULT CPart_Hand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPart_Hand::Initialize(void* pArg)
{
	HAND_DESC* pHandDesc = (HAND_DESC*)pArg;

	m_pIsGrip = pHandDesc->pIsGrip;
	m_pSocket = pHandDesc->pSocket;
	Safe_AddRef(m_pSocket);

	m_pModelComs.resize(2);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pHandDesc->strModelTags)))
		return E_FAIL;

	//m_pTransformCom->Set_Scaled(10.f, 10.f, 10.f);
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, -0.0785f, 1.f));

	return S_OK;
}

int CPart_Hand::Tick(_float fTimeDelta)
{
	return EVENT_NONE;
}

void CPart_Hand::Late_Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocket->Get_CombinedTransformMatrix());

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPart_Hand::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelComs[*m_pIsGrip]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[*m_pIsGrip]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelComs[*m_pIsGrip]->Render(i);
	}


	return S_OK;
}

HRESULT CPart_Hand::Add_Components(wstring* strModelTags)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTags[0],
		TEXT("Com_Model0"), (CComponent**)&m_pModelComs[0])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTags[1],
		TEXT("Com_Model1"), (CComponent**)&m_pModelComs[1])))
		return E_FAIL;

	return S_OK;
}

HRESULT CPart_Hand::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CPart_Hand* CPart_Hand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPart_Hand* pInstance = new CPart_Hand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_Hand"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPart_Hand::Clone(void* pArg)
{
	CPart_Hand* pInstance = new CPart_Hand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_Hand"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPart_Hand::Free()
{
	__super::Free();

	Safe_Release(m_pSocket);

	for (auto& pModel : m_pModelComs)
	{
		Safe_Release(pModel);
	}
}
