// PidInput.cpp: 구현 파일
//

#include "pch.h"
#include "HseAging.h"
#include "PidInput.h"
#include "afxdialogex.h"
#include "MessageError.h"
#include "MessageQuestion.h"


// CPidInput 대화 상자

IMPLEMENT_DYNAMIC(CPidInput, CDialog)

CPidInput::CPidInput(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PID_INPUT, pParent)
{
	m_pDefaultFont = new CFont();
	m_pDefaultFont->CreateFont(15, 6, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_nTargetRack = 0;
}

CPidInput::~CPidInput()
{
	if (m_pDefaultFont != NULL)
	{
		delete m_pDefaultFont;
	}
}

void CPidInput::DoDataExchange(CDataExchange* pDX) // 데이터 교환 및 초기화를 처리하는 함수
{
	CDialog::DoDataExchange(pDX); // 기본 클래스인 CDialog의 DoDataExchange 함수를 호출하여, 기본적인 데이터 교환 작업을 수행한다. 이는 MFC의 데이터 바인딩 메커니즘을 초기화하는 데 필요하다.
	DDX_Control(pDX, IDC_STT_PI_RACK_ID, m_sttPiRackID); // DDX_Control 매크로를 사용하여 IDC_STT_PI_RACK_ID라는 ID를 가진 컨트롤을 m_sttPiRackID멤버 변수와 연결한다. 이로 인해 해당 컨트롤의 상태를 m_sttPiRackID를 통해 접근할 수 있게 된다.
	DDX_Control(pDX, IDC_STT_PI_LAYER1, m_sttPiLayer1); // IDC_STT_PI_LAYER1 ID를 가진 컨트롤을 m_stPiLayer1 멤버 변수와 연결
	DDX_Control(pDX, IDC_STT_PI_LAYER2, m_sttPiLayer2); // IDC_STT_PI_LAYER2 ID를 가진 컨트롤을 m_sttPiLayer2 멤버 변수와 연결
	DDX_Control(pDX, IDC_STT_PI_LAYER3, m_sttPiLayer3);
	DDX_Control(pDX, IDC_STT_PI_LAYER4, m_sttPiLayer4);
	DDX_Control(pDX, IDC_STT_PI_LAYER5, m_sttPiLayer5);
	DDX_Control(pDX, IDC_LST_PI_MES_MESSAGE, m_lstPiMesMessage);
	DDX_Control(pDX, IDC_BTN_PI_SAVE_EXIT, m_btnPiSaveExit);
	DDX_Control(pDX, IDC_BTN_PI_CANCEL, m_btnPiCancel);
}


BEGIN_MESSAGE_MAP(CPidInput, CDialog) // 메시지 맵을 정의하는 부분. 특정 이벤트(메시지)에 대한 응답으로 호출될 함수를 연결하는 역할을 한다.
	ON_WM_DESTROY() // 대화 상자가 파괴될 때 발생하는 WM_DESTROY 메시지를 처리하는 핸들러를 연결
	ON_WM_CTLCOLOR() // 컨트롤의 색상을 설정할 때 발생하는 WM_CTLCOLOR 메시지를 처리하는 핸들러를 연결
	ON_WM_PAINT() // 대화 상자가 그려질 때 발생하는 WM_PAINT 메시지를 처리하는 핸들러를 연결
	ON_WM_TIMER() // 타이머 이벤트가 발생할 때 호출되는 WM_TIMER 메시지를 처리하는 핸들러를 연결
	ON_BN_CLICKED(IDC_MBC_PI_RACK1, &CPidInput::OnBnClickedMbcPiRack1) // IDC_MBC_PI_RACK1 ID를 가진 버튼이 클릭될 때 OnBnClickedMbcPiRack1 함수를 호출하도록 연결한다.
	ON_BN_CLICKED(IDC_MBC_PI_RACK2, &CPidInput::OnBnClickedMbcPiRack2)
	ON_BN_CLICKED(IDC_MBC_PI_RACK3, &CPidInput::OnBnClickedMbcPiRack3)
	ON_BN_CLICKED(IDC_MBC_PI_RACK4, &CPidInput::OnBnClickedMbcPiRack4)
	ON_BN_CLICKED(IDC_MBC_PI_RACK5, &CPidInput::OnBnClickedMbcPiRack5)
	ON_BN_CLICKED(IDC_MBC_PI_RACK6, &CPidInput::OnBnClickedMbcPiRack6)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH1, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch1) // IDC_EDT_PI_PID_LAYER1_CH1 ID를 가진 편집 상자가 포커스를 받을 때 OnEnSetfocusEdtPiPidLayer1Ch1 함수를 호출하도록 연결한다.
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH2, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch2) 
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH3, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch3)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH4, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch4)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH5, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch5)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH6, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch6)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH7, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch7)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH8, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch8)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH9, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch9)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH10, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch10)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH11, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch11)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH12, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch12)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH13, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch13)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH14, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch14)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH15, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch15)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER1_CH16, &CPidInput::OnEnSetfocusEdtPiPidLayer1Ch16)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH1, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch1)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH2, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch2)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH3, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch3)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH4, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch4)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH5, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch5)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH6, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch6)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH7, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch7)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH8, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch8)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH9, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch9)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH10, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch10)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH11, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch11)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH12, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch12)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH13, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch13)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH14, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch14)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH15, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch15)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER2_CH16, &CPidInput::OnEnSetfocusEdtPiPidLayer2Ch16)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH1, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch1)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH2, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch2)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH3, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch3)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH4, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch4)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH5, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch5)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH6, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch6)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH7, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch7)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH8, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch8)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH9, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch9)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH10, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch10)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH11, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch11)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH12, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch12)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH13, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch13)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH14, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch14)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH15, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch15)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER3_CH16, &CPidInput::OnEnSetfocusEdtPiPidLayer3Ch16)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH1, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch1)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH2, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch2)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH3, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch3)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH4, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch4)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH5, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch5)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH6, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch6)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH7, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch7)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH8, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch8)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH9, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch9)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH10, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch10)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH11, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch11)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH12, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch12)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH13, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch13)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH14, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch14)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH15, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch15)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER4_CH16, &CPidInput::OnEnSetfocusEdtPiPidLayer4Ch16)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH1, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch1)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH2, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch2)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH3, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch3)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH4, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch4)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH5, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch5)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH6, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch6)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH7, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch7)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH8, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch8)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH9, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch9)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH10, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch10)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH11, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch11)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH12, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch12)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH13, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch13)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH14, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch14)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH15, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch15)
	ON_EN_SETFOCUS(IDC_EDT_PI_PID_LAYER5_CH16, &CPidInput::OnEnSetfocusEdtPiPidLayer5Ch16)
	ON_BN_CLICKED(IDC_MBC_PI_PID_CLEAR, &CPidInput::OnBnClickedMbcPiPidClear)
	ON_BN_CLICKED(IDC_BTN_PI_MES_DSPM, &CPidInput::OnBnClickedBtnPiMesDspm)
	ON_BN_CLICKED(IDC_BTN_PI_MES_DMIN, &CPidInput::OnBnClickedBtnPiMesDmin)
	ON_BN_CLICKED(IDC_BTN_PI_MES_DMOU, &CPidInput::OnBnClickedBtnPiMesDmou)
	ON_BN_CLICKED(IDC_BTN_PI_SAVE_EXIT, &CPidInput::OnBnClickedBtnPiSaveExit)
	ON_BN_CLICKED(IDC_BTN_PI_CANCEL, &CPidInput::OnBnClickedBtnPiCancel)
	ON_BN_CLICKED(IDC_MBC_PI_CH_ALL_SELECT, &CPidInput::OnBnClickedMbcPiChAllSelect)
	ON_BN_CLICKED(IDC_MBC_PI_CH_ALL_CLEAR, &CPidInput::OnBnClickedMbcPiChAllClear)
