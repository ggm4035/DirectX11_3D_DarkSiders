#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Imgui)

class CImWindow_Inspector final : public CImWindow
{
    DECLARE_SINGLETON(CImWindow_Inspector)

private:
    explicit CImWindow_Inspector() = default;
    virtual ~CImWindow_Inspector() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    _char m_szTag[MAX_PATH] = { "" };

private:
    void Show_Components(CGameInstance* pGameInstance);
    void Show_ObjectInfo(CGameInstance* pGameInstance);
    void Show_Transform();
    void Show_Texture();
    void Show_Shader();
    void Show_Buffer();
    void Show_Trigger(CGameInstance* pGameInstance);

public:
    virtual void Free() override;
};

END
