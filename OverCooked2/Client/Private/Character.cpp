#include "stdafx.h"
#include "Character.h"

#include "PartObject.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	:CGameObject(rhs)
{
}

HRESULT CCharacter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacter::Initialize(void* pArg)
{
	__super::Initialize(pArg);
	return S_OK;
}

_int CCharacter::Tick(_float fTimeDelta)
{
	if (m_bDead)
	{
		return EVENT_DEAD;
	}

	return EVENT_NONE;
}

void CCharacter::Late_Tick(_float fTimeDelta)
{
}

HRESULT CCharacter::Render()
{
	return S_OK;
}

void CCharacter::Free()
{
	__super::Free();

	for (auto& Pair : m_PartObjects)
		Safe_Release(Pair.second);

	m_PartObjects.clear();
}