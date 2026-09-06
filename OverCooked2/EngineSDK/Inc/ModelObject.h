#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CModelObject abstract : public CGameObject
{
public:
	typedef struct tagModelObjectDesc : public CGameObject::GAMEOBJECT_DESC
	{
		wstring strModelTag = TEXT("");
	}MODELOBJECT_DESC;


protected:
	CModelObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelObject(const CModelObject& rhs);
	virtual ~CModelObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CModel* Get_ModelCom() { return m_pModelCom; }

protected:
	class CModel* m_pModelCom = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