END_MESSAGE_MAP()


// CPidInput 메시지 처리기


BOOL CPidInput::OnInitDialog()
{
	CDialog::OnInitDialog();
	lpSystemInfo = m_pApp->GetSystemInfo(); // m_pApp 객체를 통해 시스템 정보를 가져와 IpSystemInfo 포인터에 저장
	lpModelInfo = m_pApp->GetModelInfo(); // m_pApp 객체를 통해 모델 정보를 가져와 IpModelInfo 포인터에 저장
	lpInspWorkInfo = m_pApp->GetInspWorkInfo(); // m_pApp 객체를 통해 검사 작업 정보를 가져와 IpInspWorkInfo 포인터에 저장

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pApp->Gf_writeMLog(_T("<TEST> 'Panel ID Input' Dialog Open")); // 로그 파일에 대화 상자가 열렸다는 메시지를 기록한다. _T 매크로는 문자열을 유니코드 또는 멀티바이트 문자열로 변환한다.

	// Dialog의 기본 FONT 설정.
	SendMessageToDescendants(WM_SETFONT, (WPARAM)m_pDefaultFont->GetSafeHandle(), 1, TRUE, FALSE); // 대화 상자와 그 자식 컨트롤에 기본 폰트를 설정합니다. WM_SETFONT 메시지를 사용하여 폰트를 적용한다.

	Lf_InitLocalValue(); // Lf_InitLocalValue 함수를 호출하여 지역 변수를 초기화합니다. 이 함수의 구체적인 내용은 코드에서 정의된 부분에 따라 다르다.
	Lf_InitFontset(); // Lf_InitFontset 함수를 호출하여 폰트 세트를 초기화
	Lf_InitColorBrush(); // Lf_InitColorBrush 함수를 호출하여 색상 브러시를 초기화
	Lf_InitDialogDesign(); // Lf_InitDialogDesign 함수를 호출하여 대화 상자의 디자인 초기화

	Lf_ChangeColorRackButton(m_nTargetRack); // Lf_ChangeColorRackButton 함수를 호출하여 특정 랙 버튼의 색상을 변경한다. m_nTragetRack은 변경할 랙의 ID를 나타낸다.
	Lf_updatePanelID(); // Lf_updatePanelID 함수를 호출하여 패널 ID를 업데이트 한다.

	if (m_nMesAutoDMOU == MES_DMOU_MODE_AUTO) // m_nMesAutoDMOU가 자동 모드인지 확인하는 조건문이다.
		SetTimer(99, 1000, NULL); // 타이머를 설정한다. ID가 99이고, 1000 밀리초(1초) 간격으로 타이머 이벤트가 발생하도록 설정한다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPidInput::OnDestroy()
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


BOOL CPidInput::PreTranslateMessage(MSG* pMsg) // 
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4) // 시스템 키가 눌렸고, 그 키가 F4인지 확인하는 조건문
	{
		if (::GetKeyState(VK_MENU) < 0)	return TRUE; // Alt 키가 눌린 상태에서 F4 키가 눌렸다면 TRUE를 반환하여 메시지를 처리했음을 나타낸다. 이는 대화상자가 닫히지 않도록 한다
	}

	// 일반 Key 동작에 대한 Event
	if (pMsg->message == WM_KEYDOWN) // 키가 눌렸을 때 발생하는 메시지인 WM_KEYDOWN인지 확인하는 조건문이다.
	{
		switch (pMsg->wParam) // 눌린 키에 따라 다른 동작을 수행하기 위해 wParam 값을 스위치문으로 분기한다.
		{
		case VK_ESCAPE: // ESC 키가 눌렸을 때의 경우
			return 1; // ESC 키가 눌리면 1을 반환하여 메시지를 처리했음을 나타낸다
		case VK_RETURN: // Enter 키가 눌렸을 때의 경우이다.
			if (GetDlgItem(IDC_EDT_PI_RACK_ID) == GetFocus()) // 현재 포커스가 IDC_EDT_PI_RACK_ID라는 컨트롤에 있는지 확인한다.
			{
				m_pedtPannelID[LAYER_1][CH_1]->SetFocus(); // 포커스가 IDC_EDT_PI_RACK_ID에 있을 경우, m_pedtPannelID[LAYER_1][CH_1]컨트롤로 포커스를 이동한다.
			}
			else
			{
				Lf_setFocus(); // 포커스가 IDC_EDT_PI_RACK_ID에 없을 경우, Lf_setFocus 함수를 호출하여 다른 컨트롤로 포커스를 설정한다.
			}
			return 1;
		case VK_SPACE: // Sapce 키가 눌렸을 때의 경우
			return 1;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


HBRUSH CPidInput::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	switch (nCtlColor)
	{
		case CTLCOLOR_MSGBOX:
			break;
		case CTLCOLOR_EDIT:
			if ((pWnd->GetDlgCtrlID() == IDC_EDT_PI_RACK_ID)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_EDT_PI_PID_LAYER5_CH16)
				)
			{
				if (Lf_checkPIDMesInfo(pWnd->GetDlgCtrlID()) != 0)
				{
					// MES 완료 색상
					pDC->SetBkColor(COLOR_SKYBLUE);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_SKYBLUE];
				}
				if (Lf_checkPIDFocusInfo(pWnd->GetDlgCtrlID()) != 0)
				{
					// Focus 위치 색상
					pDC->SetBkColor(COLOR_JADEBLUE);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_JADEBLUE];
				}
				if (Lf_checkPIDErrorInfo(pWnd->GetDlgCtrlID()) != 0)
				{
					// PID ERROR 색상
					pDC->SetBkColor(COLOR_RED);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED];
				}
				if (Lf_checkPIDValidInfo(pWnd->GetDlgCtrlID()) != 0)
				{
					// PID 입력 완료 색상
					pDC->SetBkColor(COLOR_LIGHT_GREEN);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_LIGHT_GREEN];
				}

				// PID 입력 대기 색상
				pDC->SetBkColor(COLOR_WHITE);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_WHITE];
			}

			break;
		case CTLCOLOR_LISTBOX:
			if (pWnd->GetDlgCtrlID() == IDC_LST_PI_MES_MESSAGE)
			{
				pDC->SetBkColor(COLOR_WHITE);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_WHITE];
			}
			break;
		case CTLCOLOR_SCROLLBAR:
			break;
		case CTLCOLOR_BTN:
			break;
		case CTLCOLOR_STATIC:		// Static, CheckBox control
			if ((pWnd->GetDlgCtrlID() == IDC_STATIC)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_RIGHT_SIDE)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LEFT_SIDE)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LAYER5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LEFT_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LEFT_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LEFT_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LEFT_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_LEFT_LAYER5)
				)
			{
				pDC->SetBkColor(COLOR_SKYBLUE);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_SKYBLUE];
			}
			if (pWnd->GetDlgCtrlID() == IDC_STT_PI_TITLE)
			{
				pDC->SetBkColor(COLOR_DARK_NAVY);
				pDC->SetTextColor(COLOR_WHITE);
				return m_Brush[COLOR_IDX_DARK_NAVY];
			}
			if ((pWnd->GetDlgCtrlID() == IDC_GRP_PI_PID_OPERATION)
				|| (pWnd->GetDlgCtrlID() == IDC_GRP_PI_MES_OPERATION)
				)
			{
				pDC->SetBkColor(COLOR_GRAY192);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_GRAY192];
			}
			if ((pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH1) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH3) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH5) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH7) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH9) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH11) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH13) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH15) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH1) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH3) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH5) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH7) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH9) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH11) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH13) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH15) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH1) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH3) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH5) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH7) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH9) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH11) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH13) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH15) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH1) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH3) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH5) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH7) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH9) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH11) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH13) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH15) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH1) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH3) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH5) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH7) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH9) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH11) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH13) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH15) || (pWnd->GetDlgCtrlID() == IDC_CHK_PI_LAYER5_CH16)
				)
			{
				pDC->SetBkColor(COLOR_GRAY192);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_GRAY192];
			}
			if ((pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH7)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH9)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH11)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH13)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH15)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_PI_MSG_LAYER5_CH16)
				)
			{
				pDC->SetBkColor(COLOR_GRAY159);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_GRAY159];

			}
			break;
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void CPidInput::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
	CRect rect, rectOri;
	GetClientRect(&rect);
	rectOri = rect;

	rect.bottom = 70;
	dc.FillSolidRect(rect, COLOR_DARK_NAVY);

	rect.top = rect.bottom;
	rect.bottom = rectOri.bottom;
	dc.FillSolidRect(rect, COLOR_GRAY192);
}


