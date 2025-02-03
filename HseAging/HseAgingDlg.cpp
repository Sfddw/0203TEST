﻿
// HseAgingDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "HseAging.h"
#include "HseAgingDlg.h"
#include "afxdialogex.h"
#include "UserID.h"
#include "Monitoring.h"
#include "ModelInfo.h"
#include "PidInput.h"
#include "System.h"
#include "MessageQuestion.h"
#include "AutoFirmware.h"
#include "CableOpen.h"
#include "Password.h"

#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "setupapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT ThreadAgingStartRack(LPVOID pParam)
{
	CHseAgingDlg* pDlg = (CHseAgingDlg*)pParam;
	ULONGLONG agingTotalSec, agingElapseSec;
	LPINSPWORKINFO lpInspWorkInfo = m_pApp->GetInspWorkInfo();

	while (1)
	{
		for (int rack = 0; rack < MAX_RACK; rack++)
		{
			if (pDlg->m_nAgingStart[rack] == FALSE)
				continue;

			agingTotalSec = lpInspWorkInfo->m_nAgingSetTime[rack] * 60;

			agingElapseSec = (::GetTickCount64() - lpInspWorkInfo->m_nAgingStartTick[rack]) / 1000;

			// Aging 완료되면 Start Flag Clear
			if (agingElapseSec > agingTotalSec)
			{
				pDlg->m_nAgingStart[rack] = FALSE;
				lpInspWorkInfo->m_nAgingOperatingMode[rack] = AGING_COMPLETE;
				break;
			}

			// Aging 경과 시간을 저장 (UI Update 는 Main Dialog 에서 업데이트
			lpInspWorkInfo->m_nAgingRunTime[rack] = (int)(agingElapseSec / 60);
		}

		Sleep(10);
	}

	return (0);
}

UINT ThreadHandBcrSearch(LPVOID pParam)
{
	CString sLog;
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA stDevInfoData = SP_DEVINFO_DATA();
	LPINSPWORKINFO lpInspWorkInfo;
	lpInspWorkInfo = m_pApp->GetInspWorkInfo();

	hDevInfo = SetupDiGetClassDevs(0L, 0L, 0L, DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_PROFILE);

	if (hDevInfo == INVALID_HANDLE_VALUE)
		return FALSE;

	stDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &stDevInfoData); i++)
	{
		TCHAR szInstanceId[MAX_PATH] = { 0, };
		TCHAR szClassName[MAX_PATH] = { 0, };
		TCHAR szFriendlyName[MAX_PATH] = { 0, };
		TCHAR szClassDescription[MAX_PATH] = { 0, };
		TCHAR szDeviceDescription[MAX_PATH] = { 0, };

		// Get Device Instance ID
		BOOL bResult = SetupDiGetDeviceInstanceId(hDevInfo, &stDevInfoData, szInstanceId, _countof(szInstanceId), 0);
		if (!bResult)
		{
			AfxMessageBox(_T("Failed to get device instance ID"));
			continue;
		}

		CString instanceID;
		instanceID.Format(_T("%s"), szInstanceId);
		if (instanceID.Find(_T("VID_0C2E&PID_1221")) != -1)
		{
			lpInspWorkInfo->m_nConnectInfo[CONNECT_BARCODE] = TRUE;
			return (0);
		}

		// 		(VOID)SetupDiGetDeviceRegistryProperty(
		// 			hDevInfo,
		// 			&stDevInfoData,
		// 			SPDRP_CLASS,
		// 			0,
		// 			(PBYTE)szClassName,
		// 			_countof(szClassName),
		// 			0
		// 		);
		// 
		// 		(VOID)SetupDiGetDeviceRegistryProperty(
		// 			hDevInfo,
		// 			&stDevInfoData,
		// 			SPDRP_DEVICEDESC,
		// 			0,
		// 			(PBYTE)szDeviceDescription,
		// 			_countof(szDeviceDescription),
		// 			0
		// 		);
		// 
		// 		(VOID)SetupDiGetDeviceRegistryProperty(
		// 			hDevInfo,
		// 			&stDevInfoData,
		// 			SPDRP_FRIENDLYNAME,
		// 			0,
		// 			(PBYTE)szFriendlyName,
		// 			_countof(szFriendlyName),
		// 			0
		// 		);
		// 
		// 		(VOID)SetupDiGetClassDescription(
		// 			&stDevInfoData.ClassGuid,
		// 			szClassDescription,
		// 			_countof(szClassDescription),
		// 			0
		// 		);
		// 
		// 		sLog.Format(_T("[%d]"), i);
		// 		m_pApp->Gf_writeMLog(sLog);
		// 
		// 		sLog.Format(_T("-- Class: %s"), szClassName);
		// 		m_pApp->Gf_writeMLog(sLog);
		// 
		// 		sLog.Format(_T("-- Friendly Name: %s"), szFriendlyName);
		// 		m_pApp->Gf_writeMLog(sLog);
		// 
		// 		sLog.Format(_T("-- Instance ID: %s"), szInstanceId);
		// 		m_pApp->Gf_writeMLog(sLog);
		// 
		// 		sLog.Format(_T("-- Class Description: %s"), szClassDescription);
		// 		m_pApp->Gf_writeMLog(sLog);
		// 
		// 		sLog.Format(_T("-- Device Description: %s"), szDeviceDescription);
		// 		m_pApp->Gf_writeMLog(sLog);
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	lpInspWorkInfo->m_nConnectInfo[CONNECT_BARCODE] = FALSE;


	return (0);
}
UINT ThreadFwVersionRead(LPVOID pParam)
{
	// FW Read 함수를 Timer 에서 동작 시 S/W 멈춤현상이 있다. Thread 에서 동작하도록 이동
	m_pApp->pCommand->Gf_getMainBoardFwVersionAll();

	return (0);
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHseAgingDlg 대화 상자



CHseAgingDlg::CHseAgingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HSEAGING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int ly = 0; ly < MAX_LAYER; ly++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				m_pSttRackState[rack][ly][ch] = NULL;
			}
		}
	}

	m_pDefaultFont = new CFont();
	m_pDefaultFont->CreateFont(15, 6, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
}

void CHseAgingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_MA_USER, m_btnIconUser);
	DDX_Control(pDX, IDC_BTN_MA_MONITORING, m_btnIconMonitoring);
	DDX_Control(pDX, IDC_BTN_MA_MODEL, m_btnIconModel);
	DDX_Control(pDX, IDC_BTN_MA_PID_INPUT, m_btnIconPIDInput);
	DDX_Control(pDX, IDC_BTN_MA_FIRMWARE, m_btnIconFirmware);
	DDX_Control(pDX, IDC_BTN_MA_SYSTEM, m_btnIconSystem);
	DDX_Control(pDX, IDC_BTN_MA_EXIT, m_btnIconExit);
	DDX_Control(pDX, IDC_MBT_MA_BUZZ_OFF, m_mbtBuzzOff);
	DDX_Control(pDX, IDC_STT_MA_MLOG_RACK1, m_sttMaMLogRack1);
	DDX_Control(pDX, IDC_STT_MA_MLOG_RACK2, m_sttMaMLogRack2);
	DDX_Control(pDX, IDC_STT_MA_MLOG_RACK3, m_sttMaMLogRack3);
	DDX_Control(pDX, IDC_STT_MA_MLOG_RACK4, m_sttMaMLogRack4);
	DDX_Control(pDX, IDC_STT_MA_MLOG_RACK5, m_sttMaMLogRack5);
	DDX_Control(pDX, IDC_STT_MA_MLOG_RACK6, m_sttMaMLogRack6);
	DDX_Control(pDX, IDC_LST_MA_MLOG_RACK1, m_lstMaMLogRack1);
	DDX_Control(pDX, IDC_LST_MA_MLOG_RACK2, m_lstMaMLogRack2);
	DDX_Control(pDX, IDC_LST_MA_MLOG_RACK3, m_lstMaMLogRack3);
	DDX_Control(pDX, IDC_LST_MA_MLOG_RACK4, m_lstMaMLogRack4);
	DDX_Control(pDX, IDC_LST_MA_MLOG_RACK5, m_lstMaMLogRack5);
	DDX_Control(pDX, IDC_LST_MA_MLOG_RACK6, m_lstMaMLogRack6);
	DDX_Control(pDX, IDC_CMB_MA_MODEL_RACK1, m_cmbMaModelRack1);
	DDX_Control(pDX, IDC_CMB_MA_MODEL_RACK2, m_cmbMaModelRack2);
	DDX_Control(pDX, IDC_CMB_MA_MODEL_RACK3, m_cmbMaModelRack3);
	DDX_Control(pDX, IDC_CMB_MA_MODEL_RACK4, m_cmbMaModelRack4);
	DDX_Control(pDX, IDC_CMB_MA_MODEL_RACK5, m_cmbMaModelRack5);
	DDX_Control(pDX, IDC_CMB_MA_MODEL_RACK6, m_cmbMaModelRack6);
	DDX_Control(pDX, IDC_CTR_MA_PROGRESS_RACK1, m_ctrMaProgressRack1);
	DDX_Control(pDX, IDC_CTR_MA_PROGRESS_RACK2, m_ctrMaProgressRack2);
	DDX_Control(pDX, IDC_CTR_MA_PROGRESS_RACK3, m_ctrMaProgressRack3);
	DDX_Control(pDX, IDC_CTR_MA_PROGRESS_RACK4, m_ctrMaProgressRack4);
	DDX_Control(pDX, IDC_CTR_MA_PROGRESS_RACK5, m_ctrMaProgressRack5);
	DDX_Control(pDX, IDC_CTR_MA_PROGRESS_RACK6, m_ctrMaProgressRack6);
	DDX_Control(pDX, IDC_STT_TEMP_SENSOR, m_sttTempSensor);
	DDX_Control(pDX, IDC_STT_TEMP_SENSOR1_T, m_sttTempSensor1T);
	DDX_Control(pDX, IDC_STT_TEMP_SENSOR2_T, m_sttTempSensor2T);
	DDX_Control(pDX, IDC_STT_TEMP_SENSOR3_T, m_sttTempSensor3T);
	DDX_Control(pDX, IDC_STT_TEMP_SENSOR4_T, m_sttTempSensor4T);
	DDX_Control(pDX, IDC_STT_TEMP_SENSOR5_T, m_sttTempSensor5T);
	DDX_Control(pDX, IDC_STT_TEMP_SENSOR6_T, m_sttTempSensor6T);
	DDX_Control(pDX, IDC_STT_CONNECT_INFO, m_sttConnectInfo);
}

BEGIN_MESSAGE_MAP(CHseAgingDlg, CDialogEx)
	ON_MESSAGE(WM_ETH_UDP_RECEIVE, OnUdpReceive)
	ON_MESSAGE(WM_ETH_UDP_RECEIVE_DIO, OnUdpReceiveDio)
	ON_MESSAGE(WM_RS232_RECEIVED2, OnRs232Receive)
	ON_MESSAGE(WM_UPDATE_SYSTEM_INFO, OnUpdateSystemInfo)
	ON_MESSAGE(WM_BCR_RACK_ID_INPUT, OnBcrRackIDInput)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_MA_USER, &CHseAgingDlg::OnBnClickedBtnMaUser)
	ON_BN_CLICKED(IDC_BTN_MA_MONITORING, &CHseAgingDlg::OnBnClickedBtnMaMonitoring)
	ON_BN_CLICKED(IDC_BTN_MA_MODEL, &CHseAgingDlg::OnBnClickedBtnMaModel)
	ON_BN_CLICKED(IDC_BTN_MA_SYSTEM, &CHseAgingDlg::OnBnClickedBtnMaSystem)
	ON_BN_CLICKED(IDC_BTN_MA_PID_INPUT, &CHseAgingDlg::OnBnClickedBtnMaPidInput)
	ON_BN_CLICKED(IDC_BTN_MA_FIRMWARE, &CHseAgingDlg::OnBnClickedBtnMaFirmware)
	ON_BN_CLICKED(IDC_BTN_MA_EXIT, &CHseAgingDlg::OnBnClickedBtnMaExit)
	ON_BN_CLICKED(IDC_MBC_MA_FUSING_RACK1, &CHseAgingDlg::OnBnClickedMbcMaFusingRack1)
	ON_BN_CLICKED(IDC_MBC_MA_FUSING_RACK2, &CHseAgingDlg::OnBnClickedMbcMaFusingRack2)
	ON_BN_CLICKED(IDC_MBC_MA_FUSING_RACK3, &CHseAgingDlg::OnBnClickedMbcMaFusingRack3)
	ON_BN_CLICKED(IDC_MBC_MA_FUSING_RACK4, &CHseAgingDlg::OnBnClickedMbcMaFusingRack4)
	ON_BN_CLICKED(IDC_MBC_MA_FUSING_RACK5, &CHseAgingDlg::OnBnClickedMbcMaFusingRack5)
	ON_BN_CLICKED(IDC_MBC_MA_FUSING_RACK6, &CHseAgingDlg::OnBnClickedMbcMaFusingRack6)
	ON_BN_CLICKED(IDC_MBC_MA_START_RACK1, &CHseAgingDlg::OnBnClickedMbcMaStartRack1)
	ON_BN_CLICKED(IDC_MBC_MA_START_RACK2, &CHseAgingDlg::OnBnClickedMbcMaStartRack2)
	ON_BN_CLICKED(IDC_MBC_MA_START_RACK3, &CHseAgingDlg::OnBnClickedMbcMaStartRack3)
	ON_BN_CLICKED(IDC_MBC_MA_START_RACK4, &CHseAgingDlg::OnBnClickedMbcMaStartRack4)
	ON_BN_CLICKED(IDC_MBC_MA_START_RACK5, &CHseAgingDlg::OnBnClickedMbcMaStartRack5)
	ON_BN_CLICKED(IDC_MBC_MA_START_RACK6, &CHseAgingDlg::OnBnClickedMbcMaStartRack6)
	ON_BN_CLICKED(IDC_MBC_MA_STOP_RACK1, &CHseAgingDlg::OnBnClickedMbcMaStopRack1)
	ON_BN_CLICKED(IDC_MBC_MA_STOP_RACK2, &CHseAgingDlg::OnBnClickedMbcMaStopRack2)
	ON_BN_CLICKED(IDC_MBC_MA_STOP_RACK3, &CHseAgingDlg::OnBnClickedMbcMaStopRack3)
	ON_BN_CLICKED(IDC_MBC_MA_STOP_RACK4, &CHseAgingDlg::OnBnClickedMbcMaStopRack4)
	ON_BN_CLICKED(IDC_MBC_MA_STOP_RACK5, &CHseAgingDlg::OnBnClickedMbcMaStopRack5)
	ON_BN_CLICKED(IDC_MBC_MA_STOP_RACK6, &CHseAgingDlg::OnBnClickedMbcMaStopRack6)
	ON_BN_CLICKED(IDC_MBC_MA_CH_SET_RACK1, &CHseAgingDlg::OnBnClickedMbcMaChSetRack1)
	ON_BN_CLICKED(IDC_MBC_MA_CH_SET_RACK2, &CHseAgingDlg::OnBnClickedMbcMaChSetRack2)
	ON_BN_CLICKED(IDC_MBC_MA_CH_SET_RACK3, &CHseAgingDlg::OnBnClickedMbcMaChSetRack3)
	ON_BN_CLICKED(IDC_MBC_MA_CH_SET_RACK4, &CHseAgingDlg::OnBnClickedMbcMaChSetRack4)
	ON_BN_CLICKED(IDC_MBC_MA_CH_SET_RACK5, &CHseAgingDlg::OnBnClickedMbcMaChSetRack5)
	ON_BN_CLICKED(IDC_MBC_MA_CH_SET_RACK6, &CHseAgingDlg::OnBnClickedMbcMaChSetRack6)
	ON_BN_CLICKED(IDC_CHK_MA_SELECT_RACK1, &CHseAgingDlg::OnBnClickedChkMaSelectRack1)
	ON_BN_CLICKED(IDC_CHK_MA_SELECT_RACK2, &CHseAgingDlg::OnBnClickedChkMaSelectRack2)
	ON_BN_CLICKED(IDC_CHK_MA_SELECT_RACK3, &CHseAgingDlg::OnBnClickedChkMaSelectRack3)
	ON_BN_CLICKED(IDC_CHK_MA_SELECT_RACK4, &CHseAgingDlg::OnBnClickedChkMaSelectRack4)
	ON_BN_CLICKED(IDC_CHK_MA_SELECT_RACK5, &CHseAgingDlg::OnBnClickedChkMaSelectRack5)
	ON_BN_CLICKED(IDC_CHK_MA_SELECT_RACK6, &CHseAgingDlg::OnBnClickedChkMaSelectRack6)
	ON_COMMAND_RANGE(IDC_STT_RACK1L1_CH1, IDC_STT_RACK6L5_CH16, &CHseAgingDlg::OnStnClickedChEnableDisable)
	ON_BN_CLICKED(IDC_MBT_MA_BUZZ_OFF, &CHseAgingDlg::OnBnClickedMbtMaBuzzOff)
END_MESSAGE_MAP()


// CHseAgingDlg 메시지 처리기
LRESULT CHseAgingDlg::OnUdpReceive(WPARAM wParam, LPARAM lParam)
{
	CString strPacket;

#if (DEBUG_COMM_LOG==1)
	CString sLog;
	sLog.Format(_T("<UDP Recv> %S"), (char*)wParam);
	m_pApp->Gf_writeMLog(sLog);
#endif

	strPacket.Format(_T("%S"), (char*)wParam);
	m_pApp->udp_processPacket(strPacket);

	return 0;
}

LRESULT CHseAgingDlg::OnUdpReceiveDio(WPARAM wParam, LPARAM lParam)
{
	CString strPacket;

	strPacket.Format(_T("%S"), (char*)wParam);
	m_pApp->udp_processDioPacket(strPacket);

	return 0;
}

