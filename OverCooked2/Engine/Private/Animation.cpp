#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

#ifdef USEASSIMP

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;

	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션은 몇개의 뼈를 컨트롤해야하는가? */
	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}
#endif

HRESULT CAnimation::Initialize(ifstream& ifs)
{
	Read_Data(ifs);
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);
	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop)
{
	m_isFinished = false;

	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fDuration <= m_fTrackPosition)
	{
		/*if (m_isFinished)
			m_isFinished = false;*/

		if (false == isLoop)
		{
			m_isFinished = true;
			
			return;
		}
		m_isFinished = true;
		m_fTrackPosition = 0.f;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i]);
	}
}

void CAnimation::Reset_Animation_KeyFrame(const vector<class CBone*>& Bones)
{
	m_fTrackPosition = 0.f;
	m_isFinished = false;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i]);
	}
}

HRESULT CAnimation::Write_Data(ofstream& ofs)
{
	_uint iNameLength = (_uint)strlen(m_szName) + 1;
	ofs.write(reinterpret_cast<const _char*>(&iNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&m_szName), sizeof(_char) * iNameLength);
	ofs.write(reinterpret_cast<const _char*>(&m_fDuration), sizeof(_float));
	ofs.write(reinterpret_cast<const _char*>(&m_fTickPerSecond), sizeof(_float));
	ofs.write(reinterpret_cast<const _char*>(&m_iNumChannels), sizeof(_uint));

	for (auto Channel : m_Channels)
	{
		Channel->Write_Data(ofs);
	}
	return S_OK;
}

HRESULT CAnimation::Read_Data(ifstream& ifs)
{
	_uint iNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNameLength), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_szName), sizeof(_char) * iNameLength);
	ifs.read(reinterpret_cast<_char*>(&m_fDuration), sizeof(_float));
	ifs.read(reinterpret_cast<_char*>(&m_fTickPerSecond), sizeof(_float));
	ifs.read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof(_uint));

	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(ifs);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

#ifdef USEASSIMP
CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const vector<CBone*>& Bones)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif

CAnimation* CAnimation::Create(ifstream& ifs)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(ifs)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
	return nullptr;
}

CAnimation* CAnimation::Clone()
{
	CAnimation* pAnimation = new CAnimation(*this);
	pAnimation->m_IsCloned = true;
	return pAnimation;
}


void CAnimation::Free()
{
	if (!m_IsCloned)
	{
		for (auto& pChannel : m_Channels)
			Safe_Release(pChannel);
	}


	m_Channels.clear();
}

