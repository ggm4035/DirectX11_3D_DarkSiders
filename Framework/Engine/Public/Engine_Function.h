#pragma once

namespace Engine
{
	template <typename T>
	/* �ּڰ� �ִ� ������ ������ ���� */
	static void Saturate(T& _value, T _min, T _max)
	{
		_value = max(_value, _min);
		_value = min(_value, _max);
	}

	/* lowBound ���� highBound�� ��ȯ */
	static float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound)
			return lowBound;

		float f = (rand() % 10000) * 0.0001f;

		return (f * (highBound - lowBound)) + lowBound;
	}

	////////////////////////////////////////////////////////////////////////
	/////////////////////////////////Template �Լ�//////////////////////////

	template <typename T>			// Ŭ���� �����͵��� ����
	unsigned long Safe_AddRef(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
			dwRefCnt = pointer->AddRef();

		return dwRefCnt;
	}


	template <typename T>			// Ŭ���� �����͵��� ����
	unsigned long Safe_Release(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
		{
			dwRefCnt = pointer->Release();

			if (dwRefCnt == 0)
				pointer = nullptr;
		}
		return dwRefCnt;
	}

	template <typename T>			// ���� �ڷ���, ����ü ������ ����
	void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>			// ���� �迭�� �����ϴ� �뵵
	void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_BinaryData(T& Data)
	{
		Data.vecAnimations.clear();

		for (_uint i = 0; i < Data.iNumMeshes; ++i)
		{
			Safe_Delete_Array(Data.pMeshDatas[i].pBoneIndices);
			Safe_Delete_Array(Data.pMeshDatas[i].pNonAnimVertices);
			Safe_Delete_Array(Data.pMeshDatas[i].pAnimVertices);
			Safe_Delete_Array(Data.pMeshDatas[i].pIndices);
		}
		Safe_Delete_Array(Data.pMaterialPaths);
		Safe_Delete_Array(Data.pMeshDatas);
		Safe_Delete_Array(Data.pBoneDatas);
	}

	///////////////////////////////////////////////////////////////////////////
	/////////////////////////////////Functor �Լ���ü//////////////////////////

	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() {		}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const wchar_t* m_pTargetTag = nullptr;
	};

}