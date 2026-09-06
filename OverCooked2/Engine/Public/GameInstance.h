#pragma once

/* 클라이언트개발자가 엔진의 기능을 사용하기위해서 항상 접근해야하는 클래스. */
#include "Renderer.h"
#include "Component_Manager.h"
#include "Sound_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	HRESULT	Initialize_IMGUI(class CTool* pTool);

	void Tick_Engine(_float fTimeDelta);
	HRESULT Begin_Draw(const _float4& vClearColor);
	HRESULT End_Draw();
	HRESULT Draw();
	HRESULT Clear(_uint iClearLevelIndex);

public: /* For.Input_Device */
	EKeyState	Get_DIKeyState(_ubyte byKeyID);
	EKeyState	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);

public: /* For.Renderer */
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iNewLevelID, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, _float4 Position, void* pArg = nullptr);
	HRESULT Add_Clone(_Out_ class CGameObject** pObject, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg = nullptr);
	const CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	list<class CGameObject*>* Get_LayerList(int iLevelIndex, const wstring& strLayerTag);


public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Picking */
	void Transform_PickingToLocalSpace(const class CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos);

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	_float4 Get_CamPosition_Float4() const;

public: /* For. Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Add_Light(class CLight* pLight);
	HRESULT Delete_Light(class CLight* pDeleteLight);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Clear_Lights();

public: /* For. Font_Manager */
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRadian);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RTShaderResource(class CShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT Copy_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D** ppTextureHub);

#ifdef _DEBUG
	HRESULT Ready_RTVDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Draw_RTVDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public: /* For. Collision_Manager */
	HRESULT Add_Collision_Group(CGameObject* pCollisionObject);


public: /* For Sound_Manager*/
	int  VolumeUp(CSound_Manager::CHANNELID eID, _float _vol);
	int  VolumeDown(CSound_Manager::CHANNELID eID, _float _vol);
	int  BGMVolumeUp(_float _vol);
	int  BGMVolumeDown(_float _vol);
	int  Pause(CSound_Manager::CHANNELID eID);
	void Play_Sound(TCHAR* pSoundKey, CSound_Manager::CHANNELID eID, _float _vol);
	void Play_BGM(TCHAR* pSoundKey);
	void StopSound(CSound_Manager::CHANNELID eID);
	void StopAll();

private:
	class CComponent_Manager* m_pComponent_Manager = { nullptr };
	class CCollision_Manager* m_pCollision_Manager = { nullptr };
	class CGraphic_Device* m_pGraphic_Device = { nullptr };
	class CObject_Manager* m_pObject_Manager = { nullptr };
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CSound_Manager* m_pSound_Manager = { nullptr };
	class CTimer_Manager* m_pTimer_Manager = { nullptr };
	class CLevel_Manager* m_pLevel_Manager = { nullptr };
	class CIMGUI_Manager* m_pIMGUI_Mnanager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };
	class CInput_Device* m_pInput_Device = { nullptr };
	class CFont_Manager* m_pFont_Manager = { nullptr };
	class CRenderer* m_pRenderer = { nullptr };
	class CPipeLine* m_pPipeLine = { nullptr };
	class CPicking* m_pPicking = { nullptr };

public:
	static void Release_Engine();
	virtual void Free() override;
};

END