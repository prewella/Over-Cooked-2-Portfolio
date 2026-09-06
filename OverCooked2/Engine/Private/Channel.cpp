#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

#ifdef USEASSIMP
HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
		{
			++m_iBoneIndex;
			return pBone->Compare_Name(m_szName);
		});

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;
	_float			fTime;

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			fTime = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			/*memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));*/
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			fTime = (_float)pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			fTime = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;
		KeyFrame.fTime = fTime;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}
#endif

HRESULT CChannel::Initialize(ifstream& ifs)
{

	Read_Data(ifs);
	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	// 애니메이션이 첫 시작 부분이면
	if (0.0f == fTrackPosition)
		// 현재 키프레임 인덱스 값도 0 넣어주기
		(*pCurrentKeyFrameIndex) = 0;

	KEYFRAME		KeyFrame = m_KeyFrames.back();

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;

	if (KeyFrame.fTime <= fTrackPosition)
	{
		vScale = KeyFrame.vScale;
		vRotation = KeyFrame.vRotation;
		vTranslation = KeyFrame.vTranslation;
	}
	else
	{
		// 프레임 저하등으로 인해 트랙 포지션이 키프레임을 1이상으로 차이가 날 경우 루프를 돌며 현재 트랙에 맞게 키 프레임을 증가
		while (fTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
			++(*pCurrentKeyFrameIndex);

		_float		fRatio = (fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTime)
			/ (m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTime - m_KeyFrames[*pCurrentKeyFrameIndex].fTime);

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation), fRatio));
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f));

	Bones[m_iBoneIndex]->Set_TransformMatrix(TransformationMatrix);
}

HRESULT CChannel::Write_Data(ofstream& ofs)
{
	_uint iNameLength = (_uint)strlen(m_szName) + 1;
	ofs.write(reinterpret_cast<const _char*>(&iNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&m_szName), sizeof(_char) * iNameLength);
	ofs.write(reinterpret_cast<const _char*>(&m_iBoneIndex), sizeof(_int));
	ofs.write(reinterpret_cast<const _char*>(&m_iNumKeyFrames), sizeof(_uint));
	for (auto KeyFrame : m_KeyFrames)
	{
		ofs.write(reinterpret_cast<const _char*>(&KeyFrame), sizeof(KEYFRAME));
	}

	return S_OK;
}

HRESULT CChannel::Read_Data(ifstream& ifs)
{
	_uint iNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNameLength), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_szName), sizeof(_char) * iNameLength);
	ifs.read(reinterpret_cast<_char*>(&m_iBoneIndex), sizeof(_int));
	ifs.read(reinterpret_cast<_char*>(&m_iNumKeyFrames), sizeof(_uint));
	for (_uint i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame{};
		ifs.read(reinterpret_cast<_char*>(&KeyFrame), sizeof(KEYFRAME));
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

#ifdef USEASSIMP
CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<class CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif

CChannel* CChannel::Create(ifstream& ifs)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(ifs)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CChannel::Free()
{
	m_KeyFrames.clear();
}
