#pragma once

#include "CRenderer.h"
#include "CVIBuffer_Rect.h"
#include "CVIBuffer_Terrain.h"
#include "CVIBuffer_Coordnate.h"
#include "CTexture.h"
#include "CTransform.h"
#include "CShader.h"
#include "CCamera.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Containers(_uint iNumLevels, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	class CComponent* Clone_Transform(void* pArg);
	void Clear_LevelResources(_uint iLevelIndex);

private:
	_uint m_iNumLevels = { 0 };

private:
	typedef unordered_map<const _tchar*, class CComponent*> PROTOTYPES;
	PROTOTYPES* m_pPrototypes = { nullptr };
	class CTransform* m_pPrototype_Transform = { nullptr };

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);

public:
	virtual void Free() override;
};

END