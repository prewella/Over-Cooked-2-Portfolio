#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Instance;
class CTexture;
class CShader;
END

BEGIN(Client)

class CMy_Instance abstract : public CGameObject  
{
protected:
	CMy_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMy_Instance(const CMy_Instance& rhs);
	virtual ~CMy_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

protected:
	CVIBuffer_Instance*		m_pVIBufferCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END
