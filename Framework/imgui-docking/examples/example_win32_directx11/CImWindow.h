#pragma once

#include "Imgui_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Imgui)

class CImWindow abstract : public CBase
{
protected:
    explicit CImWindow();
    virtual ~CImWindow() = default;

public:
    virtual HRESULT Initialize(void* pArg);
    virtual void Tick(const _double& TimeDelta) = 0;
    virtual void Refresh();

protected:
    list<CGameObject*> m_GameObjectList;
    _uint m_iNumGameObjects = { 0 };

protected:
    CGameObject* Find_GameObject(const wstring& GameObjectTag);

public:
    virtual void Free(void) override;
};

END
