#pragma once

#include "Imgui_Defines.h"
#include "CBase.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Imgui)

class CImWindow abstract : public CBase
{
public:
    enum LAYERTYPE { LAYER_STATIC, LAYER_MONSTER, LAYER_END };

protected:
    explicit CImWindow();
    virtual ~CImWindow() = default;

public:
    virtual HRESULT Initialize();
    virtual void Tick(const _double& TimeDelta) = 0;
    virtual void Refresh();

protected: /* For.CDummyObject3D */
    list<class CDummyObject3D*> m_GameObjectList[LAYER_END];
    _uint m_iNumGameObjects[LAYER_END] = { 0 };

protected: /* For.CDummyUI */

protected: /* For.CDummyCamera */

protected:
    class CDummyObject3D* Find_GameObject(const wstring& GameObjectTag, LAYERTYPE eType = LAYER_STATIC);
    class CDummyUI* Find_UI(const wstring& UITag);

public:
    virtual void Free(void) override;
};

END
