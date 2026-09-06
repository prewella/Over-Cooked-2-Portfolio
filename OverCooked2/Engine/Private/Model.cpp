#include "Model.h"

#include "Bone.h"
#include "Mesh.h"
#include "Animation.h"

#include "Shader.h"
#include "Texture.h"

#include "Transform.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
	, m_eModelType{ rhs.m_eModelType }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	, m_TransformMatrix{ rhs.m_TransformMatrix }
	, m_iNumAnimations{ rhs.m_iNumAnimations }
{

	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < 21; i++)
			Safe_AddRef(Material.MaterialTextures[i]);
	}
}

void CModel::Set_Animation(_uint iAnimIndex, _bool isLoop)
{
	if (m_iCurrentAnimIndex != iAnimIndex)
		m_Animations[m_iCurrentAnimIndex]->Reset_Animation_KeyFrame(m_Bones);

	m_iCurrentAnimIndex = iAnimIndex;

	m_isLoop = isLoop;
}

CBone* CModel::Get_BonePtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(pBoneName);
		});

	return *iter;
}

void CModel::Set_NextAnimation(_bool isLoop)
{
	m_Animations[m_iCurrentAnimIndex]->Reset_Animation_KeyFrame(m_Bones);

	if (m_iCurrentAnimIndex < m_iNumAnimations - 1)
	{
		m_iCurrentAnimIndex++;
	}
	else if (m_iCurrentAnimIndex == m_iNumAnimations - 1)
	{
		m_iCurrentAnimIndex = 0;
	}
	m_isLoop = isLoop;
}

_bool CModel::Check_Picking(CTransform* pTransform, _float& fDist)
{
	_float fMinDist = 100.f;
	_float fTempDist = 0.f;
	for (auto pMesh : m_Meshes)
	{
		pMesh->Compute_Picking(pTransform, fTempDist);

		if (0 < fTempDist)
			fMinDist = min(fMinDist, fTempDist);
	}

	if (0 < fMinDist)
	{
		fDist = fMinDist;
		return true;
	}
	return false;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	m_eModelType = eType;

	XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);

	char szFilePath[MAX_PATH] = { "" };
	strcpy_s(szFilePath, strModelFilePath.c_str());

	// 넣어줄 경로 -> ../Bin/Resources/Models/Anim/모델 이름
	_char szFileName[MAX_PATH] = { "" };

	size_t lastSlashPos = strModelFilePath.find_last_of('/');
	string strFolderName = strModelFilePath.substr(lastSlashPos, strModelFilePath.length());
	strcpy_s(szFileName, strFolderName.c_str());

	strcat_s(szFilePath, szFileName);

	char szDataFilePath[MAX_PATH] = { "" };
	strcpy_s(szDataFilePath, szFilePath);
	strcat_s(szDataFilePath, ".dat");

	// 데이터 파일 열어보기
	ifstream ifs;
	ifs.open(szDataFilePath, ios::binary);

	if (ifs.is_open())
		Read_Data_File(ifs);

#ifdef USEASSIMP
	else
	{
		//fbx파일로 읽어
		char szFbxFilePath[MAX_PATH] = { "" };
		strcpy_s(szFbxFilePath, szFilePath);
		strcat_s(szFbxFilePath, ".fbx");

		ifs.open(szFbxFilePath, ios::binary);
		if (ifs.is_open())
		{
			HRESULT hrReadFbx;
			hrReadFbx = Read_FBX_File(szFbxFilePath);
			if (hrReadFbx == S_OK)
			{
				ofstream ofs;
				ofs.open(szDataFilePath, ios::binary);
				if (ofs.is_open())
					Write_Data_File(ofs);
				else
					return E_FAIL;;
			}
		}
		else
			return E_FAIL;
	}

#endif

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

HRESULT CModel::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMeshMaterialIndex = { m_Meshes[iMeshIndex]->Get_MaterialIndex() };

	if (iMeshMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType])
		m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType]->Bind_ShaderResource(pShader, pConstantName);

	return S_OK;
}

HRESULT CModel::Play_Animation(_float fTimeDelta)
{
	/* 현재 애니메이션에 맞는 뼈의 상태(m_TrnasformationMatrix)를 갱신해준다. */
	m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop);

	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformMatrix));

	return S_OK;
}

