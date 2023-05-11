// CTabNaviMesh.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabNaviMesh.h"
#include "afxdialogex.h"


// CTabNaviMesh 대화 상자

IMPLEMENT_DYNAMIC(CTabNaviMesh, CDialogEx)

CTabNaviMesh::CTabNaviMesh(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_NAVIMESH, pParent)
{

}

CTabNaviMesh::~CTabNaviMesh()
{
}

void CTabNaviMesh::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabNaviMesh, CDialogEx)
END_MESSAGE_MAP()


// CTabNaviMesh 메시지 처리기
