// Monitoring.cpp: 구현 파일
//

#include "pch.h"
#include "HseAging.h"
#include "Monitoring.h"
#include "afxdialogex.h"


// CMonitoring 대화 상자

IMPLEMENT_DYNAMIC(CMonitoring, CDialog)

CMonitoring::CMonitoring(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MONITORING, pParent)
{
	m_pDefaultFont = new CFont();
	m_pDefaultFont->CreateFont(16, 7, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
}

CMonitoring::~CMonitoring()
{
	if (m_pDefaultFont != NULL)
	{
		delete m_pDefaultFont;
	}
}

void CMonitoring::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_MT_RACK1, m_lstMtListRack1);
	DDX_Control(pDX, IDC_LST_MT_RACK2, m_lstMtListRack2);
	DDX_Control(pDX, IDC_LST_MT_RACK3, m_lstMtListRack3);
	DDX_Control(pDX, IDC_LST_MT_RACK4, m_lstMtListRack4);
	DDX_Control(pDX, IDC_LST_MT_RACK5, m_lstMtListRack5);
	DDX_Control(pDX, IDC_LST_MT_RACK6, m_lstMtListRack6);
}


BEGIN_MESSAGE_MAP(CMonitoring, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_MT_RACK1, &CMonitoring::OnNMCustomdrawLstMtRack1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_MT_RACK2, &CMonitoring::OnNMCustomdrawLstMtRack2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_MT_RACK3, &CMonitoring::OnNMCustomdrawLstMtRack3)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_MT_RACK4, &CMonitoring::OnNMCustomdrawLstMtRack4)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_MT_RACK5, &CMonitoring::OnNMCustomdrawLstMtRack5)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LST_MT_RACK6, &CMonitoring::OnNMCustomdrawLstMtRack6)
END_MESSAGE_MAP()


// CMonitoring 메시지 처리기


BOOL CMonitoring::OnInitDialog()
{
	CDialog::OnInitDialog();
	lpSystemInfo = m_pApp->GetSystemInfo();
	lpModelInfo = m_pApp->GetModelInfo();
	lpInspWorkInfo = m_pApp->GetInspWorkInfo();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pApp->Gf_writeMLog(_T("<TEST> 'Monitorinng' Dialog Open"));

	// Dialog의 기본 FONT 설정.
	SendMessageToDescendants(WM_SETFONT, (WPARAM)m_pDefaultFont->GetSafeHandle(), 1, TRUE, FALSE);

	Lf_InitLocalValue();
	Lf_InitFontset();
	Lf_InitColorBrush();
	Lf_InitDialogDesign();

	Lf_InitListControl();
	Lf_InsertItemListControl();

	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CMonitoring::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	for (int i = 0; i < COLOR_IDX_MAX; i++)
	{
		m_Brush[i].DeleteObject();
	}

	for (int i = 0; i < FONT_IDX_MAX; i++)
	{
		m_Font[i].DeleteObject();
	}
}


BOOL CMonitoring::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// 일반 Key 동작에 대한 Event
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_RETURN:
				return 1;
			case VK_SPACE:
				return 1;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH CMonitoring::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	switch (nCtlColor)
	{
		case CTLCOLOR_MSGBOX:
			break;
		case CTLCOLOR_EDIT:
			break;
		case CTLCOLOR_LISTBOX:
			break;
		case CTLCOLOR_SCROLLBAR:
			break;
		case CTLCOLOR_BTN:
			break;
		case CTLCOLOR_STATIC:		// Static, CheckBox control
			if (pWnd->GetDlgCtrlID() == IDC_STT_MT_TITLE)
			{
				pDC->SetBkColor(COLOR_DARK_NAVY);
				pDC->SetTextColor(COLOR_WHITE);
				return m_Brush[COLOR_IDX_DARK_NAVY];
			}
			if ((pWnd->GetDlgCtrlID() == IDC_STATIC) 
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MT_TITLE_RACK1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MT_TITLE_RACK2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MT_TITLE_RACK3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MT_TITLE_RACK4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MT_TITLE_RACK5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MT_TITLE_RACK6)
				)
			{
				pDC->SetBkColor(COLOR_SKYBLUE);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_SKYBLUE];
			}
			break;
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void CMonitoring::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.

	CRect rect, rectOri;
	GetClientRect(&rect);
	rectOri = rect;

	rect.bottom = 80;
	dc.FillSolidRect(rect, COLOR_DARK_NAVY);

	rect.top = rect.bottom;
	rect.bottom = rectOri.bottom;
	dc.FillSolidRect(rect, COLOR_GRAY192);
}


