#pragma once

#include "Base.h"

/* 하나의 애니메이션이 몇개의 뼈를 컨트롤해야하는지. 그 뼈들은 무엇인지.  */
/* 이 애니메이션을 재생하는데 걸리는 총 거리.. */
/* 이 애니메이션을 구동하는 속도. */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	_bool isFinished() const {
		return m_isFinished;
	}

	_uint Get_KeyFrame() {
		_uint iMaxKeyFrame = 0;

		for (auto& CurKeyFrame : m_CurrentKeyFrameIndices)
		{
			iMaxKeyFrame = max(iMaxKeyFrame, CurKeyFrame);
		}

		return iMaxKeyFrame;
	}


public:
#ifdef USEASSIMP
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
#endif
	HRESULT Initialize(ifstream& ifs);
	void Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	void Reset_Animation_KeyFrame(const vector<class CBone*>& Bones);

public:
	HRESULT Write_Data(ofstream& ofs);
	HRESULT Read_Data(ifstream& ifs);

private:
	_char								m_szName[MAX_PATH] = { "" };
	
	_float								m_fDuration = { 0.0f };		 /* 전체 재생 길이. */
	_float								m_fTickPerSecond = { 0.0f }; /* 초당 얼마나 재생을 해야하는가 (속도) */
	_float								m_fTrackPosition = { 0.0f }; /* 현재 애니메이션이 어디까지 재생되었는지"? */

	_uint								m_iNumChannels = { 0 };
	vector<class CChannel*>				m_Channels;
	vector<_uint>						m_CurrentKeyFrameIndices;

	_bool								m_isFinished = { false };
	_bool								m_IsCloned = { false };

public:
#ifdef USEASSIMP
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
#endif
	static CAnimation* Create(ifstream& ifs);
	CAnimation* Clone();
	virtual void Free() override;
};

END