void CPidInput::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 99)
	{
		KillTimer(99);

		if (Lf_setExecuteMesDMOU() == TRUE)
		{
			CDialog::OnOK();
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CPidInput::OnBnClickedMbcPiRack1() // RACK_1 버튼이 클릭되었을 때 호출된다
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_ChangeColorRackButton(RACK_1); // Lf_ChangeColorRackButton()함수를 호출하여 해당 랙의 색상을 변경한다.
	Lf_updatePanelID(); // Lf_updatePanelID() 함수를 호출하여 패널 ID를 업데이트 한다.
}


void CPidInput::OnBnClickedMbcPiRack2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_ChangeColorRackButton(RACK_2);
	Lf_updatePanelID();
}


void CPidInput::OnBnClickedMbcPiRack3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_ChangeColorRackButton(RACK_3);
	Lf_updatePanelID();
}


void CPidInput::OnBnClickedMbcPiRack4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_ChangeColorRackButton(RACK_4);
	Lf_updatePanelID();
}


void CPidInput::OnBnClickedMbcPiRack5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_ChangeColorRackButton(RACK_5);
	Lf_updatePanelID();
}


void CPidInput::OnBnClickedMbcPiRack6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_ChangeColorRackButton(RACK_6);
	Lf_updatePanelID();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch1() // 특정 입력 필드에 포커스가 설정될 때 호출된다.
{   // LAYER_1과 CH_1을 선택하고, 윈도우를 무효화한다.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1; 
	m_nSelChannel = CH_1; // m_nSelLayer와 m_SelChannel 변수를 설정하여 현재 선택된 레이어와 채널을 지정한다.
	Lf_InvalidateWindow(); // Lf_InvalidateWindow 함수를 호출하여 윈도우를 무효화하고, 화면을 다시 그리도록 요청한다.
	// 포커스 이벤트 핸들러는 입력 필드에 포커스가 설정될 때 선택된 레이어와 채널을 업데이트하고 화면을 다시 그리도록 요청한다.
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_2;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_3;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_4;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_5;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_6;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_7;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_8;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_9;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_10;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_11;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_12;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_13;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_14;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_15;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer1Ch16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_1;
	m_nSelChannel = CH_16;
	Lf_InvalidateWindow();
}

void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_1;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_2;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_3;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_4;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_5;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_6;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_7;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_8;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_9;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_10;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_11;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_12;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_13;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_14;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_15;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer2Ch16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_2;
	m_nSelChannel = CH_16;
	Lf_InvalidateWindow();
}

void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_1;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_2;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_3;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_4;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_5;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_6;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_7;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_8;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_9;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_10;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_11;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_12;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_13;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_14;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_15;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer3Ch16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_3;
	m_nSelChannel = CH_16;
	Lf_InvalidateWindow();
}

void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_1;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_2;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_3;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_4;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_5;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_6;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_7;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_8;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_9;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_10;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_11;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_12;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_13;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_14;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_15;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer4Ch16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_4;
	m_nSelChannel = CH_16;
	Lf_InvalidateWindow();
}

