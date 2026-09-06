#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC
	{		
	}GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const wstring& strComTag);
	void	Set_PrototypeName(wstring strObjectName) { m_strObjectName = strObjectName; }
	wstring	Get_PrototypeName() { return m_strObjectName; }
	void	Set_Erase(_bool bDead = true) { m_bDead = bDead; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };

	class CGameInstance*				m_pGameInstance = { nullptr };
	CTransform*							m_pTransformCom = { nullptr };

	class CShader*						m_pShaderCom = { nullptr };

	wstring								m_strObjectName = { TEXT("") };

	_bool								m_bDead = { false };

protected:
	map<const wstring, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, class CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END