void CMonitoring::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
		Lf_updatePowerMeasValue();
	}

	CDialog::OnTimer(nIDEvent);
}

void CMonitoring::OnNMCustomdrawLstMtRack1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
	}

	if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {

		int i = (int)m_lstMtListRack1.GetItemData((int)pNMCD->dwItemSpec);
		// 한 줄 (row) 가 그려질 때. 여기서만 설정하면 한 줄이 모두 동일하게 설정이 된다.
		if (m_lstMtListRack1.GetItemData((int)pNMCD->dwItemSpec) == 0) {//dwItemSpec 이 현재 그려지는 row index
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			//*pResult = (LRESULT)CDRF_NEWFONT;//여기서 나가면 sub-item 이 변경되지 않는다.
			*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;//sub-item 을 변경하기 위해서. 
			return;//여기서 중단해야 *pResult 값이 유지된다.
		}
		else { // When all matrices are already made. 
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			*pResult = (LRESULT)CDRF_NEWFONT;
			return;
		}
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)) {
		// sub-item 이 그려지는 순간. 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로
		// 올 수 있었던 것이다.
		NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);

		int rack = RACK_1;
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] != LIMIT_NONE)
		{
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VCC)
			{
				if (pDraw->iSubItem == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VBL)
			{
				if (pDraw->iSubItem == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_ICC)
			{
				if (pDraw->iSubItem == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_IBL)
			{
				if (pDraw->iSubItem == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
		}
		else
		{
			if (pDraw->iSubItem == 0)
			{
				int layer;
				layer = (int)pNMCD->dwItemSpec / MAX_LAYER_CHANNEL;
				if (layer == 0)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 102, 0);
				}
				else if (layer == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 51, 102);
				}
				else if (layer == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 102);;
				}
				else if (layer == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 0);;
				}
				else if (layer == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 102, 51);;
				}
			}
			else
			{
				pDraw->clrText = COLOR_GRAY192;
				pDraw->clrTextBk = COLOR_BLACK;
			}
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
		return;
	}
	*pResult = 0;
}
void CMonitoring::OnNMCustomdrawLstMtRack2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
	}

	if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {

		int i = (int)m_lstMtListRack2.GetItemData((int)pNMCD->dwItemSpec);
		// 한 줄 (row) 가 그려질 때. 여기서만 설정하면 한 줄이 모두 동일하게 설정이 된다.
		if (m_lstMtListRack2.GetItemData((int)pNMCD->dwItemSpec) == 0) {//dwItemSpec 이 현재 그려지는 row index
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			//*pResult = (LRESULT)CDRF_NEWFONT;//여기서 나가면 sub-item 이 변경되지 않는다.
			*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;//sub-item 을 변경하기 위해서. 
			return;//여기서 중단해야 *pResult 값이 유지된다.
		}
		else { // When all matrices are already made. 
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			*pResult = (LRESULT)CDRF_NEWFONT;
			return;
		}
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)) {
		// sub-item 이 그려지는 순간. 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로
		// 올 수 있었던 것이다.
		NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);

		int rack = RACK_2;
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] != LIMIT_NONE)
		{
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VCC)
			{
				if (pDraw->iSubItem == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VBL)
			{
				if (pDraw->iSubItem == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_ICC)
			{
				if (pDraw->iSubItem == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_IBL)
			{
				if (pDraw->iSubItem == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
		}
		else
		{
			if (pDraw->iSubItem == 0)
			{
				int layer;
				layer = (int)pNMCD->dwItemSpec / MAX_LAYER_CHANNEL;
				if (layer == 0)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 102, 0);
				}
				else if (layer == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 51, 102);
				}
				else if (layer == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 102);;
				}
				else if (layer == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 0);;
				}
				else if (layer == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 102, 51);;
				}
			}
			else
			{
				pDraw->clrText = COLOR_GRAY192;
				pDraw->clrTextBk = COLOR_BLACK;
			}
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
		return;
	}
	*pResult = 0;
}
void CMonitoring::OnNMCustomdrawLstMtRack3(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
	}

	if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {

		int i = (int)m_lstMtListRack3.GetItemData((int)pNMCD->dwItemSpec);
		// 한 줄 (row) 가 그려질 때. 여기서만 설정하면 한 줄이 모두 동일하게 설정이 된다.
		if (m_lstMtListRack3.GetItemData((int)pNMCD->dwItemSpec) == 0) {//dwItemSpec 이 현재 그려지는 row index
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			//*pResult = (LRESULT)CDRF_NEWFONT;//여기서 나가면 sub-item 이 변경되지 않는다.
			*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;//sub-item 을 변경하기 위해서. 
			return;//여기서 중단해야 *pResult 값이 유지된다.
		}
		else { // When all matrices are already made. 
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			*pResult = (LRESULT)CDRF_NEWFONT;
			return;
		}
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)) {
		// sub-item 이 그려지는 순간. 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로
		// 올 수 있었던 것이다.
		NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);

		int rack = RACK_3;
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] != LIMIT_NONE)
		{
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VCC)
			{
				if (pDraw->iSubItem == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VBL)
			{
				if (pDraw->iSubItem == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_ICC)
			{
				if (pDraw->iSubItem == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_IBL)
			{
				if (pDraw->iSubItem == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
		}
		else
		{
			if (pDraw->iSubItem == 0)
			{
				int layer;
				layer = (int)pNMCD->dwItemSpec / MAX_LAYER_CHANNEL;
				if (layer == 0)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 102, 0);
				}
				else if (layer == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 51, 102);
				}
				else if (layer == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 102);;
				}
				else if (layer == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 0);;
				}
				else if (layer == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 102, 51);;
				}
			}
			else
			{
				pDraw->clrText = COLOR_GRAY192;
				pDraw->clrTextBk = COLOR_BLACK;
			}
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
		return;
	}
	*pResult = 0;
}
void CMonitoring::OnNMCustomdrawLstMtRack4(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
	}

	if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {

		int i = (int)m_lstMtListRack4.GetItemData((int)pNMCD->dwItemSpec);
		// 한 줄 (row) 가 그려질 때. 여기서만 설정하면 한 줄이 모두 동일하게 설정이 된다.
		if (m_lstMtListRack4.GetItemData((int)pNMCD->dwItemSpec) == 0) {//dwItemSpec 이 현재 그려지는 row index
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			//*pResult = (LRESULT)CDRF_NEWFONT;//여기서 나가면 sub-item 이 변경되지 않는다.
			*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;//sub-item 을 변경하기 위해서. 
			return;//여기서 중단해야 *pResult 값이 유지된다.
		}
		else { // When all matrices are already made. 
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			*pResult = (LRESULT)CDRF_NEWFONT;
			return;
		}
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)) {
		// sub-item 이 그려지는 순간. 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로
		// 올 수 있었던 것이다.
		NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);

		int rack = RACK_4;
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] != LIMIT_NONE)
		{
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VCC)
			{
				if (pDraw->iSubItem == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VBL)
			{
				if (pDraw->iSubItem == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_ICC)
			{
				if (pDraw->iSubItem == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_IBL)
			{
				if (pDraw->iSubItem == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
		}
		else
		{
			if (pDraw->iSubItem == 0)
			{
				int layer;
				layer = (int)pNMCD->dwItemSpec / MAX_LAYER_CHANNEL;
				if (layer == 0)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 102, 0);
				}
				else if (layer == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 51, 102);
				}
				else if (layer == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 102);;
				}
				else if (layer == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 0);;
				}
				else if (layer == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 102, 51);;
				}
			}
			else
			{
				pDraw->clrText = COLOR_GRAY192;
				pDraw->clrTextBk = COLOR_BLACK;
			}
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
		return;
	}
	*pResult = 0;
}