LRESULT CHseAgingDlg::OnRs232Receive(WPARAM wParam, LPARAM lParam)
{
#if (DEBUG_COMM_LOG==1)
	///////////////////////////////////////////////
	CString sLog;
	sLog.Format(_T("<RS232> RECV : %S"), (char*)wParam);
	m_pApp->Gf_writeMLog(sLog);
	///////////////////////////////////////////////
#endif

	Lf_parseSDR100Packet((char*)wParam);

	return (0);
}

LRESULT CHseAgingDlg::OnUpdateSystemInfo(WPARAM wParam, LPARAM lParam)
{
	Lf_updateSystemInfo();

	return (0);
}

LRESULT CHseAgingDlg::OnBcrRackIDInput(WPARAM wParam, LPARAM lParam)
{
	KillTimer(1);

	CPidInput id_dlg;
	id_dlg.m_nTargetRack = (int)wParam;
	id_dlg.m_nMesAutoDMOU = MES_DMOU_MODE_MANUAL;
	id_dlg.DoModal();

	SetTimer(1, 100, NULL);

	return (0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHseAgingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	lpSystemInfo = m_pApp->GetSystemInfo();
	lpModelInfo = m_pApp->GetModelInfo();
	lpInspWorkInfo = m_pApp->GetInspWorkInfo();

	// Dialog의 기본 FONT 설정.
	SendMessageToDescendants(WM_SETFONT, (WPARAM)m_pDefaultFont->GetSafeHandle(), 1, TRUE, FALSE);

	Lf_InitLocalValue();
	Lf_InitFontset();
	Lf_InitColorBrush();
	Lf_InitButtonIcon();
	Lf_InitDialogDesign();
	Lf_InitCobmoRackModelList();

	// DIO Board 초기화 명령을 전달한다
	Lf_setDIOBoardInitial();

	// Aging Thread 를 시작한다.
	AfxBeginThread(ThreadAgingStartRack, this);
	AfxBeginThread(ThreadHandBcrSearch, this);

	ShowWindow(SW_MAXIMIZE);

	SetTimer(1, 100, NULL);
	SetTimer(2, 1000, NULL);
	SetTimer(3, 3000, NULL);
	SetTimer(8, 1000, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CHseAgingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CHseAgingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트
		CRect rect, rectOri;
		GetClientRect(&rect);
		rectOri = rect;

		rect.bottom = 70;
		dc.FillSolidRect(rect, RGB(48,55,63));

		rect.top = rect.bottom;
		rect.bottom = rectOri.bottom;
		dc.FillSolidRect(rect, COLOR_USER_BACKGROUND);

		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CHseAgingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



HBRUSH CHseAgingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	switch (nCtlColor)
	{
		case CTLCOLOR_MSGBOX:
			break;
		case CTLCOLOR_EDIT:
			break;
		case CTLCOLOR_LISTBOX:
			if ((pWnd->GetDlgCtrlID() == IDC_LST_MA_MLOG_RACK1)
				|| (pWnd->GetDlgCtrlID() == IDC_LST_MA_MLOG_RACK2)
				|| (pWnd->GetDlgCtrlID() == IDC_LST_MA_MLOG_RACK3)
				|| (pWnd->GetDlgCtrlID() == IDC_LST_MA_MLOG_RACK4)
				|| (pWnd->GetDlgCtrlID() == IDC_LST_MA_MLOG_RACK5)
				|| (pWnd->GetDlgCtrlID() == IDC_LST_MA_MLOG_RACK6)
				)
			{
				pDC->SetBkColor(COLOR_BLACK);
				pDC->SetTextColor(COLOR_WHITE);
				return m_Brush[COLOR_IDX_BLACK];
			}
			break;
		case CTLCOLOR_SCROLLBAR:
			break;
		case CTLCOLOR_BTN:
			break;
		case CTLCOLOR_STATIC:		// Static, CheckBox control
			if ((pWnd->GetDlgCtrlID() == IDC_STATIC)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_CHAMBER_NO)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK1_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK1_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK1_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK1_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK1_LAYER5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK2_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK2_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK2_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK2_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK2_LAYER5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK3_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK3_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK3_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK3_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK3_LAYER5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK4_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK4_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK4_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK4_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK4_LAYER5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK5_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK5_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK5_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK5_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK5_LAYER5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK6_LAYER1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK6_LAYER2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK6_LAYER3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK6_LAYER4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RACK6_LAYER5)
				)
			{
				pDC->SetBkColor(COLOR_SKYBLUE);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_SKYBLUE];
			}

			if ((pWnd->GetDlgCtrlID() == IDC_GRP_MA_CHAMBER_NO)
				|| (pWnd->GetDlgCtrlID() == IDC_GRP_MA_DESCRIPTION)
				|| (pWnd->GetDlgCtrlID() == IDC_GRP_MA_FW_VERSION)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_MA_SELECT_RACK1)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_MA_SELECT_RACK2)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_MA_SELECT_RACK3)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_MA_SELECT_RACK4)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_MA_SELECT_RACK5)
				|| (pWnd->GetDlgCtrlID() == IDC_CHK_MA_SELECT_RACK6)
				)
			{
				pDC->SetBkColor(COLOR_USER_BACKGROUND);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_USER_BACKGROUND];
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DOOR1)
			{
				if (lpInspWorkInfo->m_nDoorOpenClose[DOOR_1] == DOOR_OPEN)
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
				else
				{
					pDC->SetBkColor(COLOR_BLUE128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_BLUE128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DOOR2)
			{
				if (lpInspWorkInfo->m_nDoorOpenClose[DOOR_2] == DOOR_OPEN)
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
				else
				{
					pDC->SetBkColor(COLOR_BLUE128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_BLUE128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DOOR3)
			{
				if (lpInspWorkInfo->m_nDoorOpenClose[DOOR_3] == DOOR_OPEN)
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
				else
				{
					pDC->SetBkColor(COLOR_BLUE128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_BLUE128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DOOR4)
			{
				if (lpInspWorkInfo->m_nDoorOpenClose[DOOR_4] == DOOR_OPEN)
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
				else
				{
					pDC->SetBkColor(COLOR_BLUE128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_BLUE128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DOOR5)
			{
				if (lpInspWorkInfo->m_nDoorOpenClose[DOOR_5] == DOOR_OPEN)
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
				else
				{
					pDC->SetBkColor(COLOR_BLUE128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_BLUE128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DOOR6)
			{
				if (lpInspWorkInfo->m_nDoorOpenClose[DOOR_6] == DOOR_OPEN)
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
				else
				{
					pDC->SetBkColor(COLOR_BLUE128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_BLUE128];
				}
			}

			if ((pWnd->GetDlgCtrlID() == IDC_STT_MA_SET_TIME_RACK1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_SET_TIME_RACK2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_SET_TIME_RACK3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_SET_TIME_RACK4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_SET_TIME_RACK5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_SET_TIME_RACK6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RUN_TIME_RACK1)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RUN_TIME_RACK2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RUN_TIME_RACK3)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RUN_TIME_RACK4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RUN_TIME_RACK5)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_MA_RUN_TIME_RACK6)
				)
			{
				pDC->SetBkColor(COLOR_BLACK);
				pDC->SetTextColor(COLOR_GREEN);
				return m_Brush[COLOR_IDX_BLACK];
			}

			if ((pWnd->GetDlgCtrlID() == IDC_STT_TEMP_SENSOR1_V)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_TEMP_SENSOR2_V)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_TEMP_SENSOR3_V)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_TEMP_SENSOR4_V)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_TEMP_SENSOR5_V)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_TEMP_SENSOR6_V)
				)
			{
				pDC->SetBkColor(COLOR_GREEN128);
				pDC->SetTextColor(COLOR_GRAY224);
				return m_Brush[COLOR_IDX_GREEN128];
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_CONNECT_INFO_PG)
			{
				if (lpInspWorkInfo->m_nConnectInfo[CONNECT_PG] == TRUE)
				{
					pDC->SetBkColor(COLOR_GREEN128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_GREEN128];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_CONNECT_INFO_DIO)
			{
				if (lpInspWorkInfo->m_nConnectInfo[CONNECT_DIO] != 0)
				{
					pDC->SetBkColor(COLOR_GREEN128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_GREEN128];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_CONNECT_INFO_TEMP)
			{
				if (lpInspWorkInfo->m_nConnectInfo[CONNECT_TEMP] != 0)
				{
					pDC->SetBkColor(COLOR_GREEN128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_GREEN128];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_CONNECT_INFO_BARCODE)
			{
				if (lpInspWorkInfo->m_nConnectInfo[CONNECT_BARCODE] == TRUE)
				{
					pDC->SetBkColor(COLOR_GREEN128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_GREEN128];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_CONNECT_INFO_MES)
			{
				if (lpInspWorkInfo->m_nConnectInfo[CONNECT_MES] == TRUE)
				{
					pDC->SetBkColor(COLOR_GREEN128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_GREEN128];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}

			if ((pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH7) | (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH14) 
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK1L5_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH6) 
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH6) 
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH10) 
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH2) 
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK2L5_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK3L5_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK4L5_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK5L5_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L1_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L2_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L3_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L4_CH16)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH1) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH2)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH3) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH4)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH5) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH6)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH7) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH8)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH9) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH10)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH11) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH12)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH13) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH14)
				|| (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH15) || (pWnd->GetDlgCtrlID() == IDC_STT_RACK6L5_CH16)
				)
			{
				int chStt;
				chStt = Lf_getChannelInfo(pWnd->GetDlgCtrlID());
				if (chStt == STATUS_IDLE)
				{
					pDC->SetBkColor(COLOR_GRAY128);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_GRAY128];
				}
				else if (chStt == STATUS_RUN)
				{
					pDC->SetBkColor(COLOR_LIGHT_GREEN);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_LIGHT_GREEN];
				}
				else if (chStt == STATUS_ERROR)
				{
					pDC->SetBkColor(COLOR_PURPLE);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_PURPLE];
				}
				else if (chStt == STATUS_UNUSE)
				{
					pDC->SetBkColor(COLOR_DARK_YELLOW);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_DARK_YELLOW];
				}
				else
				{
					pDC->SetBkColor(COLOR_DARK_RED);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_DARK_RED];
				}
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DESC_NOT_CONN)
			{
				pDC->SetBkColor(COLOR_DARK_RED);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_DARK_RED];
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DESC_IDLE)
			{
				pDC->SetBkColor(COLOR_GRAY128);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_GRAY128];
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DESC_RUN)
			{
				pDC->SetBkColor(COLOR_LIGHT_GREEN);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_LIGHT_GREEN];
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DESC_ERROR)
			{
				pDC->SetBkColor(COLOR_PURPLE);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_PURPLE];
			}
			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_DESC_UNUSE)
			{
				pDC->SetBkColor(COLOR_DARK_YELLOW);
				pDC->SetTextColor(COLOR_BLACK);
				return m_Brush[COLOR_IDX_DARK_YELLOW];
			}

			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_FW_VER_RACK1)
			{
				if (lpInspWorkInfo->m_nFwVerifyResult[RACK_1] == TRUE)
				{
					pDC->SetBkColor(COLOR_GRAY224);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_GRAY224];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}
			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_FW_VER_RACK2)
			{
				if (lpInspWorkInfo->m_nFwVerifyResult[RACK_2] == TRUE)
				{
					pDC->SetBkColor(COLOR_GRAY224);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_GRAY224];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}
			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_FW_VER_RACK3)
			{
				if (lpInspWorkInfo->m_nFwVerifyResult[RACK_3] == TRUE)
				{
					pDC->SetBkColor(COLOR_GRAY224);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_GRAY224];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}
			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_FW_VER_RACK4)
			{
				if (lpInspWorkInfo->m_nFwVerifyResult[RACK_4] == TRUE)
				{
					pDC->SetBkColor(COLOR_GRAY224);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_GRAY224];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}
			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_FW_VER_RACK5)
			{
				if (lpInspWorkInfo->m_nFwVerifyResult[RACK_5] == TRUE)
				{
					pDC->SetBkColor(COLOR_GRAY224);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_GRAY224];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}
			if (pWnd->GetDlgCtrlID() == IDC_STT_MA_FW_VER_RACK6)
			{
				if (lpInspWorkInfo->m_nFwVerifyResult[RACK_6] == TRUE)
				{
					pDC->SetBkColor(COLOR_GRAY224);
					pDC->SetTextColor(COLOR_BLACK);
					return m_Brush[COLOR_IDX_GRAY224];
				}
				else
				{
					pDC->SetBkColor(COLOR_RED128);
					pDC->SetTextColor(COLOR_WHITE);
					return m_Brush[COLOR_IDX_RED128];
				}
			}

			break;
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

BOOL CHseAgingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4)
	{
		if (::GetKeyState(VK_MENU) < 0)	return TRUE;
	}

	// 일반 Key 동작에 대한 Event
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return 1;
		}
		else if (pMsg->wParam == VK_RETURN)
		{
			return 1;
		}
		else if (pMsg->wParam == VK_SPACE)
		{
			return 1;
		}
		else if ((pMsg->wParam >= 33) && (pMsg->wParam <= 'z'))	// ! ~ z 까지의문자만 입력받음
		{
            CString sdata;
 			sdata.Format(_T("%c"), pMsg->wParam);
 			m_nMainKeyInData.Append(sdata);
 			Lf_checkBcrRackIDInput();
			return 1;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CHseAgingDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
		KillTimer(1);

		// Power Measure와 Aging Status를 Read한다.
		m_nMeasureCnt++;
		if (m_nMeasureCnt >= 5)
		{
			// 1초마다 Power Measure와 Status를 Check한다.
			m_nMeasureCnt = 0;
			if (m_nMeasureMode == MEAS_MODE_POWER)
			{
				Lf_getMeasurePower();
			}
			else if (m_nMeasureMode == MEAS_MODE_STATUS)
			{
				Lf_getAgingStatus();
			}
			else if (m_nMeasureMode == MEAS_MODE_VERSION)
			{
				// MCU Firmware Version Read.
				Lf_updateFirmwareMatching();
			}

			if (++m_nMeasureMode >= MEAS_MAX)
				m_nMeasureMode = 0;;


			Lf_updateEthConnectInfo();
			Lf_updateAgingStatus();
			Lf_updateTowerLamp();
			Lf_checkPowerLimitAlarm();
		}

		SetTimer(1, 100, NULL);
	}
	if (nIDEvent == 2)
	{
		// 1초 Timer
		Lf_getTemperature();
		Lf_getDIOStatus();
	}
	if (nIDEvent == 3)
	{
		// 3초 Timer
		AfxBeginThread(ThreadHandBcrSearch, this);
		AfxBeginThread(ThreadFwVersionRead, this);
	}

	if (nIDEvent == 8)
	{
		if (lpInspWorkInfo->m_bAlarmOccur == TRUE)
		{
			KillTimer(8);
			lpInspWorkInfo->m_bAlarmOccur = FALSE;

			m_pApp->Gf_ShowMessageBox(lpInspWorkInfo->m_sAlarmMessage);

			SetTimer(8, 1000, NULL);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CHseAgingDlg::OnBnClickedBtnMaUser()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	KillTimer(1);

	CUserID user_dlg;
	if (user_dlg.DoModal() == IDOK)
	{
		Lf_updateSystemInfo();
	}

	SetTimer(1, 100, NULL);
}

void CHseAgingDlg::OnBnClickedBtnMaMonitoring()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMonitoring mnt_dlg;
	mnt_dlg.DoModal();
}


void CHseAgingDlg::OnBnClickedBtnMaModel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Lf_checkAgingIDLEMode() == FALSE)
		return;

	KillTimer(1);

	CModelInfo model_dlg;
	model_dlg.DoModal();

	Lf_InitCobmoRackModelList();

	SetTimer(1, 100, NULL);
}


void CHseAgingDlg::OnBnClickedBtnMaPidInput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	KillTimer(1);

	CPidInput id_dlg;
	id_dlg.m_nMesAutoDMOU = MES_DMOU_MODE_MANUAL;
	id_dlg.DoModal();

	SetTimer(1, 100, NULL);
}


void CHseAgingDlg::OnBnClickedBtnMaFirmware()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Lf_checkAgingIDLEMode() == FALSE)
		return;

	KillTimer(1);

	CAutoFirmware af_dlg;
	af_dlg.DoModal();

	SetTimer(1, 100, NULL);
}


void CHseAgingDlg::OnBnClickedBtnMaSystem()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Lf_checkAgingIDLEMode() == FALSE)
		return;

	// 2025-01-13 PDH. Password 입력 기능 추가
	CPassword pw_dlg;
	if (pw_dlg.DoModal() == IDCANCEL)
		return;

	KillTimer(1);

	CSystem sys_dlg;
	if (sys_dlg.DoModal() == IDOK)
	{
		m_pApp->Gf_InitialSystemInfo();
		Lf_updateSystemInfo();
	}

	SetTimer(1, 100, NULL);
}


void CHseAgingDlg::OnBnClickedBtnMaExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (Lf_checkAgingIDLEMode() == FALSE)
		return;

	KillTimer(1);

	CMessageQuestion msg_dlg;
	msg_dlg.m_strQMessage.Format(_T("Do you want exit the program ?"));
	if (msg_dlg.DoModal() == IDOK)
	{
		CDialog::OnOK();
	}

	SetTimer(1, 100, NULL);
}

void CHseAgingDlg::OnBnClickedMbtMaBuzzOff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	lpInspWorkInfo->m_nDioOutputData = lpInspWorkInfo->m_nDioOutputData & ~DIO_OUT_BUZZER;

	m_pApp->pCommand->Gf_dio_setDIOWriteOutput(lpInspWorkInfo->m_nDioOutputData, lpInspWorkInfo->m_nDioOutputMode);
}

void CHseAgingDlg::OnBnClickedMbcMaStartRack1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTART(RACK_1);
}


void CHseAgingDlg::OnBnClickedMbcMaStopRack1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTOP(RACK_1);
}


void CHseAgingDlg::OnBnClickedMbcMaFusingRack1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingFUSING(RACK_1);
}


