#pragma once

#include "CLevel.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	explicit CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(const _double& TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float4x4 m_PlayerWorldMatrix;
	_float3 m_vPlayerAngle;
	vector<MODELDATA> m_vecMonsterDatas;

private:
	class CUI_Rect* m_pFadeIn = { nullptr };
	class CUI_Rect* m_pPlayerUI[8] = { nullptr };

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_BackGround(wstring pLayerTag);
	HRESULT Ready_Layer_Player(wstring pLayerTag);
	HRESULT Ready_Layer_Monster(wstring pLayerTag);
	HRESULT Ready_HUD();

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END