#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Editor final : public CLevel
{
private:
	CLevel_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Editor() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;

private:
	HRESULT				Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT				Ready_Layer_Terrain(const wstring& strLayerTag);

public:
	static CLevel_Editor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

END