void CHseAgingDlg::OnBnClickedMbcMaStartRack2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTART(RACK_2);
}


void CHseAgingDlg::OnBnClickedMbcMaStopRack2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTOP(RACK_2);
}


void CHseAgingDlg::OnBnClickedMbcMaFusingRack2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingFUSING(RACK_2);
}


void CHseAgingDlg::OnBnClickedMbcMaStartRack3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTART(RACK_3);
}


void CHseAgingDlg::OnBnClickedMbcMaStopRack3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTOP(RACK_3);
}


void CHseAgingDlg::OnBnClickedMbcMaFusingRack3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingFUSING(RACK_3);
}


void CHseAgingDlg::OnBnClickedMbcMaStartRack4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTART(RACK_4);
}


void CHseAgingDlg::OnBnClickedMbcMaStopRack4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTOP(RACK_4);
}


void CHseAgingDlg::OnBnClickedMbcMaFusingRack4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingFUSING(RACK_4);
}


void CHseAgingDlg::OnBnClickedMbcMaStartRack5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTART(RACK_5);
}


void CHseAgingDlg::OnBnClickedMbcMaStopRack5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTOP(RACK_5);
}


void CHseAgingDlg::OnBnClickedMbcMaFusingRack5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingFUSING(RACK_5);
}


void CHseAgingDlg::OnBnClickedMbcMaStartRack6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTART(RACK_6);
}


void CHseAgingDlg::OnBnClickedMbcMaStopRack6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingSTOP(RACK_6);
}


void CHseAgingDlg::OnBnClickedMbcMaFusingRack6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setAgingFUSING(RACK_6);
}

void CHseAgingDlg::OnBnClickedChkMaSelectRack1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_channelUseButtonShowHide(RACK_1);
}


void CHseAgingDlg::OnBnClickedChkMaSelectRack2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_channelUseButtonShowHide(RACK_2);
}


void CHseAgingDlg::OnBnClickedChkMaSelectRack3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_channelUseButtonShowHide(RACK_3);
}


void CHseAgingDlg::OnBnClickedChkMaSelectRack4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_channelUseButtonShowHide(RACK_4);
}


void CHseAgingDlg::OnBnClickedChkMaSelectRack5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_channelUseButtonShowHide(RACK_5);
}


void CHseAgingDlg::OnBnClickedChkMaSelectRack6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_channelUseButtonShowHide(RACK_6);
}


void CHseAgingDlg::OnBnClickedMbcMaChSetRack1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setChannelUseUnuse(RACK_1);
}


void CHseAgingDlg::OnBnClickedMbcMaChSetRack2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setChannelUseUnuse(RACK_2);
}


void CHseAgingDlg::OnBnClickedMbcMaChSetRack3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setChannelUseUnuse(RACK_3);
}


void CHseAgingDlg::OnBnClickedMbcMaChSetRack4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setChannelUseUnuse(RACK_4);
}


void CHseAgingDlg::OnBnClickedMbcMaChSetRack5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setChannelUseUnuse(RACK_5);
}


void CHseAgingDlg::OnBnClickedMbcMaChSetRack6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Lf_setChannelUseUnuse(RACK_6);
}