CBone* CModel::Get_Bone(const _char* szBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(szBoneName);
		});

	return *iter;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Write_Data_File(ofstream& ofs)
{
	// 메쉬 갯수 저장
	ofs.write(reinterpret_cast<const _char*>(&m_iNumMeshes), sizeof(_uint));

	// 매쉬 클래스에서 매쉬 정보 채워주기
	for (auto& Mesh : m_Meshes)
	{
		Mesh->Write_Data(ofs, m_eModelType);
	}

	// 메터리얼 갯수 저장
	ofs.write(reinterpret_cast<const _char*>(&m_iNumMaterials), sizeof(_uint));

	// 텍스쳐 클래스에서 메터리얼 정보 저장
	for (auto& Material : m_Materials)
	{
		_uint iNumTextures = (_uint)Material.TextureInfo.size();
		ofs.write(reinterpret_cast<const _char*>(&iNumTextures), sizeof(_uint));
		for (auto& TextureInfo : Material.TextureInfo)
		{
			ofs.write(reinterpret_cast<const _char*>(&TextureInfo.iNumTextures), sizeof(_uint));
			_uint iTexturePathLength = (_uint)strlen(TextureInfo.szTexturePath) + 1;
			ofs.write(reinterpret_cast<const _char*>(&iTexturePathLength), sizeof(_uint));
			ofs.write(reinterpret_cast<const _char*>(&TextureInfo.szTexturePath), sizeof(_char) * iTexturePathLength);
		}
		// 메터리얼 어케 해야 하노?
	}

	if (TYPE_ANIM == m_eModelType)
	{
		_uint iNumBones = (_uint)m_Bones.size();
		ofs.write(reinterpret_cast<const _char*>(&iNumBones), sizeof(_uint));

		// 뼈 클래스에서 뼈 정보 저장
		for (auto& Bone : m_Bones)
		{
			Bone->Write_Data(ofs);
		}

		// 애니메이션 갯수 저장
		ofs.write(reinterpret_cast<const _char*>(&m_iNumAnimations), sizeof(_uint));

		// 애니메이션 클래스에서 애니메이션 저장
		for (auto& Animation : m_Animations)
		{
			Animation->Write_Data(ofs);
		}
	}

	// 접근하면서 파일 정보를 채워주고 파일 닫기
	ofs.close();
	return S_OK;
}

HRESULT CModel::Read_Data_File(ifstream& ifs)
{
	ifs.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));

	for (_uint i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, ifs);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	ifs.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof(_uint));

	for (_uint i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL			MeshMaterial{};
		_uint iNumTextures = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumTextures), sizeof(_uint));

		for (_uint j = 0; j < iNumTextures; j++)
		{
			TEXTURE_INFO	TextureInfo{};

			ifs.read(reinterpret_cast<_char*>(&TextureInfo.iNumTextures), sizeof(_uint));
			_uint iTexturePathLength = (_uint)strlen(TextureInfo.szTexturePath) + 1;
			ifs.read(reinterpret_cast<_char*>(&iTexturePathLength), sizeof(_uint));
			ifs.read(reinterpret_cast<_char*>(&TextureInfo.szTexturePath), sizeof(_char) * iTexturePathLength);

			MeshMaterial.TextureInfo.push_back(TextureInfo);

			_tchar			szPerfectPath[MAX_PATH] = { L"" };

			MultiByteToWideChar(CP_ACP, 0, TextureInfo.szTexturePath, (_int)strlen(TextureInfo.szTexturePath), szPerfectPath, MAX_PATH);

			MeshMaterial.MaterialTextures[TextureInfo.iNumTextures] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
			if (nullptr == MeshMaterial.MaterialTextures[TextureInfo.iNumTextures])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
	}

	if (TYPE_ANIM == m_eModelType)
	{
		_uint iNumBones = 0;
		ifs.read(reinterpret_cast<_char*>(&iNumBones), sizeof(_uint));

		for (_uint i = 0; i < iNumBones; i++)
		{
			CBone* pBone = CBone::Create(ifs);
			if (nullptr == pBone)
				return E_FAIL;
			m_Bones.push_back(pBone);
		}

		// 애니메이션 갯수 저장
		ifs.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof(_uint));

		for (_uint i = 0; i < iNumBones; i++)
		{
			CAnimation* pAnimation = CAnimation::Create(ifs);
			if (nullptr == pAnimation)
				return E_FAIL;
			m_Animations.push_back(pAnimation);
		}
	}

	// 접근하면서 파일 정보를 채워주고 파일 닫기
	ifs.close();

	return S_OK;
}

#ifdef USEASSIMP

HRESULT CModel::Read_FBX_File(const char* szFbxFilePath)
{
	_uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };

	iOption = m_eModelType == TYPE_NONANIM ? iOption | aiProcess_PreTransformVertices : iOption;

	/* 파일의 정보를 읽어서 aiScene안에 모든 데이터를 담아주게된다. */
	m_pAIScene = m_Importer.ReadFile(szFbxFilePath, iOption);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* 읽은 정보를 바탕으로해서 내가 사용하기 좋게 정리한다.  */

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode)))
		return E_FAIL;

	/* 모델을 구성하는 메시들을 생성한다. */
	/* 모델 = 메시 + 메시 + ... */
	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(szFbxFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], m_Bones, XMLoadFloat4x4(&m_TransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MESH_MATERIAL			MeshMaterial{};

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTextureFilePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
				continue;

			_char			szDrive[MAX_PATH] = { "" };
			_char			szDirectory[MAX_PATH] = { "" };
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);


			_char			szFileName[MAX_PATH] = { "" };
			_char			szEXT[MAX_PATH] = { "" };

			/* ..\Bin\Resources\Models\Fiona\ */
			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_char			szFullPath[MAX_PATH] = { "" };
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			_tchar			szPerfectPath[MAX_PATH] = { L"" };

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			TEXTURE_INFO	TextureInfo{};
			TextureInfo.iNumTextures = (_uint)j;
			strcpy_s(TextureInfo.szTexturePath, szFullPath);

			MeshMaterial.TextureInfo.push_back(TextureInfo);
			MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
			if (nullptr == MeshMaterial.MaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	_int		iParent = (_int)m_Bones.size() - 1;

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iParent);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

#endif

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < 21; i++)
			Safe_Release(Material.MaterialTextures[i]);
	}

	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

#ifdef USEASSIMP
	m_Importer.FreeScene();
#endif // USEASSIMP

}
