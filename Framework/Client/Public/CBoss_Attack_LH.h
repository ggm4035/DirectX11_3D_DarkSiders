#pragma once

#include "CBehavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CBoss_Attack_LH final : public CBehavior
{
private:
	explicit CBoss_Attack_LH(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss_Attack_LH(const CBoss_Attack_LH& rhs);
	virtual ~CBoss_Attack_LH() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) override;

public:
	void Set_CoolTime(const _float& fTime) {
		m_fLimit = fTime;
	}

private:
	// ��ü���� ������ TimeAcc�� �����ϱ� ���� ����
	// ��Ÿ�� üũ�뵵�ε� ��� ������ �� 
	_float m_fPreTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };
	_float m_fTimeAcc = { 0.f }; // ���� üũ��
	_bool m_isFirst = { true };
	_bool m_isPlaySound = { false };

private:
	CModel* m_pModel = { nullptr };

public:
	static CBoss_Attack_LH* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBoss_Attack_LH* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual void Free() override;
};

END