void CHseAgingDlg::OnStnClickedChEnableDisable(UINT nID)
{
	if (m_pChkChSelect[RACK_1]->GetCheck() == TRUE)
	{
		switch (nID)
		{
		case IDC_STT_RACK1L1_CH1: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_1);	break; }
		case IDC_STT_RACK1L1_CH2: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_2);	break; }
		case IDC_STT_RACK1L1_CH3: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_3);	break; }
		case IDC_STT_RACK1L1_CH4: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_4);	break; }
		case IDC_STT_RACK1L1_CH5: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_5);	break; }
		case IDC_STT_RACK1L1_CH6: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_6);	break; }
		case IDC_STT_RACK1L1_CH7: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_7);	break; }
		case IDC_STT_RACK1L1_CH8: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_8);	break; }
		case IDC_STT_RACK1L1_CH9: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_9);	break; }
		case IDC_STT_RACK1L1_CH10: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_10);	break; }
		case IDC_STT_RACK1L1_CH11: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_11);	break; }
		case IDC_STT_RACK1L1_CH12: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_12);	break; }
		case IDC_STT_RACK1L1_CH13: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_13);	break; }
		case IDC_STT_RACK1L1_CH14: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_14);	break; }
		case IDC_STT_RACK1L1_CH15: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_15);	break; }
		case IDC_STT_RACK1L1_CH16: {Lf_toggleChUseUnuse(RACK_1, LAYER_1, CH_16);	break; }
		case IDC_STT_RACK1L2_CH1: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_1);	break; }
		case IDC_STT_RACK1L2_CH2: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_2);	break; }
		case IDC_STT_RACK1L2_CH3: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_3);	break; }
		case IDC_STT_RACK1L2_CH4: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_4);	break; }
		case IDC_STT_RACK1L2_CH5: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_5);	break; }
		case IDC_STT_RACK1L2_CH6: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_6);	break; }
		case IDC_STT_RACK1L2_CH7: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_7);	break; }
		case IDC_STT_RACK1L2_CH8: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_8);	break; }
		case IDC_STT_RACK1L2_CH9: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_9);	break; }
		case IDC_STT_RACK1L2_CH10: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_10);	break; }
		case IDC_STT_RACK1L2_CH11: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_11);	break; }
		case IDC_STT_RACK1L2_CH12: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_12);	break; }
		case IDC_STT_RACK1L2_CH13: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_13);	break; }
		case IDC_STT_RACK1L2_CH14: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_14);	break; }
		case IDC_STT_RACK1L2_CH15: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_15);	break; }
		case IDC_STT_RACK1L2_CH16: {Lf_toggleChUseUnuse(RACK_1, LAYER_2, CH_16);	break; }
		case IDC_STT_RACK1L3_CH1: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_1);	break; }
		case IDC_STT_RACK1L3_CH2: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_2);	break; }
		case IDC_STT_RACK1L3_CH3: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_3);	break; }
		case IDC_STT_RACK1L3_CH4: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_4);	break; }
		case IDC_STT_RACK1L3_CH5: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_5);	break; }
		case IDC_STT_RACK1L3_CH6: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_6);	break; }
		case IDC_STT_RACK1L3_CH7: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_7);	break; }
		case IDC_STT_RACK1L3_CH8: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_8);	break; }
		case IDC_STT_RACK1L3_CH9: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_9);	break; }
		case IDC_STT_RACK1L3_CH10: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_10);	break; }
		case IDC_STT_RACK1L3_CH11: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_11);	break; }
		case IDC_STT_RACK1L3_CH12: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_12);	break; }
		case IDC_STT_RACK1L3_CH13: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_13);	break; }
		case IDC_STT_RACK1L3_CH14: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_14);	break; }
		case IDC_STT_RACK1L3_CH15: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_15);	break; }
		case IDC_STT_RACK1L3_CH16: {Lf_toggleChUseUnuse(RACK_1, LAYER_3, CH_16);	break; }
		case IDC_STT_RACK1L4_CH1: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_1);	break; }
		case IDC_STT_RACK1L4_CH2: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_2);	break; }
		case IDC_STT_RACK1L4_CH3: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_3);	break; }
		case IDC_STT_RACK1L4_CH4: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_4);	break; }
		case IDC_STT_RACK1L4_CH5: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_5);	break; }
		case IDC_STT_RACK1L4_CH6: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_6);	break; }
		case IDC_STT_RACK1L4_CH7: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_7);	break; }
		case IDC_STT_RACK1L4_CH8: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_8);	break; }
		case IDC_STT_RACK1L4_CH9: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_9);	break; }
		case IDC_STT_RACK1L4_CH10: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_10);	break; }
		case IDC_STT_RACK1L4_CH11: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_11);	break; }
		case IDC_STT_RACK1L4_CH12: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_12);	break; }
		case IDC_STT_RACK1L4_CH13: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_13);	break; }
		case IDC_STT_RACK1L4_CH14: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_14);	break; }
		case IDC_STT_RACK1L4_CH15: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_15);	break; }
		case IDC_STT_RACK1L4_CH16: {Lf_toggleChUseUnuse(RACK_1, LAYER_4, CH_16);	break; }
		case IDC_STT_RACK1L5_CH1: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_1);	break; }
		case IDC_STT_RACK1L5_CH2: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_2);	break; }
		case IDC_STT_RACK1L5_CH3: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_3);	break; }
		case IDC_STT_RACK1L5_CH4: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_4);	break; }
		case IDC_STT_RACK1L5_CH5: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_5);	break; }
		case IDC_STT_RACK1L5_CH6: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_6);	break; }
		case IDC_STT_RACK1L5_CH7: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_7);	break; }
		case IDC_STT_RACK1L5_CH8: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_8);	break; }
		case IDC_STT_RACK1L5_CH9: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_9);	break; }
		case IDC_STT_RACK1L5_CH10: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_10);	break; }
		case IDC_STT_RACK1L5_CH11: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_11);	break; }
		case IDC_STT_RACK1L5_CH12: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_12);	break; }
		case IDC_STT_RACK1L5_CH13: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_13);	break; }
		case IDC_STT_RACK1L5_CH14: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_14);	break; }
		case IDC_STT_RACK1L5_CH15: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_15);	break; }
		case IDC_STT_RACK1L5_CH16: {Lf_toggleChUseUnuse(RACK_1, LAYER_5, CH_16);	break; }
		}
	}

	if (m_pChkChSelect[RACK_2]->GetCheck() == TRUE)
	{
		switch (nID)
		{
		case IDC_STT_RACK2L1_CH1: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_1);	break; }
		case IDC_STT_RACK2L1_CH2: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_2);	break; }
		case IDC_STT_RACK2L1_CH3: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_3);	break; }
		case IDC_STT_RACK2L1_CH4: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_4);	break; }
		case IDC_STT_RACK2L1_CH5: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_5);	break; }
		case IDC_STT_RACK2L1_CH6: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_6);	break; }
		case IDC_STT_RACK2L1_CH7: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_7);	break; }
		case IDC_STT_RACK2L1_CH8: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_8);	break; }
		case IDC_STT_RACK2L1_CH9: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_9);	break; }
		case IDC_STT_RACK2L1_CH10: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_10);	break; }
		case IDC_STT_RACK2L1_CH11: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_11);	break; }
		case IDC_STT_RACK2L1_CH12: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_12);	break; }
		case IDC_STT_RACK2L1_CH13: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_13);	break; }
		case IDC_STT_RACK2L1_CH14: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_14);	break; }
		case IDC_STT_RACK2L1_CH15: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_15);	break; }
		case IDC_STT_RACK2L1_CH16: {Lf_toggleChUseUnuse(RACK_2, LAYER_1, CH_16);	break; }
		case IDC_STT_RACK2L2_CH1: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_1);	break; }
		case IDC_STT_RACK2L2_CH2: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_2);	break; }
		case IDC_STT_RACK2L2_CH3: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_3);	break; }
		case IDC_STT_RACK2L2_CH4: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_4);	break; }
		case IDC_STT_RACK2L2_CH5: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_5);	break; }
		case IDC_STT_RACK2L2_CH6: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_6);	break; }
		case IDC_STT_RACK2L2_CH7: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_7);	break; }
		case IDC_STT_RACK2L2_CH8: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_8);	break; }
		case IDC_STT_RACK2L2_CH9: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_9);	break; }
		case IDC_STT_RACK2L2_CH10: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_10);	break; }
		case IDC_STT_RACK2L2_CH11: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_11);	break; }
		case IDC_STT_RACK2L2_CH12: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_12);	break; }
		case IDC_STT_RACK2L2_CH13: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_13);	break; }
		case IDC_STT_RACK2L2_CH14: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_14);	break; }
		case IDC_STT_RACK2L2_CH15: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_15);	break; }
		case IDC_STT_RACK2L2_CH16: {Lf_toggleChUseUnuse(RACK_2, LAYER_2, CH_16);	break; }
		case IDC_STT_RACK2L3_CH1: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_1);	break; }
		case IDC_STT_RACK2L3_CH2: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_2);	break; }
		case IDC_STT_RACK2L3_CH3: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_3);	break; }
		case IDC_STT_RACK2L3_CH4: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_4);	break; }
		case IDC_STT_RACK2L3_CH5: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_5);	break; }
		case IDC_STT_RACK2L3_CH6: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_6);	break; }
		case IDC_STT_RACK2L3_CH7: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_7);	break; }
		case IDC_STT_RACK2L3_CH8: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_8);	break; }
		case IDC_STT_RACK2L3_CH9: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_9);	break; }
		case IDC_STT_RACK2L3_CH10: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_10);	break; }
		case IDC_STT_RACK2L3_CH11: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_11);	break; }
		case IDC_STT_RACK2L3_CH12: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_12);	break; }
		case IDC_STT_RACK2L3_CH13: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_13);	break; }
		case IDC_STT_RACK2L3_CH14: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_14);	break; }
		case IDC_STT_RACK2L3_CH15: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_15);	break; }
		case IDC_STT_RACK2L3_CH16: {Lf_toggleChUseUnuse(RACK_2, LAYER_3, CH_16);	break; }
		case IDC_STT_RACK2L4_CH1: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_1);	break; }
		case IDC_STT_RACK2L4_CH2: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_2);	break; }
		case IDC_STT_RACK2L4_CH3: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_3);	break; }
		case IDC_STT_RACK2L4_CH4: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_4);	break; }
		case IDC_STT_RACK2L4_CH5: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_5);	break; }
		case IDC_STT_RACK2L4_CH6: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_6);	break; }
		case IDC_STT_RACK2L4_CH7: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_7);	break; }
		case IDC_STT_RACK2L4_CH8: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_8);	break; }
		case IDC_STT_RACK2L4_CH9: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_9);	break; }
		case IDC_STT_RACK2L4_CH10: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_10);	break; }
		case IDC_STT_RACK2L4_CH11: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_11);	break; }
		case IDC_STT_RACK2L4_CH12: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_12);	break; }
		case IDC_STT_RACK2L4_CH13: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_13);	break; }
		case IDC_STT_RACK2L4_CH14: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_14);	break; }
		case IDC_STT_RACK2L4_CH15: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_15);	break; }
		case IDC_STT_RACK2L4_CH16: {Lf_toggleChUseUnuse(RACK_2, LAYER_4, CH_16);	break; }
		case IDC_STT_RACK2L5_CH1: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_1);	break; }
		case IDC_STT_RACK2L5_CH2: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_2);	break; }
		case IDC_STT_RACK2L5_CH3: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_3);	break; }
		case IDC_STT_RACK2L5_CH4: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_4);	break; }
		case IDC_STT_RACK2L5_CH5: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_5);	break; }
		case IDC_STT_RACK2L5_CH6: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_6);	break; }
		case IDC_STT_RACK2L5_CH7: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_7);	break; }
		case IDC_STT_RACK2L5_CH8: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_8);	break; }
		case IDC_STT_RACK2L5_CH9: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_9);	break; }
		case IDC_STT_RACK2L5_CH10: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_10);	break; }
		case IDC_STT_RACK2L5_CH11: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_11);	break; }
		case IDC_STT_RACK2L5_CH12: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_12);	break; }
		case IDC_STT_RACK2L5_CH13: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_13);	break; }
		case IDC_STT_RACK2L5_CH14: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_14);	break; }
		case IDC_STT_RACK2L5_CH15: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_15);	break; }
		case IDC_STT_RACK2L5_CH16: {Lf_toggleChUseUnuse(RACK_2, LAYER_5, CH_16);	break; }
		}
	}

	if (m_pChkChSelect[RACK_3]->GetCheck() == TRUE)
	{
		switch (nID)
		{
		case IDC_STT_RACK3L1_CH1: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_1);	break; }
		case IDC_STT_RACK3L1_CH2: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_2);	break; }
		case IDC_STT_RACK3L1_CH3: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_3);	break; }
		case IDC_STT_RACK3L1_CH4: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_4);	break; }
		case IDC_STT_RACK3L1_CH5: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_5);	break; }
		case IDC_STT_RACK3L1_CH6: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_6);	break; }
		case IDC_STT_RACK3L1_CH7: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_7);	break; }
		case IDC_STT_RACK3L1_CH8: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_8);	break; }
		case IDC_STT_RACK3L1_CH9: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_9);	break; }
		case IDC_STT_RACK3L1_CH10: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_10);	break; }
		case IDC_STT_RACK3L1_CH11: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_11);	break; }
		case IDC_STT_RACK3L1_CH12: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_12);	break; }
		case IDC_STT_RACK3L1_CH13: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_13);	break; }
		case IDC_STT_RACK3L1_CH14: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_14);	break; }
		case IDC_STT_RACK3L1_CH15: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_15);	break; }
		case IDC_STT_RACK3L1_CH16: {Lf_toggleChUseUnuse(RACK_3, LAYER_1, CH_16);	break; }
		case IDC_STT_RACK3L2_CH1: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_1);	break; }
		case IDC_STT_RACK3L2_CH2: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_2);	break; }
		case IDC_STT_RACK3L2_CH3: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_3);	break; }
		case IDC_STT_RACK3L2_CH4: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_4);	break; }
		case IDC_STT_RACK3L2_CH5: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_5);	break; }
		case IDC_STT_RACK3L2_CH6: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_6);	break; }
		case IDC_STT_RACK3L2_CH7: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_7);	break; }
		case IDC_STT_RACK3L2_CH8: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_8);	break; }
		case IDC_STT_RACK3L2_CH9: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_9);	break; }
		case IDC_STT_RACK3L2_CH10: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_10);	break; }
		case IDC_STT_RACK3L2_CH11: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_11);	break; }
		case IDC_STT_RACK3L2_CH12: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_12);	break; }
		case IDC_STT_RACK3L2_CH13: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_13);	break; }
		case IDC_STT_RACK3L2_CH14: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_14);	break; }
		case IDC_STT_RACK3L2_CH15: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_15);	break; }
		case IDC_STT_RACK3L2_CH16: {Lf_toggleChUseUnuse(RACK_3, LAYER_2, CH_16);	break; }
		case IDC_STT_RACK3L3_CH1: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_1);	break; }
		case IDC_STT_RACK3L3_CH2: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_2);	break; }
		case IDC_STT_RACK3L3_CH3: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_3);	break; }
		case IDC_STT_RACK3L3_CH4: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_4);	break; }
		case IDC_STT_RACK3L3_CH5: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_5);	break; }
		case IDC_STT_RACK3L3_CH6: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_6);	break; }
		case IDC_STT_RACK3L3_CH7: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_7);	break; }
		case IDC_STT_RACK3L3_CH8: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_8);	break; }
		case IDC_STT_RACK3L3_CH9: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_9);	break; }
		case IDC_STT_RACK3L3_CH10: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_10);	break; }
		case IDC_STT_RACK3L3_CH11: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_11);	break; }
		case IDC_STT_RACK3L3_CH12: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_12);	break; }
		case IDC_STT_RACK3L3_CH13: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_13);	break; }
		case IDC_STT_RACK3L3_CH14: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_14);	break; }
		case IDC_STT_RACK3L3_CH15: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_15);	break; }
		case IDC_STT_RACK3L3_CH16: {Lf_toggleChUseUnuse(RACK_3, LAYER_3, CH_16);	break; }
		case IDC_STT_RACK3L4_CH1: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_1);	break; }
		case IDC_STT_RACK3L4_CH2: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_2);	break; }
		case IDC_STT_RACK3L4_CH3: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_3);	break; }
		case IDC_STT_RACK3L4_CH4: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_4);	break; }
		case IDC_STT_RACK3L4_CH5: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_5);	break; }
		case IDC_STT_RACK3L4_CH6: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_6);	break; }
		case IDC_STT_RACK3L4_CH7: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_7);	break; }
		case IDC_STT_RACK3L4_CH8: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_8);	break; }
		case IDC_STT_RACK3L4_CH9: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_9);	break; }
		case IDC_STT_RACK3L4_CH10: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_10);	break; }
		case IDC_STT_RACK3L4_CH11: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_11);	break; }
		case IDC_STT_RACK3L4_CH12: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_12);	break; }
		case IDC_STT_RACK3L4_CH13: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_13);	break; }
		case IDC_STT_RACK3L4_CH14: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_14);	break; }
		case IDC_STT_RACK3L4_CH15: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_15);	break; }
		case IDC_STT_RACK3L4_CH16: {Lf_toggleChUseUnuse(RACK_3, LAYER_4, CH_16);	break; }
		case IDC_STT_RACK3L5_CH1: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_1);	break; }
		case IDC_STT_RACK3L5_CH2: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_2);	break; }
		case IDC_STT_RACK3L5_CH3: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_3);	break; }
		case IDC_STT_RACK3L5_CH4: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_4);	break; }
		case IDC_STT_RACK3L5_CH5: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_5);	break; }
		case IDC_STT_RACK3L5_CH6: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_6);	break; }
		case IDC_STT_RACK3L5_CH7: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_7);	break; }
		case IDC_STT_RACK3L5_CH8: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_8);	break; }
		case IDC_STT_RACK3L5_CH9: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_9);	break; }
		case IDC_STT_RACK3L5_CH10: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_10);	break; }
		case IDC_STT_RACK3L5_CH11: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_11);	break; }
		case IDC_STT_RACK3L5_CH12: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_12);	break; }
		case IDC_STT_RACK3L5_CH13: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_13);	break; }
		case IDC_STT_RACK3L5_CH14: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_14);	break; }
		case IDC_STT_RACK3L5_CH15: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_15);	break; }
		case IDC_STT_RACK3L5_CH16: {Lf_toggleChUseUnuse(RACK_3, LAYER_5, CH_16);	break; }
		}
	}

	if (m_pChkChSelect[RACK_4]->GetCheck() == TRUE)
	{
		switch (nID)
		{
		case IDC_STT_RACK4L1_CH1: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_1);	break; }
		case IDC_STT_RACK4L1_CH2: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_2);	break; }
		case IDC_STT_RACK4L1_CH3: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_3);	break; }
		case IDC_STT_RACK4L1_CH4: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_4);	break; }
		case IDC_STT_RACK4L1_CH5: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_5);	break; }
		case IDC_STT_RACK4L1_CH6: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_6);	break; }
		case IDC_STT_RACK4L1_CH7: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_7);	break; }
		case IDC_STT_RACK4L1_CH8: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_8);	break; }
		case IDC_STT_RACK4L1_CH9: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_9);	break; }
		case IDC_STT_RACK4L1_CH10: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_10);	break; }
		case IDC_STT_RACK4L1_CH11: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_11);	break; }
		case IDC_STT_RACK4L1_CH12: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_12);	break; }
		case IDC_STT_RACK4L1_CH13: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_13);	break; }
		case IDC_STT_RACK4L1_CH14: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_14);	break; }
		case IDC_STT_RACK4L1_CH15: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_15);	break; }
		case IDC_STT_RACK4L1_CH16: {Lf_toggleChUseUnuse(RACK_4, LAYER_1, CH_16);	break; }
		case IDC_STT_RACK4L2_CH1: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_1);	break; }
		case IDC_STT_RACK4L2_CH2: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_2);	break; }
		case IDC_STT_RACK4L2_CH3: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_3);	break; }
		case IDC_STT_RACK4L2_CH4: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_4);	break; }
		case IDC_STT_RACK4L2_CH5: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_5);	break; }
		case IDC_STT_RACK4L2_CH6: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_6);	break; }
		case IDC_STT_RACK4L2_CH7: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_7);	break; }
		case IDC_STT_RACK4L2_CH8: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_8);	break; }
		case IDC_STT_RACK4L2_CH9: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_9);	break; }
		case IDC_STT_RACK4L2_CH10: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_10);	break; }
		case IDC_STT_RACK4L2_CH11: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_11);	break; }
		case IDC_STT_RACK4L2_CH12: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_12);	break; }
		case IDC_STT_RACK4L2_CH13: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_13);	break; }
		case IDC_STT_RACK4L2_CH14: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_14);	break; }
		case IDC_STT_RACK4L2_CH15: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_15);	break; }
		case IDC_STT_RACK4L2_CH16: {Lf_toggleChUseUnuse(RACK_4, LAYER_2, CH_16);	break; }
		case IDC_STT_RACK4L3_CH1: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_1);	break; }
		case IDC_STT_RACK4L3_CH2: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_2);	break; }
		case IDC_STT_RACK4L3_CH3: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_3);	break; }
		case IDC_STT_RACK4L3_CH4: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_4);	break; }
		case IDC_STT_RACK4L3_CH5: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_5);	break; }
		case IDC_STT_RACK4L3_CH6: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_6);	break; }
		case IDC_STT_RACK4L3_CH7: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_7);	break; }
		case IDC_STT_RACK4L3_CH8: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_8);	break; }
		case IDC_STT_RACK4L3_CH9: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_9);	break; }
		case IDC_STT_RACK4L3_CH10: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_10);	break; }
		case IDC_STT_RACK4L3_CH11: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_11);	break; }
		case IDC_STT_RACK4L3_CH12: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_12);	break; }
		case IDC_STT_RACK4L3_CH13: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_13);	break; }
		case IDC_STT_RACK4L3_CH14: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_14);	break; }
		case IDC_STT_RACK4L3_CH15: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_15);	break; }
		case IDC_STT_RACK4L3_CH16: {Lf_toggleChUseUnuse(RACK_4, LAYER_3, CH_16);	break; }
		case IDC_STT_RACK4L4_CH1: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_1);	break; }
		case IDC_STT_RACK4L4_CH2: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_2);	break; }
		case IDC_STT_RACK4L4_CH3: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_3);	break; }
		case IDC_STT_RACK4L4_CH4: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_4);	break; }
		case IDC_STT_RACK4L4_CH5: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_5);	break; }
		case IDC_STT_RACK4L4_CH6: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_6);	break; }
		case IDC_STT_RACK4L4_CH7: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_7);	break; }
		case IDC_STT_RACK4L4_CH8: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_8);	break; }
		case IDC_STT_RACK4L4_CH9: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_9);	break; }
		case IDC_STT_RACK4L4_CH10: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_10);	break; }
		case IDC_STT_RACK4L4_CH11: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_11);	break; }
		case IDC_STT_RACK4L4_CH12: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_12);	break; }
		case IDC_STT_RACK4L4_CH13: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_13);	break; }
		case IDC_STT_RACK4L4_CH14: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_14);	break; }
		case IDC_STT_RACK4L4_CH15: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_15);	break; }
		case IDC_STT_RACK4L4_CH16: {Lf_toggleChUseUnuse(RACK_4, LAYER_4, CH_16);	break; }
		case IDC_STT_RACK4L5_CH1: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_1);	break; }
		case IDC_STT_RACK4L5_CH2: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_2);	break; }
		case IDC_STT_RACK4L5_CH3: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_3);	break; }
		case IDC_STT_RACK4L5_CH4: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_4);	break; }
		case IDC_STT_RACK4L5_CH5: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_5);	break; }
		case IDC_STT_RACK4L5_CH6: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_6);	break; }
		case IDC_STT_RACK4L5_CH7: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_7);	break; }
		case IDC_STT_RACK4L5_CH8: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_8);	break; }
		case IDC_STT_RACK4L5_CH9: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_9);	break; }
		case IDC_STT_RACK4L5_CH10: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_10);	break; }
		case IDC_STT_RACK4L5_CH11: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_11);	break; }
		case IDC_STT_RACK4L5_CH12: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_12);	break; }
		case IDC_STT_RACK4L5_CH13: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_13);	break; }
		case IDC_STT_RACK4L5_CH14: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_14);	break; }
		case IDC_STT_RACK4L5_CH15: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_15);	break; }
		case IDC_STT_RACK4L5_CH16: {Lf_toggleChUseUnuse(RACK_4, LAYER_5, CH_16);	break; }
		}
	}

	if (m_pChkChSelect[RACK_5]->GetCheck() == TRUE)
	{
		switch (nID)
		{
		case IDC_STT_RACK5L1_CH1: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_1);	break; }
		case IDC_STT_RACK5L1_CH2: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_2);	break; }
		case IDC_STT_RACK5L1_CH3: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_3);	break; }
		case IDC_STT_RACK5L1_CH4: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_4);	break; }
		case IDC_STT_RACK5L1_CH5: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_5);	break; }
		case IDC_STT_RACK5L1_CH6: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_6);	break; }
		case IDC_STT_RACK5L1_CH7: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_7);	break; }
		case IDC_STT_RACK5L1_CH8: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_8);	break; }
		case IDC_STT_RACK5L1_CH9: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_9);	break; }
		case IDC_STT_RACK5L1_CH10: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_10);	break; }
		case IDC_STT_RACK5L1_CH11: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_11);	break; }
		case IDC_STT_RACK5L1_CH12: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_12);	break; }
		case IDC_STT_RACK5L1_CH13: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_13);	break; }
		case IDC_STT_RACK5L1_CH14: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_14);	break; }
		case IDC_STT_RACK5L1_CH15: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_15);	break; }
		case IDC_STT_RACK5L1_CH16: {Lf_toggleChUseUnuse(RACK_5, LAYER_1, CH_16);	break; }
		case IDC_STT_RACK5L2_CH1: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_1);	break; }
		case IDC_STT_RACK5L2_CH2: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_2);	break; }
		case IDC_STT_RACK5L2_CH3: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_3);	break; }
		case IDC_STT_RACK5L2_CH4: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_4);	break; }
		case IDC_STT_RACK5L2_CH5: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_5);	break; }
		case IDC_STT_RACK5L2_CH6: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_6);	break; }
		case IDC_STT_RACK5L2_CH7: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_7);	break; }
		case IDC_STT_RACK5L2_CH8: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_8);	break; }
		case IDC_STT_RACK5L2_CH9: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_9);	break; }
		case IDC_STT_RACK5L2_CH10: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_10);	break; }
		case IDC_STT_RACK5L2_CH11: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_11);	break; }
		case IDC_STT_RACK5L2_CH12: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_12);	break; }
		case IDC_STT_RACK5L2_CH13: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_13);	break; }
		case IDC_STT_RACK5L2_CH14: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_14);	break; }
		case IDC_STT_RACK5L2_CH15: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_15);	break; }
		case IDC_STT_RACK5L2_CH16: {Lf_toggleChUseUnuse(RACK_5, LAYER_2, CH_16);	break; }
		case IDC_STT_RACK5L3_CH1: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_1);	break; }
		case IDC_STT_RACK5L3_CH2: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_2);	break; }
		case IDC_STT_RACK5L3_CH3: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_3);	break; }
		case IDC_STT_RACK5L3_CH4: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_4);	break; }
		case IDC_STT_RACK5L3_CH5: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_5);	break; }
		case IDC_STT_RACK5L3_CH6: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_6);	break; }
		case IDC_STT_RACK5L3_CH7: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_7);	break; }
		case IDC_STT_RACK5L3_CH8: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_8);	break; }
		case IDC_STT_RACK5L3_CH9: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_9);	break; }
		case IDC_STT_RACK5L3_CH10: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_10);	break; }
		case IDC_STT_RACK5L3_CH11: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_11);	break; }
		case IDC_STT_RACK5L3_CH12: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_12);	break; }
		case IDC_STT_RACK5L3_CH13: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_13);	break; }
		case IDC_STT_RACK5L3_CH14: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_14);	break; }
		case IDC_STT_RACK5L3_CH15: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_15);	break; }
		case IDC_STT_RACK5L3_CH16: {Lf_toggleChUseUnuse(RACK_5, LAYER_3, CH_16);	break; }
		case IDC_STT_RACK5L4_CH1: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_1);	break; }
		case IDC_STT_RACK5L4_CH2: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_2);	break; }
		case IDC_STT_RACK5L4_CH3: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_3);	break; }
		case IDC_STT_RACK5L4_CH4: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_4);	break; }
		case IDC_STT_RACK5L4_CH5: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_5);	break; }
		case IDC_STT_RACK5L4_CH6: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_6);	break; }
		case IDC_STT_RACK5L4_CH7: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_7);	break; }
		case IDC_STT_RACK5L4_CH8: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_8);	break; }
		case IDC_STT_RACK5L4_CH9: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_9);	break; }
		case IDC_STT_RACK5L4_CH10: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_10);	break; }
		case IDC_STT_RACK5L4_CH11: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_11);	break; }
		case IDC_STT_RACK5L4_CH12: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_12);	break; }
		case IDC_STT_RACK5L4_CH13: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_13);	break; }
		case IDC_STT_RACK5L4_CH14: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_14);	break; }
		case IDC_STT_RACK5L4_CH15: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_15);	break; }
		case IDC_STT_RACK5L4_CH16: {Lf_toggleChUseUnuse(RACK_5, LAYER_4, CH_16);	break; }
		case IDC_STT_RACK5L5_CH1: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_1);	break; }
		case IDC_STT_RACK5L5_CH2: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_2);	break; }
		case IDC_STT_RACK5L5_CH3: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_3);	break; }
		case IDC_STT_RACK5L5_CH4: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_4);	break; }
		case IDC_STT_RACK5L5_CH5: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_5);	break; }
		case IDC_STT_RACK5L5_CH6: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_6);	break; }
		case IDC_STT_RACK5L5_CH7: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_7);	break; }
		case IDC_STT_RACK5L5_CH8: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_8);	break; }
		case IDC_STT_RACK5L5_CH9: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_9);	break; }
		case IDC_STT_RACK5L5_CH10: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_10);	break; }
		case IDC_STT_RACK5L5_CH11: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_11);	break; }
		case IDC_STT_RACK5L5_CH12: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_12);	break; }
		case IDC_STT_RACK5L5_CH13: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_13);	break; }
		case IDC_STT_RACK5L5_CH14: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_14);	break; }
		case IDC_STT_RACK5L5_CH15: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_15);	break; }
		case IDC_STT_RACK5L5_CH16: {Lf_toggleChUseUnuse(RACK_5, LAYER_5, CH_16);	break; }
		}
	}

	if (m_pChkChSelect[RACK_6]->GetCheck() == TRUE)
	{
		switch (nID)
		{
		case IDC_STT_RACK6L1_CH1: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_1);	break; }
		case IDC_STT_RACK6L1_CH2: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_2);	break; }
		case IDC_STT_RACK6L1_CH3: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_3);	break; }
		case IDC_STT_RACK6L1_CH4: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_4);	break; }
		case IDC_STT_RACK6L1_CH5: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_5);	break; }
		case IDC_STT_RACK6L1_CH6: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_6);	break; }
		case IDC_STT_RACK6L1_CH7: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_7);	break; }
		case IDC_STT_RACK6L1_CH8: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_8);	break; }
		case IDC_STT_RACK6L1_CH9: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_9);	break; }
		case IDC_STT_RACK6L1_CH10: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_10);	break; }
		case IDC_STT_RACK6L1_CH11: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_11);	break; }
		case IDC_STT_RACK6L1_CH12: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_12);	break; }
		case IDC_STT_RACK6L1_CH13: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_13);	break; }
		case IDC_STT_RACK6L1_CH14: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_14);	break; }
		case IDC_STT_RACK6L1_CH15: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_15);	break; }
		case IDC_STT_RACK6L1_CH16: {Lf_toggleChUseUnuse(RACK_6, LAYER_1, CH_16);	break; }
		case IDC_STT_RACK6L2_CH1: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_1);	break; }
		case IDC_STT_RACK6L2_CH2: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_2);	break; }
		case IDC_STT_RACK6L2_CH3: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_3);	break; }
		case IDC_STT_RACK6L2_CH4: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_4);	break; }
		case IDC_STT_RACK6L2_CH5: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_5);	break; }
		case IDC_STT_RACK6L2_CH6: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_6);	break; }
		case IDC_STT_RACK6L2_CH7: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_7);	break; }
		case IDC_STT_RACK6L2_CH8: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_8);	break; }
		case IDC_STT_RACK6L2_CH9: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_9);	break; }
		case IDC_STT_RACK6L2_CH10: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_10);	break; }
		case IDC_STT_RACK6L2_CH11: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_11);	break; }
		case IDC_STT_RACK6L2_CH12: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_12);	break; }
		case IDC_STT_RACK6L2_CH13: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_13);	break; }
		case IDC_STT_RACK6L2_CH14: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_14);	break; }
		case IDC_STT_RACK6L2_CH15: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_15);	break; }
		case IDC_STT_RACK6L2_CH16: {Lf_toggleChUseUnuse(RACK_6, LAYER_2, CH_16);	break; }
		case IDC_STT_RACK6L3_CH1: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_1);	break; }
		case IDC_STT_RACK6L3_CH2: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_2);	break; }
		case IDC_STT_RACK6L3_CH3: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_3);	break; }
		case IDC_STT_RACK6L3_CH4: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_4);	break; }
		case IDC_STT_RACK6L3_CH5: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_5);	break; }
		case IDC_STT_RACK6L3_CH6: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_6);	break; }
		case IDC_STT_RACK6L3_CH7: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_7);	break; }
		case IDC_STT_RACK6L3_CH8: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_8);	break; }
		case IDC_STT_RACK6L3_CH9: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_9);	break; }
		case IDC_STT_RACK6L3_CH10: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_10);	break; }
		case IDC_STT_RACK6L3_CH11: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_11);	break; }
		case IDC_STT_RACK6L3_CH12: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_12);	break; }
		case IDC_STT_RACK6L3_CH13: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_13);	break; }
		case IDC_STT_RACK6L3_CH14: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_14);	break; }
		case IDC_STT_RACK6L3_CH15: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_15);	break; }
		case IDC_STT_RACK6L3_CH16: {Lf_toggleChUseUnuse(RACK_6, LAYER_3, CH_16);	break; }
		case IDC_STT_RACK6L4_CH1: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_1);	break; }
		case IDC_STT_RACK6L4_CH2: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_2);	break; }
		case IDC_STT_RACK6L4_CH3: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_3);	break; }
		case IDC_STT_RACK6L4_CH4: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_4);	break; }
		case IDC_STT_RACK6L4_CH5: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_5);	break; }
		case IDC_STT_RACK6L4_CH6: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_6);	break; }
		case IDC_STT_RACK6L4_CH7: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_7);	break; }
		case IDC_STT_RACK6L4_CH8: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_8);	break; }
		case IDC_STT_RACK6L4_CH9: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_9);	break; }
		case IDC_STT_RACK6L4_CH10: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_10);	break; }
		case IDC_STT_RACK6L4_CH11: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_11);	break; }
		case IDC_STT_RACK6L4_CH12: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_12);	break; }
		case IDC_STT_RACK6L4_CH13: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_13);	break; }
		case IDC_STT_RACK6L4_CH14: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_14);	break; }
		case IDC_STT_RACK6L4_CH15: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_15);	break; }
		case IDC_STT_RACK6L4_CH16: {Lf_toggleChUseUnuse(RACK_6, LAYER_4, CH_16);	break; }
		case IDC_STT_RACK6L5_CH1: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_1);	break; }
		case IDC_STT_RACK6L5_CH2: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_2);	break; }
		case IDC_STT_RACK6L5_CH3: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_3);	break; }
		case IDC_STT_RACK6L5_CH4: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_4);	break; }
		case IDC_STT_RACK6L5_CH5: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_5);	break; }
		case IDC_STT_RACK6L5_CH6: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_6);	break; }
		case IDC_STT_RACK6L5_CH7: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_7);	break; }
		case IDC_STT_RACK6L5_CH8: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_8);	break; }
		case IDC_STT_RACK6L5_CH9: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_9);	break; }
		case IDC_STT_RACK6L5_CH10: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_10);	break; }
		case IDC_STT_RACK6L5_CH11: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_11);	break; }
		case IDC_STT_RACK6L5_CH12: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_12);	break; }
		case IDC_STT_RACK6L5_CH13: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_13);	break; }
		case IDC_STT_RACK6L5_CH14: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_14);	break; }
		case IDC_STT_RACK6L5_CH15: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_15);	break; }
		case IDC_STT_RACK6L5_CH16: {Lf_toggleChUseUnuse(RACK_6, LAYER_5, CH_16);	break; }
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
void CHseAgingDlg::Lf_InitLocalValue()
{
	//////////////////////////////////////////////////////////////////////////////////////
	// Local 변수 초기화
	//////////////////////////////////////////////////////////////////////////////////////
	m_nMeasureCnt = 0;
	m_nMeasureMode = 0;
	memset(m_nAgnOutFlag, 0, sizeof(m_nAgnOutFlag));
	memset(m_nAgingStart, 0, sizeof(m_nAgingStart));

	if (lpInspWorkInfo->m_nConnectInfo[CONNECT_MES] == TRUE)
	{
		GetDlgItem(IDC_STT_CONNECT_INFO_MES)->SetWindowText(_T("[MES]ON-LINE"));
	}
	else
	{
		GetDlgItem(IDC_STT_CONNECT_INFO_MES)->SetWindowText(_T("[MES]OFF-LINE"));
	}
	GetDlgItem(IDC_STT_MES_USER_ID)->SetWindowText(m_pApp->m_sUserID);
	GetDlgItem(IDC_STT_MES_USER_ID2)->SetWindowText(m_pApp->m_sUserName);

	//////////////////////////////////////////////////////////////////////////////////////
	// Control ID 초기화 및 변수 할당
	//////////////////////////////////////////////////////////////////////////////////////
	int i = 0, rack = 0, ly = 0, ch = 0;

	m_pDoorState[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_DOOR1);
	m_pDoorState[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_DOOR2);
	m_pDoorState[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_DOOR3);
	m_pDoorState[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_DOOR4);
	m_pDoorState[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_DOOR5);
	m_pDoorState[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_DOOR6);

	i = 0;
	m_pCmbMaModel[i++] = (CComboBox*)GetDlgItem(IDC_CMB_MA_MODEL_RACK1);
	m_pCmbMaModel[i++] = (CComboBox*)GetDlgItem(IDC_CMB_MA_MODEL_RACK2);
	m_pCmbMaModel[i++] = (CComboBox*)GetDlgItem(IDC_CMB_MA_MODEL_RACK3);
	m_pCmbMaModel[i++] = (CComboBox*)GetDlgItem(IDC_CMB_MA_MODEL_RACK4);
	m_pCmbMaModel[i++] = (CComboBox*)GetDlgItem(IDC_CMB_MA_MODEL_RACK5);
	m_pCmbMaModel[i++] = (CComboBox*)GetDlgItem(IDC_CMB_MA_MODEL_RACK6);

	i = 0;
	m_pBtnAgingStart[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_START_RACK1);
	m_pBtnAgingStart[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_START_RACK2);
	m_pBtnAgingStart[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_START_RACK3);
	m_pBtnAgingStart[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_START_RACK4);
	m_pBtnAgingStart[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_START_RACK5);
	m_pBtnAgingStart[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_START_RACK6);

	i = 0;
	m_pBtnAgingStop[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_STOP_RACK1);
	m_pBtnAgingStop[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_STOP_RACK2);
	m_pBtnAgingStop[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_STOP_RACK3);
	m_pBtnAgingStop[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_STOP_RACK4);
	m_pBtnAgingStop[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_STOP_RACK5);
	m_pBtnAgingStop[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_STOP_RACK6);

	i = 0;
	m_pBtnAgingFusing[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_FUSING_RACK1);
	m_pBtnAgingFusing[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_FUSING_RACK2);
	m_pBtnAgingFusing[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_FUSING_RACK3);
	m_pBtnAgingFusing[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_FUSING_RACK4);
	m_pBtnAgingFusing[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_FUSING_RACK5);
	m_pBtnAgingFusing[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_FUSING_RACK6);

	i = 0;
	m_pBtnChUseUnuseSet[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_CH_SET_RACK1);
	m_pBtnChUseUnuseSet[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_CH_SET_RACK2);
	m_pBtnChUseUnuseSet[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_CH_SET_RACK3);
	m_pBtnChUseUnuseSet[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_CH_SET_RACK4);
	m_pBtnChUseUnuseSet[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_CH_SET_RACK5);
	m_pBtnChUseUnuseSet[i++] = (CMFCButton*)GetDlgItem(IDC_MBC_MA_CH_SET_RACK6);

	i = 0;
	m_pCtrMaProgress[i++] = (CProgressCtrl*)GetDlgItem(IDC_CTR_MA_PROGRESS_RACK1);
	m_pCtrMaProgress[i++] = (CProgressCtrl*)GetDlgItem(IDC_CTR_MA_PROGRESS_RACK2);
	m_pCtrMaProgress[i++] = (CProgressCtrl*)GetDlgItem(IDC_CTR_MA_PROGRESS_RACK3);
	m_pCtrMaProgress[i++] = (CProgressCtrl*)GetDlgItem(IDC_CTR_MA_PROGRESS_RACK4);
	m_pCtrMaProgress[i++] = (CProgressCtrl*)GetDlgItem(IDC_CTR_MA_PROGRESS_RACK5);
	m_pCtrMaProgress[i++] = (CProgressCtrl*)GetDlgItem(IDC_CTR_MA_PROGRESS_RACK6);

	i = 0;
	m_pSttMaSetTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_SET_TIME_RACK1);
	m_pSttMaSetTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_SET_TIME_RACK2);
	m_pSttMaSetTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_SET_TIME_RACK3);
	m_pSttMaSetTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_SET_TIME_RACK4);
	m_pSttMaSetTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_SET_TIME_RACK5);
	m_pSttMaSetTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_SET_TIME_RACK6);

	i = 0;
	m_pSttMaRunTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_RUN_TIME_RACK1);
	m_pSttMaRunTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_RUN_TIME_RACK2);
	m_pSttMaRunTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_RUN_TIME_RACK3);
	m_pSttMaRunTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_RUN_TIME_RACK4);
	m_pSttMaRunTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_RUN_TIME_RACK5);
	m_pSttMaRunTime[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_RUN_TIME_RACK6);

	i = 0;
	m_pLstMLog[i++] = (CListBox*)GetDlgItem(IDC_LST_MA_MLOG_RACK1);
	m_pLstMLog[i++] = (CListBox*)GetDlgItem(IDC_LST_MA_MLOG_RACK2);
	m_pLstMLog[i++] = (CListBox*)GetDlgItem(IDC_LST_MA_MLOG_RACK3);
	m_pLstMLog[i++] = (CListBox*)GetDlgItem(IDC_LST_MA_MLOG_RACK4);
	m_pLstMLog[i++] = (CListBox*)GetDlgItem(IDC_LST_MA_MLOG_RACK5);
	m_pLstMLog[i++] = (CListBox*)GetDlgItem(IDC_LST_MA_MLOG_RACK6);

	rack = RACK_1;
	ly = LAYER_1;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L1_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L1_CH16;


	rack = RACK_1;
	ly = LAYER_2;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L2_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L2_CH16;

	rack = RACK_1;
	ly = LAYER_3;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L3_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L3_CH16;

	rack = RACK_1;
	ly = LAYER_4;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L4_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L4_CH16;

	rack = RACK_1;
	ly = LAYER_5;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK1L5_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK1L5_CH16;

	rack = RACK_2;
	ly = LAYER_1;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L1_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L1_CH16;

	rack = RACK_2;
	ly = LAYER_2;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L2_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L2_CH16;

	rack = RACK_2;
	ly = LAYER_3;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L3_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L3_CH16;

	rack = RACK_2;
	ly = LAYER_4;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L4_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L4_CH16;

	rack = RACK_2;
	ly = LAYER_5;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK2L5_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK2L5_CH16;

	rack = RACK_3;
	ly = LAYER_1;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L1_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L1_CH16;

	rack = RACK_3;
	ly = LAYER_2;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L2_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L2_CH16;

	rack = RACK_3;
	ly = LAYER_3;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L3_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L3_CH16;

	rack = RACK_3;
	ly = LAYER_4;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L4_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L4_CH16;

	rack = RACK_3;
	ly = LAYER_5;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK3L5_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK3L5_CH16;

	rack = RACK_4;
	ly = LAYER_1;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L1_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L1_CH16;

	rack = RACK_4;
	ly = LAYER_2;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L2_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L2_CH16;


	rack = RACK_4;
	ly = LAYER_3;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L3_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L3_CH16;

	rack = RACK_4;
	ly = LAYER_4;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L4_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L4_CH16;

	rack = RACK_4;
	ly = LAYER_5;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK4L5_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK4L5_CH16;

	rack = RACK_5;
	ly = LAYER_1;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L1_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L1_CH16;

	rack = RACK_5;
	ly = LAYER_2;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L2_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L2_CH16;

	rack = RACK_5;
	ly = LAYER_3;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L3_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L3_CH16;

	rack = RACK_5;
	ly = LAYER_4;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L4_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L4_CH16;

	rack = RACK_5;
	ly = LAYER_5;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK5L5_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK5L5_CH16;

	rack = RACK_6;
	ly = LAYER_1;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L1_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L1_CH16;

	rack = RACK_6;
	ly = LAYER_2;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L2_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L2_CH16;

	rack = RACK_6;
	ly = LAYER_3;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L3_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L3_CH16;

	rack = RACK_6;
	ly = LAYER_4;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L4_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L4_CH16;

	rack = RACK_6;
	ly = LAYER_5;
	ch = 0;
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH1);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH2);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH3);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH4);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH5);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH6);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH7);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH8);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH9);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH10);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH11);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH12);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH13);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH14);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH15);
	m_pSttRackState[rack][ly][ch++] = (CStatic*)GetDlgItem(IDC_STT_RACK6L5_CH16);
	ch = 0;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH1;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH2;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH3;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH4;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH5;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH6;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH7;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH8;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH9;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH10;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH11;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH12;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH13;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH14;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH15;
	m_pSttRackCtrlID[rack][ly][ch++] = IDC_STT_RACK6L5_CH16;

	i = 0;
	m_pSttTempInfo[i++] = (CStatic*)GetDlgItem(IDC_STT_TEMP_SENSOR1_V);
	m_pSttTempInfo[i++] = (CStatic*)GetDlgItem(IDC_STT_TEMP_SENSOR2_V);
	m_pSttTempInfo[i++] = (CStatic*)GetDlgItem(IDC_STT_TEMP_SENSOR3_V);
	m_pSttTempInfo[i++] = (CStatic*)GetDlgItem(IDC_STT_TEMP_SENSOR4_V);
	m_pSttTempInfo[i++] = (CStatic*)GetDlgItem(IDC_STT_TEMP_SENSOR5_V);
	m_pSttTempInfo[i++] = (CStatic*)GetDlgItem(IDC_STT_TEMP_SENSOR6_V);

	i = 0;
	m_pChkChSelect[i++] = (CButton*)GetDlgItem(IDC_CHK_MA_SELECT_RACK1);
	m_pChkChSelect[i++] = (CButton*)GetDlgItem(IDC_CHK_MA_SELECT_RACK2);
	m_pChkChSelect[i++] = (CButton*)GetDlgItem(IDC_CHK_MA_SELECT_RACK3);
	m_pChkChSelect[i++] = (CButton*)GetDlgItem(IDC_CHK_MA_SELECT_RACK4);
	m_pChkChSelect[i++] = (CButton*)GetDlgItem(IDC_CHK_MA_SELECT_RACK5);
	m_pChkChSelect[i++] = (CButton*)GetDlgItem(IDC_CHK_MA_SELECT_RACK6);

	i = 0;
	m_pSttFWVersion[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_FW_VER_RACK1);
	m_pSttFWVersion[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_FW_VER_RACK2);
	m_pSttFWVersion[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_FW_VER_RACK3);
	m_pSttFWVersion[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_FW_VER_RACK4);
	m_pSttFWVersion[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_FW_VER_RACK5);
	m_pSttFWVersion[i++] = (CStatic*)GetDlgItem(IDC_STT_MA_FW_VER_RACK6);
}

