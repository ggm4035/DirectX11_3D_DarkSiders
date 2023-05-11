// CTabCollider.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabCollider.h"
#include "afxdialogex.h"


// CTabCollider 대화 상자

IMPLEMENT_DYNAMIC(CTabCollider, CDialogEx)

CTabCollider::CTabCollider(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_COLLIDER, pParent)
{

}

CTabCollider::~CTabCollider()
{
}

void CTabCollider::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabCollider, CDialogEx)
END_MESSAGE_MAP()


// CTabCollider 메시지 처리기
