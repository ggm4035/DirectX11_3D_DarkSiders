#pragma once

#include "CBase.h"

BEGIN(Engine)

class CCalculator final : public CBase
{
	DECLARE_SINGLETON(CCalculator)

private:
	explicit CCalculator() = default;
	virtual ~CCalculator() = default;

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	_vector Picking_On_Triangle(const POINT& ptMouse, class CVIBuffer* pBuffer, class CTransform* pTransform);
	_vector Picking_On_Triangle(const POINT& ptMouse, class CModel* pModel, class CTransform* pTransform);
	_fvector Get_RandomVectorInSphere(const _float& fRadius);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;

#if defined(_USE_IMGUI) || defined(_DEBUG)

	_vector Picking_On_Spheres(const POINT& ptMouse, class CNavigation* pNavigation, class CTransform* pTransform);
	vector<pair<_uint, _int>> Pick_Spheres(const POINT& ptMouse, class CNavigation* pNavigation, class CTransform* pTransform);

#endif
};

END