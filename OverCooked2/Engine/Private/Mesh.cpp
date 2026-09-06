#include "Mesh.h"
#include "Bone.h"

#include "GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

#ifdef USEASSIMP

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix TransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = sizeof(_uint);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = CModel::TYPE_NONANIM == eModelType ? Ready_Vertices_For_NonAnimModel(pAIMesh, TransformMatrix) : Ready_Vertices_For_AnimModel(pAIMesh, Bones);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 인덱스 버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	m_pIndices = new _uint[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		m_pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

#endif // USEASSIMP


HRESULT CMesh::Initialize_Prototype(CModel::TYPE eModelType, ifstream& ifs)
{
	Read_Data(ifs, eModelType);

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Stock_Matrices(const vector<CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&pMeshBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * XMLoadFloat4x4(Bones[m_Bones[i]]->Get_CombinedTransformMatrix()));
	}

	return S_OK;
}

HRESULT CMesh::Write_Data(ofstream& ofs, CModel::TYPE eModelType)
{
	_uint iNameLength = (_uint)strlen(m_szName) + 1;
	ofs.write(reinterpret_cast<const _char*>(&iNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&m_szName), sizeof(_char) * iNameLength);
	ofs.write(reinterpret_cast<const _char*>(&m_iMaterialIndex), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&m_iNumVertices), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&m_iNumIndices), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(m_pVerticesPos), sizeof(_float3) * m_iNumVertices);
	ofs.write(reinterpret_cast<const _char*>(m_pIndices), sizeof(_uint) * m_iNumIndices);

	switch (eModelType)
	{
	case CModel::TYPE_NONANIM:
		ofs.write(reinterpret_cast<const _char*>(m_pNonAnimVertices), sizeof(VTXMESH) * m_iNumVertices);
		break;
	case CModel::TYPE_ANIM:
		ofs.write(reinterpret_cast<const _char*>(m_pAnimVertices), sizeof(VTXANIMMESH) * m_iNumVertices);
		ofs.write(reinterpret_cast<const _char*>(&m_iNumBones), sizeof(_uint));
		for (auto& iNumBone : m_Bones)
		{
			ofs.write(reinterpret_cast<const _char*>(&iNumBone), sizeof(_uint));
		}
		for (const auto& OffsetMatrix : m_OffsetMatrices)
		{
			ofs.write(reinterpret_cast<const _char*>(&OffsetMatrix), sizeof(_float4x4));
		}
		break;
	}

	return S_OK;
}

HRESULT CMesh::Read_Data(ifstream& ifs, CModel::TYPE eModelType)
{
	_uint iNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNameLength), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_szName), sizeof(_char) * iNameLength);
	ifs.read(reinterpret_cast<_char*>(&m_iMaterialIndex), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_iNumVertices), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_iNumIndices), sizeof(_uint));

	m_pVerticesPos = new _float3[m_iNumVertices];
	ifs.read(reinterpret_cast<_char*>(m_pVerticesPos), sizeof(_float3) * m_iNumVertices);
	m_pIndices = new _uint[m_iNumIndices];
	ifs.read(reinterpret_cast<_char*>(m_pIndices), sizeof(_uint) * m_iNumIndices);

	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 버텍스 버퍼

	switch (eModelType)
	{
	case CModel::TYPE_NONANIM:
		m_iVertexStride = sizeof(VTXMESH);

		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

		/* 정점버퍼의 byte크기 */
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;

		m_pNonAnimVertices = new VTXMESH[m_iNumVertices];
		ZeroMemory(m_pNonAnimVertices, sizeof(VTXMESH) * m_iNumVertices);

		ifs.read(reinterpret_cast<_char*>(m_pNonAnimVertices), sizeof(VTXMESH) * m_iNumVertices);

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = m_pNonAnimVertices;
		break;
	case CModel::TYPE_ANIM:

		m_iVertexStride = sizeof(VTXANIMMESH);

		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

		/* 정점버퍼의 byte크기 */
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;

		m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
		ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

		ifs.read(reinterpret_cast<_char*>(m_pAnimVertices), sizeof(VTXANIMMESH) * m_iNumVertices);
		ifs.read(reinterpret_cast<_char*>(&m_iNumBones), sizeof(_uint));

		for (_uint i = 0; i < m_iNumBones; i++)
		{
			_uint iNumBone = 0;
			ifs.read(reinterpret_cast<_char*>(&iNumBone), sizeof(_uint));
			m_Bones.push_back(iNumBone);
		}

		for (_uint i = 0; i < m_iNumBones; i++)
		{
			_float4x4 mOffsetMatrix;
			ifs.read(reinterpret_cast<_char*>(&mOffsetMatrix), sizeof(_float4x4));
			m_OffsetMatrices.push_back(mOffsetMatrix);
		}

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = m_pAnimVertices;
		break;
	}

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	// 인덱스 버퍼
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 인덱스 버퍼의 byte크기 */
	m_iIndexStride = sizeof(_uint);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	return E_NOTIMPL;
}

#ifdef USEASSIMP
HRESULT CMesh::Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_pNonAnimVertices = new VTXMESH[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pNonAnimVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pNonAnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&m_pNonAnimVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pNonAnimVertices[i].vPosition), TransformMatrix));
		m_pVerticesPos[i] = m_pNonAnimVertices[i].vPosition;

		memcpy(&m_pNonAnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&m_pNonAnimVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&m_pNonAnimVertices[i].vNormal), TransformMatrix));

		memcpy(&m_pNonAnimVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pNonAnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pNonAnimVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const vector<CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	m_pAnimVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMESH) * m_iNumVertices);
	m_pVerticesPos = new _float3[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&m_pAnimVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		m_pVerticesPos[i] = m_pAnimVertices[i].vPosition;
		memcpy(&m_pAnimVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pAnimVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4	OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_int	iBoneIndex = { -1 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				++iBoneIndex;
				return pBone->Compare_Name(pAIBone->mName.data);
			});

		m_Bones.push_back(iBoneIndex);

		/* 이 뼈는 몇개의 정점들에게 영향을 준다. */
		_uint		iNumWeights = pAIBone->mNumWeights;

		for (_uint j = 0; j < iNumWeights; j++)
		{
			if (0.0f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				m_pAnimVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_int	iBoneIndex = { -1 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool
			{
				++iBoneIndex;
				return pBone->Compare_Name(m_szName);
			});

		m_Bones.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pAnimVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, const vector<CBone*>& Bones, _fmatrix TransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, Bones, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CMesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, ifstream& ifs)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, ifs)))
	{
		MSG_BOX(TEXT("Failed To Created : CMesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Clone(void* pArg)
{
	return nullptr;
}

void CMesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pAnimVertices);
	Safe_Delete_Array(m_pNonAnimVertices);
}
