#pragma once

#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		explicit tagNavigationDesc() = default;
		explicit tagNavigationDesc(const tagNavigationDesc& rhs)
			: iCurrentIndex(rhs.iCurrentIndex) {}

		_int iCurrentIndex = { -1 };
	}NAVIGATIONDESC;

private:
	explicit CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

#ifdef _DEBUG
public:
	HRESULT Render_Navigation();

private:
	class CShader* m_pShader = { nullptr };
#endif // DEBUG

public:
	virtual HRESULT Initialize_Prototype(const wstring& wstrCellFilePath);
	virtual HRESULT Initialize(CComponent* pOwner, void* pArg);

private:
	NAVIGATIONDESC m_NavigationDesc;
	vector<class CCell*> m_vecCells;

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& wstrCellFilePath);
	virtual CNavigation* Clone(CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END