#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_MainMenu final : public CLevel
{
private:
	CLevel_MainMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MainMenu() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Load_Data(const wstring& strLayerTag);
	HRESULT Ready_Layer_UI();

private:
	class CUI_Stage_Select_Header* m_pUI_Stage_Select_Header = { nullptr };

	class CUI_Stage_Select_Manager* m_pUI_Stage_Select_Manager = { nullptr };

	class CUI_Transition* m_pUI_Transition = { nullptr };
	_bool m_IsSelected_Stage = { false };

public:
	static CLevel_MainMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END