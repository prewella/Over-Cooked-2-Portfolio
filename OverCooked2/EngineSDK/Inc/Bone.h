#pragma once

#include "Base.h"

/* 뼈의 정보를 표현하기위한 타입 세가지 제공하낟. */
/* aiNode, aiBone, aiAnimNode */

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	void Set_PreTransformMatrix(_fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_PreTransformMatrix, TransformMatrix);
	}

	void Set_TransformMatrix(_fmatrix TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
	}

	void Set_Combined_Matrix(_vector vPos) {
		memcpy(&m_CombinedTransformMatrix.m[3], &vPos, sizeof(_float3));
	}

	const _float4x4* Get_CombinedTransformMatrix() const {
		return &m_CombinedTransformMatrix;
	}

	_bool Compare_Name(const _char* pBoneName) {
		return !strcmp(m_szName, pBoneName);
	}

public:
#ifdef USEASSIMP
	HRESULT Initialize(const aiNode* pAINode, _int iParentIndex);
#endif
	HRESULT Initialize(ifstream& ifs);

public:
	void Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix TransformMatrix);
	_float4x4 Get_TransformMatrix() {
		return m_TransformMatrix;
	}

	_float3 Get_PosDiff() {
		return m_vPosDiff;
	}

	HRESULT Write_Data(ofstream& ofs);
	HRESULT Read_Data(ifstream& ifs);

private:
	_char				m_szName[MAX_PATH] = { "" };

	/* 이 뼈 만의 상태행렬 */
	_float4x4			m_PreTransformMatrix;
	_float4x4			m_TransformMatrix;

	_float3				m_vPosDiff = { 0.f, 0.f, 0.f };

	/* 최종 상태행렬. */
	_float4x4			m_CombinedTransformMatrix;

	_int				m_iParentBoneIndex = { -1 };

public:
#ifdef USEASSIMP
	static CBone* Create(const aiNode* pAINode, _int  iParentIndex);
#endif
	static CBone* Create(ifstream& ifs);
	CBone* Clone();
	virtual void Free() override;
};

END