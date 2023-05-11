// CTabCamera.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "CTabCamera.h"
#include "afxdialogex.h"


// CTabCamera 대화 상자

IMPLEMENT_DYNAMIC(CTabCamera, CDialogEx)

CTabCamera::CTabCamera(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TAB_CAMERA, pParent)
{

}

CTabCamera::~CTabCamera()
{
}

void CTabCamera::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTabCamera, CDialogEx)
END_MESSAGE_MAP()


// CTabCamera 메시지 처리기
