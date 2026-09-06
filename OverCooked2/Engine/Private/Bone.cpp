#include "Bone.h"

CBone::CBone()
{
}


#ifdef USEASSIMP
HRESULT CBone::Initialize(const aiNode* pAINode, _int  iParentIndex)
{
	m_iParentBoneIndex = iParentIndex;

	strcpy_s(m_szName, pAINode->mName.data);

	memcpy(&m_TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformMatrix, XMMatrixIdentity());
	memcpy(&m_PreTransformMatrix, &m_TransformMatrix, sizeof(_float4x4));

	return S_OK;
}
#endif

HRESULT CBone::Initialize(ifstream& ifs)
{
	Read_Data(ifs);
	XMStoreFloat4x4(&m_CombinedTransformMatrix, XMMatrixIdentity());
	memcpy(&m_PreTransformMatrix, &m_TransformMatrix, sizeof(_float4x4));

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix TransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) * TransformMatrix);
	else
	{
		XMStoreFloat4x4(&m_CombinedTransformMatrix,
			XMLoadFloat4x4(&m_TransformMatrix) * XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformMatrix));
	}
}

HRESULT CBone::Write_Data(ofstream& ofs)
{
	_uint iNameLength = (_uint)strlen(m_szName) + 1;
	ofs.write(reinterpret_cast<const _char*>(&iNameLength), sizeof(_uint));
	ofs.write(reinterpret_cast<const _char*>(&m_szName), sizeof(_char) * iNameLength);
	ofs.write(reinterpret_cast<const _char*>(&m_TransformMatrix), sizeof(_float4x4));
	ofs.write(reinterpret_cast<const _char*>(&m_iParentBoneIndex), sizeof(_int));

	return S_OK;
}

HRESULT CBone::Read_Data(ifstream& ifs)
{
	_uint iNameLength = 0;
	ifs.read(reinterpret_cast<_char*>(&iNameLength), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_szName), sizeof(_char) * iNameLength);
	ifs.read(reinterpret_cast<_char*>(&m_TransformMatrix), sizeof(_float4x4));
	ifs.read(reinterpret_cast<_char*>(&m_iParentBoneIndex), sizeof(_int));

	return S_OK;
}

#ifdef USEASSIMP
CBone* CBone::Create(const aiNode* pAINode, _int  iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}
#endif

CBone* CBone::Create(ifstream& ifs)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(ifs)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{

}