void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_1;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_2;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_3;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_4;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_5;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_6;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_7;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_8;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_9;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch10()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_10;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch11()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_11;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch12()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_12;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch13()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_13;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch14()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_14;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch15()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_15;
	Lf_InvalidateWindow();
}


void CPidInput::OnEnSetfocusEdtPiPidLayer5Ch16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSelLayer = LAYER_5;
	m_nSelChannel = CH_16;
	Lf_InvalidateWindow();
}


void CPidInput::OnBnClickedBtnPiMesDspm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setExecuteMesDSPM();
}


void CPidInput::OnBnClickedBtnPiMesDmin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setExecuteMesDMIN();
}


void CPidInput::OnBnClickedBtnPiMesDmou()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setExecuteMesDMOU();
}

void CPidInput::OnBnClickedMbcPiPidClear() // 클리어 버튼 클릭 시 호출되는 함수
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sLog, sdata = _T("");

	m_pApp->Gf_writeMLog(_T("<PID> 'PID ALL Clear' Button Click.")); // 로그파일에 메시지 기록

	CMessageQuestion msg_dlg; // 사용자에게 확인 메시지를 표시하기 위한 CMessageQuestion 객체를 생성
	msg_dlg.m_strQMessage = _T("Do you want clear all ID ?");
	msg_dlg.m_strLButton = _T("YES");
	msg_dlg.m_strRButton = _T("NO");
	// 확인 대화상자의 메시지와 버튼 텍스트를 설정한다. "Do you want clear all ID?"라는 질문과 "YES", "NO" 버튼을 설정한다.

	if (msg_dlg.DoModal() == IDOK) // 대화상자를 모달로 표시하고, 사용자가 "YES" 버튼을 클릭하면 IDOK가 반환된다. 이 조건이 참일 경우 아래 코드를 실행한다.
	{
		CString sSection, sKey, sValue; // sSection, sKey, sValue라는 문자열 변수를 선언한다. 이 변수들은 나중에 PID 정보를 저장하는 데 사용된다.
		sSection.Format(_T("MES_PID_RACK%d"), m_nSelRack + 1); // sSection 변수를 설정하여 현재 선택된 랙의 PID 정보를 저장할 섹션 이름을 형식화한다. m_nSelRack + 1을 사용하여 1부터 시작하는 인덱스를 만든다.

		for(int layer=0; layer<MAX_LAYER; layer++) // layer 변수를 사용하여 최대 레이어 수(MAX_LAYER)만큼 반복하는 루프를 시작한다.
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // ch 변수를 사용하여 각 레이어의 최대 채널 수(MAX_LAYER_CHANNEL)만큼 반복하는 내부 루프를 시작한다.
			{
				lpInspWorkInfo->m_sMesPanelID[m_nSelRack][layer][ch].Empty(); // 현재 선택된 랙의 특정 레이어와 채널에 해당하는 PID 정보를 비운다. Empty() 메서드는 해당 문자열을 빈 문자열로 설정한다.
				m_pedtPannelID[layer][ch]->SetWindowText(_T("")); // GUI의 해당 레이어와 채널에 대한 패널 ID 텍스트 박스를 빈 문자열로 설정하여 화면에서도 PID 정보를 지운다.

				// PID Save
				sKey.Format(_T("RACK%d_LAYER%d_CH%d"), m_nSelRack + 1, layer + 1, ch + 1); // sKey 변수를 설정하여 현재 선택된 랙, 레이어, 채널에 대한 키를 형식화 한다. 이 키는 PID 정보를 저장하는 데 사용된다.
				Write_MesPIDInfo(sSection, sKey, _T("")); // Write_MesPIDInfo 함수를 호출하여 sSection과 sKey에 해당하는 PID 정보를 빈 문자열로 저장한다. 즉, 해당 PID 정보를 지우는 작업을 수행한다.
			}
		}
		m_pedtPannelID[0][0]->SetFocus(); // PID 정보를 지운 후, 첫 번째 패널 ID 텍스트 박스에 포커스를 설정하여 사용자가 바로 입력할 수 있도록 한다.
	}
}

void CPidInput::OnBnClickedMbcPiChAllSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_AllChannelSelect(TRUE);

	// 2025-01-17 PDH. 채널 전체 선택 시 PID 입력 포커스 CH1로 이동
	m_pedtPannelID[LAYER_1][CH_1]->SetFocus();
}


void CPidInput::OnBnClickedMbcPiChAllClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_AllChannelSelect(FALSE);
}

void CPidInput::OnBnClickedBtnPiSaveExit() // Save And Exit 버튼 클릭 시 동작하는 함수.
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMessageQuestion msg_dlg;
	msg_dlg.m_strQMessage = _T("Do you want save ?");
	msg_dlg.m_strLButton = _T("YES");
	msg_dlg.m_strRButton = _T("NO"); // CMessageQuestion 객체를 생성하여 사용자에게 "저장하시겠습니까?"라는 질문을 표시한다. "YES", "NO" 버튼을 설정한다.

	if (msg_dlg.DoModal() == IDOK) 
	{
		CString sSection, sKey, sValue; // PID 정보를 저장하는데 사용하는 변수값 

		sSection.Format(_T("MES_PID_RACK%d"), m_nSelRack + 1);

		// RACK ID 저장
		GetDlgItem(IDC_EDT_PI_RACK_ID)->GetWindowText(sValue);

		lpInspWorkInfo->m_sRackID[m_nSelRack] = sValue;
		sKey.Format(_T("RACK%d_BCR_ID"), m_nSelRack + 1);
		Write_SysIniFile(_T("SYSTEM"), sKey, sValue);

		// CHANNEL PID 저장
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				// Channel Use Save
				sKey.Format(_T("RACK%d_LAYER%d_CH%d_USE"), m_nSelRack + 1, layer + 1, ch + 1);
				lpInspWorkInfo->m_bMesChannelUse[m_nSelRack][layer][ch] = m_pChkChannelUse[layer][ch]->GetCheck();
				sValue.Format(_T("%d"), m_pChkChannelUse[layer][ch]->GetCheck());
				Write_MesPIDInfo(sSection, sKey, sValue);

				// Channel PID Save
				sKey.Format(_T("RACK%d_LAYER%d_CH%d"), m_nSelRack + 1, layer + 1, ch + 1);
				m_pedtPannelID[layer][ch]->GetWindowText(sValue);
				lpInspWorkInfo->m_sMesPanelID[m_nSelRack][layer][ch] = sValue;
				Write_MesPIDInfo(sSection, sKey, sValue);
			}
		}

		CDialog::OnOK();
	}
}


void CPidInput::OnBnClickedBtnPiCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPidInput::Lf_InitLocalValue() // PID 제어 시스템의 UI 요소를 초기화하는 역할을 함.
{
	memset(m_bPIDRuleError, FALSE, sizeof(m_bPIDRuleError)); // m_bPIDRuleError 배열을 모두 FALSE로 초기화. 이 배열은 PID 규칙 오류 상태를 저장하는 데 사용된다.
	m_nSelRack = RACK_1; // 선택된 랙의 초기값을 RACK_1으로 설정한다. 이는 PID 제어 시스템에서 사용할 랙을 지정한다.

	////////////////////////////////////////////////////////////////////////////////////////////
	int layer;
	int i = 0; // layer와 i라는 두 개의 정수 변수를 선언. layer는 레이어를 나타내고, i는 인덱스 카운터로 사용된다.
	m_pBtnPidRack[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_PI_RACK1); // IDC_MBC_PI_RACK1 ID를 가진 버튼을 찾아 m_pBtnPidRack 배열의 첫 번째 요소에 저장한다. i는 ㅣ후에 증가한다.
	m_pBtnPidRack[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_PI_RACK2);
	m_pBtnPidRack[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_PI_RACK3);
	m_pBtnPidRack[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_PI_RACK4);
	m_pBtnPidRack[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_PI_RACK5);
	m_pBtnPidRack[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_PI_RACK6); // m_pBtnPidRack 배열에 나머지 랙 버튼들을 추가한다. 각 버튼들은 해당 ID를 가진 UI 요소를 가져와서 배열에 저장한다.

	layer = LAYER_1;
	i = 0; // layer 변수를 LAYER_1로 설정하고, i를 0으로 초기화한다. 이는 첫 번째 레이어의 UI 요소를 초기화하기 위한 준비이다.
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH1); // LAYER_1의 첫 번째 채널 사용 체크박스를 찾아 m_pChkChannelUse 배열에 저장한다. 이후 i를 증가시킨다.
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH2);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH3);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH4);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH5);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH6);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH7);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH8);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH9);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH10);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH11);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH12);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH13);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH14);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH15);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER1_CH16); // LAYER_1의 나머지 채널 사용 체크박스들을 배열에 추가한다.
	i = 0;
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH1); // i를 0으로 초기화하고, LAYER_1의 상태 메시지를 표시할 정적 텍스트 UI 요소를 배열에 저장한다.
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH2);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH3);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH4);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH5);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH6);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH7);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH8);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH9);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH10);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH11);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH12);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH13);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH14);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH15);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER1_CH16); // LAYER_1의 나머지 상태 메시지 정적 텍스트 UI 요소들을 배열에 추가한다.
	i = 0;
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH1); // i를 0으로 초기화하고, LAYER_1의 PID 패널 ID를 입력할 편집 상자를 배열에 저장한다.
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH2);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH3);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH4);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH5);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH6);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH7);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH8);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH9);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH10);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH11);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH12);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH13);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH14);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH15);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER1_CH16); // LAYER_1의 나머지 PID 패널 ID 편집 상자들을 배열에 추가한다.

	i = 0;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH1; // i를 0으로 초기화하고, LAYER_1의 PID 패널 ID에 해당하는 컨트롤 ID를 배열에 저장한다.
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH2;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH3;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH4;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH5;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH6;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH7;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH8;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH9;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH10;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH11;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH12;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH13;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH14;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH15;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER1_CH16; // LAYER_1의 나머지 PID 패널 ID 컨트롤 ID들을 배열에 추가한다.


	layer = LAYER_2; 
	i = 0; // layer 변수를 LAYER_2로 설정하고, i를 0으로 초기화한다. 이는 두 번째 레이어의 UI 요소를 초기화하기 위한 준비이다.
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH1); // LAYER_2의 첫 번째 채널 사용 체크박스를 찾아 m_pChkChannelUse 배열에 저장한다.
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH2);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH3);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH4);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH5);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH6);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH7);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH8);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH9);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH10);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH11);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH12);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH13);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH14);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH15);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER2_CH16); // LAYER_2의 나머지 채널 사용 체크박스들을 배열에 추가한다.
	i = 0;
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH1);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH2);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH3);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH4);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH5);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH6);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH7);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH8);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH9);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH10);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH11);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH12);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH13);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH14);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH15);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER2_CH16);
	i = 0;
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH1);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH2);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH3);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH4);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH5);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH6);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH7);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH8);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH9);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH10);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH11);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH12);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH13);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH14);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH15);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER2_CH16);

	i = 0;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH1;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH2;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH3;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH4;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH5;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH6;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH7;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH8;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH9;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH10;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH11;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH12;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH13;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH14;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH15;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER2_CH16;

	layer = LAYER_3;
	i = 0;
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH1);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH2);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH3);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH4);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH5);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH6);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH7);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH8);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH9);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH10);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH11);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH12);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH13);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH14);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH15);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER3_CH16);
	i = 0;
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH1);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH2);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH3);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH4);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH5);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH6);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH7);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH8);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH9);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH10);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH11);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH12);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH13);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH14);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH15);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER3_CH16);
	i = 0;
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH1);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH2);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH3);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH4);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH5);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH6);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH7);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH8);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH9);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH10);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH11);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH12);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH13);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH14);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH15);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER3_CH16);

	i = 0;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH1;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH2;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH3;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH4;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH5;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH6;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH7;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH8;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH9;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH10;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH11;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH12;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH13;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH14;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH15;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER3_CH16;

	layer = LAYER_4;
	i = 0;
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH1);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH2);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH3);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH4);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH5);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH6);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH7);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH8);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH9);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH10);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH11);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH12);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH13);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH14);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH15);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER4_CH16);
	i = 0;
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH1);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH2);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH3);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH4);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH5);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH6);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH7);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH8);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH9);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH10);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH11);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH12);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH13);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH14);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH15);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER4_CH16);
	i = 0;
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH1);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH2);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH3);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH4);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH5);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH6);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH7);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH8);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH9);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH10);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH11);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH12);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH13);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH14);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH15);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER4_CH16);

	i = 0;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH1;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH2;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH3;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH4;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH5;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH6;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH7;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH8;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH9;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH10;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH11;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH12;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH13;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH14;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH15;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER4_CH16;

	layer = LAYER_5;
	i = 0;
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH1);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH2);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH3);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH4);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH5);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH6);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH7);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH8);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH9);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH10);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH11);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH12);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH13);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH14);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH15);
	m_pChkChannelUse[layer][i++] = (CButton*)GetDlgItem(IDC_CHK_PI_LAYER5_CH16);
	i = 0;
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH1);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH2);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH3);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH4);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH5);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH6);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH7);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH8);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH9);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH10);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH11);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH12);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH13);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH14);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH15);
	m_pSttChInfo[layer][i++] = (CStatic*)GetDlgItem(IDC_STT_PI_MSG_LAYER5_CH16);
	i = 0;
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH1);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH2);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH3);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH4);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH5);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH6);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH7);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH8);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH9);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH10);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH11);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH12);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH13);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH14);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH15);
	m_pedtPannelID[layer][i++] = (CEdit*)GetDlgItem(IDC_EDT_PI_PID_LAYER5_CH16);

	i = 0;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH1;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH2;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH3;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH4;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH5;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH6;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH7;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH8;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH9;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH10;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH11;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH12;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH13;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH14;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH15;
	m_nedtDlgCtrlID[layer][i++] = IDC_EDT_PI_PID_LAYER5_CH16;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Channel 별 Check 상태 초기화
	////////////////////////////////////////////////////////////////////////////////////////////
	Lf_loadMesStatusInfo(); // Lf_loadMesStatusInfo 함수를 호출하여 MES(Manufacturing Execution System) 상태 정보를 로드한다.
	                        // 이 함수는PID 제어와 관련된 상태 정보를 초기화하거나 업데이트 하는 역할을 한다.

}