void CMonitoring::OnNMCustomdrawLstMtRack5(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
	}

	if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {

		int i = (int)m_lstMtListRack5.GetItemData((int)pNMCD->dwItemSpec);
		// 한 줄 (row) 가 그려질 때. 여기서만 설정하면 한 줄이 모두 동일하게 설정이 된다.
		if (m_lstMtListRack5.GetItemData((int)pNMCD->dwItemSpec) == 0) {//dwItemSpec 이 현재 그려지는 row index
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			//*pResult = (LRESULT)CDRF_NEWFONT;//여기서 나가면 sub-item 이 변경되지 않는다.
			*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;//sub-item 을 변경하기 위해서. 
			return;//여기서 중단해야 *pResult 값이 유지된다.
		}
		else { // When all matrices are already made. 
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			*pResult = (LRESULT)CDRF_NEWFONT;
			return;
		}
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)) {
		// sub-item 이 그려지는 순간. 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로
		// 올 수 있었던 것이다.
		NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);

		int rack = RACK_5;
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] != LIMIT_NONE)
		{
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VCC)
			{
				if (pDraw->iSubItem == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VBL)
			{
				if (pDraw->iSubItem == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_ICC)
			{
				if (pDraw->iSubItem == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_IBL)
			{
				if (pDraw->iSubItem == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
		}
		else
		{
			if (pDraw->iSubItem == 0)
			{
				int layer;
				layer = (int)pNMCD->dwItemSpec / MAX_LAYER_CHANNEL;
				if (layer == 0)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 102, 0);
				}
				else if (layer == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 51, 102);
				}
				else if (layer == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 102);;
				}
				else if (layer == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 0);;
				}
				else if (layer == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 102, 51);;
				}
			}
			else
			{
				pDraw->clrText = COLOR_GRAY192;
				pDraw->clrTextBk = COLOR_BLACK;
			}
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
		return;
	}
	*pResult = 0;
}

