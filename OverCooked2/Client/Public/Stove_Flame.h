#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Client)

class CStove_Flame final : public CPartObject
{
public:
	typedef struct tagPartCookerDesc : public CPartObject::PARTOBJECT_DESC
	{
		_bool* pIsRender = { nullptr };
	}STOVEFLAME_DESC;

private:
	CStove_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStove_Flame(const CStove_Flame& rhs);
	virtual ~CStove_Flame() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool* m_pIsRender; 

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CStove_Flame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END