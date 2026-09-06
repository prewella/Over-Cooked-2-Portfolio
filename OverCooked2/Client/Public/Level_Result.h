#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Result final : public CLevel
{
private:
	CLevel_Result(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Result() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Calc_Score_Render_Time(_float fTimeDelta);
	void Add_Star(_float fTimeDelta);

	void Render_ScoreFont();

	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_NPC();
	HRESULT Ready_Layer_UI();

private:
	// Score 
	class CData_Storage* m_pDataStorage = { nullptr };

	_float m_fRenderFontCounter = { 0.f };
	_bool m_IsRenderPassOrderNum = { false };
	_bool m_IsRenderPassOrderScore = { false };
	_bool m_IsRenderTip = { false };
	_bool m_IsRenderTipScore = { false };
	_bool m_IsRenderTotal = { false };
	_bool m_IsRenderTotal_Score = { false };


	/* Star UI */
	_uint	m_iNumAddStarIdx = { 0 };
	_uint	m_iAddStarIdx = { 0 };
	_float	m_fAddStar_Counter = { 0.f };

	list<class CUI_Result_Empty_Star*> m_pResultStars;

public:
	static CLevel_Result* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END