void CHseAgingDlg::Lf_InitFontset()
{
	m_Font[0].CreateFont(150, 70, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[1].CreateFont(80, 35, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[2].CreateFont(50, 20, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[3].CreateFont(23, 10, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_STT_MA_CHAMBER_NO)->SetFont(&m_Font[3]);
	GetDlgItem(IDC_MBT_MA_BUZZ_OFF)->SetFont(&m_Font[3]);

	m_Font[4].CreateFont(21, 9, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);

	m_Font[5].CreateFont(19, 8, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_STT_MA_SET_TIME_RACK1)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_SET_TIME_RACK2)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_SET_TIME_RACK3)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_SET_TIME_RACK4)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_SET_TIME_RACK5)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_SET_TIME_RACK6)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_RUN_TIME_RACK1)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_RUN_TIME_RACK2)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_RUN_TIME_RACK3)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_RUN_TIME_RACK4)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_RUN_TIME_RACK5)->SetFont(&m_Font[5]);
	GetDlgItem(IDC_STT_MA_RUN_TIME_RACK6)->SetFont(&m_Font[5]);


	m_Font[6].CreateFont(17, 7, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_STT_MA_RACK1)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_RACK2)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_RACK3)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_RACK4)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_RACK5)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_RACK6)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_TEMP_SENSOR1_V)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_TEMP_SENSOR2_V)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_TEMP_SENSOR3_V)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_TEMP_SENSOR4_V)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_TEMP_SENSOR5_V)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_TEMP_SENSOR6_V)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_DOOR1)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_DOOR2)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_DOOR3)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_DOOR4)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_DOOR5)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_MA_DOOR6)->SetFont(&m_Font[6]);

	GetDlgItem(IDC_STT_CONNECT_INFO_PG)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_CONNECT_INFO_DIO)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_CONNECT_INFO_TEMP)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_CONNECT_INFO_BARCODE)->SetFont(&m_Font[6]);
	GetDlgItem(IDC_STT_CONNECT_INFO_MES)->SetFont(&m_Font[6]);


	m_Font[7].CreateFont(15, 6, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	GetDlgItem(IDC_LST_MA_MLOG_RACK1)->SetFont(&m_Font[7]);
	GetDlgItem(IDC_LST_MA_MLOG_RACK2)->SetFont(&m_Font[7]);
	GetDlgItem(IDC_LST_MA_MLOG_RACK3)->SetFont(&m_Font[7]);
	GetDlgItem(IDC_LST_MA_MLOG_RACK4)->SetFont(&m_Font[7]);
	GetDlgItem(IDC_LST_MA_MLOG_RACK5)->SetFont(&m_Font[7]);
	GetDlgItem(IDC_LST_MA_MLOG_RACK6)->SetFont(&m_Font[7]);

	m_Font[8].CreateFont(13, 5, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);


	m_Font[9].CreateFont(11, 4, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_FONT);
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				m_pSttRackState[rack][layer][ch]->SetFont(&m_Font[9]);
			}
		}
	}
}

