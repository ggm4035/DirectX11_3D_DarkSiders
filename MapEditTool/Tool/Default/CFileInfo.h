#pragma once

#include "CBase.h"

class CFileInfo
{
private:
	CFileInfo() = default;
	~CFileInfo() = default;

public:
	// �����θ� ����η� �����ϴ� �Լ�
	static CString	ConvertRelatePath(CString strFullPath);
	//static void	DirInfoExtraction(const std::wstring& wstrPath, std::list<IMGPATH*>& rPathInfoList); ������ 2d �̹������ fbx���� ������ ���߿� �߰��ϴ°ŷ� 
	//static _int		DirFileCnt(const std::wstring& wstrPath); �̰Ŵ� ��������Ʈ ���� Ȯ�� �ϴ°ǵ� ���߿��� �ִϸ��̼� ����� fbx���� ���Ƽ� ���߿� �߰��ϴ°ŷ�
};