void CPidInput::Lf_InitFontset()
{
	m_Font[0].CreateFont(150, 70, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[1].CreateFont(44, 20, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_STT_PI_TITLE)->SetFont(&m_Font[1]);

	m_Font[2].CreateFont(32, 14, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_EDT_PI_RACK_ID)->SetFont(&m_Font[2]);

	m_Font[3].CreateFont(24, 11, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_MBC_PI_RACK1)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_MBC_PI_RACK2)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_MBC_PI_RACK3)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_MBC_PI_RACK4)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_MBC_PI_RACK5)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_MBC_PI_RACK6)->SetFont(&m_Font[3]);

	m_Font[4].CreateFont(19, 8, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_STT_PI_LAYER1)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LAYER2)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LAYER3)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LAYER4)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LAYER5)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LEFT_LAYER1)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LEFT_LAYER2)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LEFT_LAYER3)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LEFT_LAYER4)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_STT_PI_LEFT_LAYER5)->SetFont(&m_Font[4]);

	GetDlgItem(IDC_GRP_PI_PID_OPERATION)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_GRP_PI_MES_OPERATION)->SetFont(&m_Font[4]);

	GetDlgItem(IDC_MBC_PI_PID_CLEAR)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_MBC_PI_CH_ALL_SELECT)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_MBC_PI_CH_ALL_CLEAR)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_BTN_PI_MES_DSPM)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_BTN_PI_MES_DMIN)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_BTN_PI_MES_DMOU)->SetFont(&m_Font[4]);

	GetDlgItem(IDC_BTN_PI_SAVE_EXIT)->SetFont(&m_Font[4]);
	GetDlgItem(IDC_BTN_PI_CANCEL)->SetFont(&m_Font[4]);

	m_Font[5].CreateFont(16, 7, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
}

void CPidInput::Lf_InitColorBrush()
{
	// 각 Control의 COLOR 설정을 위한 Brush를 Setting 한다.
	m_Brush[COLOR_IDX_BLACK].CreateSolidBrush(COLOR_BLACK);
	m_Brush[COLOR_IDX_WHITE].CreateSolidBrush(COLOR_WHITE);
	m_Brush[COLOR_IDX_RED].CreateSolidBrush(COLOR_RED);
	m_Brush[COLOR_IDX_GREEN].CreateSolidBrush(COLOR_GREEN);
	m_Brush[COLOR_IDX_BLUE].CreateSolidBrush(COLOR_BLUE);
	m_Brush[COLOR_IDX_SEABLUE].CreateSolidBrush(COLOR_SEABLUE);
	m_Brush[COLOR_IDX_SKYBLUE].CreateSolidBrush(COLOR_SKYBLUE);
	m_Brush[COLOR_IDX_ORANGE].CreateSolidBrush(COLOR_ORANGE);
	m_Brush[COLOR_IDX_VERDANT].CreateSolidBrush(COLOR_VERDANT);
	m_Brush[COLOR_IDX_YELLOW].CreateSolidBrush(COLOR_YELLOW);
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
	m_Brush[COLOR_IDX_DARK_BG].CreateSolidBrush(COLOR_DARK_BG);
	m_Brush[COLOR_IDX_ITEM_HEAD].CreateSolidBrush(COLOR_ITEM_HEAD);
	m_Brush[COLOR_IDX_ITEM_TITLE].CreateSolidBrush(COLOR_ITEM_TITLE);
}

void CPidInput::Lf_InitDialogDesign()
{
	SetWindowTheme(GetDlgItem(IDC_MBC_PI_RACK1)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_MBC_PI_RACK2)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_MBC_PI_RACK3)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_MBC_PI_RACK4)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_MBC_PI_RACK5)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_MBC_PI_RACK6)->GetSafeHwnd(), L"", L"");

	SetWindowTheme(GetDlgItem(IDC_GRP_PI_PID_OPERATION)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_GRP_PI_MES_OPERATION)->GetSafeHwnd(), L"", L"");

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			SetWindowTheme(m_pChkChannelUse[layer][ch]->GetSafeHwnd(), L"", L"");
		}
	}

	m_pApp->Gf_setGradientStatic02(&m_sttPiRackID, &m_Font[2], FALSE);

	m_btnPiSaveExit.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OX_O));
	m_btnPiCancel.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_OX_X));
}

void CPidInput::Lf_ChangeColorRackButton(int selectRack)
{
	m_nSelRack = selectRack;

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		if (rack == selectRack)
		{
			m_pBtnPidRack[rack]->EnableWindowsTheming(FALSE);
			m_pBtnPidRack[rack]->SetFaceColor(COLOR_BUTTON_SEL);
			m_pBtnPidRack[rack]->SetTextColor(COLOR_GRAY224);
		}
		else
		{
			m_pBtnPidRack[rack]->EnableWindowsTheming(FALSE);
			m_pBtnPidRack[rack]->SetFaceColor(COLOR_BUTTON_DARK);
			m_pBtnPidRack[rack]->SetTextColor(COLOR_GRAY224);
		}
	}
}

void CPidInput::Lf_updatePanelID()
{
	GetDlgItem(IDC_EDT_PI_RACK_ID)->SetWindowText(lpInspWorkInfo->m_sRackID[m_nSelRack]);

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			m_pChkChannelUse[layer][ch]->SetCheck(lpInspWorkInfo->m_bMesChannelUse[m_nSelRack][layer][ch]);
			m_pedtPannelID[layer][ch]->SetWindowText(lpInspWorkInfo->m_sMesPanelID[m_nSelRack][layer][ch]);
		}
	}
}