void CHseAgingDlg::Lf_InitColorBrush()
{
	m_Brush[COLOR_IDX_USER_BACKGROUND].CreateSolidBrush(COLOR_USER_BACKGROUND);
	m_Brush[COLOR_IDX_BLACK].CreateSolidBrush(COLOR_BLACK);
	m_Brush[COLOR_IDX_WHITE].CreateSolidBrush(COLOR_WHITE);
	m_Brush[COLOR_IDX_RED].CreateSolidBrush(COLOR_RED);
	m_Brush[COLOR_IDX_RED128].CreateSolidBrush(COLOR_RED128);
	m_Brush[COLOR_IDX_GREEN].CreateSolidBrush(COLOR_GREEN);
	m_Brush[COLOR_IDX_GREEN128].CreateSolidBrush(COLOR_GREEN128);
	m_Brush[COLOR_IDX_BLUE].CreateSolidBrush(COLOR_BLUE);
	m_Brush[COLOR_IDX_BLUE128].CreateSolidBrush(COLOR_BLUE128);
	m_Brush[COLOR_IDX_ORANGE].CreateSolidBrush(COLOR_ORANGE);
	m_Brush[COLOR_IDX_YELLOW].CreateSolidBrush(COLOR_YELLOW);
	m_Brush[COLOR_IDX_MAGENTA].CreateSolidBrush(COLOR_MAGENTA);
	m_Brush[COLOR_IDX_VERDANT2].CreateSolidBrush(COLOR_VERDANT2);
	m_Brush[COLOR_IDX_SKYBLUE].CreateSolidBrush(COLOR_SKYBLUE);
	m_Brush[COLOR_IDX_JADEBLUE].CreateSolidBrush(COLOR_JADEBLUE);
	m_Brush[COLOR_IDX_JADEGREEN].CreateSolidBrush(COLOR_JADEGREEN);
	m_Brush[COLOR_IDX_BLUISH].CreateSolidBrush(COLOR_BLUISH);
	m_Brush[COLOR_IDX_PURPLE].CreateSolidBrush(COLOR_PURPLE);
	m_Brush[COLOR_IDX_LIGHT_GREEN].CreateSolidBrush(COLOR_LIGHT_GREEN);
	m_Brush[COLOR_IDX_LIGHT_RED].CreateSolidBrush(COLOR_LIGHT_RED);
	m_Brush[COLOR_IDX_LIGHT_YELLOW].CreateSolidBrush(COLOR_LIGHT_YELLOW);
	m_Brush[COLOR_IDX_LIGHT_ORANGE].CreateSolidBrush(COLOR_LIGHT_ORANGE);
	m_Brush[COLOR_IDX_DARK_RED].CreateSolidBrush(COLOR_DARK_RED);
	m_Brush[COLOR_IDX_DARK_GREEN].CreateSolidBrush(COLOR_DARK_GREEN);
	m_Brush[COLOR_IDX_DARK_BLUE].CreateSolidBrush(COLOR_DARK_BLUE);
	m_Brush[COLOR_IDX_DARK_MAGENTA].CreateSolidBrush(COLOR_DARK_MAGENTA);
	m_Brush[COLOR_IDX_DARK_ORANGE].CreateSolidBrush(COLOR_DARK_ORANGE);
	m_Brush[COLOR_IDX_DARK_YELLOW].CreateSolidBrush(COLOR_DARK_YELLOW);
	m_Brush[COLOR_IDX_GRAY96].CreateSolidBrush(COLOR_GRAY96);
	m_Brush[COLOR_IDX_GRAY128].CreateSolidBrush(COLOR_GRAY128);
	m_Brush[COLOR_IDX_GRAY159].CreateSolidBrush(COLOR_GRAY159);
	m_Brush[COLOR_IDX_GRAY192].CreateSolidBrush(COLOR_GRAY192);
	m_Brush[COLOR_IDX_GRAY224].CreateSolidBrush(COLOR_GRAY224);
	m_Brush[COLOR_IDX_GRAY240].CreateSolidBrush(COLOR_GRAY240);
}


void CHseAgingDlg::Lf_InitButtonIcon()
{
	m_btnIconUser.LoadBitmaps(IDB_BMP_USER, IDB_BMP_USER, IDB_BMP_USER, IDB_BMP_USER);
	m_btnIconUser.SizeToContent();
	m_btnIconMonitoring.LoadBitmaps(IDB_BMP_MONITORING, IDB_BMP_MONITORING, IDB_BMP_MONITORING, IDB_BMP_MONITORING);
	m_btnIconMonitoring.SizeToContent();
	m_btnIconModel.LoadBitmaps(IDB_BMP_MODEL, IDB_BMP_MODEL, IDB_BMP_MODEL, IDB_BMP_MODEL);
	m_btnIconModel.SizeToContent();
	m_btnIconPIDInput.LoadBitmaps(IDB_BMP_PID_INPUT, IDB_BMP_PID_INPUT, IDB_BMP_PID_INPUT, IDB_BMP_PID_INPUT);
	m_btnIconPIDInput.SizeToContent();
	m_btnIconFirmware.LoadBitmaps(IDB_BMP_FIRMWARE, IDB_BMP_FIRMWARE, IDB_BMP_FIRMWARE, IDB_BMP_FIRMWARE);
	m_btnIconFirmware.SizeToContent();
	m_btnIconSystem.LoadBitmaps(IDB_BMP_SYSTEM, IDB_BMP_SYSTEM, IDB_BMP_SYSTEM, IDB_BMP_SYSTEM);
	m_btnIconSystem.SizeToContent();
	m_btnIconExit.LoadBitmaps(IDB_BMP_EXIT, IDB_BMP_EXIT, IDB_BMP_EXIT, IDB_BMP_EXIT);
	m_btnIconExit.SizeToContent();
}

void CHseAgingDlg::Lf_InitDialogDesign()
{
	// X Button 비활성화
	CMenu* p_menu = this->GetSystemMenu(FALSE);
	p_menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

	SetWindowTheme(GetDlgItem(IDC_GRP_MA_CHAMBER_NO)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_GRP_MA_DESCRIPTION)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_GRP_MA_FW_VERSION)->GetSafeHwnd(), L"", L"");

	SetWindowTheme(GetDlgItem(IDC_CHK_MA_SELECT_RACK1)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHK_MA_SELECT_RACK2)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHK_MA_SELECT_RACK3)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHK_MA_SELECT_RACK4)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHK_MA_SELECT_RACK5)->GetSafeHwnd(), L"", L"");
	SetWindowTheme(GetDlgItem(IDC_CHK_MA_SELECT_RACK6)->GetSafeHwnd(), L"", L"");

	m_mbtBuzzOff.EnableWindowsTheming(FALSE);
	m_mbtBuzzOff.SetFaceColor(RGB(236,28,66));
	m_mbtBuzzOff.SetTextColor(COLOR_WHITE);

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		m_pBtnAgingStart[rack]->EnableWindowsTheming(FALSE);
		m_pBtnAgingStart[rack]->SetFaceColor(COLOR_VERDANT2);
		m_pBtnAgingStart[rack]->SetTextColor(COLOR_BLACK);

		m_pBtnAgingStop[rack]->EnableWindowsTheming(FALSE);
		m_pBtnAgingStop[rack]->SetFaceColor(COLOR_LIGHT_RED);
		m_pBtnAgingStop[rack]->SetTextColor(COLOR_BLACK);

		m_pBtnAgingFusing[rack]->EnableWindowsTheming(FALSE);
		m_pBtnAgingFusing[rack]->SetFaceColor(COLOR_LIGHT_BLUE);
		m_pBtnAgingFusing[rack]->SetTextColor(COLOR_BLACK);

		m_pBtnChUseUnuseSet[rack]->EnableWindowsTheming(FALSE);
		m_pBtnChUseUnuseSet[rack]->SetFaceColor(COLOR_LIGHT_RED);
		m_pBtnChUseUnuseSet[rack]->SetTextColor(COLOR_BLACK);

		CRect rect_move, rect_curr;
		m_pBtnAgingStart[rack]->GetWindowRect(rect_move);
		m_pBtnChUseUnuseSet[rack]->GetWindowRect(rect_curr);

		rect_curr.left = rect_move.left - 9;
		rect_curr.right = rect_curr.right - 9;
		rect_curr.top = rect_move.top - 31;
		rect_curr.bottom = rect_move.bottom - 31;
		m_pBtnChUseUnuseSet[rack]->MoveWindow(rect_curr);
	}



	int nWidth = 50;
	m_pApp->Gf_setGradientStatic01(&m_sttTempSensor, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic01(&m_sttConnectInfo, &m_Font[6], FALSE);

	m_pApp->Gf_setGradientStatic02(&m_sttMaMLogRack1, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic02(&m_sttMaMLogRack2, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic02(&m_sttMaMLogRack3, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic02(&m_sttMaMLogRack4, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic02(&m_sttMaMLogRack5, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic02(&m_sttMaMLogRack6, &m_Font[6], FALSE);

	m_pApp->Gf_setGradientStatic04(&m_sttTempSensor1T, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic04(&m_sttTempSensor2T, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic04(&m_sttTempSensor3T, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic04(&m_sttTempSensor4T, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic04(&m_sttTempSensor5T, &m_Font[6], FALSE);
	m_pApp->Gf_setGradientStatic04(&m_sttTempSensor6T, &m_Font[6], FALSE);
}

void CHseAgingDlg::Lf_InitCobmoRackModelList()
{
	CString strfilename = _T("");
	CString strfilepath = _T("");
	WIN32_FIND_DATA wfd;
	HANDLE hSearch;

	strfilepath.Format(_T("./Model/*.ini"));
	hSearch = FindFirstFile(strfilepath, &wfd);

	m_cmbMaModelRack1.ResetContent();
	m_cmbMaModelRack2.ResetContent();
	m_cmbMaModelRack3.ResetContent();
	m_cmbMaModelRack4.ResetContent();
	m_cmbMaModelRack5.ResetContent();
	m_cmbMaModelRack6.ResetContent();
	m_cmbMaModelRack1.AddString(_T("- MODEL LIST -"));
	m_cmbMaModelRack2.AddString(_T("- MODEL LIST -"));
	m_cmbMaModelRack3.AddString(_T("- MODEL LIST -"));
	m_cmbMaModelRack4.AddString(_T("- MODEL LIST -"));
	m_cmbMaModelRack5.AddString(_T("- MODEL LIST -"));
	m_cmbMaModelRack6.AddString(_T("- MODEL LIST -"));
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		if (wfd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		{
			strfilename.Format(_T("%s"), wfd.cFileName);
			strfilename = strfilename.Mid(0, strfilename.GetLength() - 4);
			strfilename.MakeUpper();
			m_cmbMaModelRack1.AddString(strfilename);
			m_cmbMaModelRack2.AddString(strfilename);
			m_cmbMaModelRack3.AddString(strfilename);
			m_cmbMaModelRack4.AddString(strfilename);
			m_cmbMaModelRack5.AddString(strfilename);
			m_cmbMaModelRack6.AddString(strfilename);
		}
		while (FindNextFile(hSearch, &wfd))
		{
			if (wfd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
			{
				strfilename.Format(_T("%s"), wfd.cFileName);
				strfilename = strfilename.Mid(0, strfilename.GetLength() - 4);
				strfilename.MakeUpper();
				m_cmbMaModelRack1.AddString(strfilename);
				m_cmbMaModelRack2.AddString(strfilename);
				m_cmbMaModelRack3.AddString(strfilename);
				m_cmbMaModelRack4.AddString(strfilename);
				m_cmbMaModelRack5.AddString(strfilename);
				m_cmbMaModelRack6.AddString(strfilename);
			}
		}
		FindClose(hSearch);
	}

	m_cmbMaModelRack1.SetCurSel(0);
	m_cmbMaModelRack2.SetCurSel(0);
	m_cmbMaModelRack3.SetCurSel(0);
	m_cmbMaModelRack4.SetCurSel(0);
	m_cmbMaModelRack5.SetCurSel(0);
	m_cmbMaModelRack6.SetCurSel(0);
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int i = 0; i < m_pCmbMaModel[rack]->GetCount(); i++)
		{
			CString selModelName;
			selModelName = lpSystemInfo->m_sLastModelName[rack];
			m_pCmbMaModel[rack]->GetLBText(i, strfilename);
			if (strfilename == selModelName)
			{
				m_pCmbMaModel[rack]->SetCurSel(i);
				break;
			}
		}
	}
}

void CHseAgingDlg::Lf_setAgingSTART(int rack)
{
	if (m_pCmbMaModel[rack]->GetCurSel() == 0)
	{
		m_pApp->Gf_ShowMessageBox(_T("No model was selected. Please select a model."));
		return;
	}

	CString sModelName, sdata, sLog;

	// Button Disable
	m_pBtnAgingStart[rack]->EnableWindow(FALSE);
	m_pBtnAgingFusing[rack]->EnableWindow(FALSE);
	m_pChkChSelect[rack]->EnableWindow(FALSE);

	// Cable Open Check 진행한다.
	if (Lf_checkCableOpen(rack) == FALSE)
	{
		// Button Disable
		m_pBtnAgingStart[rack]->EnableWindow(TRUE);
		m_pBtnAgingFusing[rack]->EnableWindow(TRUE);
		m_pChkChSelect[rack]->EnableWindow(TRUE);

		return;
	}

	// Aging START 및 Panel Power ON
	m_pApp->pCommand->Gf_setAgingSTART(rack);
	m_pApp->pCommand->Gf_setPowerSequenceOnOff(rack, POWER_ON);

	// 모델명에서 Aging Set Time 정보를 Read 한다.
	m_pCmbMaModel[rack]->GetWindowText(sModelName);
	Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("AGING_TIME_MINUTE"), &lpInspWorkInfo->m_nAgingSetTime[rack]);

	// 모델에 설정된 AGING 시간 정보를 UI에 표시한다.
	sdata.Format(_T("%02d:%02d"), (lpInspWorkInfo->m_nAgingSetTime[rack] / 60), (lpInspWorkInfo->m_nAgingSetTime[rack] % 60));
	m_pSttMaSetTime[rack]->SetWindowText(sdata);
	m_pSttMaRunTime[rack]->SetWindowText(_T("00:00"));

	// Thread Aging 시작 Flag Set
	m_nAgingStart[rack] = TRUE;

	// Aging 상태를 Running 으로 변경한다.
	lpInspWorkInfo->m_nAgingOperatingMode[rack] = AGING_RUNNING;

	// AGING Start Time 을 기록한다. (시/분/초 Summary Log 기록 위함)
	m_pApp->Gf_sumSetStartTime(rack);

	// AGING Start Tick 을 기록한다. (Thread 에서 경과시간 확인을 위함)
	lpInspWorkInfo->m_nAgingStartTick[rack] = ::GetTickCount64();

	// Last Model Name을 저장한다.
	CString skey;
	lpSystemInfo->m_sLastModelName[rack] = sModelName;
	skey.Format(_T("LAST_MODELNAME_RACK%d"), (rack + 1));
	Write_SysIniFile(_T("SYSTEM"), skey, sModelName);

	// RACK Log 출력
	sLog.Format(_T("Aging START : %s"), sModelName);
	Lf_writeRackMLog(rack, sLog);
}

void CHseAgingDlg::Lf_setAgingSTOP(int rack)
{
	CString sLog;

	// Thread Aging 시작 Flag Clear
	m_nAgingStart[rack] = FALSE;

	// Aging 상태를 IDLE 으로 변경한다.
	lpInspWorkInfo->m_nAgingOperatingMode[rack] = AGING_IDLE;

	// Aging STOP 및 Panel Power OFF
	m_pApp->pCommand->Gf_setAgingSTOP(rack);
	m_pApp->pCommand->Gf_setPowerSequenceOnOff(rack, POWER_OFF);

	// RACK Log 출력
	sLog.Format(_T("Aging STOP"));
	Lf_writeRackMLog(rack, sLog);

	// Button Enable
	m_pBtnAgingStart[rack]->EnableWindow(TRUE);
	m_pBtnAgingFusing[rack]->EnableWindow(TRUE);
	m_pChkChSelect[rack]->EnableWindow(TRUE);
}

void CHseAgingDlg::Lf_setAgingFUSING(int rack)
{
	if (m_pCmbMaModel[rack]->GetCurSel() == 0)
	{
		m_pApp->Gf_ShowMessageBox(_T("No model was selected. Please select a model."));
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// 2025-01-13 PDH. 모든 Layer 연결이 안되어있을 경우 N/C Fusing Skip 하도록 수정
	BOOL bFusing = FALSE;
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		if ((lpInspWorkInfo->m_nMainEthConnect[rack][layer]) != 0)
		{
			bFusing = TRUE;
			break;
		}
	}
	if (bFusing == FALSE)
	{
		Lf_writeRackMLog(rack, _T("Fusing : N/C"));
		return;
	}
	////////////////////////////////////////////////////////////////////////////////////



	CString sModelName, sdata;
	int nValue;

	// Button Disable
	m_pBtnAgingStart[rack]->EnableWindow(FALSE);
	m_pBtnAgingStop[rack]->EnableWindow(FALSE);
	m_pBtnAgingFusing[rack]->EnableWindow(FALSE);

	memset(lpInspWorkInfo->m_ast_AgingChErrorResult[rack], 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorResult[rack]));
	memset(lpInspWorkInfo->m_ast_AgingChErrorType[rack], 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorType[rack]));
	memset(lpInspWorkInfo->m_ast_AgingChErrorValue[rack], 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorValue[rack]));


	// RACK 선택된 모델명을 가져온다
	m_pCmbMaModel[rack]->GetWindowText(sModelName);
	m_pApp->Gf_loadModelData(sModelName);

	// 모델정보에서 AGING 시간 정보를 LOAD하여 UI에 표시한다.
	Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("AGING_TIME_MINUTE"), &nValue);
	sdata.Format(_T("%02d:%02d"), nValue / 60, nValue % 60);
	m_pSttMaSetTime[rack]->SetWindowText(sdata);
	m_pSttMaRunTime[rack]->SetWindowText(_T("00:00"));

	// SYSTEM Fusing
	Lf_writeRackMLog(rack, _T("Fusing : Start"));
	if (m_pApp->pCommand->Gf_setFusingSystemInfo(rack) == TRUE)
		Lf_writeRackMLog(rack, _T("Fusing : OK"));
	else
		Lf_writeRackMLog(rack, _T("Fusing : NG"));

	// Progress Bard 진행상태를 초기화 한다.
	m_pCtrMaProgress[rack]->SetPos(0);

	// Last Model Name을 저장한다.
	CString skey;
	lpSystemInfo->m_sLastModelName[rack] = sModelName;
	skey.Format(_T("LAST_MODELNAME_RACK%d"), (rack + 1));
	Write_SysIniFile(_T("SYSTEM"), skey, sModelName);

	// Button Disable
	m_pBtnAgingStart[rack]->EnableWindow(TRUE);
	m_pBtnAgingStop[rack]->EnableWindow(TRUE);
	m_pBtnAgingFusing[rack]->EnableWindow(TRUE);
}

int CHseAgingDlg::Lf_getChannelInfo(int ctrlID)
{
	int retChStatus = STATUS_IDLE;
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				if (ctrlID == m_pSttRackCtrlID[rack][layer][ch])
				{
#if 1
					if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] == FALSE)
						retChStatus = STATUS_NOT_CONNECT;
					else if (lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] == CHANNEL_UNUSE)
						retChStatus = STATUS_UNUSE;
					else if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] != LIMIT_NONE)
						retChStatus = STATUS_ERROR;
					else if (lpInspWorkInfo->m_ast_AgingChOnOff[rack][layer][ch] == ON)
						retChStatus = STATUS_RUN;