void CMonitoring::OnNMCustomdrawLstMtRack6(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (pNMCD->dwDrawStage == CDDS_PREPAINT) {
		*pResult = (LRESULT)CDRF_NOTIFYITEMDRAW;
		return; // 여기서 함수를 빠져 나가야 *pResult 값이 유지된다.
	}

	if (pNMCD->dwDrawStage == CDDS_ITEMPREPAINT) {

		int i = (int)m_lstMtListRack6.GetItemData((int)pNMCD->dwItemSpec);
		// 한 줄 (row) 가 그려질 때. 여기서만 설정하면 한 줄이 모두 동일하게 설정이 된다.
		if (m_lstMtListRack6.GetItemData((int)pNMCD->dwItemSpec) == 0) {//dwItemSpec 이 현재 그려지는 row index
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			//*pResult = (LRESULT)CDRF_NEWFONT;//여기서 나가면 sub-item 이 변경되지 않는다.
			*pResult = (LRESULT)CDRF_NOTIFYSUBITEMDRAW;//sub-item 을 변경하기 위해서. 
			return;//여기서 중단해야 *pResult 값이 유지된다.
		}
		else { // When all matrices are already made. 
			NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
			pDraw->clrText = COLOR_GRAY192;
			pDraw->clrTextBk = COLOR_BLACK;
			*pResult = (LRESULT)CDRF_NEWFONT;
			return;
		}
	}
	else if (pNMCD->dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT)) {
		// sub-item 이 그려지는 순간. 위에서 *pResult 에 CDRF_NOTIFYSUBITEMDRAW 를 해서 여기로
		// 올 수 있었던 것이다.
		NMLVCUSTOMDRAW* pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);

		int rack = RACK_6;
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] != LIMIT_NONE)
		{
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VCC)
			{
				if (pDraw->iSubItem == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_VBL)
			{
				if (pDraw->iSubItem == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_ICC)
			{
				if (pDraw->iSubItem == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
			if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][pNMCD->dwItemSpec / 16][pNMCD->dwItemSpec % 16] == ERR_INFO_IBL)
			{
				if (pDraw->iSubItem == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = COLOR_RED128;
				}
				else
				{
					pDraw->clrText = COLOR_GRAY192;
					pDraw->clrTextBk = COLOR_BLACK;
				}
			}
		}
		else
		{
			if (pDraw->iSubItem == 0)
			{
				int layer;
				layer = (int)pNMCD->dwItemSpec / MAX_LAYER_CHANNEL;
				if (layer == 0)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 102, 0);
				}
				else if (layer == 1)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 51, 102);
				}
				else if (layer == 2)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 102);;
				}
				else if (layer == 3)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(102, 51, 0);;
				}
				else if (layer == 4)
				{
					pDraw->clrText = COLOR_WHITE;
					pDraw->clrTextBk = RGB(51, 102, 51);;
				}
			}
			else
			{
				pDraw->clrText = COLOR_GRAY192;
				pDraw->clrTextBk = COLOR_BLACK;
			}
		}

		*pResult = (LRESULT)CDRF_NEWFONT; // 이렇게 해야 설정한 대로 그려진다.
		return;
	}
	*pResult = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMonitoring::Lf_InitLocalValue() // CListCtrl 객체를 초기화하는 역할을 하는 함수이다.
{
	pListControl[RACK_1] = (CListCtrl*)GetDlgItem(IDC_LST_MT_RACK1); // IDC_LST_MT_RACK1 ID를 가진 대화 상자 항목을 가져와서 CListCtrl 포인터로 캐스팅한 후, pListControl 배열의 RACK_1 인덱스에 저장한다. 이는 첫 번째 랙의 리스트 컨트롤을 초기화하는 것이다.
	pListControl[RACK_2] = (CListCtrl*)GetDlgItem(IDC_LST_MT_RACK2);
	pListControl[RACK_3] = (CListCtrl*)GetDlgItem(IDC_LST_MT_RACK3);
	pListControl[RACK_4] = (CListCtrl*)GetDlgItem(IDC_LST_MT_RACK4);
	pListControl[RACK_5] = (CListCtrl*)GetDlgItem(IDC_LST_MT_RACK5);
	pListControl[RACK_6] = (CListCtrl*)GetDlgItem(IDC_LST_MT_RACK6);
}

