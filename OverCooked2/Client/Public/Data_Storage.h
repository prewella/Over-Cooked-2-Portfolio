#pragma once

#include "Client_Defines.h"
#include "Component.h"

BEGIN(Client)

class CData_Storage final : public CComponent
{
#define StageNum 2

private:
	CData_Storage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CData_Storage(const CData_Storage& rhs);
	virtual ~CData_Storage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	STAGE_SCORE_INFO* Get_PlayData(_uint iStageIdx) {
		return (*m_StagePlayDatas)[iStageIdx];
	}

private:
	vector<STAGE_SCORE_INFO*>* m_StagePlayDatas = { nullptr };

public:
	static CData_Storage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END