void CPidInput::Lf_addMessage(CString msg)
{
	m_lstPiMesMessage.SetCurSel(m_lstPiMesMessage.AddString(msg));
}


void CPidInput::Lf_setFocus()
{
	CString sdata;

	while (1)
	{
		m_nSelChannel++;
		if ((m_nSelLayer == LAYER_1) && (m_nSelChannel >= MAX_LAYER_CHANNEL))
			return;

		if (m_nSelChannel <= 8)
		{
			if ((m_nSelChannel % 2) == 0)
			{
				if (m_nSelLayer == LAYER_5)
				{
					if (m_nSelChannel == CH_9)
					{
						m_nSelLayer = LAYER_5;
					}
					else
					{
						m_nSelLayer = 0;
					}
				}
				else
				{
					m_nSelLayer++;
					m_nSelChannel = m_nSelChannel - 2;
				}
			}
		}
		else
		{
			if ((m_nSelChannel % 2) == 0)
			{
				if (m_nSelLayer == LAYER_1)
				{
					m_nSelLayer = LAYER_5;
				}
				else
				{
					m_nSelLayer--;
					m_nSelChannel = m_nSelChannel - 2;
				}
			}
		}

		if (m_pChkChannelUse[m_nSelLayer][m_nSelChannel]->GetCheck() == TRUE)
			break;
	}

	m_pedtPannelID[m_nSelLayer][m_nSelChannel]->SetFocus();
	m_pedtPannelID[m_nSelLayer][m_nSelChannel]->SetSel(0, m_pedtPannelID[m_nSelLayer][m_nSelChannel]->GetWindowTextLength());
}

void CPidInput::Lf_loadMesStatusInfo() // MES 상태 정보를 로드하는 역할을 한다.
{
	CString section, key; // MES 상태 정보를 읽기 위한 섹션과 키를 저장하는데 사용되는 변수

	for (int layer = 0; layer < MAX_LAYER; layer++) // layer 변수를 0부터 MAX_LAYER 미만까지 반복하는 루프를 시작한다. 각 레이어에 대해 MES 상태 정보를 읽기 위한 준비를 한다.
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // ch 변수를 0부터 MAX_LAYER_CHANNEL 미만까지 반복하는 루프를 시작한다. 각 레이어의 채널에 대해 MES 상태 정보를 읽기 위한 준비를 한다.
		{
			section.Format(_T("RACK-%d"), m_nSelRack + 1); // section 변수를 현재 선택된 랙의 이름으로 포맷한다. m_nSelRack에 1을 더하여 랙 번호를 1부터 시작하도록 설정한다. 예를 들어, m_nSelRack이 0이면 section은 "RACK-1"이 된다.
			key.Format(_T("LAYER%d-CH%d"), layer + 1, ch + 1); // key 변수를 현재 레이어와 채널 번호로 포맷한다. layer와 ch에 각각 1을 더하여 레이어와 채널 번호를 1부터 시작하도록 설정한다. 예를 들어, layer가 0이고 ch가 0이면 key는 "LAYER1-CH1"이 된다.
			Read_MesStatusInfo(section, key, &lpInspWorkInfo->m_nMesDspmOK[m_nSelRack][layer][ch]); // Read_MesStatusInfo 함수를 호출하여 MES 상태 정보를 읽는다. section과 key를 인자로 전달하고, 읽은 정보를 IpInspWorkInfo 구조체의 m_nMesDspmOK 배열의 해당 위치에 저장한다. 이 배열은 선택된 랙, 레이어, 채널에 대한 MES 상태 정보를 저장하는 데 사용된다.
		}
	}
}

void CPidInput::Lf_saveMesStatusInfo()
{
	CString section, key;

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			section.Format(_T("RACK-%d"), m_nSelRack + 1);
			key.Format(_T("LAYER%d-CH%d"), layer + 1, ch + 1);

			Write_MesStatusInfo(section, key, lpInspWorkInfo->m_nMesDspmOK[m_nSelRack][layer][ch]);
		}
	}
}

void CPidInput::Lf_clearMesStatusInfo()
{
	CString section, key;

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			// AGN_IN
			section.Format(_T("RACK-%d"), m_nSelRack + 1);
			key.Format(_T("LAYER%d-CH%d"), layer + 1, ch + 1);
			lpInspWorkInfo->m_nMesDspmOK[m_nSelRack][layer][ch] = 0;
			Write_MesStatusInfo(section, key, 0);
		}
	}
}

int CPidInput::Lf_checkPIDMesInfo(int ctrl_id)
{
	CString bcr_info;

	GetDlgItem(ctrl_id)->GetWindowText(bcr_info);


	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			if (ctrl_id == m_nedtDlgCtrlID[layer][ch])
			{
				if (lpInspWorkInfo->m_nMesDspmOK[m_nSelRack][layer][ch] == 1)
					return TRUE;
				else
					return FALSE;
			}
		}
	}

	return FALSE;
}

int CPidInput::Lf_checkPIDErrorInfo(int ctrl_id)
{
	int isMatch=FALSE, match_layer, match_ch;
	CString bcr_info;

	GetDlgItem(ctrl_id)->GetWindowText(bcr_info);

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			if (ctrl_id == m_nedtDlgCtrlID[layer][ch])
			{
				isMatch = TRUE;
				match_layer = layer;
				match_ch = ch;
			}
		}

		if (isMatch == TRUE)
			break;
	}
	if (isMatch == FALSE)
		return 0;

	if (bcr_info.IsEmpty() == FALSE)
	{
		/////////////////////////////////////////////////////////////////////
		// BCR에 PID CHECk CODE가 없으면 Error로 처리한다.
		/////////////////////////////////////////////////////////////////////
		if (bcr_info.Find(lpModelInfo->m_sPanelIDCode) == -1)
		{
			m_bPIDRuleError[match_layer][match_ch] = TRUE;
			return -1;
		}

		/////////////////////////////////////////////////////////////////////
		// 중복으로 입력된 Code가 있으면 Error로 처리한다.
		/////////////////////////////////////////////////////////////////////
		CString sBuff;
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				if ((layer==match_layer) && (ch == match_ch))	// 자신의 ID 위치는 Skip 한다.
					continue;

				if (GetDlgItem(m_nedtDlgCtrlID[layer][ch])->IsWindowVisible() == TRUE)
				{
					GetDlgItem(m_nedtDlgCtrlID[layer][ch])->GetWindowText(sBuff);

					if (bcr_info == sBuff)
					{
						m_bPIDRuleError[match_layer][match_ch] = TRUE;
						return -1;
					}
				}
			}
		}
		/////////////////////////////////////////////////////////////////////
	}

	m_bPIDRuleError[match_layer][match_ch] = FALSE;
	return 0;
}

