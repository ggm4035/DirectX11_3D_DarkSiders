#pragma once

#include "CBase.h"

class CFileInfo
{
private:
	CFileInfo() = default;
	~CFileInfo() = default;

public:
	// 절대경로를 상대경로로 변경하는 함수
	static CString	ConvertRelatePath(CString strFullPath);
	//static void	DirInfoExtraction(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList); 지금은 2d 이미지대신 fbx쓸거 같은데 나중에 추가하는거로 
	//static _int		DirFileCnt(const std::wstring& wstrPath); 이거는 스프라이트 개수 확인 하는건데 나중에는 애니메이션 적용된 fbx쓸거 같아서 나중에 추가하는거로
};

