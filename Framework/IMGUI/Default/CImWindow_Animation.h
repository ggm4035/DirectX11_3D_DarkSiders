#pragma once

#include "CImWindow.h"

BEGIN(Engine)
class CGameInstance;
class CModel;
END

BEGIN(Imgui)

class CImWindow_Animation final : public CImWindow
{
    DECLARE_SINGLETON(CImWindow_Animation)

private:
    explicit CImWindow_Animation() = default;
    virtual ~CImWindow_Animation() = default;

public:
    _bool m_bIsOpen = { false };

    _int m_iCurrentFrame = { 0 };
    _int m_iCurrentRootFrame = { 0 };

public:
    void Set_Model(class CDummyObject3D* pModel);
    void Refresh_Animation();
    
public:
    virtual HRESULT Initialize() override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Refresh() override;

private:
    class CDummyObject3D* m_pModel = { nullptr };
    CModel* m_pModelCom = { nullptr };

    _uint m_iMaxKeyFrames = { 0 };
    _uint m_iMaxRootKeyFrames = { 0 };

private:
    void Show_Animation_KeyFrame(CGameInstance* pGameInstance);

public:
    virtual void Free() override;
};

END