void CMonitoring::Lf_InitFontset()
{
	m_Font[0].CreateFont(150, 70, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[1].CreateFont(44, 20, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_STT_MT_TITLE)->SetFont(&m_Font[1]);

	m_Font[2].CreateFont(32, 14, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[3].CreateFont(26, 12, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_STT_MT_TITLE_RACK1)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_STT_MT_TITLE_RACK2)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_STT_MT_TITLE_RACK3)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_STT_MT_TITLE_RACK4)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_STT_MT_TITLE_RACK5)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_STT_MT_TITLE_RACK6)->SetFont(&m_Font[3]);

	m_Font[4].CreateFont(19, 8, 0, 0, FW_BOLD, TRUE, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[5].CreateFont(16, 7, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_LST_MT_RACK1)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_LST_MT_RACK2)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_LST_MT_RACK3)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_LST_MT_RACK4)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_LST_MT_RACK5)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_LST_MT_RACK6)->SetFont(&m_Font[5]);
}

void CMonitoring::Lf_InitColorBrush()
{
	// 각 Control의 COLOR 설정을 위한 Brush를 Setting 한다.
	m_Brush[COLOR_IDX_BLACK].CreateSolidBrush(COLOR_BLACK);
	m_Brush[COLOR_IDX_WHITE].CreateSolidBrush(COLOR_WHITE);
	m_Brush[COLOR_IDX_RED].CreateSolidBrush(COLOR_RED);
	m_Brush[COLOR_IDX_GREEN].CreateSolidBrush(COLOR_GREEN);
	m_Brush[COLOR_IDX_BLUE].CreateSolidBrush(COLOR_BLUE);
	m_Brush[COLOR_IDX_SKYBLUE].CreateSolidBrush(COLOR_SKYBLUE);
	m_Brush[COLOR_IDX_SEABLUE].CreateSolidBrush(COLOR_SEABLUE);
	m_Brush[COLOR_IDX_ORANGE].CreateSolidBrush(COLOR_ORANGE);
	m_Brush[COLOR_IDX_VERDANT].CreateSolidBrush(COLOR_VERDANT);
	m_Brush[COLOR_IDX_JADEGREEN].CreateSolidBrush(COLOR_JADEGREEN);
	m_Brush[COLOR_IDX_JADEBLUE].CreateSolidBrush(COLOR_JADEBLUE);
	m_Brush[COLOR_IDX_JADERED].CreateSolidBrush(COLOR_JADERED);
	m_Brush[COLOR_IDX_LIGHT_RED].CreateSolidBrush(COLOR_LIGHT_RED);
	m_Brush[COLOR_IDX_LIGHT_GREEN].CreateSolidBrush(COLOR_LIGHT_GREEN);
	m_Brush[COLOR_IDX_LIGHT_BLUE].CreateSolidBrush(COLOR_LIGHT_BLUE);
	m_Brush[COLOR_IDX_LIGHT_ORANGE].CreateSolidBrush(COLOR_LIGHT_ORANGE);
	m_Brush[COLOR_IDX_DARK_RED].CreateSolidBrush(COLOR_DARK_RED);
	m_Brush[COLOR_IDX_DARK_ORANGE].CreateSolidBrush(COLOR_DARK_ORANGE);
	m_Brush[COLOR_IDX_GRAY64].CreateSolidBrush(COLOR_GRAY64);
	m_Brush[COLOR_IDX_GRAY128].CreateSolidBrush(COLOR_GRAY128);
	m_Brush[COLOR_IDX_GRAY159].CreateSolidBrush(COLOR_GRAY159);
	m_Brush[COLOR_IDX_GRAY192].CreateSolidBrush(COLOR_GRAY192);
	m_Brush[COLOR_IDX_GRAY224].CreateSolidBrush(COLOR_GRAY224);
	m_Brush[COLOR_IDX_BLUISH].CreateSolidBrush(COLOR_BLUISH);
	m_Brush[COLOR_IDX_DARK_BLUE].CreateSolidBrush(COLOR_DARK_BLUE);
	m_Brush[COLOR_IDX_DARK_NAVY].CreateSolidBrush(COLOR_DARK_NAVY);
}

