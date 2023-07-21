#include "CQuardTree.h"

#include "CFrustum.h"
#include "CPipeLine.h"

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenter = (iRT + iLB) >> 1;

	_uint iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	m_pChilds[CHILD_LT] = CQuadTree::Create(iLT, iTC, m_iCenter, iLC);
	m_pChilds[CHILD_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenter);
	m_pChilds[CHILD_RB] = CQuadTree::Create(m_iCenter, iRC, iRB, iBC);
	m_pChilds[CHILD_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, iLB);

	return S_OK;
}

HRESULT CQuadTree::Make_Neighbors()
{
	if (nullptr == m_pChilds[CHILD_LT]->m_pChilds[CHILD_LT])
		return S_OK;

	m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RT];
	m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_LB];

	m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CHILD_LT];
	m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChilds[CHILD_RB];

	m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CHILD_RT];
	m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChilds[CHILD_LB];

	m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChilds[CHILD_LT];
	m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChilds[CHILD_RB];

	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RT];
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChilds[CHILD_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		m_pChilds[CHILD_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_LB];
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChilds[CHILD_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		m_pChilds[CHILD_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LT];
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChilds[CHILD_LB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		m_pChilds[CHILD_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_LT];
		m_pChilds[CHILD_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChilds[CHILD_RT];
	}

	for (_uint i = 0; i < CHILD_END; ++i)
	{
		if (nullptr != m_pChilds[i])
			m_pChilds[i]->Make_Neighbors();
	}

	return S_OK;
}

void CQuadTree::Culling(CFrustum* pFrustum, const _float3* pVerticesPos, OUT _ulong* pIndices, OUT _uint* pNumIndices)
{
	if (nullptr == m_pChilds[CHILD_LT])
	{
		_uint		iIndices[4] = {
			m_iCorners[CORNER_LT],
			m_iCorners[CORNER_RT],
			m_iCorners[CORNER_RB],
			m_iCorners[CORNER_LB],
		};

		_bool		isIn[4] = {
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 0.0f),
		};

		/* 오른쪽 위 삼각형을 구성하는 정점 중, 최소 하나라도 절두체 안에 있다. */
		if (true == isIn[0] &&
			true == isIn[1] &&
			true == isIn[2])
		{
			pIndices[(*pNumIndices)++] = iIndices[0];
			pIndices[(*pNumIndices)++] = iIndices[1];
			pIndices[(*pNumIndices)++] = iIndices[2];
		}

		/* 왼쪽 아래 삼각형을 구성하는 정점 중, 최소 하나라도 절두체 안에 있다. */
		if (true == isIn[0] &&
			true == isIn[2] &&
			true == isIn[3])
		{
			pIndices[(*pNumIndices)++] = iIndices[0];
			pIndices[(*pNumIndices)++] = iIndices[2];
			pIndices[(*pNumIndices)++] = iIndices[3];
		}
		return;
	}

	_float		fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	if (true == pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		for (size_t i = 0; i < CHILD_END; i++)
		{
			if (nullptr != m_pChilds[i])
				m_pChilds[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumIndices);
		}
	}
}

void CQuadTree::Culling_LOD(CFrustum* pFrustum, const _float3* pVerticesPos, OUT _ulong* pIndices, OUT _uint* pNumIndices)
{
	if (nullptr == m_pChilds[CHILD_LT] ||
		true == isDraw(pVerticesPos))
	{
		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		for (size_t i = 0; i < NEIGHBOR_END; i++)
		{
			if (nullptr != m_pNeighbors[i])
				isDraw[i] = m_pNeighbors[i]->isDraw(pVerticesPos);
		}

		_uint		iIndices[4] = {
			m_iCorners[CORNER_LT],
			m_iCorners[CORNER_RT],
			m_iCorners[CORNER_RB],
			m_iCorners[CORNER_LB],
		};

		_bool		isIn[4] = {
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[0]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[1]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[2]]), 0.0f),
			pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[iIndices[3]]), 0.0f),
		};

		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			/* 오른쪽 위 삼각형을 구성하는 정점 중, 최소 하나라도 절두체 안에 있다. */
			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[(*pNumIndices)++] = iIndices[0];
				pIndices[(*pNumIndices)++] = iIndices[1];
				pIndices[(*pNumIndices)++] = iIndices[2];
			}

			/* 왼쪽 아래 삼각형을 구성하는 정점 중, 최소 하나라도 절두체 안에 있다. */
			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[(*pNumIndices)++] = iIndices[0];
				pIndices[(*pNumIndices)++] = iIndices[2];
				pIndices[(*pNumIndices)++] = iIndices[3];
			}
			return;
		}

		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;


		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iLC;

				pIndices[(*pNumIndices)++] = iLC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iBC;

				pIndices[(*pNumIndices)++] = iBC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}

			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
		}

		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			}

			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}

			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
		}

		return;
	}

	_float		fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	if (true == pFrustum->isIn_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenter]), fRadius))
	{
		for (size_t i = 0; i < CHILD_END; i++)
		{
			if (nullptr != m_pChilds[i])
				m_pChilds[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumIndices);
		}
	}
}

_bool CQuadTree::isDraw(const _float3* pVerticesPos)
{
	_float fWidth = 0.f, fDistance = 0.f;

	fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_RT]]) - XMLoadFloat3(&pVerticesPos[m_iCorners[CORNER_LT]])));

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&pPipeLine->Get_Camera_Position()) - XMLoadFloat3(&pVerticesPos[m_iCenter])));

	Safe_Release(pPipeLine);

	if (fDistance * 0.2f > fWidth)
		return true;

	return false;
}

CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pChild : m_pChilds)
		Safe_Release(pChild);
}
