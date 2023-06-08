#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Imgui)

class CImWindow_Inspector final : public CImWindow
{
private:
    explicit CImWindow_Inspector();
    virtual ~CImWindow_Inspector() = default;

public:
    HRESULT Bind_GameObject(const wstring& wstrGameObjectTag);

public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

public:
    CGameObject* m_pCurGameObject = { nullptr };
    _char m_szTag[256];

private:
    void Show_Components();
    void Show_Transform();
    void Show_Renderer();
    void Show_Texture();
    void Show_Shader();
    void Show_Buffer();

public:
    static CImWindow_Inspector* Create(void* pArg = nullptr);
    virtual void Free() override;
};

END