void CMonitoring::Lf_InitDialogDesign()
{
}

void CMonitoring::Lf_InitListControl() // 리스트 컨트롤을 초기화하고 설정하는 역할을 하는 함수
{
	for (int rack = 0; rack < MAX_RACK; rack++) // rack 변수를 사용하여 MAX_RACK 수만큼 반복. 각 랙에 대해 리스트 컨트롤을 초기화한다.
	{
		pListControl[rack]->InsertColumn(0, _T("CH"), LVCFMT_CENTER, -1, -1); // pListControl[rack] 리스트 컨트롤에 첫 번째 열(인덱스 0)을 삽입한다. 열 제목은 "CH"로 설정되며, 열의 정렬 방식은 중앙정렬이다. 너비는 기본값(-1)으로 설정
		pListControl[rack]->SetColumnWidth(0, LVSCW_AUTOSIZE | LVSCW_AUTOSIZE_USEHEADER); // 첫 번째 열의 너비를 자동으로 조절. LVSCW_AUTOSIZE는 열의 내용에 맞게 너비를 조정하고, LVSCW_AUTOSIZE_USEHEADER는 열의 제목 너비를 기준으로 조정한다.
		pListControl[rack]->SetColumnWidth(0, 50); // 첫 번째 열의 너비를 50픽셀로 설정. 이 설정은 자동 조정 후에 적용된다.

		pListControl[rack]->InsertColumn(1, _T("VCC"), LVCFMT_CENTER, -1, -1); // 두 번째 열(인덱스 1)을 삽입하고, 열 제목을 "VCC"로 설정한다. 열의 정렬 방식은 중앙 정렬이다.
		pListControl[rack]->SetColumnWidth(1, LVSCW_AUTOSIZE | LVSCW_AUTOSIZE_USEHEADER); // 두 번째 열의 너비를 자동으로 조정한다. 열의 내용과 제목에 맞게 너비가 조정된다.
		pListControl[rack]->SetColumnWidth(1, 55); // 두 번째 열의 너비를 55픽셀로 설정한다.

		pListControl[rack]->InsertColumn(2, _T("VBL"), LVCFMT_CENTER, -1, -1);
		pListControl[rack]->SetColumnWidth(2, LVSCW_AUTOSIZE | LVSCW_AUTOSIZE_USEHEADER);
		pListControl[rack]->SetColumnWidth(2, 55);

		pListControl[rack]->InsertColumn(3, _T("ICC"), LVCFMT_CENTER, -1, -1);
		pListControl[rack]->SetColumnWidth(3, LVSCW_AUTOSIZE | LVSCW_AUTOSIZE_USEHEADER);
		pListControl[rack]->SetColumnWidth(3, 55);

		pListControl[rack]->InsertColumn(4, _T("IBL"), LVCFMT_CENTER, -1, -1);
		pListControl[rack]->SetColumnWidth(4, LVSCW_AUTOSIZE | LVSCW_AUTOSIZE_USEHEADER);
		pListControl[rack]->SetColumnWidth(4, 55);


		DWORD dwStype = pListControl[rack]->GetExtendedStyle(); // CListCtrl:: (현재 리스트 컨트롤의 확장 스타일을 가져와 dwStype 변수에 저장한다.)
		dwStype |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER; // dwStype에 여러 확장 스타일을 추가한다. LVS_EX_FULLROWSELECT는 전체 행 선택을 활성화하고, LVS_EX_GRIDLINES는 그리드 라인을 표시하며, LVS_EX_LABELTIP은 레이블 툴팁을 활성화하고, LVS_EX_DOUBLEBUFFER는 더블 버퍼링을 활성화한다.
		pListControl[rack]->SetExtendedStyle(dwStype); // CListCtrl:: (수정된 확장 스타일을 리스트 컨트롤에 설정하여, 사용자 인터페이스의 동작과 외관을 개선한다.)
	}
}

