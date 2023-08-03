#pragma once

#include "CRandomSelector.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CBoss_Attacks final : public CSelector
{
private:
	explicit CBoss_Attacks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_Attacks(const CBoss_Attacks& rhs);
	virtual ~CBoss_Attacks() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	HRESULT Assemble_Childs();
	void Set_CoolTime(const _float& fTime) {
		m_fLimit = fTime;
	}

private:
	// ��ü���� ������ TimeAcc�� �����ϱ� ���� ����
	// ��Ÿ�� üũ�뵵�ε� ��� ������ �� 
	_float m_fPreTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

private:
	CModel* m_pModel = { nullptr };

public:
	static CBoss_Attacks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBoss_Attacks* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END