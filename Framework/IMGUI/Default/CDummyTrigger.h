#pragma once

#include "IMGUI_Defines.h"
#include "CTrigger.h"

BEGIN(Imgui)

class CDummyTrigger final : public CTrigger
{
private:
    explicit CDummyTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CDummyTrigger(const CDummyTrigger& rhs);
    virtual ~CDummyTrigger() = default;

public:
    void Set_Extents(const _float3& vExtents) {
        m_pColliderCom->Set_Extents(vExtents);
        m_TriggerDesc.vExtents = vExtents;
    }
    const TRIGGERDESC& Get_TriggerDesc() {
        return m_TriggerDesc;
    }
    
    CTransform* Get_Transform() { return m_pTransformCom; }
    void Updata_Toggle() { m_isUpdate = m_isUpdate ? false : true; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
    virtual void Tick(const _double& TimeDelta) override;
    virtual void Late_Tick(const _double& TimeDelta) override;
    virtual HRESULT Render() override;

public:
    virtual void OnCollisionEnter(Engine::CCollider::COLLISION Collision, const Engine::_double& TimeDelta) {}
    virtual void OnCollisionStay(Engine::CCollider::COLLISION Collision, const Engine::_double& TimeDelta) {}
    virtual void OnCollisionExit(Engine::CCollider::COLLISION Collision, const Engine::_double& TimeDelta) {}

private:
    _bool m_isUpdate = { true };

private:
    virtual HRESULT Add_Components() override;

public:
    static CDummyTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CDummyTrigger* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg);
    virtual void Free() override;
};

END