void CMonitoring::Lf_InsertItemListControl() // 리스트 컨트롤을 항목에 삽입하고 초기화하는 역할을 하는 함수
{
	CString sdata; // 각 항목의 텍스트를 저장할 변수 선언
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 3중 루프 - 랙, 레이어, 채널을 반복하여 처리
			{
				int i = (layer * MAX_LAYER_CHANNEL) + ch; // i 변수를 계산하여 현재 레이어와 채널에 대한 인덱스 생성
				sdata.Format(_T("%d-%02d"), layer + 1, ch + 1); // sdata에 레이어와 채널 번호를 포맷하여 "레이어-채널" 형식의 문자열을 생성. 레이어는 1부터 시작하고 채널은 1부터 시작
				pListControl[rack]->InsertItem(i, sdata); // 리스트 컨트롤에 항목 삽입 - pListControl[rack]의 InsertItem메서드를 호출하여 인덱스 i 위치에 sdata를 텍스트로 하는 항목을 삽입
				pListControl[rack]->SetItemText(i, 1, _T("")); // 첫 번째 열 초기화 - 삽입된 항목의 첫 번째 열(인덱스 1)의 텍스트를 빈 문자열로 설정
				pListControl[rack]->SetItemText(i, 2, _T("")); // 두 번째 열 초기화 - 삽입된 항목의 두 번째 열(인덱스 2)의 텍스트를 빈 문자열로 설정
				pListControl[rack]->SetItemText(i, 3, _T("")); // 세 번째 열 초기화 - 삽입된 항목의 세 번째 열(인덱스 3)의 텍스트를 빈 문자열로 설정
				pListControl[rack]->SetItemText(i, 4, _T("")); // 네 번째 열 초기화 - 삽입된 항목의 네 번째 열(인덱스 4)의 텍스트를 빈 문자열로 설정
			}
		}
	}
}

