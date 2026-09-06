#include "GameInstance.h"

#include "Collision_Manager.h"
#include "Graphic_Device.h"
#include "Object_Manager.h"
#include "Target_Manager.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "IMGUI_Manager.h"
#include "Light_Manager.h"
#include "Input_Device.h"
#include "FontManager.h"
#include "Picking.h"

#include "Renderer.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스를 초기화한다 .*/
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pInput_Device = CInput_Device::Create(hInstance, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create(*ppDevice, *ppContext);
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	/* 인풋 디바이스를 초기화한다 .*/

	/* 사운드 디바이스를 초기화한다 .*/
	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	/* 오브젝트 매니져의 공간 예약을 한다. */
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	/* 컴포넌트 매니져의 공간 예약을 한다. */
	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;


	return S_OK;
}

HRESULT CGameInstance::Initialize_IMGUI(CTool* pTool)
{
	m_pIMGUI_Mnanager = CIMGUI_Manager::Create(pTool);

	if (nullptr == m_pIMGUI_Mnanager)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pPipeLine ||
		nullptr == m_pPicking)
		return;

	m_pInput_Device->Tick();

	m_pObject_Manager->Tick(fTimeDelta);

	m_pPicking->Update();

	m_pPipeLine->Tick();

	m_pCollision_Manager->Update_Collision();


	/* 반복적인 갱신이 필요한 객체들의 Tick함수를 호출한다. */
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Late_Tick(fTimeDelta);


	if (nullptr != m_pIMGUI_Mnanager)
		m_pIMGUI_Mnanager->Tick(fTimeDelta);
}

HRESULT CGameInstance::Begin_Draw(const _float4& vClearColor)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Draw()
{
	/* 화면에 그려져야할 객체들을 그리낟. == 오브젝트 매니져에 들어가있을꺼야 .*/
	/* 오브젝트 매니져에 렌더함수를 만들어서 호출하면 객체들을 다 그린다. */

	/* But. CRenderer객체의 렌더함수를 호출하여 객체를 그리낟. */
	m_pRenderer->Render();

	m_pLevel_Manager->Render();

	m_pRenderer->AfterRender();

	if (nullptr != m_pIMGUI_Mnanager)
		m_pIMGUI_Mnanager->Render();


	return S_OK;
}

HRESULT CGameInstance::Clear(_uint iClearLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	/* 지정된 레벨용 자원(텍스쳐, 사운드, 객체등등) 을 삭제한다. */

	/* 사본 게임오브젝트. */
	m_pObject_Manager->Clear(iClearLevelIndex);

	/* 컴포넌트 원형 */
	m_pComponent_Manager->Clear(iClearLevelIndex);

	return S_OK;
}

EKeyState CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return EKeyState::_END;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

EKeyState CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return EKeyState::_END;

	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

HRESULT CGameInstance::Open_Level(_uint iNewLevelID, CLevel* pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iNewLevelID, pNewLevel);
}

HRESULT CGameInstance::Add_Prototype(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, _float4 Position, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, Position, pArg);
}

HRESULT CGameInstance::Add_Clone(_Out_ CGameObject** pObject, _uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clone(pObject, iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_Object(strPrototypeTag, pArg);
}

const CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);
}

list<CGameObject*>* CGameInstance::Get_LayerList(int iLevelIndex, const wstring& strLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_LayerList(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

void CGameInstance::Transform_PickingToLocalSpace(const CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos)
{
	if (nullptr == m_pPicking)
		return;

	return m_pPicking->Transform_PickingToLocalSpace(pTransform, pRayDir, pRayPos);
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPosition_Vector();
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition_Float4();
}

const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Add_Light(CLight* pLight)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pLight);
}

HRESULT CGameInstance::Delete_Light(CLight* pDeleteLight)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Delete_Light(pDeleteLight);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Clear_Lights()
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pLight_Manager->Clear();
	return S_OK;
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRadian)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRadian);
}


HRESULT CGameInstance::Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strRenderTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RTShaderResource(CShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strRenderTargetTag, pConstantName);
}

HRESULT CGameInstance::Copy_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D** ppTextureHub)
{
	return m_pTarget_Manager->Copy_Resource(strRenderTargetTag, ppTextureHub);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RTVDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Draw_RTVDebug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif


HRESULT CGameInstance::Add_Collision_Group(CGameObject* pCollisionObject)
{
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	return m_pCollision_Manager->Add_Collision_Group(pCollisionObject);
}

int CGameInstance::VolumeUp(CSound_Manager::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->VolumeUp(eID, _vol);
}

int CGameInstance::VolumeDown(CSound_Manager::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->VolumeDown(eID, _vol);
}

int CGameInstance::BGMVolumeUp(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->BGMVolumeUp(_vol);
}

int CGameInstance::BGMVolumeDown(_float _vol)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->BGMVolumeDown(_vol);
}

int CGameInstance::Pause(CSound_Manager::CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return 0;

	return m_pSound_Manager->Pause(eID);
}

void CGameInstance::Play_Sound(TCHAR* pSoundKey, CSound_Manager::CHANNELID eID, _float _vol)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Play_Sound(pSoundKey, eID, _vol);
}

void CGameInstance::Play_BGM(TCHAR* pSoundKey)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->Play_BGM(pSoundKey);
}

void CGameInstance::StopSound(CSound_Manager::CHANNELID eID)
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->StopSound(eID);
}

void CGameInstance::StopAll()
{
	if (nullptr == m_pSound_Manager)
		return;

	return m_pSound_Manager->StopAll();
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Free();

	Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pIMGUI_Mnanager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pPicking);
	Safe_Release(m_pPipeLine);
}
