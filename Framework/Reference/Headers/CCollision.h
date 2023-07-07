#pragma once

#include "CBase.h"

BEGIN(Engine)

class CCollision final : public CBase
{
private:
	explicit CCollision();
	virtual ~CCollision() = default;

public:

private:
	class CGameObject3D* pOther = { nullptr };
	class CCollider* pOtherCollider = { nullptr };
	class CTransform* pOtherTransform = { nullptr };

public:
	virtual void Free() override;
};

END