void CMonitoring::Lf_updatePowerMeasValue() // 전력 측정 값을 업데이트하여 사용자 인터페이스의 리스트 컨트롤에 표시하는 역할을 한다
{
	CString sdata; // sdata라는 문자열 변수를 선언하여 측정 값을 문자열 형식으로 저장

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 3중 루프 - rack, layer, ch를 반복하여 각 랙, 레이어, 채널에 대한 전력 측정 값을 처리
			{
				int i = (layer * MAX_LAYER_CHANNEL) + ch; 

				if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] == FALSE) // 현재 랙과 레이어의 Ethernet 연결 상태를 확인
				{
					// Ethernet 연결이 끊어지면 0으로 표시한다.
					pListControl[rack]->SetItemText(i, 1, _T("0.00"));
					pListControl[rack]->SetItemText(i, 2, _T("0.00"));
					pListControl[rack]->SetItemText(i, 3, _T("0.00"));
					pListControl[rack]->SetItemText(i, 4, _T("0.00"));
				}
				else // 연결이 정상인 경우, 각 전력 측정 값(VCC, VBL, ICC, IBL)을 100으로 나누어 소수점 두 자리 형식으로 변환한다.
				{
					sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_nMeasVCC[rack][layer][ch] / 100.f));
					pListControl[rack]->SetItemText(i, 1, sdata);
					sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_nMeasVBL[rack][layer][ch] / 100.f));
					pListControl[rack]->SetItemText(i, 2, sdata);
					sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_nMeasICC[rack][layer][ch] / 100.f));
					pListControl[rack]->SetItemText(i, 3, sdata);
					sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_nMeasIBL[rack][layer][ch] / 100.f));
					pListControl[rack]->SetItemText(i, 4, sdata);

					// Limit NG 발생한 위치에는 Limit NG 당시의 Value 값을 표시한다.
					if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_NONE) // Limit NG가 발생한 경우, 해당 채널의 에러 타입에 따라 에러 값을 리스트 컨트롤에 표시한다.
					{
						if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_VCC) // 에러 타입이 VCC인 경우, Limit NG 당시의 VCC 값을 리스트 컨트롤에 설정
						{
							sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
							pListControl[rack]->SetItemText(i, 1, sdata);
						}
						if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_VBL)
						{
							sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
							pListControl[rack]->SetItemText(i, 2, sdata);
						}
						if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_ICC)
						{
							sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
							pListControl[rack]->SetItemText(i, 3, sdata);
						}
						if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_IBL)
						{
							sdata.Format(_T("%.2f"), (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
							pListControl[rack]->SetItemText(i, 4, sdata);
						}
					}
				}
			}
		}
	}
	// 위 함수는 전력 측정 값을 업데이트하고, Ethernet 연결 상태에 따라 적절한 값을 리스트 컨트롤에 표시하여 사용자에게 현재 상태를 보여주는 역할을 한다.
}
