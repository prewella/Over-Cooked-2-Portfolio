#pragma once

namespace Engine
{
	typedef struct tagEngineDesc
	{
		HWND			hWnd;
		bool			isWindowed;
		unsigned int	iWinSizeX, iWinSizeY;
	}ENGINE_DESC;

	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END};
		TYPE	eType;

		_float4 vDirection;
		_float4 vPosition;
		_float fRange;

		_float4 vDiffuse;
		_float4 vAmbient;
		_float4 vSpecular;
	}LIGHT_DESC;

	typedef struct tagTextureInfo
	{
		_uint iNumTextures;
		_char szTexturePath[MAX_PATH];
	}TEXTURE_INFO;

	typedef struct tagMeshMaterialDesc
	{
		// AI_TEXTURE_TYPE_MAX == 21
		class CTexture*	MaterialTextures[21];

		vector<TEXTURE_INFO> TextureInfo;
	}MESH_MATERIAL;

	typedef struct tagKeyframeDesc
	{
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vTranslation;
		float		fTime;
	}KEYFRAME;

	typedef struct ENGINE_DLL tagVTXPOSDesc
	{
		XMFLOAT3		vPosition;

		static const unsigned int	iNumElements = { 1 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[1];
	}VTXPOS;

	typedef struct ENGINE_DLL tagVTXPOSTEXDesc
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 2 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[2];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVTXNORTEXDesc
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int	iNumElements = { 3 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[3];
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVTXMATRIXDesc
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
		bool			isLived;

	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVTXINSTANCE_RECTDesc
	{
		static const unsigned int	iNumElements = { 7 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[7];
	}VTXINSTANCE_RECT;

	typedef struct ENGINE_DLL tagVTXINSTANCE_POINTDesc
	{
		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];
	}VTXINSTANCE_POINT;

	typedef struct ENGINE_DLL tagVTXMESHDesc
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;	

		static const unsigned int	iNumElements = { 4 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[4];
	}VTXMESH;

	typedef struct ENGINE_DLL tagVTXANIMMESHDesc
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		/* 이 정점에게 영향을 주는 뼈들의 인덱스 */
		/* 뼈들의 인덱스 : 이 메시에게 영향을 주는 뼈들의 인덱스를 의미한다. */
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int	iNumElements = { 6 };
		static const D3D11_INPUT_ELEMENT_DESC	Elements[6];
	}VTXANIMMESH;

	


}