int CPidInput::Lf_checkPIDValidInfo(int ctrl_id)
{
	CString bcr_info;

	if (GetDlgItem(ctrl_id)->GetWindowTextLength() != 0)
		return 1;

	return 0;
}

int CPidInput::Lf_checkPIDFocusInfo(int ctrl_id)
{
	if (GetDlgItem(ctrl_id) == GetFocus())
		return 1;

	return 0;
}

void CPidInput::Lf_InvalidateWindow()
{
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			GetDlgItem(m_nedtDlgCtrlID[layer][ch])->Invalidate(FALSE);
		}
	}
}

BOOL CPidInput::Lf_isExistErrorChannel()
{
	CMessageError err_dlg;

	////////////////////////////////////////////////////////////////
	// PID Rule Error 조건이 있는지 확인한다.
	////////////////////////////////////////////////////////////////
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			if (m_bPIDRuleError[layer][ch] == TRUE)
			{
				err_dlg.m_strEMessage.Format(_T("'LAYER%d-CH%d' PID was entered incorrectly."), layer + 1, ch + 1);
				err_dlg.DoModal();

				return TRUE;
			}
		}
	}
	////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////
	// ICC 값이 있으나 PID 입력되지 않은 조건이 있는지 확인한다.
	// ICC 값이 없지만 PID가 입력된 조건이 있는지 확인한다.
	////////////////////////////////////////////////////////////////
	CString pid_buf;
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			GetDlgItem(m_nedtDlgCtrlID[layer][ch])->GetWindowText(pid_buf);

			if (lpInspWorkInfo->m_nMeasICC[m_nSelRack][layer][ch] < 50)
			{
				// ICC 측정되지 않으나 PID 입력된 경우.
				if (pid_buf.GetLength() != 0)
				{
					err_dlg.m_strEMessage.Format(_T("'LAYER%d-CH%d' PID Input Error.  ICC-%dmA  PID-%s"), layer + 1, ch + 1, lpInspWorkInfo->m_nMeasICC[m_nSelRack][ch], pid_buf);
					err_dlg.DoModal();

					return TRUE;
				}
			}
			else
			{
				// ICC 측정되었지만 PID 입력되지 않은 경우.
				if (pid_buf.GetLength() == 0)
				{
					err_dlg.m_strEMessage.Format(_T("'LAYER%d-CH%d' PID Input Error.  ICC-%dmA  PID-NULL"), layer + 1, ch + 1, lpInspWorkInfo->m_nMeasICC[m_nSelRack][ch]);
					err_dlg.DoModal();

					return TRUE;
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////


	return FALSE;
}


void CPidInput::Lf_AllChannelSelect(int onoff)
{
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			m_pChkChannelUse[layer][ch]->SetCheck(onoff);
		}
	}
}

BOOL CPidInput::Lf_setExecuteMesDSPM()
{
	CString sdata = _T(""), sLog;

	if (m_pApp->m_bIsGmesConnect == FALSE)
	{
		Lf_addMessage(_T("MES not connected"));
		return TRUE;
	}

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			if (m_pChkChannelUse[layer][ch]->GetCheck() == FALSE)
			{
				sdata.Format(_T("<MES> DSPM Send SKIP - R%d-L%d-C%d"), m_nSelRack + 1, layer + 1, ch + 1);
				Lf_addMessage(sdata);
				continue;
			}

			if (lpInspWorkInfo->m_sMesPanelID[m_nSelRack][layer][ch].IsEmpty() == TRUE)
				continue;


			sdata.Format(_T("Rack%d-Layer%d-Ch%d  PID:%s"), m_nSelRack + 1, layer + 1, ch + 1, lpInspWorkInfo->m_sMesPanelID[m_nSelRack][layer][ch]);
			if (m_pApp->Gf_gmesSendHost(HOST_DSPM, m_nSelRack, layer, ch) == TRUE)
			{
				sLog.Format(_T("<MES> DSPM Send OK - %s"), sdata);
				m_pApp->Gf_writeMLog(sLog);

				sdata.Format(_T("<MES> DSPM Send OK - R%d-L%d-C%d"), m_nSelRack + 1, layer + 1, ch + 1);
				Lf_addMessage(sdata);
			}
			else
			{
				sLog.Format(_T("<MES> DSPM Send NG - %s"), sdata);
				m_pApp->Gf_writeMLog(sLog);

				sdata.Format(_T("<MES> DSPM Send NG - R%d-L%d-C%d"), m_nSelRack + 1, layer + 1, ch + 1);
				Lf_addMessage(sdata);
			}
		}
	}

	return TRUE;
}

BOOL CPidInput::Lf_setExecuteMesDMIN()
{
	CString sLog;

	if (m_pApp->m_bIsGmesConnect == FALSE)
	{
		Lf_addMessage(_T("MES not connected"));
		return TRUE;
	}

	if (m_pApp->Gf_gmesSendHost(HOST_DMIN, NULL, NULL, NULL) == TRUE)
	{
		sLog.Format(_T("<MES> DMIN Send OK - RACK%d"), m_nSelRack + 1);
		m_pApp->Gf_writeMLog(sLog);
		Lf_addMessage(sLog);

		Lf_saveMesStatusInfo();
	}
	else
	{
		sLog.Format(_T("<MES> DMIN Send NG - RACK%d"), m_nSelRack + 1);
		m_pApp->Gf_writeMLog(sLog);
	}

	return TRUE;
}

BOOL CPidInput::Lf_setExecuteMesDMOU()
{
	CString sLog;

	if (m_pApp->m_bIsGmesConnect == FALSE)
	{
		Lf_addMessage(_T("MES not connected"));
		return TRUE;
	}

	if (m_pApp->Gf_gmesSendHost(HOST_DMOU, m_nMesAutoRackNo, NULL, NULL) == TRUE)
	{
		sLog.Format(_T("<MES> DMOU Send OK - RACK%d"), m_nMesAutoRackNo + 1);
		m_pApp->Gf_writeMLog(sLog);
		Lf_addMessage(sLog);

		Lf_clearMesStatusInfo();
	}
	else
	{
		sLog.Format(_T("<MES> DMOU Send NG - RACK%d"), m_nMesAutoRackNo + 1);
		m_pApp->Gf_writeMLog(sLog);
	}

	return TRUE;
}