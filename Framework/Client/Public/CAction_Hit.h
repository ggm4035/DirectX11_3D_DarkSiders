#pragma once

#include "CSequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CModel;
END

BEGIN(Client)

class CAction_Hit final : public CSequence
{
private:
	explicit CAction_Hit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAction_Hit(const CAction_Hit& rhs);
	virtual ~CAction_Hit() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	HRESULT Assemble_Childs();
	
public:
	void Bind_SoundTag(const wstring& wstrSoundTag) {
		m_wstrSoundTag = wstrSoundTag;
	}
	void Not_Impact() {
		m_isImpact = false;
	}
	void Not_Look() {
		m_isLook = false;
	}

private:
	wstring m_wstrSoundTag = { L"" };
	_bool m_isLook = { true };
	_bool m_isImpact = { true };

public:
	static CAction_Hit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAction_Hit* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END