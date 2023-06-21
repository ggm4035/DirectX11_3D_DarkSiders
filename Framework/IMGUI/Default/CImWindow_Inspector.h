#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Imgui)

class CImWindow_Inspector final : public CImWindow
{
    DECLARE_SINGLETON(CImWindow_Inspector)

private:
    explicit CImWindow_Inspector();
    virtual ~CImWindow_Inspector() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    _char m_szTag[MAX_PATH] = { "" };

private:
    void Show_Components();
    void Show_Transform();
    void Show_Texture();
    void Show_Shader();
    void Show_Buffer();

public:
    virtual void Free() override;
};

END
