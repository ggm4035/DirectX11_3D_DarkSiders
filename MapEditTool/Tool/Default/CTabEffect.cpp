// CTabEffect.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabEffect.h"
#include "afxdialogex.h"


// CTabEffect 대화 상자

IMPLEMENT_DYNAMIC(CTabEffect, CDialogEx)

CTabEffect::CTabEffect(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_EFFECT, pParent)
{

}

CTabEffect::~CTabEffect()
{
}

void CTabEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabEffect, CDialogEx)
END_MESSAGE_MAP()


// CTabEffect 메시지 처리기
