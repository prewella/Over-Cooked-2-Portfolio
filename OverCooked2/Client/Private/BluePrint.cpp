#include "stdafx.h"
#include "BluePrint.h"

CBluePrint::CBluePrint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModelObject{ pDevice, pContext }
{
}

CBluePrint::CBluePrint(const CBluePrint& rhs)
	: CModelObject{ rhs }
{

}

HRESULT CBluePrint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBluePrint::Initialize(void* pArg)
{
	MODELOBJECT_DESC* pDesc = (MODELOBJECT_DESC*)pArg;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(pDesc->strModelTag)))
		return E_FAIL;

	return S_OK;
}

_int CBluePrint::Tick(_float fTimeDelta)
{
	if (m_bDead)
		return EVENT_DEAD;

	return EVENT_NONE;
}

void CBluePrint::Late_Tick(_float fTimeDelta)
{
	if (CModel::TYPE_ANIM == m_pModelCom->Get_ModelType())
		m_pModelCom->Play_Animation(fTimeDelta);

	if (m_bRender)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CBluePrint::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, 1)))
			return E_FAIL;
		if (CModel::TYPE_ANIM == m_pModelCom->Get_ModelType())
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				return E_FAIL;
		}

		/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBluePrint::Set_Model(wstring strModelTag)
{
	Safe_Release(m_pModelCom);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	Safe_Release(m_pShaderCom);

	switch (m_pModelCom->Get_ModelType())
	{
	case CModel::TYPE_ANIM:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	case CModel::TYPE_NONANIM:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CBluePrint::Add_Components(wstring strModelTag)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	switch (m_pModelCom->Get_ModelType())
	{
	case CModel::TYPE_ANIM:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	case CModel::TYPE_NONANIM:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
		break;
	}
	
	return S_OK;
}

HRESULT CBluePrint::Bind_ShaderResources()
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

CBluePrint* CBluePrint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBluePrint* pInstance = new CBluePrint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBluePrint"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBluePrint::Clone(void* pArg)
{
	CBluePrint* pInstance = new CBluePrint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBluePrint"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBluePrint::Free()
{
	__super::Free();
}
