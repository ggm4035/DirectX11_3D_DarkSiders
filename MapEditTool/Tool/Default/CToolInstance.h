#pragma once

#include "CBase.h"

struct CToolInstance final : public CBase
{
	DECLARE_SINGLETON(CToolInstance)

private:
	CToolInstance();
	virtual ~CToolInstance() = default;

public:
	class CTerrain* m_pTerrain = { nullptr };
	class CMainCamera* m_pCamera = { nullptr };
	class CToolView* m_pToolView = { nullptr };
	class CMainFrame* m_pMainFrm = { nullptr };
	class CCoordnate_Axis* m_pCoordnate_Axis = { nullptr };

private:
	virtual void Free() override {}
};

#define TOOL CToolInstance::GetInstance()