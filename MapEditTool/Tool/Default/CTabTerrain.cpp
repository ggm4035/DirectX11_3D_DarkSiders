// CTabTerrain.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabTerrain.h"
#include "afxdialogex.h"


// CTabTerrain 대화 상자

IMPLEMENT_DYNAMIC(CTabTerrain, CDialogEx)

CTabTerrain::CTabTerrain(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_TERRAIN, pParent)
{

}

CTabTerrain::~CTabTerrain()
{
}

void CTabTerrain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabTerrain, CDialogEx)
END_MESSAGE_MAP()


// CTabTerrain 메시지 처리기
