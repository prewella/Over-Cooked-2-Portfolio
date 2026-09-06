#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT Initialize();

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Add_Light(class CLight* pLight);
	HRESULT Delete_Light(class CLight* pDeleteLight);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
	void Clear();

private:
	list<class CLight*> m_Lights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END