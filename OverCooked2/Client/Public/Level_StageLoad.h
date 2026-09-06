#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_StageLoad final : public CLevel
{
private:
	CLevel_StageLoad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_StageLoad() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI();

private:
	_float fLoadingPercent = { 0.f };
	class CUI_Transition* m_pUI_Transition = { nullptr };

public:
	static CLevel_StageLoad* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END