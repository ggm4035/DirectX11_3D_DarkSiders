// CTabUI.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabUI.h"
#include "afxdialogex.h"


// CTabUI 대화 상자

IMPLEMENT_DYNAMIC(CTabUI, CDialogEx)

CTabUI::CTabUI(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_UI, pParent)
{

}

CTabUI::~CTabUI()
{
}

void CTabUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabUI, CDialogEx)
END_MESSAGE_MAP()


// CTabUI 메시지 처리기
