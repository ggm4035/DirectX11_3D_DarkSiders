#pragma once

#include "IMGUI_Defines.h"

BEGIN(Imgui)

struct CToolInstance final : public CBase
{
	DECLARE_SINGLETON(CToolInstance)

private:
	explicit CToolInstance();
	virtual ~CToolInstance() = default;

public:
    class CImWindow_Base* m_pBaseWindow = { nullptr };
    class CImWindow_Inspector* m_pInspectorWindow = { nullptr };
    class CImWindow_Create* m_pCreateWindow = { nullptr };
    class CImWindow_Top* m_pTopWindow = { nullptr };

public:
    class CDummyObject3D* m_pCurrentObject = { nullptr };
    vector<MODEL_BINARYDATA>* m_pModelDatas;
    vector<MODEL_BINARYDATA>* m_pAnimModelDatas;
    list<string>* m_pFilePaths;

public:
    class CMainCamera* m_pCamera = { nullptr };

private:
    virtual void Free() override;
};

END

#define TOOL CToolInstance::GetInstance()
