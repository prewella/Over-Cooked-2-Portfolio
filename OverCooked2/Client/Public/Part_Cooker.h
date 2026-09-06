#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CPart_Cooker final : public CPartObject
{
public:
	typedef struct tagPartCookerDesc: public CPartObject::PARTOBJECT_DESC
	{
		CBone* pSocket = { nullptr };
		INTERACTIVE_TOOL_TYPE* pTool = { nullptr };
	}COOKER_DESC;

private:
	CPart_Cooker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPart_Cooker(const CPart_Cooker& rhs);
	virtual ~CPart_Cooker() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CBone* m_pSocket = { nullptr };
	INTERACTIVE_TOOL_TYPE* m_pTool = { nullptr };

private:
	HRESULT Add_Components(wstring strModelTag);
	HRESULT Bind_ShaderResources();

public:
	static CPart_Cooker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END