#else
					if (lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] == CHANNEL_UNUSE)
						retChStatus = STATUS_UNUSE;
					else if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] == FALSE)
						retChStatus = STATUS_NOT_CONNECT;
					else if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] != LIMIT_NONE)
						retChStatus = STATUS_ERROR;
					else if (lpInspWorkInfo->m_ast_AgingChOnOff[rack][layer][ch] == ON)
						retChStatus = STATUS_RUN;
#endif
					lpInspWorkInfo->m_nChMainUiStatusOld[rack][layer][ch] = retChStatus;
				}
			}
		}
	}

	return retChStatus;
}

int CHseAgingDlg::Lf_getChannelStatus(int rack, int layer, int ch)
{
	int retChStatus = STATUS_IDLE;

#if 1
	if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] == FALSE)
		retChStatus = STATUS_NOT_CONNECT;
	else if (lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] == CHANNEL_UNUSE)
		retChStatus = STATUS_UNUSE;
	else if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] != LIMIT_NONE)
		retChStatus = STATUS_ERROR;
	else if (lpInspWorkInfo->m_ast_AgingChOnOff[rack][layer][ch] == ON)
		retChStatus = STATUS_RUN;
#else
	if (lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] == CHANNEL_UNUSE)
		retChStatus = STATUS_UNUSE;
	else if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] == FALSE)
		retChStatus = STATUS_NOT_CONNECT;
	else if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] != LIMIT_NONE)
		retChStatus = STATUS_ERROR;
	else if (lpInspWorkInfo->m_ast_AgingChOnOff[rack][layer][ch] == ON)
		retChStatus = STATUS_RUN;
#endif

	return retChStatus;
}

void CHseAgingDlg::Lf_updateSystemInfo()
{
	////////////////////////////////////////////////////////////////////////////////
	GetDlgItem(IDC_STT_CONNECT_INFO_PG)->Invalidate(FALSE);
	GetDlgItem(IDC_STT_CONNECT_INFO_DIO)->Invalidate(FALSE);
	GetDlgItem(IDC_STT_CONNECT_INFO_TEMP)->Invalidate(FALSE);
	GetDlgItem(IDC_STT_CONNECT_INFO_BARCODE)->Invalidate(FALSE);
	GetDlgItem(IDC_STT_CONNECT_INFO_MES)->Invalidate(FALSE);

	GetDlgItem(IDC_STT_MA_CHAMBER_NO)->SetWindowText(lpSystemInfo->m_sChamberNo);

	if (lpInspWorkInfo->m_nConnectInfo[CONNECT_MES] == TRUE)
	{
		GetDlgItem(IDC_STT_CONNECT_INFO_MES)->SetWindowText(_T("[MES]ON-LINE"));
	}
	else
	{
		GetDlgItem(IDC_STT_CONNECT_INFO_MES)->SetWindowText(_T("[MES]OFF-LINE"));
	}
	GetDlgItem(IDC_STT_MES_USER_ID)->SetWindowText(m_pApp->m_sUserID);
	GetDlgItem(IDC_STT_MES_USER_ID2)->SetWindowText(m_pApp->m_sUserName);
}

void CHseAgingDlg::Lf_writeRackMLog(int rack, CString sLog)
{
	int maxAlarmList = 1000;

	if (m_pLstMLog[rack]->GetCount() > maxAlarmList)
	{
		m_pLstMLog[rack]->DeleteString(maxAlarmList - 1);
	}

	CString message;
	CTime time = CTime::GetCurrentTime();
	message.Format(_T("[%02d:%02d] %s"), time.GetHour(), time.GetMinute(), sLog);
	m_pLstMLog[rack]->SetCurSel(m_pLstMLog[rack]->AddString(message));
}

void CHseAgingDlg::Lf_getMeasurePower()
{
	m_pApp->pCommand->Gf_getPowerMeasureAllGroup();
}


void CHseAgingDlg::Lf_getAgingStatus()
{
	int skipGroup[6];

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		skipGroup[rack] = m_pChkChSelect[rack]->GetCheck();
	}

	m_pApp->pCommand->Gf_getAgingStatusAllGroup(skipGroup);
}


void CHseAgingDlg::Lf_updateEthConnectInfo()
{
	lpInspWorkInfo->m_nConnectInfo[CONNECT_PG] = FALSE;

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] != 0)
			{
				lpInspWorkInfo->m_nConnectInfo[CONNECT_PG] = TRUE;
			}
		}
	}
	Lf_updateSystemInfo();
}

void CHseAgingDlg::Lf_updateAgingStatus()
{
	int ActiveGroup = 0, chStatus;
	CString sLog, sevent, sdata, skey;

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		if (m_pChkChSelect[rack]->GetCheck() == TRUE)
			continue;

		// CH의 상태를 Update 한다.
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				chStatus = Lf_getChannelStatus(rack, layer, ch);
				if (chStatus != lpInspWorkInfo->m_nChMainUiStatusOld[rack][layer][ch])
				{
					m_pSttRackState[rack][layer][ch]->Invalidate(FALSE);
				}
			}
		}

		if (lpInspWorkInfo->m_nAgingOperatingMode[rack] == AGING_RUNNING)
		{
			// 선택된 Group 의 Aging  Status
			CString sdata;
			int nVal;

			sdata.Format(_T("%02d:%02d"), lpInspWorkInfo->m_nAgingRunTime[rack] / 60, lpInspWorkInfo->m_nAgingRunTime[rack] % 60);
			m_pSttMaRunTime[rack]->SetWindowText(sdata);
			if (lpInspWorkInfo->m_nAgingSetTime[rack] > 0)
			{
				nVal = (int)(((float)lpInspWorkInfo->m_nAgingRunTime[rack] / (float)lpInspWorkInfo->m_nAgingSetTime[rack]) * 100.0f);
				m_pCtrMaProgress[rack]->SetPos(nVal);
			}
		}
		else if (lpInspWorkInfo->m_nAgingOperatingMode[rack] == AGING_COMPLETE)
		{
			if (m_nAgnOutFlag[rack] == FALSE)
			{
				// RACK Log 출력
				sLog.Format(_T("Aging COMPLETE"));
				Lf_writeRackMLog(rack, sLog);

				Lf_setAgingSTOP(rack);

				// Aging END 시간을 기록한다.
				m_pApp->Gf_sumSetEndTime(rack);

				// RACK Log 출력
				sLog.Format(_T("MES Send"));
				Lf_writeRackMLog(rack, sLog);

				// Progress, Run Time을 Max로 표시한다.
				m_pCtrMaProgress[rack]->SetPos(100);
				sdata.Format(_T("%02d:%02d"), lpInspWorkInfo->m_nAgingSetTime[rack] / 60, lpInspWorkInfo->m_nAgingSetTime[rack] % 60);
				m_pSttMaRunTime[rack]->SetWindowText(sdata);

				// MES 자동보고 진행한다.
				CPidInput idDlg;
				idDlg.m_nMesAutoRackNo = rack;
				idDlg.m_nMesAutoDMOU = MES_DMOU_MODE_AUTO;
				idDlg.DoModal();

				Lf_readSummaryIni(rack);

				// RACK Log 출력
				sLog.Format(_T("Summary Log Write"));
				Lf_writeRackMLog(rack, sLog);

				for (int layer = 0; layer < MAX_LAYER; layer++)
				{
					for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
					{
						m_pApp->Gf_sumWriteSummaryLog(rack, layer, ch);

						///////////////////////////////////////////////////////////////////////////////////////////////
						// 2023-12-01 PDH. Aging 실처리 보고
						// Summary Log Write 정보 기준으로 실처리하기 때문에 Summary Log Write 뒤에 보고해야 한다.
						///////////////////////////////////////////////////////////////////////////////////////////////
						if ((m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_PID].IsEmpty() == TRUE)
							|| (m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_CHANNEL].IsEmpty() == TRUE)
							)
						{
							CString strLog;

							strLog.Format(_T("<EAS> APDR send skip. 'Channel ID' or 'Panel ID' is empty, (%s,%s)"), m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_CHANNEL], m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_PID]);
							m_pApp->Gf_writeMLog(strLog);
							continue;
						}

						m_pApp->Gf_gmesSendHost(HOST_APDR, rack, layer, ch);
						///////////////////////////////////////////////////////////////////////////////////////////////
						///////////////////////////////////////////////////////////////////////////////////////////////
					}
				}

				m_pApp->Gf_sumInitSummaryInfo(rack);

				// Time Out 값을 초기화 한다.
				skey.Format(_T("LAST_TIMEOUT_RACK%d"), rack+1);
				Write_SysIniFile(_T("SYSTEM"), skey, 0);

			}


			if (m_bOnOffFlag[rack] == FALSE)
			{
				m_bOnOffFlag[rack] = TRUE;
				m_pApp->pCommand->Gf_setPowerSequenceOnOff(rack, OFF, NACK);
			}
		}
	}
}

void CHseAgingDlg::Lf_updateTowerLamp()
{
	int towerStatus = TOWER_LAMP_READY;
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		if (lpInspWorkInfo->m_nAgingOperatingMode[rack] == AGING_ERROR)
		{
			towerStatus |= TOWER_LAMP_ERROR;
		}
		else
		{
			if (lpInspWorkInfo->m_nAgingOperatingMode[rack] == AGING_RUNNING)
				towerStatus |= TOWER_LAMP_RUNNING;
			if (lpInspWorkInfo->m_nAgingOperatingMode[rack] == AGING_COMPLETE)
				towerStatus |= TOWER_LAMP_COMPLETE;
		}
	}

	if (towerStatus != lpInspWorkInfo->m_nTowerLampStatus)
	{
		lpInspWorkInfo->m_nTowerLampStatus = towerStatus;
		if (towerStatus & TOWER_LAMP_ERROR)
		{
			Lf_setDIOWrite(DIO_OUT_RED | DIO_OUT_BUZZER, 1);
		}
		else if (towerStatus & TOWER_LAMP_RUNNING)
		{
			Lf_setDIOWrite(DIO_OUT_GREEN, 0);
		}
		else if (towerStatus & TOWER_LAMP_COMPLETE)
		{
			Lf_setDIOWrite(DIO_OUT_YELLOW, 0);
		}
		else
		{
			Lf_setDIOWrite(DIO_OUT_YELLOW, 1);
		}
	}
}

void CHseAgingDlg::Lf_setDIOWrite(int outData, int mode)
{
	lpInspWorkInfo->m_nDioOutputData = outData;
	lpInspWorkInfo->m_nDioOutputMode = mode;

	m_pApp->pCommand->Gf_dio_setDIOWriteOutput(outData, mode);
}

void CHseAgingDlg::Lf_setDIOBoardInitial()
{
	m_pApp->pCommand->Gf_dio_setDIOBoardInitial();
}

void CHseAgingDlg::Lf_getDIOStatus()
{
	if (lpInspWorkInfo->m_nDioNeedInitial == TRUE)
	{
		Lf_setDIOBoardInitial();
		return;
	}

	if (lpInspWorkInfo->m_nConnectInfo[CONNECT_DIO] != 0)
	{
		lpInspWorkInfo->m_nConnectInfo[CONNECT_DIO] = lpInspWorkInfo->m_nConnectInfo[CONNECT_DIO] - 1;

		if (lpInspWorkInfo->m_nConnectInfo[CONNECT_DIO] == 0)
		{
			lpInspWorkInfo->m_nDioNeedInitial = TRUE;
		}
	}

	m_pApp->pCommand->Gf_dio_getDIOReadStatus();

	///////////////////////////////////////////////////////////////////
	Lf_checkDoorOpenClose();
}

void CHseAgingDlg::Lf_readSummaryIni(int rack)
{
	int ch = 0;
	CString skey = _T("");

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			skey.Format(_T("RACK%d_LAYER%d_CH%d"), rack, layer, (ch + 1));
			Read_SummaryInfo(_T("FIRMWARE"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_FW_VER]);
			Read_SummaryInfo(_T("PID"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_PID]);
			Read_SummaryInfo(_T("START_TIME"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_START_TIME]);
			Read_SummaryInfo(_T("FAIL_MESSAGE"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_FAILED_MESSAGE]);
			Read_SummaryInfo(_T("FAIL_TIME"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_FAILED_MESSAGE_TIME]);
			Read_SummaryInfo(_T("VCC"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_MEAS_VCC]);
			Read_SummaryInfo(_T("ICC"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_MEAS_ICC]);
			Read_SummaryInfo(_T("VBL"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_MEAS_VBL]);
			Read_SummaryInfo(_T("IBL"), skey, &m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_MEAS_IBL]);
		}
	}
}

void CHseAgingDlg::Lf_getTemperature()
{
	if (lpInspWorkInfo->m_nConnectInfo[CONNECT_TEMP] != 0)
		lpInspWorkInfo->m_nConnectInfo[CONNECT_TEMP]--;

	m_pApp->m_pTemp2xxx->TempSDR100_readTemp();

	// 1분에 1회씩 Temperature Log를 기록한다.
	CTime time = CTime::GetCurrentTime();
	if (m_nTempLogWriteMin != time.GetMinute())
	{
		m_nTempLogWriteMin = time.GetMinute();
		Lf_writeTempLog();
	}

	// 온도 정보를 UI에 업데이트 한다.
	Lf_updateTempature();
}


void CHseAgingDlg::Lf_writeTempLog()
{
	FILE* fp;

	char filepath[128] = { 0 };
	char buff[256] = { 0 };
	char dataline[1024] = { 0 };

	SYSTEMTIME sysTime;
	::GetSystemTime(&sysTime);
	CTime time = CTime::GetCurrentTime();

	sprintf_s(filepath, ".\\Logs\\TemperatureLog\\TempLog_%04d%02d%02d.csv", time.GetYear(), time.GetMonth(), time.GetDay());
	fopen_s(&fp, filepath, "r+");
	if (fp == NULL)
	{
		if ((_access(".\\Logs\\TemperatureLog", 0)) == -1)
			_mkdir(".\\Logs\\TemperatureLog");

		delayMs(1);
		fopen_s(&fp, filepath, "a+");
		if (fp == NULL)
		{
			if ((_access(filepath, 2)) != -1)
			{
				delayMs(1);
				fopen_s(&fp, filepath, "a+");
				if (fp == NULL)
				{
					return;
				}
			}
		}
		sprintf_s(buff, "Hour,Minute,CH1,CH2,CH3,CH4,CH5,CH6\n");
		fprintf(fp, "%s", buff);
	}

	fseek(fp, 0L, SEEK_END);

	sprintf_s(buff, "%02d,%02d,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f",
		time.GetHour(),
		time.GetMinute(),
		(lpInspWorkInfo->m_fTempReadVal[0]),
		(lpInspWorkInfo->m_fTempReadVal[1]),
		(lpInspWorkInfo->m_fTempReadVal[2]),
		(lpInspWorkInfo->m_fTempReadVal[3]),
		(lpInspWorkInfo->m_fTempReadVal[4]),
		(lpInspWorkInfo->m_fTempReadVal[5])
	);

	char* pos = dataline;

	sprintf_s(dataline, "%s\n", buff);
	fprintf(fp, "%s", pos);

	fclose(fp);
}

