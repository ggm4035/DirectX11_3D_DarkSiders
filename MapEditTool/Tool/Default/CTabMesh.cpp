// CTabMesh.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabMesh.h"
#include "afxdialogex.h"


// CTabMesh 대화 상자

IMPLEMENT_DYNAMIC(CTabMesh, CDialogEx)

CTabMesh::CTabMesh(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_MESH, pParent)
{

}

CTabMesh::~CTabMesh()
{
}

void CTabMesh::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabMesh, CDialogEx)
END_MESSAGE_MAP()


// CTabMesh 메시지 처리기
