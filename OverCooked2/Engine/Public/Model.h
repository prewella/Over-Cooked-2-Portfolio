#pragma once

#include "Component.h"

#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	TYPE Get_ModelType() { return m_eModelType; }

	_float4x4 Get_TransformMatrix() {
		return m_TransformMatrix;
	}

	_float3 Get_RootPosDiff() {
		return m_vRootPosDiff;
	}

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_bool isFinished() {
		return m_Animations[m_iCurrentAnimIndex]->isFinished();
	}

	void Set_Animation(_uint iAnimIndex, _bool isLoop);

	class CBone* Get_BonePtr(const _char* pBoneName) const;

	void Set_NextAnimation(_bool isLoop);

	_uint Get_NumAnimation() { return m_iNumAnimations; }

	CAnimation* Get_CurrentAnimation() { return m_Animations[m_iCurrentAnimIndex]; }
	_uint Get_CurrentAnimationIdx() { return m_iCurrentAnimIndex; }

	_uint Get_KeyFrame() {
		return m_Animations[m_iCurrentAnimIndex]->Get_KeyFrame();
	}

public:
	_bool Check_Picking(class CTransform* pTransform, _Out_ _float& fDist);

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint eTextureType);

	HRESULT Play_Animation(_float fTimeDelta);

	class CBone* Get_Bone(const _char* szBoneName);

private:
	HRESULT Write_Data_File(ofstream& ofs);

	HRESULT Read_Data_File(ifstream& ifs);

#ifdef USEASSIMP
	HRESULT Read_FBX_File(const char* szFbxFilePath);

	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, _int iParentIndex = -1);
	HRESULT Ready_Animations();
#endif

private:
	TYPE						m_eModelType = { TYPE_END };

#ifdef USEASSIMP
	// assimp 안쓰면 사용안함
	/* For Assimp*/
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
#endif

private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	_float4x4					m_TransformMatrix;

	vector<class CBone*>		m_Bones;

	_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	_bool						m_isLoop = { false };
	vector<class CAnimation*>	m_Animations;

	/* For Root Animation*/
	_float4x4					m_RootTransformMatrix;

	_float3						m_vRootPosDiff = { 0.f, 0.f, 0.f };
	_float4x4					m_MeshBoneMatrices[512];

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END