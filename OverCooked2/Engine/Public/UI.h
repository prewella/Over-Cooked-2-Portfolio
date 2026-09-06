#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI abstract : public CGameObject
{
public:
	typedef struct tagUIDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float2 vPos;
		_float2 vSize;
	}UI_DESC;

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_uint Get_RendOrder() { return m_iRenderOrder; }
	_float2 Get_Size() { return m_vSize; }
	void Set_Size(_float2 vSettingSize) {
		m_vSize = vSettingSize;
		m_pTransformCom->Set_Scaled(m_vSize.x, m_vSize.y, 1.f);
	}
	_float2 Get_Position() { return m_vPos; };
	void Set_Pos(_uint iWinSizeX, _uint iWinSizeY, _float2 vSettingPos) {
		m_vPos = vSettingPos;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,
			XMVectorSet(m_vPos.x - iWinSizeX * 0.5f, -m_vPos.y + iWinSizeY * 0.5f, 0.f, 1.f ));
	}

protected:
	void Initialize_UI_Setting(_uint iWinSizeX, _uint iWinSizeY);

protected:
	_uint				m_iRenderOrder = { 0 };

	class CTexture*		m_pTextureCom = { nullptr };
	class CVIBuffer*	m_pVIBufferCom = { nullptr };

	_float2				m_vPos = {}, m_vSize = {};
	_float4x4			m_ViewMatrix = {}, m_ProjMatrix = {};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