void CHseAgingDlg::Lf_updateTempature()
{
	int i = 0;
	CString sdata = _T("");

	for (i = 0; i < MAX_TEMP_SENSOR; i++)
	{
		if (m_pApp->m_pTemp2xxx->m_bPortOpen2 == TRUE)
		{
			sdata.Format(_T("%.1f"), lpInspWorkInfo->m_fTempReadVal[i]);
			m_pSttTempInfo[i]->SetWindowText(sdata);
		}
		else
		{
			m_pSttTempInfo[i]->SetWindowText(_T("N/C"));
		}
	}
}

void CHseAgingDlg::Lf_parseSDR100Packet(char* szpacket)
{
	CString sdata;
	char szCmd[5] = { 0, };
	char szRet[5] = { 0, };

	if (szpacket[0] == _STX_)
	{
		memcpy(szCmd, &szpacket[3], 3);
		memcpy(szRet, &szpacket[7], 2);

		if (!strcmp(szRet, "OK"))
		{
			lpInspWorkInfo->m_nConnectInfo[CONNECT_TEMP] = 5;

			if (!strcmp(szCmd, "RSD"))
			{
				for (int tp = 1; tp < 7; tp++)
				{
					int nVal;
					sscanf_s(&szpacket[10 + (tp - 1) * 5], "%04X", &nVal);

					if (nVal > 1000)	nVal = 0;
					lpInspWorkInfo->m_fTempReadVal[tp - 1] = (float)(nVal / 10.0);
				}
			}
		}
	}
}

CString CHseAgingDlg::Lf_getLimitErrorString(int rack, int layer, int ch)
{
	CString retString;

	// Model 설정 Limit 정보를 가져온다.
	CString sModelName;
	float modelSetValue;

	m_cmbMaModelRack1.GetWindowText(sModelName);

	// Error 정보에 따라 Error String 을 생성한다.
	if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_VCC)
	{
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_HIGH)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VCC_LIMIT_VOLT_HIGH"), &modelSetValue);
			retString.Format(_T("VCC HIGH Limit (Set:%.2fV, Meas:%.2fV)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_LOW)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VCC_LIMIT_VOLT_LOW"), &modelSetValue);
			retString.Format(_T("VCC LOW Limit (Set:%.2fV, Meas:%.2fV)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
	}
	if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_ICC)
	{
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_HIGH)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VCC_LIMIT_CURR_HIGH"), &modelSetValue);
			retString.Format(_T("ICC HIGH Limit (Set:%.2fA, Meas:%.2fA)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_LOW)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VCC_LIMIT_CURR_LOW"), &modelSetValue);
			retString.Format(_T("ICC LOW Limit (Set:%.2fA, Meas:%.2fA)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
	}
	if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_VBL)
	{
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_HIGH)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VBL_LIMIT_VOLT_HIGH"), &modelSetValue);
			retString.Format(_T("VBL HIGH Limit (Set:%.2fV, Meas:%.2fV)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_LOW)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VBL_LIMIT_VOLT_LOW"), &modelSetValue);
			retString.Format(_T("VBL LOW Limit (Set:%.2fV, Meas:%.2fV)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
	}
	if (lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][ch] == ERR_INFO_IBL)
	{
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_HIGH)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VBL_LIMIT_CURR_HIGH"), &modelSetValue);
			retString.Format(_T("IBL HIGH Limit (Set:%.2fA, Meas:%.2fA)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
		if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_LOW)
		{
			Read_ModelFile(sModelName, _T("MODEL_INFO"), _T("VBL_LIMIT_CURR_LOW"), &modelSetValue);
			retString.Format(_T("IBL LOW Limit (Set:%.2fA, Meas:%.2fA)"), modelSetValue, (float)((float)lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][ch] / 100.f));
		}
	}

	return retString;
}

void CHseAgingDlg::Lf_checkPowerLimitAlarm()
{
	CString limitAlarm = _T("");
	CString sLog, sdata, rackInfo, errString;
	int ch = 0;

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			// Channel 상태 Update. /Normal/Error
			for (ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				if (lpInspWorkInfo->m_nChErrorStatusOld[rack][layer][ch] == lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch])
					continue;

				if (lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][ch] == LIMIT_NONE)
					continue;

				//////////////////////////////////////////////////////////////////////////////////////////////////
				// Power Limit 정보를 생성한다.
				rackInfo.Format(_T("RACK[%d] LAYER[%d] CH[%d] - "), rack + 1, layer + 1, ch + 1);
				errString = Lf_getLimitErrorString(rack, layer, ch);

				// UI Rack LOG에 기록한다.
				Lf_writeRackMLog(RACK_1, errString);

				// MLog 에 상세 내용을 기록한다.
				sLog.Format(_T("<POWER> Power Limit NG. %s %s"), rackInfo, errString);
				m_pApp->Gf_writeMLog(sLog);

				//////////////////////////////////////////////////////////////////////////////////////////////////
				// Alarm을 발생 시키고, Log를 기록한다.
				sdata.Format(_T("%s  %s\r\n"), rackInfo, errString);
				limitAlarm.Append(sdata);

				// Alarm Log 를 기록한다.
				m_pApp->Gf_writeAlarmLog(rack, layer, ch, errString);

				// 2024-05-30 PDH. PID가 없는 CH은 Limit NG 정보 Summary Log 에 기록되지 않도록 한다.
				if (lpInspWorkInfo->m_sMesPanelID[rack][layer][ch].GetLength() != 0)
				{
					//////////////////////////////////////////////////////////////////////////////////////////////////
					CString skey = _T(""), stime = _T("");
					CTime time = CTime::GetCurrentTime();
					stime.Format(_T("%02d:%02d:%02d"), time.GetHour(), time.GetMinute(), time.GetSecond());
					skey.Format(_T("RACK%d_LAYER%d_CH%d"), rack + 1, layer + 1, ch + 1);
					Write_SummaryInfo(_T("FAIL_MESSAGE"), skey, errString);
					Write_SummaryInfo(_T("FAIL_TIME"), skey, stime);
					//////////////////////////////////////////////////////////////////////////////////////////////////
				}
			}
		}
	}

	// Limit Alarm 메세지가 있고 Alarm 발생 상태가 아니면 Limit Alarm 을 발생시킨다.
	if (limitAlarm.GetLength() != 0)
	{
		if (lpInspWorkInfo->m_bAlarmOccur != TRUE)
		{
			lpInspWorkInfo->m_bAlarmOccur = TRUE;
			lpInspWorkInfo->m_sAlarmMessage.Format(_T("%s"), limitAlarm);
		}
	}

	memcpy(lpInspWorkInfo->m_nChErrorStatusOld, lpInspWorkInfo->m_ast_AgingChErrorResult, sizeof(lpInspWorkInfo->m_nChErrorStatusOld));
}

void CHseAgingDlg::Lf_getFirmawareVersion()
{
	Lf_updateFirmwareMatching();
}

void CHseAgingDlg::Lf_updateFirmwareMatching()
{
#if 1
	// 2025-01-17 PDH. 가장 빠른 날짜의 F/W 버전을 표시하기 위한 알고리즘으로 수정
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		CString fwVersion, curVersion, preVersion;
		CTime backupDate = 0;
		BOOL bFWInvalidate = FALSE;
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			lpInspWorkInfo->m_sMainFWVersion[rack][layer] = _T("2024-11-11");
			// FW 버전이 Read 되어있지 않을 경우 Skip 한다.
			if(lpInspWorkInfo->m_sMainFWVersion[rack][layer].GetLength() == 0)
				continue;

			// CString 시간 문자를 CTime 형으로 변경한다.
			CTime layerDate;
			CString strDate = lpInspWorkInfo->m_sMainFWVersion[rack][layer];
			CString strYear, strMonth, strDay;
			AfxExtractSubString(strYear, strDate, 0, '-');
			AfxExtractSubString(strMonth, strDate, 1, '-');
			AfxExtractSubString(strDay, strDate, 2, '-');
			CTime tmDate(_ttoi(strYear), _ttoi(strMonth), _ttoi(strDay), 0, 0, 0);
			layerDate = tmDate;

			// 가장 마지막 FW 버전 정보를 표시한다.
			if (layerDate > backupDate)
			{
				backupDate = layerDate;
				fwVersion.Format(_T("%s"), lpInspWorkInfo->m_sMainFWVersion[rack][layer].GetString());
			}

			// 모든 FW 버전의 값이 같은지 비교한다.
			if (layer == LAYER_1)
			{
				preVersion = lpInspWorkInfo->m_sMainFWVersion[rack][layer];
			}
			else
			{
				if (preVersion != lpInspWorkInfo->m_sMainFWVersion[rack][layer])
				{
					lpInspWorkInfo->m_nFwVerifyResult[rack] = FALSE;
					bFWInvalidate = TRUE;
				}
			}
		}
		
		// Firmware Version 변경되었으면 업데이트 한다.
		m_pSttFWVersion[rack]->GetWindowText(curVersion);
		if (fwVersion != curVersion)
		{
			m_pSttFWVersion[rack]->SetWindowText(fwVersion);
		}

		// RACK의 모든 Layer F/W 버전이 같은지 확인한다.
		if (bFWInvalidate == TRUE)
		{
			m_pSttFWVersion[rack]->Invalidate(FALSE);
		}
	}
#else
	CString sMcuOld[MAX_RACK];

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] == 0)
				continue;

			if (lpInspWorkInfo->m_sMainFWVersion[rack][layer].IsEmpty() == FALSE)
				sMcuOld[layer].Format(_T("%s"), lpInspWorkInfo->m_sMainFWVersion[rack][layer]);
		}
	}

	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			if (lpInspWorkInfo->m_nMainEthConnect[rack][layer] == 0)
				continue;

			if (lpInspWorkInfo->m_sMainFWVersion[rack][layer] != sMcuOld[layer])
				m_bMcuFwComapre[rack] = FALSE;
			else
				sMcuOld[rack] = lpInspWorkInfo->m_sMainFWVersion[rack][layer];
		}
		m_pSttFWVersion[rack]->SetWindowText(sMcuOld[rack].Left(16));
	}
#endif
}

BOOL CHseAgingDlg::Lf_checkAgingIDLEMode()
{
	CString sdata;
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		if (lpInspWorkInfo->m_nAgingOperatingMode[rack] != AGING_IDLE)
		{
			sdata.Format(_T("'RACK-%d' is not IDLE mode. Please stop Aging !!"), rack + 1);
			m_pApp->Gf_ShowMessageBox(sdata);
			return FALSE;
		}
	}

	return TRUE;
}

void CHseAgingDlg::Lf_toggleChUseUnuse(int rack, int layer, int ch)
{
	if (lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] == CHANNEL_USE)
		lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] = CHANNEL_UNUSE;
	else
		lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] = CHANNEL_USE;

	m_pSttRackState[rack][layer][ch]->Invalidate(FALSE);
}

void CHseAgingDlg::Lf_setChannelUseUnuse(int rack)
{
	CString ip;
	BOOL setInfo[16];
	
	m_pBtnChUseUnuseSet[rack]->EnableWindow(FALSE);

	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			setInfo[ch] = lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch];
		}
		ip.Format(_T("192.168.10.%d"), (rack * 5) + layer + 1);
		m_pApp->pCommand->Gf_setChannelUseUnuse(ip, setInfo);
	}

	m_pBtnChUseUnuseSet[rack]->EnableWindow(TRUE);
}

void CHseAgingDlg::Lf_channelUseButtonShowHide(int rack)
{
	if (m_pChkChSelect[rack]->GetCheck() == TRUE)
	{
		m_pBtnAgingStart[rack]->ShowWindow(SW_HIDE);
		m_pBtnAgingStop[rack]->ShowWindow(SW_HIDE);
		m_pBtnAgingFusing[rack]->ShowWindow(SW_HIDE);
		m_pBtnChUseUnuseSet[rack]->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		m_pBtnAgingStart[rack]->ShowWindow(SW_SHOWNORMAL);
		m_pBtnAgingStop[rack]->ShowWindow(SW_SHOWNORMAL);
		m_pBtnAgingFusing[rack]->ShowWindow(SW_SHOWNORMAL);
		m_pBtnChUseUnuseSet[rack]->ShowWindow(SW_HIDE);
	}
}


BOOL CHseAgingDlg::Lf_checkCableOpen(int rack)
{
	CString sLog, sdata, sMessage;
	
	// Cable Open Check 옵션이 OFF 되어있으면 TRUE Return 한다.
	if (lpModelInfo->m_nFuncCableOpen == FALSE)
		return TRUE;

	CCableOpen cable_dlg;
	cable_dlg.m_nRackNo;
	cable_dlg.DoModal();

	// Cable Open 은 모든 채널에 패널이 걸려있지 않을 수 있기에 NG 일 경우에도 무조건 TRUE로 Return 한다.
	return TRUE;
}

void CHseAgingDlg::Lf_checkDoorOpenClose() // 도어 열림, 닫힘 상태를 확인하는 역할을 하는 함수
{
	CString strMsg; // 문자열을 저장하기 위한 CString 타입의 변수 선언.
	BOOL bInvalidate = FALSE; // 상태 변경 여부를 나타내는 불리언 변수 선언한다.

	for (int i = 0; i < DOOR_MAX; i++) // DOOR_MAX의 값(6)만큼 반복하는 for 루프를 시작한다. 이 루프는 각 문에 대해 상태를 확인한다.
	{
		if (lpInspWorkInfo->m_nDioInputData[i + DIO_IN_DOOR1] != lpInspWorkInfo->m_nDoorOpenClose[i]) // 현재 문 상태(m_nDioInputData)와 이전 문 상태(m_nDoorOpenClose)를 비교한다. 두 값이 다르면 문 상태가 변경된 것으로 간주한다.
		{
			lpInspWorkInfo->m_nDoorOpenClose[i] = lpInspWorkInfo->m_nDioInputData[i + DIO_IN_DOOR1]; // 문 상태가 변경된 경우, m_nDoorOpenClose 배열의 해당 인덱스에 새로운 상태 값을 저장한다.
			bInvalidate = TRUE; // 문 상태가 변경되었음을 나타내기 위해 blnvalidate 변수를 TRUE로 설정한다.

			// DOOR Open/Close 상태값을 UI에 표시
			if (lpInspWorkInfo->m_nDoorOpenClose[i] == DOOR_OPEN) // 현재 문 상태가 DOOR_OPEN인지 확인한다. 문이 열려 있는 경우에 해당한다.
				strMsg.Format(_T("DOOR%d OPEN"), i + 1); // 문이 열려 있는 경우, strMsg에 "DOOR1 OPEN", "DOOR2 OPEN" 등의 형식으로 메시지를 설정한다. i + 1을 사용하여 1부터 시작하는 문 번호를 표시한다.
			else // 문이 닫혀 있는 경우를 처리하기 위한 else 블록을 시작한다.
				strMsg.Format(_T("DOOR%d CLOSE"), i + 1); // 문이 닫혀 있는 경우, strMsg에 "DOOR1 CLOSE", "DOOR2 CLOSE" 등의 형식으로 메시지를 설정한다.

			m_pDoorState[i]->SetWindowText(strMsg); // m_pDoorState 배열의 해당 인덱스에 있는 UI 요소(예: 텍스트 박스)의 텍스트를 strMsg로 설정하여 문 상태를 사용자에게 표시
		}
	}

	// Data 변경사항 있을 경우에만 UI Update 진행한다.
	if (bInvalidate == TRUE)
	{
		GetDlgItem(IDC_STT_MA_DOOR1)->Invalidate(FALSE);
		GetDlgItem(IDC_STT_MA_DOOR2)->Invalidate(FALSE);
		GetDlgItem(IDC_STT_MA_DOOR3)->Invalidate(FALSE);
		GetDlgItem(IDC_STT_MA_DOOR4)->Invalidate(FALSE);
		GetDlgItem(IDC_STT_MA_DOOR5)->Invalidate(FALSE);
		GetDlgItem(IDC_STT_MA_DOOR6)->Invalidate(FALSE);
	}
}

void CHseAgingDlg::Lf_checkBcrRackIDInput()
{
	// Key In Data에 RACKID가 검출되면 해당 RACK PID 입력창으로 이동시킨다.
	int selRackId = -1;
	CString rack1_id, rack2_id, rack3_id, rack4_id, rack5_id, rack6_id;
	Read_SysIniFile(_T("SYSTEM"), _T("RACK1_BCR_ID"), &rack1_id);
	Read_SysIniFile(_T("SYSTEM"), _T("RACK2_BCR_ID"), &rack2_id);
	Read_SysIniFile(_T("SYSTEM"), _T("RACK3_BCR_ID"), &rack3_id);
	Read_SysIniFile(_T("SYSTEM"), _T("RACK4_BCR_ID"), &rack4_id);
	Read_SysIniFile(_T("SYSTEM"), _T("RACK5_BCR_ID"), &rack5_id);
	Read_SysIniFile(_T("SYSTEM"), _T("RACK6_BCR_ID"), &rack6_id);

	if (m_nMainKeyInData.Find(rack1_id) != -1)		selRackId = RACK_1;
	if (m_nMainKeyInData.Find(rack2_id) != -1)		selRackId = RACK_2;
	if (m_nMainKeyInData.Find(rack3_id) != -1)		selRackId = RACK_3;
	if (m_nMainKeyInData.Find(rack4_id) != -1)		selRackId = RACK_4;
	if (m_nMainKeyInData.Find(rack5_id) != -1)		selRackId = RACK_5;
	if (m_nMainKeyInData.Find(rack6_id) != -1)		selRackId = RACK_6;

	if (selRackId != -1)
	{
		m_nMainKeyInData.Empty();

		AfxGetApp()->GetMainWnd()->SendMessage(WM_BCR_RACK_ID_INPUT, (WPARAM)selRackId, NULL);
	}
}

