#pragma once

namespace Engine
{
	template <typename T>			// 클래스 포인터들을 해제
	unsigned long Safe_AddRef(T& pointer)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pointer)
			dwRefCnt = pointer->AddRef();

		return dwRefCnt;
	}


	template <typename T>			// 클래스 포인터들을 해제
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

	template <typename T>			// 원시 자료형, 구조체 포인터 해제
	void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>			// 동적 배열을 삭제하는 용도
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
		for (_uint i = 0; i < Data.iNumAnimations; ++i)
		{
			for (_uint j = 0; j < Data.pAnimations[i].iNumChannels; ++j)
			{
				Safe_Delete_Array(Data.pAnimations[i].pChannels[j].pKeyFrames);
			}
			Safe_Delete_Array(Data.pAnimations[i].pChannels);
		}
		Safe_Delete_Array(Data.pAnimations);

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

	//////////////////////////////////////////////////////////////////
	/////////////////////////////////Functor 함수객체//////////////////////////

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