#pragma once

#include "Client_Defines.h"
#include "ModelObject.h"

BEGIN(Engine)
class CLight;
END

BEGIN(Client)

#define RedLight 5.f
#define YellowLight 1.f
#define GreenLight 3.f

class CTraffic_Light final : public CModelObject
{
public:
	typedef struct tagTrafficLight_Desc : public MODELOBJECT_DESC {
		_float4 vStartPos = { _float4(0.f, 0.f, 0.f, 0.f) };
		_float4 vGoalPos = { _float4(0.f, 0.f, 0.f, 0.f) };

		_bool IsCroosAutomobile = { false };
	}TRAFFIC_LIGHT_DESC;

private:
	CTraffic_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTraffic_Light(const CTraffic_Light& rhs);
	virtual ~CTraffic_Light() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	void	Ready_Light();
	void	IsCrossAutomobile(_bool IsCroosAutomobile) {
		m_IsInitCroosAutomobile = IsCroosAutomobile;
	}
	_bool	Get_IsCrossAutomobile() {
		return m_IsInitCroosAutomobile;
	}

	void	Set_PatrolPos(_int iPosIdx, _float4 vPatrolPos) {
		m_Automobile_PatrolPos[iPosIdx] = vPatrolPos;
	}

	_float4 Get_PatrolPos(_uint iIdx) {
		return m_Automobile_PatrolPos[iIdx];
	}

private:
	void	ReSetting_Light();
	void	Cross_Automobile();

private:
	class CLight* m_pSignLight = { nullptr };
	class CLight* m_pAmbientLight = { nullptr };

	_uint	m_iSignalIdx = { 0 };
	_float	m_fSignalTime = { 0.f };

	vector<_float4> m_Automobile_PatrolPos;
	_bool	m_IsInitCroosAutomobile = { false };
	_bool	m_IsCroosAutomobile = { false };

public:
	static CTraffic_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END