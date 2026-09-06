#include "..\Public\Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{

}

void CTransform::Set_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State_Vector(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State_Vector(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State_Vector(STATE_LOOK)) * fScaleZ);

}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC* pDesc = (TRANSFORM_DESC*)pArg;

		m_fSpeedPerSec = pDesc->fSpeedPerSec;
		m_fRotationPerSec = pDesc->fRotationPerSec;

		if (pDesc->bInitWorldMatrix)
		{
			m_WorldMatrix = pDesc->InitWorldFloat4x4;
		}
	}

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

void CTransform::Move_To_SetDir(_vector vMoveDir, _float fTimeDelta, _bool IsRotation)
{
	_vector		vDir = XMVector3Normalize(vMoveDir);

	if (IsRotation)
		Rotation_To_Dir(vDir);

	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	vPosition += vDir * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Move_To_SetDir_NonRot(_vector vMoveDir, _float fTimeDelta)
{
	_vector		vDir = XMVector3Normalize(vMoveDir);

	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	vPosition += vDir * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);

}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vLook = Get_State_Vector(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vLook = Get_State_Vector(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vRight = Get_State_Vector(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State_Vector(STATE_POSITION);
	_vector		vRight = Get_State_Vector(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Set_Look(_fvector vAt)
{
	_vector		vLook = vAt;
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Look_At(_fvector vAt)
{
	_vector		vLook = vAt - Get_State_Vector(STATE_POSITION);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScaled = Get_Scaled();

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix			RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i),
			XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));
		/*Set_State(STATE(i),
			XMVector3TransformNormal(Get_State_Vector((STATE)i), RotationMatrix));*/
	}

}

void CTransform::Turn(_fvector vAxis, _float fDegree, _bool TurnToDegree)
{
	_matrix			RotationMatrix = XMMatrixRotationAxis(vAxis, fDegree);

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i),
			XMVector4Transform(Get_State_Vector((STATE)i), RotationMatrix));
	}
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vState[] = {
		XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x,
		XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y,
		XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z,
	};


	// XMConvertToRadians(Degree);

	_matrix			RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	for (size_t i = 0; i < STATE_POSITION; i++)
	{
		Set_State(STATE(i),
			XMVector4Transform(vState[(STATE)i], RotationMatrix));
	}
}

void CTransform::Rotation_To_Dir(_vector vDir)
{
	_vector		vLook = Get_State_Vector(STATE_LOOK);
	_vector		vRight = Get_State_Vector(STATE_RIGHT);
	vDir = XMVector3Normalize(vDir);

	float fLookDotX = XMVectorGetX(XMVector3Dot(vLook, vDir));
	float fRightDotX = XMVectorGetX(XMVector3Dot(vRight, vDir));

	float angle = fRightDotX > 0 ? acos(fLookDotX) : acos(fLookDotX) * -1.f;

	if (abs(angle) > 0.f)
	{
		// 회전 축을 기반으로 각도만큼 회전합니다.
		_matrix rotationMatrix = XMMatrixRotationAxis(Get_State_Vector(STATE_UP), angle * 0.3f);

		// 현재 시선 방향을 회전한 방향으로 설정합니다.
		_vector vNewLook = XMVector3TransformNormal(vLook, rotationMatrix);

		// 회전한 방향을 설정합니다.
		Set_Look(vNewLook);
	}
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}
