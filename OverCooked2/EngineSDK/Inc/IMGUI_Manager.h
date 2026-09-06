#pragma once

#include "Base.h"

BEGIN(Engine)

class CIMGUI_Manager final : public CBase
{
private:
	CIMGUI_Manager();
	virtual ~CIMGUI_Manager() = default;

public:
	HRESULT					Initiailize(class CTool* _pTool);
	void					Tick(_float fTimeDelta);
	HRESULT					Render();

public:
	static CIMGUI_Manager* Create(class CTool* _pTool);
	virtual void			Free();

private:
	class CTool* m_pTool = { nullptr };
};

END