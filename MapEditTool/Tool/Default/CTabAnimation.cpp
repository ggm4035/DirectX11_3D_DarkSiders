// CTabAnimation.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabAnimation.h"
#include "afxdialogex.h"


// CTabAnimation 대화 상자

IMPLEMENT_DYNAMIC(CTabAnimation, CDialogEx)

CTabAnimation::CTabAnimation(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_ANIMATION, pParent)
{

}

CTabAnimation::~CTabAnimation()
{
}

void CTabAnimation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabAnimation, CDialogEx)
END_MESSAGE_MAP()


// CTabAnimation 메시지 처리기
