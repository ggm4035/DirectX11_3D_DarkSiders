#pragma once

#include "CRenderer.h"
#include "CVIBuffer_Rect.h"
#include "CVIBuffer_Cube.h"
#include "CVIBuffer_Trail.h"
#include "CInstance_Point.h"
#include "CViBuffer_Sprite.h"
#include "CVIBuffer_Terrain.h"
#include "CVIBuffer_Coordnate.h"
#include "CCollider.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CNavigation.h"
#include "CBehavior.h"
#include "CShader.h"
#include "CCamera.h"
#include "CModel.h"
#include "CBone.h"

#include "CHealth.h"
#include "CAttack.h"
#include "CDeffence.h"

#include "CBounding_OBB.h"
#include "CBounding_AABB.h"
#include "CBounding_Sphere.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Containers(const _uint& iNumLevels, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Add_Prototype(const _uint& iLevelIndex, const wstring& PrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(const _uint& iLevelIndex, const wstring& PrototypeTag, class CComponent* pOwner, void* pArg);
	class CComponent* Clone_Transform(class CComponent* pOwner, void* pArg);
	void Clear_LevelResources(const _uint& iLevelIndex);

public:
	list<class CComponent*> Get_All_Prototypes();

private:
	_uint m_iNumLevels = { 0 };

private:
	typedef unordered_map<wstring, class CComponent*> PROTOTYPES;
	PROTOTYPES* m_pPrototypes = { nullptr };
	class CTransform* m_pPrototype_Transform = { nullptr };

private:
	class CComponent* Find_Prototype(const _uint& iLevelIndex, const wstring& PrototypeTag);

public:
	virtual void Free() override;
};

END