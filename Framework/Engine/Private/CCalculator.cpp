#include "CCalculator.h"

#include "CPipeLine.h"
#include "CVIBuffer.h"
#include "CTransform.h"
#include "CMesh.h"

IMPLEMENT_SINGLETON(CCalculator)

CCalculator::CCalculator()
{

}

HRESULT CCalculator::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

_vector CCalculator::Picking_On_Triangle(const POINT& ptMouse, class CVIBuffer* pBuffer, class CTransform* pTransform)
{
	D3D11_VIEWPORT ViewPort;
	ZeroMemory(&ViewPort, sizeof D3D11_VIEWPORT);

	_uint iNumViewPorts = 1;

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

	// 뷰포트 -> 투영 스페이스
	_float3 vMousePos;
	vMousePos.x = (_float)ptMouse.x / (ViewPort.Width * 0.5f) - 1.f;
	vMousePos.y = (_float)ptMouse.y / -(ViewPort.Height * 0.5f) + 1.f;
	vMousePos.z = 0.f;

	if (fabs(vMousePos.x) > 1.f || fabs(vMousePos.y) > 1.f)
		return XMVectorSet(-1.f, -1.f, -1.f, -1.f);

	// 투영 스페이스 -> 뷰 스페이스
	_matrix ProjMatrix = CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_PROJ); 
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);
	_vector vViewPos = XMVector3TransformCoord(XMLoadFloat3(&vMousePos), ProjMatrix);

	_vector vRayOrigin, vRayDirection;
	vRayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	vRayDirection = XMVector3Normalize(vViewPos - vRayOrigin);

	// 뷰 스페이스 -> 월드 스페이스
	_matrix ViewMatrix = CPipeLine::GetInstance()->Get_Transform_Matrix(CPipeLine::STATE_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	vRayOrigin = XMVector3TransformCoord(vRayOrigin, ViewMatrix);
	vRayDirection = XMVector3Normalize(XMVector3TransformNormal(vRayDirection, ViewMatrix));

	// 월드 스페이스 -> 로컬 스페이스
	_matrix WorldMatrix = XMLoadFloat4x4(&pTransform->Get_WorldMatrix());
	_matrix WorldMatrixInverse = XMMatrixInverse(nullptr, WorldMatrix);
	vRayOrigin = XMVector3TransformCoord(vRayOrigin, WorldMatrixInverse);
	vRayDirection = XMVector3Normalize(XMVector3TransformNormal(vRayDirection, WorldMatrixInverse));

	// 이제 로컬상의 좌표를 버퍼의 각 삼각형 끼리 충돌처리 연산을 진행한다.
	_float fDistance = { 0.f };
	_float fNearDistance = { FLT_MAX };
	const vector<TRIANGLE>& vecTriangle = pBuffer->Get_vecTriangle();

	_vector vDot[3];
	for (auto& iter : vecTriangle)
	{
		vDot[0] = XMLoadFloat3(&iter.vDot[0]);
		vDot[1] = XMLoadFloat3(&iter.vDot[1]);
		vDot[2] = XMLoadFloat3(&iter.vDot[2]);

		if (true == TriangleTests::Intersects(vRayOrigin, vRayDirection, vDot[0], vDot[1], vDot[2], fDistance))
			fNearDistance = fNearDistance > fDistance ? fDistance : fNearDistance;
	}

	if (999999.f < fNearDistance)
		return XMVectorSet(-1.f, -1.f, -1.f, -1.f);

	_vector vPickPos = vRayOrigin + (vRayDirection * fNearDistance);
	vPickPos = XMVector3TransformCoord(vPickPos, WorldMatrix);

	return vPickPos;
}

_vector CCalculator::Picking_On_Triangle(const POINT& ptMouse, CModel* pModel, CTransform* pTransform)
{
	_vector OutData = XMVectorSet(-1.f, -1.f, -1.f, -1.f);
	for (_uint i = 0; i < pModel->Get_NumMeshes(); ++i)
	{
		OutData = Picking_On_Triangle(ptMouse, pModel->Get_Meshes()[i], pTransform);

		if (0 < OutData.m128_f32[3])
			return OutData;
	}

	return OutData;
}

void CCalculator::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
