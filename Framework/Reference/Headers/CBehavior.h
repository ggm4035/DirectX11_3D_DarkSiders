#pragma once
#include "CComponent.h"

/*
	BehaviorƮ���� ������Ҵ� ������ ����.
	Root	-	ai Ʈ���� ���۴ܰ�. �����带 ������ ������ ��� ai�� 
				������ root�� ������ �ִ�.

	Selector-	ai�� ���� �и��� ����1. ���̿켱 Ž���� ������ ������ ������,
				�ڽ� ��尡 false�� ��ȯ�ϸ� ���� ��带 ��� Ž���ؼ� true�� ��ȯ�ϴ�
				��带 ���� �� ���� ��ȸ�� �Ѵ�. ���� true�� ��ȯ�ϴ� ��带 ���� ���
				true�� ��ȯ�ϰ� ��� ��带 ��ȸ�ߴµ� �� false�ΰ�� false�� ��ȯ�Ѵ�.

	Sequence-	ai�� ���� �и��� ����2. ��� ��带 ��ȸ�ϴ� ������ ������ ������,
				�ڽĳ�尡 �ϳ��̶� False�� ��ȯ�� ��� False�� ��ȯ�Ѵ�.
				������ true�� ��ȯ�ϸ� ���� �ڽĳ��� �̵��Ѵ�.
				�׸��� ��� �ڽ� ������ ��ȸ �� ��� True�� ��ȯ�Ѵ�.

	Task	-	ai�� ���� ��ɺ�. ai�� ����� ����ϰ� ������, ���α׷��Ӱ�
				�ʿ��� ��� BehaviorŬ������ ��ӹ޾Ƽ� ����� �����ϸ� �ȴ�.
	
	�� �� ���α׷��Ӱ� ���������� ������ �ô� �κ��� Task �κ��̴�.
*/

BEGIN(Engine)

class ENGINE_DLL CBehavior abstract : public CComponent
{
public:
	typedef struct tagBehavior
	{
		HRESULT result = { S_OK };
		wstring wstrBehaviorTag;
		CBehavior* pBehavior = { nullptr };
	}BEHAVIOR;

protected:
	enum RETURNVALUE { BEHAVIOR_SUCCESS, BEHAVIOR_RUNNING, BEHAVIOR_FAIL, BEHAVIOR_ERROR, BEHAVIOR_END };

protected:
	explicit CBehavior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBehavior(const CBehavior& rhs);
	virtual ~CBehavior() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) override;
	virtual HRESULT Tick(const _double& TimeDelta) = 0;

public:
	HRESULT Add_Decoration(function<_bool(class CBlackBoard*)> Func);
	_bool Check_Decorations();
	HRESULT Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior);

protected:
	/* ������� ��Ʈ���� �ϳ��� ������ ��Ʈ����� �ڽĵ��� ��Ʈ����� �����带 �����ϴ� ���·� ������ ���̱� ������
	   �������� ������ ��Ʈ��忡�� �����Ѵ�. */
	class CBlackBoard* m_pBlackBoard = { nullptr };
	CBehavior* m_pParentBehavior = { nullptr };
	_bool m_isCloned = { false };

	list<class CDecoration*> m_DecorationList;
	list<BEHAVIOR> m_BehaviorList;
	list<BEHAVIOR>::iterator m_iterCurBehavior;

protected:
	CBehavior* Find_Behavior(const wstring& BehaviorTag);

protected:
	virtual CBehavior* Clone(const _uint& iLevelIndex, CComponent* pOwner, void* pArg) = 0;
	virtual void Free() override;
};

END