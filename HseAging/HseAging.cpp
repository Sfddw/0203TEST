
// HseAging.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "HseAging.h"
#include "HseAgingDlg.h"
#include "MessageError.h"
#include "MessageQuestion.h"
#include "UserID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHseAgingApp

BEGIN_MESSAGE_MAP(CHseAgingApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHseAgingApp 생성

CHseAgingApp::CHseAgingApp() // CHseAgingApp 클래스의 생성자 함수 정의. 이 함수는 객체가 생성될 때 호출
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART; // 애플리케이션이 재시작 관리자 기능을 지원하도록 설정, 이 플래그는 애플리케이션이 비정상적으로 종료된 후 자동으로 재시작될 수 있도록 한다.

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
	m_pApp = (CHseAgingApp*)AfxGetApp(); // 현재 애플리케이션의 인스턴스를 m_pApp 포인터에 저장. AfxGetApp() 함수는 현재 애플리케이션의 포인터를 반환

	lpModelInfo		= new MODELINFO; // MODELINFO 구조체 또는 클래스를 동적으로 할당하여 IpModelIInfo 포인터에 저장. 이 객체는 모델 정보를 저장하는데 사용
	lpSystemInfo	= new SYSTEMINFO;
	lpInspWorkInfo	= new INSPWORKINFO;
	pCommand		= new CCommand();
	pCimNet			= new CCimNetCommApi;
	m_pTemp2xxx		= new CTemp2xxx;


	m_pUDPSocket	= new CUDPSocket();
	// 이 코드는 애플리케이션의 초기화 과정에서 필요한 여러 객체를 동적으로 생성하고 초기화하는 역할을 한다.
}


// 유일한 CHseAgingApp 개체입니다.

CHseAgingApp theApp;
CHseAgingApp* m_pApp;


// CHseAgingApp 초기화

BOOL CHseAgingApp::InitInstance() // MFC 애플리케이션의 IniTInstance 함수로 애플리케이션의 초기화 작업을 수행
{
	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls; // INITCOMMONCONTROLSEX 구조체를 선언하여 공용 컨트롤 초기화에 사용할 변수를 정의
	InitCtrls.dwSize = sizeof(InitCtrls); // InitCrtl 구조체의 크기를 설정한다. 이는 초기화 과정에서 필요한 정보이다.
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES; // 사용할 공용 컨트롤 클래스를 설정한다. 여기서 Windows 95 스타일의 컨트롤을 사용하도록 지정
	InitCommonControlsEx(&InitCtrls); // InitCommonControlsEx 함수를 호출하여 공용 컨트롤을 초기화. 이 호출이 없으면 창을 만들 수 없다.

	CWinApp::InitInstance(); // 기본 클래스인 CWinApp의 InitInstance 함수를 호출하여 기본 초기화 작업을 수행

	if (!AfxSocketInit()) // 소켓 라이브러리를 초기화. 초기화에 실패하면 if문 안의 코드 실행
	{
		AfxMessageBox(_T("Windows socket initial fail"));
		return FALSE;
	}


	AfxEnableControlContainer(); // 컨트롤 컨테이너를 활성화하여 대화 상자에서 컨트롤을 사용할 수 있도록 설정.

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager; // 셀 트리 뷰 또는 목록 뷰 컨트롤을 사용할 경우, CShellManager 객체를 동적으로 생성

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows)); // MFC 컨트롤의 테마를 설정하기 위해 Windows 스타일의 비주얼 관리자를 활성화

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));

	//////////////////////////////////////////////////////////////////
	CreateMutex(NULL, TRUE, _T("HseAging")); // 이름이 HseAging인 뮤텍스를 생성하여 애플리케이션의 단일 인스턴스 실행을 보장
	if (GetLastError() == ERROR_ALREADY_EXISTS) // 이전에 같은 이름의 뮤텍스가 존재하는지 확인. 존재하면 if문 안의 코드 실행
	{
		AfxMessageBox(_T("'HSE Aging'  Already Running!!"), MB_ICONSTOP);
		PostQuitMessage(0);
		return FALSE;
		// 이미 실행중인 경우 경고 메시지 표시 후 애플리케이션 종료
	}

	//////////////////////////////////////////////////////////////////
	Gf_LoadSystemData(); // 시스템 데이터를 로드하는 사용자 정의 함수 호출
	Gf_SoftwareStartLog();					// 프로그램 시작 LOG 기록

	//////////////////////////////////////////////////////////////////
	Lf_InitGlobalVariable(); // 전역 변수를 초기화하는 사용자 정의 함수 호출
	Lf_initCreateUdpSocket(); // UDP 소켓을 생성하는 초기화 함수 호출
	Gf_initTempRecorder(); // 온도 기록계를 초기화하는 사용자 정의 함수 호출
	Lf_initCreateFolder(); // 필요한 폴더를 생성하는 초기화 함수 호출

	// GMES DLL Initialize
	if (Gf_gmesInitServer(SERVER_MES) == FALSE) // GMES 서버 초기화를 시도. 실패하면 다음 코드를 실행
	{
		AfxMessageBox(_T("TIB Driver Init Fail.\r\nPlease check whether you have installed the TibDriver and registered the MES DLL."), MB_ICONERROR);
	}
	if (Gf_gmesInitServer(SERVER_EAS) == FALSE) // 또 다른 GMES 서버 초기화를 시도. 실패하면 다음 코드 실행
	{
		AfxMessageBox(_T("TIB Driver Init Fail.\r\nPlease check whether you have installed the TibDriver and registered the MES DLL."), MB_ICONERROR);
	}

	CUserID user_dlg; // 사용자 ID 입력을 위한 대화 상자 객체를 생성.
	user_dlg.DoModal(); // 사용자 ID 대화 상자를 모달로 표시하여 사용자의 입력을 기다린다.

	CHseAgingDlg dlg; // 메인 대화 상자 객체를 생성
	m_pMainWnd = &dlg; // 메인 윈도우 포인터를 메인 대화 상자로 설정.
	INT_PTR nResponse = dlg.DoModal(); // 메인 대화 상자를 모달로 표시하고 사용자의 응답을 기다린다.
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1) // 대화 상자 생성에 실패시
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != nullptr) // 생성한 셀 관리자를 삭제하여 메모리 누수를 방지
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp(); // MFC 컨트롤이 사용 중일 때, 컨트롤 바를 정리한다.
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고 응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LPMODELINFO CHseAgingApp::GetModelInfo() // LPMODELINFO 타입의 포인터를 반환. LPMODELINFO는 모델 정보를 나타낸다.
{
	ASSERT(NULL != lpModelInfo); // IpModelInfo 포인터가 NULL이 아닌지 확인. ASSERT 매크로는 디버그 모드에서만 작동, 조건이 FALSE일 경우
	                             // 프로그램이 중단
	VERIFY(NULL != lpModelInfo); // IpModelInfo 포인터가 NULL이 아닌지 확인. VERIFY 매크로는 디버그와 릴리스 모드 모두에서 작동,
	                             // 조건이 FALSE일 경우 프로그램이 중단된다.

	return lpModelInfo; // IpModelInfo 포인터를 반환. 이 포인터는 모델 정보를 포함
}

LPSYSTEMINFO CHseAgingApp::GetSystemInfo() // 
{
	ASSERT(NULL != lpSystemInfo);
	VERIFY(NULL != lpSystemInfo);

	return lpSystemInfo;
}

LPINSPWORKINFO CHseAgingApp::GetInspWorkInfo()
{
	ASSERT(NULL != lpInspWorkInfo);
	VERIFY(NULL != lpInspWorkInfo);

	return lpInspWorkInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::Lf_InitGlobalVariable() // 이 함수는 반환값이 없으며, 전역 변수를 초기화하는 기능을 수행
{
	m_bUserIdAdmin = FALSE; // m_bUserIdAdmin 변수를 FALSE로 초기화. 이는 사용자가 관리자 권한을 가지고 있지 않음을 나타낸다.
	m_bIsGmesConnect = FALSE; // 변수를 FALSE로 초기화. 이는 GMES와의 연결 상태가 없음을 나타낸다.
	m_bIsEasConnect = FALSE; // 변수를 FALSE로 초기화. 이는 EAS(Enterprise Application Services)와의 연결 상태가 없음을 나타낸다.

	memset(m_nAckCmdPG, 0, sizeof(m_nAckCmdPG)); // m_nAckCmdPG 배열의 모든 요소를 0으로 초기화. 이 배열은 특정 명령에 대한 응답을 저장하는데 사용될 수 있다.

	memset(lpInspWorkInfo->m_nConnectInfo, 0, sizeof(lpInspWorkInfo->m_nConnectInfo)); // IpInspWorkInfo 구조체의 m_nConnectInfo 배열을 0으로 초기화, 이는 연결 정보를 초기화 하는 것
	memset(lpInspWorkInfo->m_nMainEthConnect, 0, sizeof(lpInspWorkInfo->m_nMainEthConnect)); // 이더넷 연결 정보 초기화
	memset(lpInspWorkInfo->m_nAgingSetTime, 0, sizeof(lpInspWorkInfo->m_nAgingSetTime)); // AgingSetTime 초기화
	memset(lpInspWorkInfo->m_nAgingRunTime, 0, sizeof(lpInspWorkInfo->m_nAgingRunTime)); 
	memset(lpInspWorkInfo->m_nMeasVCC, 0, sizeof(lpInspWorkInfo->m_nMeasVCC)); 
	memset(lpInspWorkInfo->m_nMeasICC, 0, sizeof(lpInspWorkInfo->m_nMeasICC)); 
	memset(lpInspWorkInfo->m_nMeasVBL, 0, sizeof(lpInspWorkInfo->m_nMeasVBL));
	memset(lpInspWorkInfo->m_nMeasIBL, 0, sizeof(lpInspWorkInfo->m_nMeasIBL)); // 측정된 IBL 값을 초기화

	memset(lpInspWorkInfo->m_ast_AgingLayerError, 0, sizeof(lpInspWorkInfo->m_ast_AgingLayerError)); // 오류정보 초기화
	memset(lpInspWorkInfo->m_ast_AgingStartStop, 0, sizeof(lpInspWorkInfo->m_ast_AgingStartStop)); // 시작 정지 상태 초기화
	memset(lpInspWorkInfo->m_ast_AgingChOnOff, 0, sizeof(lpInspWorkInfo->m_ast_AgingChOnOff)); // 채널 켜짐/꺼짐 상태 초기화
	memset(lpInspWorkInfo->m_ast_ChUseUnuse, 0, sizeof(lpInspWorkInfo->m_ast_ChUseUnuse));
	memset(lpInspWorkInfo->m_ast_CableOpenCheck, 0, sizeof(lpInspWorkInfo->m_ast_CableOpenCheck));
	memset(lpInspWorkInfo->m_ast_AgingChErrorResult, 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorResult));
	memset(lpInspWorkInfo->m_ast_AgingChErrorType, 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorType));
	memset(lpInspWorkInfo->m_ast_AgingChErrorValue, 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorValue));

	memset(lpInspWorkInfo->m_nDioInputData, 0, sizeof(lpInspWorkInfo->m_nDioInputData));
	memset(lpInspWorkInfo->m_nDoorOpenClose, 0, sizeof(lpInspWorkInfo->m_nDoorOpenClose));

	memset(lpInspWorkInfo->m_fTempReadVal, 0, sizeof(lpInspWorkInfo->m_fTempReadVal));

	for (int rack = 0; rack < MAX_RACK; rack++) // MAX_RACK 수만큼 반복하는 루프를 시작. 각 랙에 대해 초기화를 수행
	{
		lpInspWorkInfo->m_nAgingOperatingMode[rack] = AGING_IDLE; // 각 랙의 모드를 IDEL로 초기화 (대기상태)
		lpInspWorkInfo->m_nFwVerifyResult[rack] = TRUE; // 각 랙의 펌웨어 검증 결과를 TRUE로 초기화. 이는 펌웨어 검증이 성공했음을 나타낸다.
	}
	// 이 함수는 다양한 전역 변수를 초기화하여 프로그램의 초기 상태를 설정하는 데 중요한 역할을 한다.
}

void CHseAgingApp::Lf_initCreateFolder() // 특정 디렉토리(폴더)를 생성하는 함수
{
	if (PathFileExists(_T("./Model")) != TRUE)	CreateDirectory(_T("./Model"), NULL); // ./Model 경로에 해당하는 디렉토리가 존재하지 않으면. CreateDirectory 함수를 호출하여 ./Model 디렉토리를 생성
	if (PathFileExists(_T("./Config")) != TRUE)	CreateDirectory(_T("./Config"), NULL);
	if (PathFileExists(_T("./Logs")) != TRUE)	CreateDirectory(_T("./Logs"), NULL);
	if (PathFileExists(_T("./Logs/MLog")) != TRUE)	CreateDirectory(_T("./Logs/MLog"), NULL);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::Gf_SoftwareStartLog() // 소프트웨어 시작 시 로그를 생성하는 함수
{
	// Main Form Title Set
	CString strPGMTitle; // 프로그램의 제목을 저장하는데 사용
	char D_String[15] = { 0, }; // 현재 날짜를 저장
	char Date_String[15] = { 0, }; // 포맷된 날짜 문자열을 저장
	char* Date[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	// 월 이름을 저장하는 문자열 배열 Date를 선언. 각 월의 약어가 포함되어 있다.

	sprintf_s(D_String, "%s", __DATE__); // __DATE__ 매크로를 사용하여 컴파일 날짜를 D_String에 저장. 이 날짜는 "Mmm dd yyyy"형식
	for (int i = 12; i; i--) // 12부터 1까지 반복. 이는 월을 찾기 위한 루프
	{
		for (int j = 3; j; j--) // 3부터 1까지의 반복. 이는 월 이름의 각 문자와 비교하기 위한 루프
		{
			if (D_String[j - 1] != *(Date[i - 1] + (j - 1)))
				break; // D_String의 월 이름과 Date 배열의 월 이름을 비교. 일치하지 않으면 내부 루프를 종료
			if (j == 1) // 월 이름이 일치하는 경우, j가 1일 때의 조건
			{
				if (D_String[4] == 0x20)	D_String[4] = 0x30; // 날짜 문자열의 5번째 문자가 공백인 경우, 이를 '0'으로 변경. 이는 날짜 포맷을 맞추기 위한 처리
				sprintf_s(Date_String, "%c%c%c%c-%02d-%c%c", D_String[7], D_String[8], D_String[9], D_String[10], i, D_String[4], D_String[5]); // Date_String에 포맷된 날짜 문자열을 저장. 형식은 "yyyy-mm-dd"
				i = 1; j = 1; 
				break; // 월을 찾은 후, i와 j를 1로 설정하고 내부 루프 종료
			}
		}
	}

	m_sSoftwareVersion.Format(_T("%S - %s"), Date_String, PGM_VERSION); // m_sSoftwareVersion에 포맷된 소프트웨어 버전과 날짜를 저장
	strPGMTitle.Format(_T("Hse Aging ( %s )"), m_sSoftwareVersion); // strPGMTitle에 "Hse Aging (소프트웨어 버전)" 형식으로 제목을 설정

	CString sLog; // 로그 메시지를 저장할 CString 변수 sLog를 선언
	sLog.Format(_T("***************************** %s *****************************"), strPGMTitle); // sLog에 제목을 포함한 로그 메시지를 포맷하여 저장
	Gf_writeMLog(sLog); // GF_writeMLog 함수를 호출하여 생성된 로그 메시지를 기록
}

void CHseAgingApp::Gf_writeMLog(CString sLogData) // 로그 데이터를 기록하는 함수, sLogData는 기록할 로그 메시지를 담고 있는 CString 타입의 매개변수
{
	CFile cfp; // CFile 객체 cfp를 선언, 이 객체는 파일 작업을 수행하는 데 사용된다.
	USHORT nShort = 0xfeff; // nShort라는 USHORT 타입의 변수를 선언하고, 값으로 0xfeff를 할당, 이는 UTF-16 인코딩의 BOM(Byte Order Mark)을 나타낸다.
	CString strLog, fileName, filePath, strDate; // 여러 개의 CString 변수 선언, strLog는 로그 메시지 저장, fileName과 filePath는 파일 경로 저장, strDate는 날짜 정보 저장

	// 엔터 Key 값이 있으면 문자를 변경 시키낟.
	sLogData.Replace(_T("\r\n"), _T(" | ")); // sLogData에서 줄 바꿈 문자(\r\n)를 " l "로 대체한다.

	SYSTEMTIME sysTime;
	::GetSystemTime(&sysTime); // SYSTEMTIME 구조체 sysTime을 선언하고, 현재 시스템 시간을 GetSystemTime 함수를 통해 가져온다.
	CTime time = CTime::GetCurrentTime(); // CTime 객체 time을 생성하고, 현재 시간을 GetCurrentTime 메서드를 통해 가져온다.
	strLog.Format(_T("[%02d:%02d:%02d %03d] %06d%03d\t: %s\r\n"), time.GetHour(), time.GetMinute(), time.GetSecond(), sysTime.wMilliseconds, (time.GetHour() * 3600) + (time.GetMinute() * 60) + time.GetSecond(), sysTime.wMilliseconds, sLogData);
	// strLog에 로그 메시지를 포맷하여 저장. 시간, 밀리초, 그리고 로그 데이터를 포함하여 형식화된 문자열을 생성

	strDate.Format(_T("%04d%02d%02d"), time.GetYear(), time.GetMonth(), time.GetDay()); // strDate에 현재 날짜를 YYYYMMDD 형식으로 포맷하여 저장
	filePath.Format(_T("./Logs/MLog/%s_%s.txt"), lpSystemInfo->m_sEqpName, strDate); // filePath에 로그 파일의 경로를 포맷하여 저장. 파일 이름은 장비 이름과 날짜를 포함

	if (GetFileAttributes(_T("./Logs/")) == -1)
		CreateDirectory(_T("./Logs/"), NULL); // ./Logs/디렉토리의 존재 여부를 확인하고, 존재하지 않으면 해당 디렉토리를 생성

	if (GetFileAttributes(_T("./Logs/MLog")) == -1)
		CreateDirectory(_T("./Logs/MLog"), NULL); // ./Logs/MLog 디렉토리의 존재 여부를 확인하고, 존재하지 않으면 해당 디렉토리를 생성


	if (cfp.Open(filePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary)) // filePath에 지정된 파일을 열거나 생성한다, 파일이 존재하지 않으면 새로 생성하고 존재하면 내용을 덮어쓰지 않고 추가
	{
		if (cfp.GetLength() == 0) // 파일의 길이가 0인 경우,즉 파일이 비어있다면 BOM을 파일에 기록한다.
		{
			cfp.Write(&nShort, 2);
		}
		cfp.SeekToEnd(); // 파일 포인터를 파일의 끝으로 이동시킨다. 이는 로그 데이터를 추가하기 위해 필요하다
		cfp.Write(strLog, (strLog.GetLength() * 2)); // strLog의 내용을 파일에 기록한다. CString은 UTF-16으로 인코딩되므로, 길이에 2를 곱하여 바이트 수를 계산
		cfp.Close();
	}
}

void CHseAgingApp::Gf_writeAlarmLog(int rack, int layer, int ch, CString strError) // 랙, 레이어, 채널 번호, 오류 메시지를 받아서 알람 로그를 파일에 기록
{
	FILE* fp; // 파일 포인터 fp를 선언. 이 포인터는 로그 파일을 열고 작업하는 데 사용됨.

	char szbuf[100] = { 0, }; 
	char szYear[5] = { 0, };
	char szMonth[5] = { 0, };
	char szDay[5] = { 0, };
	char filename[256] = { 0 };
	char filepath[1024] = { 0 };
	char dataline[4096] = { 0 };
	// 로그 파일 경로 및 이름을 저장하기 위한 여러 개의 char 배열을 선언. dataline은 로그메시지를 저장하는데 사용
	
	CString strDate;
	CString strTime;
	// 날짜와 시간을 저장하기 위한 CString 변수를 선언

	SYSTEMTIME sysTime;
	::GetSystemTime(&sysTime); // SYSTEMTIME 구조체 sysTime을 선언, 현재 시스템 시간을 가져온다.
	CTime time = CTime::GetCurrentTime(); // 현재 시간을 CTime 객체 time에 저장

	strDate.Format(_T("%04d%02d%02d"), time.GetYear(), time.GetMonth(), time.GetDay()); 
	strTime.Format(_T("%02d:%02d:%02d"), time.GetHour(), time.GetMinute(), time.GetSecond()); 
	// strDate에 현재 날짜를 "YYYYMMDD"형식으로, strTime에 현재 시간을 "HH:MM:SS" 형식으로 포맷하여 저장.


	// 1. 경로를 찾고 없으면 생성한다.
	sprintf_s(szYear, "%04d", time.GetYear());
	sprintf_s(szMonth, "%02d", time.GetMonth());
	sprintf_s(szDay, "%02d", time.GetDay());
	// 현재 연도, 월, 일을 각각 문자열로 포맷하여 szYear, szMonth, szDay에 저장

	if ((_access(".\\Logs\\AlarmLog", 0)) == -1)
		_mkdir(".\\Logs\\AlarmLog"); // "./Logs/AlarmLog" 디렉토리가 존재하지 않으면 생성

	sprintf_s(szbuf, ".\\Logs\\AlarmLog\\%s", szYear);
	if ((_access(szbuf, 0)) == -1) 
		_mkdir(szbuf);
	// 연도별 하위 디렉토리를 생성. 해당 연도 디렉토리가 존재하지 않으면 생성

	sprintf_s(szbuf, ".\\Logs\\AlarmLog\\%s\\%s", szYear, szMonth);
	if ((_access(szbuf, 0)) == -1)
		_mkdir(szbuf);
	// 월별 하위 디렉토리를 생성. 해당 월 디렉토리가 존재하지 않으면 생성

	// 2. file을 open한다.
	sprintf_s(filename, "%04d%02d%02d_AlarmLog.txt", time.GetYear(), time.GetMonth(), time.GetDay()); // 로그 파일 이름을 "YYYYMMDD_AlarmLog.txt"형식으로 포맷하여 filename에 저장
	sprintf_s(filepath, "%s\\%s", szbuf, filename); // 전체 파일 경로를 filepath에 저장

	fopen_s(&fp, filepath, "r+"); // 파일을 읽기 및 쓰기 모드로 엽니다. 파일이 존재하지 않으면 fp는 NULL이 된다.
	if (fp == NULL) // 파일이 열리지 않은 경우, 추가 모드로 파일을 연다. 여전히 열리지 않으면 오류 메시지를 표시하고 함수를 종료한다.
	{
		fopen_s(&fp, filepath, "a+");
		if (fp == NULL) // 2007-08-01 : fseek.c(101) error
		{
			AfxMessageBox(_T("'Aging Alarm Log' file create fail."), MB_ICONERROR);
			return;
		}
	}

	fseek(fp, 0L, SEEK_END); // 파일 포인터를 파일의 끝으로 이동시킨다.

	// 3. Log를 Write한다.
	char szdate[100] = { 0, };
	char szerror[1024] = { 0, };
	// 로그 메시지에 사용할 날짜와 오류 메시지를 저장할 char 배열을 선언

	sprintf_s(szdate, "%04d-%02d-%02d %02d:%02d:%02d", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond()); // 현재 날짜와 시간을 "YYYY-MM-DD HH:MM:SS" 형식으로 포맷하여 szdate에 저장
	sprintf_s(szerror, "%S", strError.GetBuffer(0)); // strError를 char 형식으로 변환하여 szerror에 저장

	if (rack < MAX_RACK) // rack 값에 따라 로그 메시지를 포맷하여 dataline에 저장. rack이 MAX_RACK보다 작으면 일반 로그 형식으로, rack이 19이면 온도 과열 로그 형식으로 저장
	{
		sprintf_s(dataline, "%s\tRACK-%d\tLAYER-%d\tCH_%d\t%s\t\r\n", szdate, rack + 1, layer + 1, ch + 1, szerror);
	}
	else if (rack == 19)
	{
		// TEMP OVER HEAT
		sprintf_s(dataline, "%s\tTEMP\t \tCH_%02d\t%s\t\r\n", szdate, ch + 1, szerror);
	}
	fwrite(dataline, sizeof(char), strlen(dataline), fp); // 포맷된 로그 메시지를 파일에 기록

	// 4. File을 닫는다.
	fclose(fp);
}

BOOL CHseAgingApp::Gf_ShowMessageBox(CString errMessage) // 오류 메시지를 받아서 메시지 박스를 표시하는 기능 수행
{
	CMessageError errDlg; // CMessageError 클래스의 인스턴스 errDlg를 생성. 이 클래스는 사용자 정의 메시지 박스를 나타낸다.
	errDlg.m_strEMessage = errMessage; // 전달받은 오류 메시지 errMessage를 errDlg의 멤버 변수 m_strEMessage에 저장
	errDlg.DoModal(); // errDlg를 모달 대화상자로 표시하여 사용자에게 오류 메시지를 보여준다. 사용자가 대화 상자를 닫을 때까지
	                  // 다른 작업을 할 수 없다.
	return TRUE;
}

void CHseAgingApp::Gf_setGradientStatic01(CGradientStatic* pGStt, CFont* pFont, BOOL bSplit) // magenta
{   // 주어진 OGradientSatic 객체에 그라데이션 효과와 텍스트 속성을 설정하는 기능을 수행
	pGStt->SetFont(pFont); // CGradientStatic 객체 pGStt에 주어진 폰트 pFont를 설정.
	pGStt->SetTextAlign(TEXT_ALIGN_CENTER);
	pGStt->SetColor(RGB(38, 88, 137));
	pGStt->SetGradientColor(RGB(51, 76, 100));
	pGStt->SetVerticalGradient(); // pGStt에 수직 그라데이션 효과를 설정
	if (bSplit == TRUE)	pGStt->SetSplitMode(TRUE); // bSplit이 TRUE인 경우, pGStt의 분할 모드를 활성화
	pGStt->SetTextColor(RGB(255, 255, 255)); // pGStt의 텍스트 색상을 흰색(RGB(255, 255, 255))으로 설정
}

void CHseAgingApp::Gf_setGradientStatic02(CGradientStatic* pGStt, CFont* pFont, BOOL bSplit, int txt_align) //gray
{
	pGStt->SetFont(pFont);

	if (txt_align == TEXT_ALIGN_LEFT)			pGStt->SetTextAlign(TEXT_ALIGN_LEFT);
	else if (txt_align == TEXT_ALIGN_CENTER)	pGStt->SetTextAlign(TEXT_ALIGN_CENTER);
	else										pGStt->SetTextAlign(TEXT_ALIGN_RIGHT);

	pGStt->SetColor(RGB(128, 128, 128));
	pGStt->SetGradientColor(RGB(100, 100, 100));
	pGStt->SetVerticalGradient();
	if (bSplit == TRUE)	pGStt->SetSplitMode(TRUE);
	pGStt->SetTextColor(RGB(255, 255, 255));
}

void CHseAgingApp::Gf_setGradientStatic03(CGradientStatic* pGStt, CFont* pFont, BOOL bSplit)// orange
{
	pGStt->SetFont(pFont);
	pGStt->SetTextAlign(TEXT_ALIGN_CENTER);
	pGStt->SetColor(RGB(245, 136, 22));
	pGStt->SetGradientColor(RGB(222, 118, 14));
	pGStt->SetVerticalGradient();
	if (bSplit == TRUE)	pGStt->SetSplitMode(TRUE);
	pGStt->SetTextColor(RGB(255, 255, 255));
}

void CHseAgingApp::Gf_setGradientStatic04(CGradientStatic* pGStt, CFont* pFont, BOOL bSplit)// blue
{
	pGStt->SetFont(pFont);
	pGStt->SetTextAlign(TEXT_ALIGN_CENTER);
	pGStt->SetColor(RGB(72, 82, 92));
	pGStt->SetGradientColor(RGB(42, 52, 62));
	pGStt->SetVerticalGradient();
	if (bSplit == TRUE)	pGStt->SetSplitMode(TRUE);
	pGStt->SetTextColor(COLOR_GRAY192);
}

void CHseAgingApp::Gf_setGradientStatic05(CGradientStatic* pGStt, CFont* pFont, BOOL bSplit)// green
{
	pGStt->SetFont(pFont);
	pGStt->SetTextAlign(TEXT_ALIGN_CENTER);
	pGStt->SetColor(RGB(116, 192, 24));
	pGStt->SetGradientColor(RGB(100, 161, 19));
	pGStt->SetVerticalGradient();
	if (bSplit == TRUE)	pGStt->SetSplitMode(TRUE);
	pGStt->SetTextColor(COLOR_WHITE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::Gf_InitialSystemInfo() // 온도 기록기를 초기화하는 함수
{
	Gf_initTempRecorder();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::Lf_IPStringToRackInfo(CString ip, int* rack, int* layer) // 주어진 IP 주소 문자열을 정수로 변환하여 해당하는 랙(rack)과 레이어(layer) 정보를 계산하고 포인터를 통해 반환
{
	CString sdata; // IP 주소의 마지막 두 자리 부분을 저장할 문자열 변수를 선언
	int nIpAddr; // IP 주소를 정수로 변환하여 저장할 변수를 선언

	// IP 주소를 Int 변수로 변경한다.
	sdata = ip.Right(2); // ip.Right(2);: 주어진 IP 주소 문자열에서 마지막 두 문자를 추출하여 sdata에 저장.
	if (sdata.Left(1) == _T("."))	sdata.Delete(0, 1); // sdata의 첫 문자가 '.'인 경우, 이를 삭제하여 숫자만 남긴다.
	nIpAddr = _ttoi(sdata); // sdata에 저장된 문자열을 정수를 변환하여 nIpAddr에 저장
	nIpAddr--; // nIpAddr의 값을 1 감소

	*rack = nIpAddr / 5; // nIpAddr를 5로 나누어 랙(rack) 정보를 계산하고, 포인터를 통해 반환
	*layer = nIpAddr % 5; // nIpAddr를 5로 나눈 나머지를 계산하여 레이어(layer)정보를 얻고, 포인터를 통해 반환
}

void CHseAgingApp::Lf_initCreateUdpSocket() // UDP 소켓을 생성하는 함수
{
	if (m_pUDPSocket->CreatSocket(UDP_SOCKET_PORT, SOCK_DGRAM) == FALSE) // m_pUDPSocket 포인터가 가리키는 UDP 소켓 객체의 CreateSocket 메서드를 호출하여 소켓을 생성한다. 이때, UDP_SOCKET_PORT와 SOCK_DGRAM을 인자로 전달. 소켓 생성이 실패하면 FALSE를 반환
	{
		AfxMessageBox(_T("UDP Socket Create Fail"), MB_ICONERROR);
		return;
	}
}

BOOL CHseAgingApp::procWaitRecvACK(int rack, int layer, int cmd, DWORD waitTime) // 특정 명령에 대한 ACK(확인 응답)를 기다리는 함수
{
	DWORD stTick = ::GetTickCount(); // 현재 시스템의 틱 카운트를 가져와 stTick 변수에 저장.
	                                 // 이 값은 함수가 시작된 시간을 기준으로 사용된다.

	while (1) // 이 루프는 ACK를 받을 때 까지 계속 실행
	{
		DWORD edTick = GetTickCount(); // 루프의 각 반복에서 현재 틱 카운트를 가져와 edTick 변수에 저장. 이 값은 루프가 실행된 시간을 측정하는 데 사용된다.
		if ((edTick - stTick) > waitTime) // 현재시간(edTick)과 시작시간(stTick)의 차이가 waitTime을 초과하는지 확인. 만약 초과하면
			                              // ACK를 받지 못한 것으로 간주하고 다음 줄로 이동
			return FALSE; // ACK를 받지 못하고 지정된 대기 시간이 초과된 경우, 함수는 FALSE를 반환

		if (cmd == m_nAckCmdPG[rack][layer]) // 현재 명령(cmd)이 저장된 ACK 명령(m_nAckCmdPG[rack][layer])과 일치하는지 확인.
			                                 // 일치하면 ACK를 받은 것으로 간주한다.
		{
			return TRUE; // ACK를 성공적으로 수신한 경우, 함수는 TRUE를 반환하여 성공을 나타낸다
		}

		ProcessMessage(); // ACK를 기다리는 동안 다른 메시지를 처리하는 함수. 이 호출은 프로그램이 응답성을 유지하도록 도와준다.
	}
	return FALSE;
}

BOOL CHseAgingApp::procWaitRecvACK_DIO(int cmd, DWORD waitTime)  // ACK(확인 응답)를 기다리는 함수
{
	DWORD stTick = ::GetTickCount(); // 현재 시스템의 틱 카운트를 가져와 stTick 변수에 저장, 이 값은 함수가 시작된 시간을 기준으로 사용된다.

	while (1)
	{
		if (cmd == m_nAckCmdDIO) // 현재 명령(cmd)이 저장된 DIO ACK 명령(m_nAckCmdDIO)과 일치하는지 확인. 일치하면 ACK를 받은 것으로 간주.
		{
			return TRUE; // ACK를 성공적으로 수신한 경우, 함수는 TRUE를 반환하여 성공을 나타낸다.
		}

		DWORD edTick = GetTickCount(); // 루프의 각 반복에서 현재 틱 카운트를 가져와 edTick 변수에 저장. 이 값은 루프가 실행된 시간을 측정하는 데 사용된다.

		if ((edTick - stTick) > waitTime) // 현재 시간과 시작 시간의 차이가 waitTime을 초과하는지 확인. 만약 초과하면 ACk를 받지 못한 것으로 간주
		{
			return FALSE;
		}

		ProcessMessage(); // ACK를 기다리는 동안 다른 메시지를 처리하는 함수. 이 호출은 프로그램이 응답성을 유지하도록 도와준다.
	}
	return FALSE;
}

BOOL CHseAgingApp::procParseCableOpenCheck(int rack, int layer, CString packet) // 패킷을 파싱하여 케이블 개방 상태를 확인하는 함수
{
	int retcode; // 패킷에서 읽은 값을 저장하는 변수

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); // 패킷의 특정 위치에서 16진수로 값을 읽어 recode에 저장한다. PACKET_PT_PET는 패킷에서 변환 코드의 시작 위치를 나타낸다.

	if (retcode == 0) // retcode가 0인지 확인한다. 0이면 케이블이 개방되지 않은 상태로 간주
	{
		int ptr = PACKET_PT_DATA; // ptr변수를 선언하고, 패킷에서 데이터가 시작되는 위치를 PACKET_PT_DATA로 설정한다

		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 최대 레이어 채널 수만큼 반복하는 루프를 시작
		{
			int idx = ptr + ch; // 현재 채널에 대한 인덱스를 계산하여 idx 변수에 저장
			lpInspWorkInfo->m_ast_CableOpenCheck[rack][layer][ch] = _ttoi(packet.Mid(idx, 1)); // 패킷의 현재 인덱스에서 1바이트를 읽어
			// 정수로 변환한 후, IpInspWorkInfo의 케이블 개방 체크 배열에 저장
		}
	}

	return TRUE;
}

BOOL CHseAgingApp::procParseAgingStatus(int rack, int layer, CString packet) // 패킷을 파싱하여 상태 정보 처리, 특정 레이어와 랙에 대한 상태 정보를 업데이트하는 함수
{
	CString sdata, sRet, sLayer, sStart, chUse, chOnOff, sCable, sErrInfo; // 패킷에서 읽은 데이터와 상태 정보를 저장하는 데 사용되는 변수
	int dataLen, errCnt, retcode; // 길이, 오류 개수, 반환 코드를 저장하는 변수

#if 0
	//packet.Format(_T("%cA2A100AB001E000000000000001010004010500070B5%c"), 0x02, 0x03);
	sLayer = _T("0");
	sStart = _T("0");
	chUse = _T("0000");		// 0:Use, 1:Unuse
	chOnOff = _T("0000");	// 0:PowerOff, 1:PowerOn
	sCable = _T("0000");	// 0:Connect, 1:Open
	//sErrInfo = _T("1010004010500070");
	packet.Format(_T("%cA2A100AB000E0%s%s%s%s%s%sB5%c"), 0x02, sLayer, sStart, chUse, chOnOff, sCable, sErrInfo, 0x03);
#endif

	dataLen = _tcstol(packet.Mid(PACKET_PT_LEN, 4), NULL, 16); // 패킷의 길이를 16진수로 읽어 dataLen에 저장. PACKET_PT_LEN은 패킷에서 길이 정보의 시작 위치를 나타낸다.
	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); // 패킷의 반환 코드를 16진수로 읽어 retcode에 저장. PACKET_PT_PET은 반환 코드의 시작 위치
	if (dataLen != 15) // 패킷의 길이가 15가 아닌 경우를 확인. 이 조건이 참이면 다음 줄로 이동
	{
		int a = 1;
		a++; // 이 코드는 디버깅을 위한 코드로 보이며, a 변수를 증가 실제로 아무 동작x
	}
	errCnt = (dataLen - 14) / 8; // 오류 개수를 계산. 패킷 길이에서 14를 빼고 8로 나누어 오류 개수를 구한다.

	if (retcode == 0) // 반환 코드가 0인지 확인. 0이면 정상 상태로 간주
	{
		int nVal, ptr = 0, len = 5; // 정수형 변수 nVal과 ptr을 선언. ptr은 패킷 데이터 위치를 추적하는 데 사용된다.

		ptr = PACKET_PT_DATA; // ptr을 패킷 데이터의 시작 위치로 설정

		/********************************************************************************/
		// Aging Layer Error Status
		sdata = packet.Mid(ptr, 1); // 패킷에서 현재 위치(ptr)의 1바이트를 읽어 sdata에 저장
		lpInspWorkInfo->m_ast_AgingLayerError[rack][layer] = _ttoi(sdata); // 읽은 데이터를 정수로 변환하여 m_ast_AgingLayerError 배열에 저장. 이는 레이어의 오류 상태를 나타낸다.
		ptr++; // ptr을 증가시켜 다음 데이터 위치로 이동

		// Aging Start/Stop Info
		sdata = packet.Mid(ptr, 1); // 현재 위치에서 1바이트를 읽어 sdata에 저장
		lpInspWorkInfo->m_ast_AgingStartStop[rack][layer] = _ttoi(sdata); // 읽은 데이터를 정수로 변환하여 m_ast_AgingStartStop 배열에 저장. 이는 시작/정지 정보를 나타낸다.
		ptr++; // ptr을 증가시켜 다음 데이터 위치로 이동

		// Channel Use/Unuse Status
		sdata = packet.Mid(ptr, 4); // 현재 위치에서 4바이트를 읽어 sdata에 저장
		nVal = _tcstol(sdata, NULL, 16); // 읽은 4바이트를 16진수로 변환하여 nVal에 저장
		ptr += 4; // ptr을 4만큼 증가시켜 다음 데이터 위치로 이동
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 최대 레이어 채널 수만큼 반복하는 루프를 시작
		{
			lpInspWorkInfo->m_ast_ChUseUnuse[rack][layer][ch] = (nVal >> ch) & 0x0001; // nVal의 각 비트를 확인하여 채널 사용 여부를 m_ast_ChUseUnuse 배열에 저장
		}

		// Channel On/Off Status
		sdata = packet.Mid(ptr, 4); // 현재 위치에서 4바이트를 읽어 sdata에 저장
		nVal = _tcstol(sdata, NULL, 16); // 읽은 4바이트를 16진수로 변환하여 nVal에 저장
		ptr += 4; // ptr을 4만큼 증가시켜 다음 데이터 위치로 이동.
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 최대 레이어 채널 수만큼 반복하는 루프를 시작
		{
			lpInspWorkInfo->m_ast_AgingChOnOff[rack][layer][ch] = (nVal >> ch) & 0x0001; // nVal의 각 비트를 확인하여 채널의 전원 상태를 m_ast_AgingChOnOff 배열에 저장
		}

		// Cable Open Status
		sdata = packet.Mid(ptr, 4); // 현재 위치에서 4바이트를 읽어 sdata에 저장
		nVal = _tcstol(sdata, NULL, 16); // 읽은 4바이트를 16진수로 변환하여 nVal에 저장
		ptr += 4; // ptr을 4만큼 증가시켜 다음 데이터 위치로 이동
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 최대 레이어 채널 수만큼 반복하는 루프를 시작
		{
			lpInspWorkInfo->m_ast_CableOpenCheck[rack][layer][ch] = (nVal >> ch) & 0x0001; // nVal의 각 비트를 확인하여 케이블 개방 상태를 m_ast_CableOpenCheck 배열에 저장
		}

		for(int i = 0; i < errCnt; i++) // 오류 개수만큼 반복하는 루프를 시작
		{
			int errResult, errGroup, errCh, errValue; // 오류 결과, 그룹, 채널, 값을 저장할 정수형 변수를 선언
			sdata = packet.Mid(ptr, 1);		errResult = _ttoi(sdata);		ptr += 1; // 현재 위치에서 1바이트를 읽어 오류 결과를 저장하고 ptr을 증가시킨다.
			sdata = packet.Mid(ptr, 1);		errGroup = _ttoi(sdata);		ptr += 1;
			sdata = packet.Mid(ptr, 1);		errCh = _ttoi(sdata);			ptr += 1;
			sdata = packet.Mid(ptr, 5);		errValue = _ttoi(sdata);		ptr += 5;

			if (errGroup == 0)	// VCC 0-7 (오류 그룹이 0인지 확인한다)
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh] = errResult; // 오류 결과를 해당 채널에 저장
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh] = ERR_INFO_VCC; // 오류 유형을 VCC로 설정
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh] = errValue; // 오류 값을 해당 채널에 저장
			}
			else if (errGroup == 1)	// VCC 8-15 (오류 그룹이 1인지 확인)
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh + 8] = errResult;
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh + 8] = ERR_INFO_VCC;
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh + 8] = errValue;
			}
			else if (errGroup == 2)	// ICC 0-7
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh] = errResult;
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh] = ERR_INFO_ICC;
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh] = errValue;
			}
			else if (errGroup == 3)	// ICC 8-15
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh + 8] = errResult;
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh + 8] = ERR_INFO_ICC;
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh + 8] = errValue;
			}
			else if (errGroup == 4)	// VBL 0-7
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh] = errResult;
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh] = ERR_INFO_VBL;
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh] = errValue;
			}
			else if (errGroup == 5)	// VBL 8-15
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh + 8] = errResult;
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh + 8] = ERR_INFO_VBL;
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh + 8] = errValue;
			}
			else if (errGroup == 6)	// IBL 0-7
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh] = errResult;
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh] = ERR_INFO_IBL;
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh] = errValue;
			}
			else if (errGroup == 7)	// IBL 8-15
			{
				lpInspWorkInfo->m_ast_AgingChErrorResult[rack][layer][errCh + 8] = errResult;
				lpInspWorkInfo->m_ast_AgingChErrorType[rack][layer][errCh + 8] = ERR_INFO_IBL;
				lpInspWorkInfo->m_ast_AgingChErrorValue[rack][layer][errCh + 8] = errValue;
			}
		}
	}

	return TRUE;
}

BOOL CHseAgingApp::procParseMeasurePower(int rack, int layer, CString packet) // 패킷을 파싱하여 전압 및 전류 측정 값을 처리하는 함수
{
	int retcode; // 반환 코드를 저장할 정수형 변수 선언

	// Measure Packet 잘못 입력 되었을 경우의 예외처리
	if (packet.GetLength() < 320) // 패킷의 길이가 320 바이트 미만인지 확인
		return FALSE; // 패킷의 길이가 320 바이트 미만이면 함수는 FALSE를 반환하여 종료. 이는 잘못된 패킷을 처리하기 위한 예외 처리

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); // 패킷에서 반환 코드를 16진수로 읽어 retcode에 저장. PACKET_PT_PET은 반환 코드의 시작 위치이다

	if (retcode == 0) // 반환 코드가 0인지 확인. 0이면 정상 상태로 간주
	{
		int ptr = 0, len = 5; // 포인터 ptr과 길이 len을 선언. ptr은 패킷 데이터 위치르 추적하는데 사용되며, len은 각 데이터 항목의 길이를 나타낸다.
		CString sdata = _T(""); // 빈 CString 변수를 선언하여 나중에 데이터를 저장하는 데 사용된다.

		ptr = PACKET_PT_DATA; // ptr을 패킷 데이터의 시작 위치로 설정

		/********************************************************************************/
		//전압전류
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 최대 레이어 채널 수만큼 반복하는 루프를 시작
		{
			//ptr += len;	len = 5;
			sdata = packet.Mid(ptr, len); // 현재 위치에서 len 바이트를 읽어 sdata에 저장한다		
			lpInspWorkInfo->m_nMeasVCC[rack][layer][ch] = _ttoi(sdata);  // 읽은 데이터를 정수로 변환하여 m_nMeasVCC 배열에 저장한다. 이는 전압 전류 측정 값을 나타낸다.
			ptr += len; // ptr을 len만큼 증가시켜 다음 데이터 위치로 이동.
		}

		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 
		{
			//ptr += len;	len = 5;
			sdata = packet.Mid(ptr, len);		lpInspWorkInfo->m_nMeasICC[rack][layer][ch] = _ttoi(sdata); // 이는 전류 측정 값을 나타낸다.
			ptr += len;
		}

		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			//ptr += len;	len = 5;
			sdata = packet.Mid(ptr, len);		lpInspWorkInfo->m_nMeasVBL[rack][layer][ch] = _ttoi(sdata); // 이는 전압 배터리 측정 값을 나타낸다.
			ptr += len;
		}

		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			//ptr += len;	len = 5;
			sdata = packet.Mid(ptr, len);		lpInspWorkInfo->m_nMeasIBL[rack][layer][ch] = _ttoi(sdata); // 이는 전류 배터리 측정 값을 나타낸다.
			ptr += len;
		}
	}

	return TRUE;
}

BOOL CHseAgingApp::procParseFWVersion(int rack, int layer, CString packet) // 패킷에서 펌웨어 버전을 파싱하여 저장하는 함수, 특정 레이어와 랙에 대한 펌웨어 버전 정보를 업데이트하고, 이를 요약 정보에 기록
{
	int retcode; // 반환 코드를 저장할 정수형 변수를 선언

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); // 패킷에서 반환 코드를 16진수로 읽어 retcode에 저장. PACKET_PT_PET은 반환 코드의 시작 위치이다.

	if (retcode == 0) // 반환 코드가 0인지 확인. 0이면 정상 상태로 간주
	{
		CString sdata; // CString 변수를 선언하여 나중에 데이터를 저장하는 데 사용
		sdata = packet.Mid(PACKET_PT_DATA, (packet.GetLength() - 17)); // 패킷에서 데이터 부분을 읽어 sdata에 저장. 데이터의 길이는 패킷 길이에서 17을 뺀 값이다.

		lpInspWorkInfo->m_sMainFWVersion[rack][layer] = sdata.Left(10); // 읽은 데이터의 왼쪽 10자를 m_sMainFWVersion 배열에 저장. 이는 펌웨어 버전을 나타낸다.


		CString skey = _T(""); // 빈 CString 변수를 선언하여 나중에 키를 저장하는 데 사용.
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++) // 최대 레이어 채널 수만큼 반복하는 루프를 시작.
		{
			m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_FW_VER] = lpInspWorkInfo->m_sMainFWVersion[rack][layer]; // 펌웨어 버전을 요약 정보의 해당 위치에 저장
			skey.Format(_T("RACK%d_LAYER%d_CH%d"), rack + 1, layer + 1, ch + 1); // skey를 포맷하여 현재 랙, 레이어, 채널 번호를 포함하는 문자열을 생성
			Write_SummaryInfo(_T("FIRMWARE"), skey, m_pApp->m_summaryInfo[rack][layer][ch].m_sumData[SUM_FW_VER]); // 펌웨어 정보를 요약 정보에 기록. Write_SummaryInfo 함수는 "FIRMWARE"라는 카테고리와 함께 skey및 펌웨어 버전을 기록한다.
		}
	}

	return TRUE;
}

BOOL CHseAgingApp::procParseGotoBootSection(int rack, int layer, CString packet) // 패킷을 파싱하여 부트 섹션으로 전환을 처리하는 함수. 특정 랙과 레이어에 대한 다운로드 상태를 업데이트
{
	int retcode; // 반환 코드를 저장할 정수형 변수를 선언

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); //패킷에서 반환 코드를 16진수로 읽어 retcode에 저장. PACKET_PT_RET은 반환 코드의 시작 위치이다.
	if (retcode == 0) // 반환 코드가 0인지 확인. 0이면 정상 상태로 간주
	{
		if (m_nDownloadCountUp == TRUE) // 다운로드 카운트가 증가 중인지 확인. m_nDownloadCountUp이 TRUE이면 다운로드가 진행 중이다.
		{
			m_nDownloadCountUp = FALSE; // 다운로드 카운트를 증가시키는 상태를 FALSE로 설정하여 다운로드가 완료되었음을 나타낸다.
			m_nDownloadReadyAckCount = m_nDownloadReadyAckCount + 1; // 다운로드 준비 확인 응답 카운트를 1 증가시킨다. 이는 다운로드 준비가 완료되었음을 나타낸다.
		}
	}
	return TRUE;
	// 이 함수는 패킷에서 부트 섹션으로의 전환을 처리하고, 다운로드 상태를 업데이트 하여 시스템의 다운로드 프로세스를 관리하는 역할을 수행한다.
}

BOOL CHseAgingApp::udp_sendPacketUDP(CString ip, int nCommand, int nSize, char* pdata, int recvACK, int waitTime) // 주어진 IP 주소로 UDP 패킷을 생성하고 전송하는 기능을 수행
{ // 패킷의 체크섬을 계산하고, ACK(응답)를 수신하는 기능도 포함되어 잇다.
	int datalen = 0; // 데이터 길이를 저장할 변수
	int packetlen = 0; // 패킷의 전체 길이를 저장할 변수
	BYTE nChkSum = 0; // 체크섬을 저장할 바이트형 변수
	char szbuff[5] = { 0, }; // 체크섬과 종료 문자를 저장할 버퍼
	char sendPacket[PACKET_SIZE] = { 0, }; // 전송할 패킷을 저장할 배열

	datalen = nSize; // 전송할 데이터의 길이를 datalen에 저장

	// data 앞까지 Packet 생성
	sprintf_s(sendPacket, "%cA1%02X00%02X%04X", PACKET_STX, TARGET_MAIN, nCommand, datalen); // 패킷의 헤더를 생성하여 sendPacket에 저장.
	// 헤더에는 시작 문자, 타겟, 명령, 데이터 길이가 포함되어 있다.

	// data를 포함하여 packet 생성. hex로 전송할 data가 있으므로 memcpy를 사용
	packetlen = (int)strlen(sendPacket); // 현재 패킷의 길이를 계산하여 packetlen에 저장

	memcpy(&sendPacket[packetlen], pdata, datalen); // 데이터(pdata)를 패킷에 추가한다. memcpy를 사용하여 데이터를 복사한다.

	// data 를 포함한 packet의 길이를 구한다.
	packetlen += datalen; // 패킷의 전체 길이에 데이터 길이를 추가한다.

	// 생성된 Packet을 이용하여 CheckSum을 구한다.
	for (int j = 1; j < packetlen; j++)		// Check Sum
	{
		nChkSum += sendPacket[j]; // 패킷의 각 바이트를 체크섬에 더한다.
	}
	sprintf_s(szbuff, "%02X%c", nChkSum, 0x03); // 체크섬과 종료 문자를 포맷하여 zbuff에 저장한다.

	// Checksum과 ETX 3byte를 붙여 다시 Packet을 만든다.
	memcpy(&sendPacket[packetlen], szbuff, 3); // 체크섬과 종료 문자를 패킷에 추가한다.
	packetlen += 3; // 패킷의 전체 길이에 체크섬과 종료 문자의 길이를 추가한다.

	// Packet의 마지막에 String의 끝을 알리기 위하여 NULL을 추가한다.
	sendPacket[packetlen] = 0x00; // 패킷의 끝에 NULL 문자를 추가하여 문자열의 끝을 표시한다.

	// Send Log를 기록
#if	(DEBUG_COMM_LOG==1) // 디버그 로그가 활성화된 경우 로그를 기록한다.
	CString sLog; // 로그 메시지를 저장할 CString 변수를 선언
	sLog.Format(_T("<UDP Send> [%s] %S"), ip, sendPacket); // 로그 메시지를 포맷하여 sLog에 저장한다.
	m_pApp->Gf_writeMLog(sLog); // 로그 메시지를 기록하는 함수를 호출한다.
#endif

	// 생성된 Packet을 전송.
	UINT ret = TRUE; // 전송 결과를 저장할 변수를 초기화한다.

	m_pUDPSocket->SendToUDP(ip, packetlen, sendPacket); // 생성된 패킷을 지정된 IP 주소로 전송한다.

	int rack=-1, layer=-1; // 랙과 레이어 정보를 저장할 변수를 초기화
	Lf_IPStringToRackInfo(ip, &rack, &layer); // IP 주소를 랙과 레이어 정보로 변환한다.

	// ACK Receive
	if ((rack == -1) && (layer == -1)) // 랙과 레이어 정보가 유효하지 않은 경우를 확인한다.
		return FALSE;

	m_nAckCmdPG[rack][layer] = 0; // 해당 랙과 레이어의 ACK 명령을 초기화한다.
	if (recvACK == ACK) // ACK 수신 여부를 확인한다.
	{
		if (procWaitRecvACK(rack, layer, nCommand, waitTime) == TRUE) // ACK를 기다리는 함수를 호출하여 성공 여부를 확인한다.
			ret = TRUE; // ACK를 성공적으로 수신한 경우 ret을 TRUE로 설정
		else // ACK를 수신하지 못한 경우
			ret = FALSE; // ret을 FALSE로 설정
	}

	return ret; // 최종 결과를 반환한다.
	// 이 함수는 UDP 패킷을 생성하고 전송하며, 필요한 경우 ACK를 수신하여 전송의 성공 여부를 확인하는 역할을 수행한다.
}

BOOL CHseAgingApp::udp_sendPacketUDPRack(int rack, int nCommand, int nSize, char* pdata, int recvACK, int waitTime)
{ // 주어진 랙 번호에 대해 여러 레이어에 UDP 패킷을 전송하고, 필요에 따라 ACK(응답)을 수신하는 기능을 수행.
  // 이 함수는 각 레이어에 대해 IP 주소를 생성하고, 패킷을 전송한 후 ACK를 확인한다.
	BOOL bRet = TRUE; // 함수의 반환 값을 저장할 변수를 초기화한다.
	CString ipaddr = _T(""); // IP 주소를 저장할 CString 변수를 선언하고 초기화한다.
	int baseip = (rack*MAX_LAYER) + 1; // 주어진 랙 번호에 따라 기본 Ip 주소를 계산한다.

	ZeroMemory(lpInspWorkInfo->m_nRecvACK_Rack[rack], sizeof(lpInspWorkInfo->m_nRecvACK_Rack[rack])); // 해당 랙의 ACK 수신 정보를 초기화한다.

	for (int layer = 0; layer < MAX_LAYER; layer++) // 각 레이어에 대해 반복한다
	{
		if(lpInspWorkInfo->m_nMainEthConnect[rack][layer] != 0) // 현재 레이어의 연결 상태를 확인한다.
			lpInspWorkInfo->m_nMainEthConnect[rack][layer]--; // 연결 상태가 유효한 경우, 연결 카운트를 감소시킨다.

		ipaddr.Format(_T("192.168.10.%d"), (baseip + layer)); // 현재 레이어에 대한 IP 주소를 생성하여 ipaddr에 저장한다.
		udp_sendPacketUDP(ipaddr, nCommand, nSize, pdata, NACK); // 생성된 IP 주소로 UDP 패킷을 전송한다. ACK는 NACK으로 설정한다.
	}

	// ACK를 Check한다.
	if (recvACK == ACK) // ACK 수신 여부를 확인한다.
	{
		DWORD sTick, eTick; // 시작 및 종료 시간을 저장할 변수를 선언한다
		sTick = ::GetTickCount(); // 현재 시간을 시작 시간으로 저장한다.

		while (1)
		{
			BOOL bAllAck = TRUE; // 모든 ACK 수신 여부를 확인할 변수를 초기화한다.
			for (int layer = 0; layer < MAX_LAYER; layer++) // 각 레이어에 대해 반복한다.
			{
				if ((lpInspWorkInfo->m_nMainEthConnect[rack][layer]) && (lpInspWorkInfo->m_nRecvACK_Rack[rack][layer] == FALSE)) // 현재 레이어가 연결되어 있고 ACK를 수신하지 않은 경우를 확인한다.
				{
					bAllAck = FALSE; // ACK를 수신하지 않은 경우 bAllAck를 FALSE로 설정한다.
				}
			}

			// 모든 Main에서 ACK가 전송 되었으면 Return TRUE
			if (bAllAck == TRUE) // 모든 레이어에서 ACK를 수신한 경우를 확인한다.
				return TRUE; // 모든 ACK가 수신된 경우 TRUE를 반환하여 함수 종료한다.

			// ACK Wait Time Check
			eTick = ::GetTickCount(); // 현재 시간을 종료 시간으로 저장한다.
			if ((eTick - sTick) > (DWORD)waitTime) // 대기 시간이 초과된 경우를 확인한다.
				return FALSE; // 대기 시간이 초과된 경우 FALSE를 반환하여 함수를 종료한다.

			ProcessMessage(); // 메시지를 처리하는 함수를 호출하여 다른 이벤트를 처리한다.
		}
	}

	return TRUE;

	// 이 함수는 주어진 랙 번호에 대해 각 레이어에 UDP 패킷을 전송하고, ACK를 수신하여 모든 레이어에서 응답이 있는지를 확인하는 역할을 수행한다.
}


void CHseAgingApp::udp_processPacket(CString strPacket) // 수신된 UDP 패킷을 처리하는 기능을 수행한다.
// 이 함수는 패킷에서 IP 주소와 명령어를 추출하고, 해당 명령어에 따라 적절한 처리 함수를 호출한다.
{
	CString recvPacket; // 수신된 패킷의 내용을 저장할 변수
	CString recvIP; // 수신된 IP 주소를 저장할 CString 변수
	int IPaddr; // IP 주소의 정수 값을 저장할 변수
	int rack = 0; // 랙 번호를 저장할 변수를 초기화
	int layer = 0; // 레이어 번호를 저장할 변수를 초기화
	int cmd = 0; // 명령어를 저장할 변수를 초기화
	int ntoken = 0; // 패킷에서 구분자를 찾기 위한 변수를 초기화
	int target = 0; // 타겟 값을 저장할 변수를 초기화

	ntoken = strPacket.Find(_T("#")); // 패킷에서 '#' 문자의 위치를 찾는다
	if (ntoken == -1)	return; // '#'문자가 없으면 함수 종료

	recvIP = strPacket.Left(ntoken); // 패킷의 왼쪽 부분에서 IP 주소를 추출한다.
	recvPacket = strPacket.Mid(ntoken + 1); // 패킷의 오른쪾 부분에서 나머지 데이터를 추출한다.

	IPaddr = _ttoi(recvIP.Right(recvIP.GetLength() - recvIP.ReverseFind(_T('.')) - 1)); // IP 주소의 마지막 옥텟을 정수로 변환한다.
	if (IPaddr == 0xFF)	return; // IP 주소가 255인 경우 함수를 종료한다.

	rack = (IPaddr-1) / MAX_LAYER; // IP 주소를 기반으로 랙 번호를 계산한다.
	layer = (IPaddr-1) % MAX_LAYER; // IP 주소를 기반으로 레이어 번호를 계산한다.

	target = _tcstol(recvPacket.Mid(PACKET_PT_SOURCE, 2), NULL, 16); // 패킷에서 소스 정보를 추출하여 정수로 변환한다.
	cmd = _tcstol(recvPacket.Mid(PACKET_PT_CMD, 2), NULL, 16); // 패킷에서 명령어를 추출하여 정수로 변환한다.

	if (IPaddr <= (MAX_RACK * MAX_LAYER)) // IP 주소가 유효한 범위 내에 있는지 확인한다.
	{
		lpInspWorkInfo->m_nMainEthConnect[rack][layer] = 5;	// Connect status count reset
	}

	// Message 처리
	switch (cmd) // 명령어에 따라 분기한다.
	{
		case CMD_SET_CABLE_OPEN_CHECK:	// 0xA5 
		{
			procParseCableOpenCheck(rack, layer, recvPacket); // 해당 명령어에 대한 처리 함수를 호출한다.
			break;
		}
		case CMD_GET_AGING_STATUS:		// 0xAB
		{
			procParseAgingStatus(rack, layer, recvPacket);
			break;
		}
		case CMD_GET_POWER_MEASURE:		// 0xBD
		{
			procParseMeasurePower(rack, layer, recvPacket);
			break;
		}
		case CMD_GET_FW_VERSION:		// 0xFE
		{
			procParseFWVersion(rack, layer, recvPacket);
			break;
		}
		case CMD_GOTOBOOT_DSECTION:
		{
			procParseGotoBootSection(rack, layer, recvPacket);
			break;
		}
#if 0
		case CMD_GET_CAL_ALL_POWER_MEASURE:
		{
			procParseMeasurePower(grp, ma, ch1, ch2, recvPacket);
			break;
		}
		case CMD_SET_BMP_DONE_CHECK:
		{
			procParseDoneCheck(grp, ma, recvPacket);
			break;
		}
		case CMD_SRUN_STATUS_READ:
		{
			procParseSRunnerStatus(grp, ma, recvPacket);
			break;
		}
		case CMD_SRUN_DATA_WRITE:
		{
			procParseSRunnerWriteRet(grp, ma, recvPacket);
			break;
		}
		case CMD_SET_FUSINGSYSTEMINFO:
		{
			procParseFusingSystem(grp, ma, recvPacket);
			break;
		}
		case  CMD_GET_TCON_ABD_RESULT:
		{
			procParseTconABDResult(grp, ma, recvPacket);
			break;
		}
		case CMD_GET_CAL_READ_ADC:
		{
			procParseCalibrationReadADC(grp, ma, recvPacket);
			break;
		}
		case CMD_SET_CAL_ERASE_DATA:
		{
			procParseCalibrationErase(grp, ma, recvPacket);
			break;
		}
		case CMD_SET_CAL_VOLTAGE:
		{
			procParseCalibrationVoltage(grp, ma, recvPacket);
			break;
		}
		case CMD_SET_CAL_CURRENT:
		{
			procParseCalibrationCurrent(grp, ma, recvPacket);
			break;
		}
		case  CMD_GET_PANEL_SYNC_MEASURE:
		{
			procParsePanelSyncMeasure(grp, ma, recvPacket);
			break;
		}

		case CMD_GET_FPGA_VERSION:
		{
			procParseFpgaVersion(grp, ma, recvPacket);
			break;
		}

		case CMD_TIME_OUT:
		{
			m_nAckCmd = cmd; // 타임아웃 명령어를 저장한다.
			lpWorkInfo->m_nRecvCmdACK[ma] = FALSE; // 해당 명령어의 ACK 수신 상태를 FALSE로 설정한다.
			if (grp == GROUP_A) { m_nAckCmd_A = cmd;		lpWorkInfo->m_nRecvACK_A[ma] = FALSE; } // 그룹 A의 경우 ACK 상태를 설정한다.
			else if (grp == GROUP_B) { m_nAckCmd_B = cmd;		lpWorkInfo->m_nRecvACK_B[ma] = FALSE; } // 그룹 B의 경우 ACK 상태를 설정한다.
			else if (grp == GROUP_C) { m_nAckCmd_C = cmd;		lpWorkInfo->m_nRecvACK_C[ma] = FALSE; }
			else if (grp == GROUP_D) { m_nAckCmd_D = cmd;		lpWorkInfo->m_nRecvACK_D[ma] = FALSE; }
			else if (grp == GROUP_E) { m_nAckCmd_E = cmd;		lpWorkInfo->m_nRecvACK_E[ma] = FALSE; }
			else if (grp == GROUP_F) { m_nAckCmd_F = cmd;		lpWorkInfo->m_nRecvACK_F[ma] = FALSE; }
			else if (grp == GROUP_G) { m_nAckCmd_G = cmd;		lpWorkInfo->m_nRecvACK_G[ma] = FALSE; }
			else if (grp == GROUP_H) { m_nAckCmd_H = cmd;		lpWorkInfo->m_nRecvACK_H[ma] = FALSE; }
			return;
		}
#endif
	}

	// ACK Receive Check는 모든 Packet처리가 완료된 이후 Set한다.
	m_nAckCmdPG[rack][layer] = cmd; // 처리된 명령어를 저장한다.
	lpInspWorkInfo->m_nRecvACK_Rack[rack][layer] = TRUE; // 해당 랙과 레이어에서 ACK 수신 상태를 TRUE로 설정한다.
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::procDioParseBoardInitial(CString packet) // 특정 패킷을 처리하여 보드 초기화 상태를 설정하는 기능을 수행하는 함수
{
	int retcode; // 반환 코드를 저장할 변수를 선언

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); // 패킷에서 반환 코드를 추출하여 정수로 변환
	//if (retcode == 0)
	{
		lpInspWorkInfo->m_nDioNeedInitial = FALSE; // 보드 초기화가 필요하지 않음을 설정한다.
	}
	// 패킷을 처리하여 보드 초기화 상태를 업데이트하는 역할을 한다.
}

void CHseAgingApp::procDioParseInputRead(CString packet) // 입력 패킷을 파싱하여 DIO 입력 데이터를 읽고 저장하는 역할을 하는 함수
{
	int retcode;

	if (packet.GetLength() < 25) // 패킷의 길이가 25보다 짧으면
		return;

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); // 패킷의 특정 위치(PACKET_PT_PET)에서 1글자를 16진수로 변환하여 retcode에 저장
	if (retcode == 0) // retcode가 0이면 (성공적인 응답)
	{
		int ptr = PACKET_PT_DATA; // 데이터 포인터를 PACKET_PT_DATA로 초기화

		for (int i = 0; i < 1; i++)
		{
			ptr = ptr + i; // 포인터를 증가 (i가 0이므로 ptr은 변하지 않음)
			lpInspWorkInfo->m_nDioInputData[i] = _ttoi(packet.Mid(ptr, 1)); // 패킷의 특정 위치에서 1글자를 정수로 변환하여 m_nDioInputData 배열에 저장
		}
	}
}

void CHseAgingApp::procDioParseFWVersion(CString packet) // 입력 패킷에서 펌웨어 버전을 파싱하여 저장하는 역할을 하는 함수
{
	int retcode;

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16); // 패킷의 특정 위치(PACKET_PT_RET)에서 1글자를 16진수로 변환하여 retcode에 저장

	if (retcode == 0) // retcode가 0이면 (성공적인 응답)
	{
		CString sdata; // 문자열 변수를 선언
		sdata = packet.Mid(PACKET_PT_DATA, (packet.GetLength() - 17)); // 패킷의 특정 위치(PACKET_PT_DATA)에서 (패킷 길이 - 17) 만큼의 문자열을 sdata에 저장

		lpInspWorkInfo->m_sDioFWVersion = sdata.Left(10); // sdata의 왼쪽 10글자를 m_sDioFWVersion에 저장
	}
}

void CHseAgingApp::procDioParseGotoBootSection(CString packet)
{
	int retcode;

	retcode = _tcstol(packet.Mid(PACKET_PT_RET, 1), NULL, 16);
	if (retcode == 0)
	{
		if (m_nDownloadCountUp == TRUE)
		{
			m_nDownloadCountUp = FALSE;
			m_nDownloadReadyAckCount = m_nDownloadReadyAckCount + 1;
		}
	}
}

void CHseAgingApp::udp_processDioPacket(CString strPacket) // 수신된 DIO 패킷을 처리하는 역할을 한다.
{
	CString recvPacket; // 수신된 패킷 데이터를 저장할 변수 recvPacket 선언
	CString recvIP; // 수신된 IP 주소를 저장할 변수 recvIP 선언
	int cmd = 0; // 명령어를 저장할 변수 cmd 선언
	int ntoken = 0; // '#' 문자의 위치를 저장할 변수 ntoken 선언
	int target = 0; // 타겟을 저장할 변수 target 선언 (현재 사용되지 않음)

	ntoken = strPacket.Find(_T("#")); // strPacket에서 '#' 문자의 위치를 찾음
	if (ntoken == -1) return; // '#' 문자가 없으면 함수 종료

	recvIP = strPacket.Left(ntoken); // '#' 이전의 문자열을 recvIP에 저장 (IP 주소)
	recvPacket = strPacket.Mid(ntoken + 1); // '#' 이후의 문자열을 recvPacket에 저장 (패킷 데이터)
	cmd = _tcstol(recvPacket.Mid(PACKET_PT_CMD, 2), NULL, 16); // 패킷의 명령어 부분을 16진수로 변환하여 cmd에 저장

	lpInspWorkInfo->m_nConnectInfo[CONNECT_DIO] = 3; // DIO 연결 정보를 3으로 설정

	// Message 처리
	switch (cmd) // cmd에 따라 다른 처리를 수행
	{
	case CMD_DIO_OUTPUT: // LAMP Set 명령 처리
	{
		break; // 현재는 아무 동작도 하지 않음
	}
	case CMD_DIO_BOARD_INITIAL: // DIO Board Initial 응답 처리
	{
		procDioParseBoardInitial(recvPacket); // 응답을 파싱하는 함수 호출
		break;
	}
	case CMD_DIO_INPUT: // DIO Read 명령 처리
	{
		procDioParseInputRead(recvPacket); // 입력을 파싱하는 함수 호출
		break;
	}
	case CMD_GET_FW_VERSION: // 펌웨어 버전 요청 처리
	{
		procDioParseFWVersion(recvPacket); // 펌웨어 버전을 파싱하는 함수 호출
		break;
	}
	case CMD_GOTOBOOT_DSECTION: // 부트 섹션으로 이동 명령 처리
	{
		procDioParseGotoBootSection(recvPacket); // 부트 섹션으로 이동하는 함수 호출
		break;
	}
	}
	// ACK Receive Check는 모든 Packet처리가 완료된 이후 Set한다.
	int rack, layer; // 랙과 레이어 정보를 저장할 변수 선언
	Lf_IPStringToRackInfo(recvIP, &rack, &layer); // recvIP에서 랙과 레이어 정보를 추출
	m_nAckCmdPG[rack][layer] = cmd; // 해당 랙과 레이어에 cmd를 저장
	//m_nAckCmdDIO = cmd; // 주석 처리된 코드 (현재 사용되지 않음)

	// 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::Gf_LoadSystemData() // 시스템 데이터를 초기화하기 위해 설정 파일에서 정보를 읽어오는 역할을 한다.
{
	CString skey, sdata = _T(""); // skey와 sdata 변수를 선언 (sdata는 빈 문자열로 초기화)

	// SYSTEM 섹션에서 다양한 설정 값을 읽어 lpSystemInfo 구조체에 저장
	Read_SysIniFile(_T("SYSTEM"), _T("CHAMBER_NO"), &lpSystemInfo->m_sChamberNo); // 챔버 번호 읽기
	Read_SysIniFile(_T("SYSTEM"), _T("EQP_NAME"), &lpSystemInfo->m_sEqpName); // 장비 이름 읽기
	Read_SysIniFile(_T("SYSTEM"), _T("TEMP_RECORDER_PORT"), &lpSystemInfo->m_nTempRecorderPort); // 온도 기록기 포트 읽기
	Read_SysIniFile(_T("SYSTEM"), _T("TEMP_LOG_INTERVAL"), &lpSystemInfo->m_nTempLogInterval); // 온도 로그 간격 읽기
	lpSystemInfo->m_nMesIDType = MES_ID_TYPE_PID; // MES ID 타입을 PID로 설정

	// 각 랙에 대한 마지막 모델 이름을 읽어옴
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		skey.Format(_T("LAST_MODELNAME_RACK%d"), (rack + 1)); // LAST_MODELNAME_RACKX 형식의 키 생성
		Read_SysIniFile(_T("SYSTEM"), skey, &lpSystemInfo->m_sLastModelName[rack]); // 마지막 모델 이름 읽기
	}

	// MES 섹션에서 다양한 설정 값을 읽어 lpSystemInfo 구조체에 저장
	Read_SysIniFile(_T("MES"), _T("MES_SERVICE_PORT"), &lpSystemInfo->m_sMesServicePort); // MES 서비스 포트 읽기
	Read_SysIniFile(_T("MES"), _T("MES_NETWORK"), &lpSystemInfo->m_sMesNetWork); // MES 네트워크 읽기
	Read_SysIniFile(_T("MES"), _T("MES_DAEMON_PORT"), &lpSystemInfo->m_sMesDaemonPort); // MES 데몬 포트 읽기
	Read_SysIniFile(_T("MES"), _T("MES_LOCAL_SUBJECT"), &lpSystemInfo->m_sMesLocalSubject); // MES 로컬 주제 읽기
	Read_SysIniFile(_T("MES"), _T("MES_REMOTE_SUBJECT"), &lpSystemInfo->m_sMesRemoteSubject); // MES 원격 주제 읽기

	// EAS 섹션에서 다양한 설정 값을 읽어 lpSystemInfo 구조체에 저장
	Read_SysIniFile(_T("EAS"), _T("EAS_SERVICE_PORT"), &lpSystemInfo->m_sEasServicePort); // EAS 서비스 포트 읽기
	Read_SysIniFile(_T("EAS"), _T("EAS_NETWORK"), &lpSystemInfo->m_sEasNetWork); // EAS 네트워크 읽기
	Read_SysIniFile(_T("EAS"), _T("EAS_DAEMON_PORT"), &lpSystemInfo->m_sEasDaemonPort); // EAS 데몬 포트 읽기
	Read_SysIniFile(_T("EAS"), _T("EAS_LOCAL_SUBJECT"), &lpSystemInfo->m_sEasLocalSubject); // EAS 로컬 주제 읽기
	Read_SysIniFile(_T("EAS"), _T("EAS_REMOTE_SUBJECT"), &lpSystemInfo->m_sEasRemoteSubject); // EAS 원격 주제 읽기

	CString sSection, sKey, sValue; // 섹션, 키, 값을 저장할 변수 선언
	// 각 랙에 대한 BCR ID를 읽어옴
	for (int rack = 0; rack < MAX_RACK; rack++)
	{
		sKey.Format(_T("RACK%d_BCR_ID"), rack + 1); // RACKX_BCR_ID 형식의 키 생성
		Read_SysIniFile(_T("SYSTEM"), sKey, &lpInspWorkInfo->m_sRackID[rack]); // BCR ID 읽기

		// 각 레이어와 채널에 대한 MES PID 정보를 읽어옴
		for (int layer = 0; layer < MAX_LAYER; layer++)
		{
			for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
			{
				sSection.Format(_T("MES_PID_RACK%d"), rack + 1); // MES_PID_RACKX 형식의 섹션 생성

				sKey.Format(_T("RACK%d_LAYER%d_CH%d_USE"), rack + 1, layer + 1, ch + 1); // RACKX_LAYERY_CHZ_USE 형식의 키 생성
				Read_MesPIDInfo(sSection, sKey, &sValue); // MES PID 정보 읽기
				lpInspWorkInfo->m_bMesChannelUse[rack][layer][ch] = _ttoi(sValue); // 채널 사용 여부 저장

				sKey.Format(_T("RACK%d_LAYER%d_CH%d"), rack + 1, layer + 1, ch + 1); // RACKX_LAYERY_CHZ 형식의 키 생성
				Read_MesPIDInfo(sSection, sKey, &lpInspWorkInfo->m_sMesPanelID[rack][layer][ch]); // MES 패널 ID 읽기
			}
		}
	}
}

void CHseAgingApp::Gf_loadModelData(CString modelName)
{
	CString sdata;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Timing Set
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_MAIN_CLOCK"), &lpModelInfo->m_fTimingMainClock);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_HOR_TOTAL"), &lpModelInfo->m_nTimingHorTotal);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_HOR_ACTIVE"), &lpModelInfo->m_nTimingHorActive);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_HOR_WIDTH"), &lpModelInfo->m_nTimingHorWidth);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_HOR_BACKPORCH"), &lpModelInfo->m_nTimingHorBP);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_HOR_FRONTPORCH"), &lpModelInfo->m_nTimingHorFP);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_VER_TOTAL"), &lpModelInfo->m_nTimingVerTotal);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_VER_ACTIVE"), &lpModelInfo->m_nTimingVerActive);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_VER_WIDTH"), &lpModelInfo->m_nTimingVerWidth);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_VER_BACKPORCH"), &lpModelInfo->m_nTimingVerBP);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("TIMING_VER_FRONTPORCH"), &lpModelInfo->m_nTimingVerFP);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// LCM Set
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("LCM_SIGNAL_TYPE"), &lpModelInfo->m_nLcmSignalType);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("LCM_PIXEL_TYPE"), &lpModelInfo->m_nLcmPixelType);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("LCM_ODD_EVEN"), &lpModelInfo->m_nLcmOddEven);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("LCM_SIGNAL_BIT"), &lpModelInfo->m_nLcmSignalBit);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("LCM_BIT_SWAP"), &lpModelInfo->m_nLcmBitSwap);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("LCM_LVDS_RS_SEL"), &lpModelInfo->m_nLcmLvdsRsSel);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Inverter & PWM Set
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("DIMMING_SEL"), &lpModelInfo->m_nDimmingSel);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("PWM_FREQ"), &lpModelInfo->m_nPwmFreq);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("PWM_DUTY"), &lpModelInfo->m_nPwmDuty);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VBR_VOLT"), &lpModelInfo->m_fVbrVolt);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Function Set
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("CABLE_OPEN"), &lpModelInfo->m_nFuncCableOpen);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Power Sequence Set
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ1"), &lpModelInfo->m_nPowerOnSeq1);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ2"), &lpModelInfo->m_nPowerOnSeq2);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ3"), &lpModelInfo->m_nPowerOnSeq3);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ4"), &lpModelInfo->m_nPowerOnSeq4);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ5"), &lpModelInfo->m_nPowerOnSeq5);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ6"), &lpModelInfo->m_nPowerOnSeq6);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ7"), &lpModelInfo->m_nPowerOnSeq7);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ8"), &lpModelInfo->m_nPowerOnSeq8);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ9"), &lpModelInfo->m_nPowerOnSeq9);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ10"), &lpModelInfo->m_nPowerOnSeq10);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_SEQ11"), &lpModelInfo->m_nPowerOnSeq11);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY1"), &lpModelInfo->m_nPowerOnDelay1);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY2"), &lpModelInfo->m_nPowerOnDelay2);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY3"), &lpModelInfo->m_nPowerOnDelay3);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY4"), &lpModelInfo->m_nPowerOnDelay4);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY5"), &lpModelInfo->m_nPowerOnDelay5);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY6"), &lpModelInfo->m_nPowerOnDelay6);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY7"), &lpModelInfo->m_nPowerOnDelay7);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY8"), &lpModelInfo->m_nPowerOnDelay8);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY9"), &lpModelInfo->m_nPowerOnDelay9);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_ON_DELAY10"), &lpModelInfo->m_nPowerOnDelay10);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ1"), &lpModelInfo->m_nPowerOffSeq1);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ2"), &lpModelInfo->m_nPowerOffSeq2);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ3"), &lpModelInfo->m_nPowerOffSeq3);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ4"), &lpModelInfo->m_nPowerOffSeq4);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ5"), &lpModelInfo->m_nPowerOffSeq5);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ6"), &lpModelInfo->m_nPowerOffSeq6);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ7"), &lpModelInfo->m_nPowerOffSeq7);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ8"), &lpModelInfo->m_nPowerOffSeq8);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ9"), &lpModelInfo->m_nPowerOffSeq9);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ10"), &lpModelInfo->m_nPowerOffSeq10);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_SEQ11"), &lpModelInfo->m_nPowerOffSeq11);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY1"), &lpModelInfo->m_nPowerOffDelay1);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY2"), &lpModelInfo->m_nPowerOffDelay2);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY3"), &lpModelInfo->m_nPowerOffDelay3);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY4"), &lpModelInfo->m_nPowerOffDelay4);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY5"), &lpModelInfo->m_nPowerOffDelay5);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY6"), &lpModelInfo->m_nPowerOffDelay6);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY7"), &lpModelInfo->m_nPowerOffDelay7);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY8"), &lpModelInfo->m_nPowerOffDelay8);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY9"), &lpModelInfo->m_nPowerOffDelay9);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY10"), &lpModelInfo->m_nPowerOffDelay10);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("POWER_OFF_DELAY"), &lpModelInfo->m_nPowerOffDelay);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Power Set
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VCC_VOLT"), &lpModelInfo->m_fVccVolt);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VCC_VOLT_OFFSET"), &lpModelInfo->m_fVccVoltOffset);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VCC_LIMIT_VOLT_LOW"), &lpModelInfo->m_fVccLimitVoltLow);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VCC_LIMIT_VOLT_HIGH"), &lpModelInfo->m_fVccLimitVoltHigh);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VCC_LIMIT_CURR_LOW"), &lpModelInfo->m_fVccLimitCurrLow);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VCC_LIMIT_CURR_HIGH"), &lpModelInfo->m_fVccLimitCurrHigh);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VBL_VOLT"), &lpModelInfo->m_fVblVolt);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VBL_VOLT_OFFSET"), &lpModelInfo->m_fVblVoltOffset);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VBL_LIMIT_VOLT_LOW"), &lpModelInfo->m_fVblLimitVoltLow);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VBL_LIMIT_VOLT_HIGH"), &lpModelInfo->m_fVblLimitVoltHigh);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VBL_LIMIT_CURR_LOW"), &lpModelInfo->m_fVblLimitCurrLow);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("VBL_LIMIT_CURR_HIGH"), &lpModelInfo->m_fVblLimitCurrHigh);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Aging Set
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("AGING_TIME_HH"), &lpModelInfo->m_nAgingTimeHH);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("AGING_TIME_MM"), &lpModelInfo->m_nAgingTimeMM);
	Read_ModelFile(modelName, _T("MODEL_INFO"), _T("AGING_TIME_MINUTE"), &lpModelInfo->m_nAgingTimeMinute);
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::Gf_sumWriteSummaryLog(int rack, int layer, int channel) // 주어진 랙, 레이어, 채널에 대한 요약 로그를 CSV 파일에 기록하는 기능을 수행하는 함수
{
	FILE* fp; // 파일 포인터 선언

	BOOL bNewCsv = FALSE; // 새로운 CSV 파일인지 여부를 나타내는 변수
	char filepath[128] = { 0 }; // 파일 경로를 저장할 배열
	char buff[2048] = { 0 }; // 로그 데이터를 저장할 배열
	CString sResult = _T("NG"); // 결과 초기화 (기본값은 "NG")

	SYSTEMTIME sysTime; // 시스템 시간을 저장할 구조체
	::GetSystemTime(&sysTime); // 현재 시스템 시간을 가져옴
	CTime time = CTime::GetCurrentTime(); // 현재 시간을 CTime 형식으로 가져옴

	// 로그 디렉토리가 존재하지 않으면 생성
	if ((_access(".\\Logs\\SummaryLog", 0)) == -1)
		_mkdir(".\\Logs\\SummaryLog");

	// 파일 경로 설정 (현재 날짜를 포함)
	sprintf_s(filepath, ".\\Logs\\SummaryLog\\Summary_%04d%02d%02d.csv", time.GetYear(), time.GetMonth(), time.GetDay());
	fopen_s(&fp, filepath, "r+"); // 파일을 읽기 및 쓰기 모드로 열기
	if (fp == NULL) // 파일이 열리지 않으면
	{
		delayMs(1); // 1ms 지연
		fopen_s(&fp, filepath, "a+"); // 파일을 추가 모드로 열기
		if (fp == NULL) // 여전히 파일이 열리지 않으면
		{
			if ((_access(filepath, 2)) != -1) // 파일에 대한 쓰기 권한이 있는지 확인
			{
				delayMs(1); // 1ms 지연
				fopen_s(&fp, filepath, "a+"); // 파일을 추가 모드로 다시 열기
				if (fp == NULL) // 여전히 파일이 열리지 않으면
				{
					return; // 함수 종료
				}
			}
		}
		bNewCsv = TRUE; // 새로운 CSV 파일임을 표시
	}

	// 소프트웨어 버전 정보 가져오기
	CString softwareVer;
	TCHAR szSwVer[1024] = { 0, };
	GetModuleFileName(NULL, szSwVer, 1024); // 현재 모듈의 파일 이름을 가져옴
	softwareVer.Format(_T("%s"), szSwVer); // 소프트웨어 버전 문자열 포맷
	softwareVer = softwareVer.Mid(softwareVer.ReverseFind(_T('\\')) + 1); // 파일 이름만 추출
	softwareVer.Delete(softwareVer.GetLength() - 4, 4); // 확장자 제거

	// 요약 정보에 데이터 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_SW_VER] = softwareVer; // 소프트웨어 버전 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_FW_VER] = lpInspWorkInfo->m_sMainFWVersion[rack][layer]; // 펌웨어 버전 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_MODEL] = lpSystemInfo->m_sLastModelName[rack]; // 모델 이름 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_EQP_NAME] = lpSystemInfo->m_sEqpName; // 장비 이름 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_RACK].Format(_T("%d"), rack + 1); // 랙 번호 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_LAYER].Format(_T("%d"), layer + 1); // 레이어 번호 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_CHANNEL].Format(_T("%d"), channel + 1); // 채널 번호 저장
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_USER_ID] = m_pApp->m_sUserID; // 사용자 ID 저장

	// 새로운 CSV 파일인 경우 헤더 작성
	if (bNewCsv == TRUE)
	{
		sprintf_s(buff, "SW_VER,FW_VER,MODEL,EQP_NAME,PID,RACK,LAYER,CHANNEL,USER_ID,START_TIME,END_TIME,RESULT,FAILED_MESSAGE,FAILED_MESSAGE_TIME,VCC,ICC,VBL,IBL\n");
		fprintf(fp, "%s", buff); // 헤더를 파일에 기록
	}

	// 실패 메시지가 비어 있으면 결과를 "OK"로 설정
	if (m_summaryInfo[rack][layer][channel].m_sumData[SUM_FAILED_MESSAGE].IsEmpty())
	{
		sResult.Format(_T("OK"));
	}
	m_summaryInfo[rack][layer][channel].m_sumData[SUM_RESULT] = sResult; // 결과 저장

	// 로그 데이터를 CSV 형식으로 포맷
	sprintf_s(buff, "%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S\n",
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_SW_VER].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_FW_VER].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_MODEL].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_EQP_NAME].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_PID].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_RACK].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_LAYER].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_CHANNEL].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_USER_ID].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_START_TIME].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_END_TIME].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_RESULT].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_FAILED_MESSAGE].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_FAILED_MESSAGE_TIME].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_MEAS_VCC].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_MEAS_ICC].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_MEAS_VBL].GetBuffer(0),
		m_summaryInfo[rack][layer][channel].m_sumData[SUM_MEAS_IBL].GetBuffer(0)
	);

	fseek(fp, 0L, SEEK_END); // 파일 포인터를 파일 끝으로 이동
	fprintf(fp, "%s", buff); // 로그 데이터를 파일에 기록

	fclose(fp); // 파일 닫기
}

void CHseAgingApp::Gf_sumInitSummaryInfo(int rack) // 주어진 랙에 대한 요약 정보를 초기화하는 기능을 수행
{
	CString skey; // 요약 정보를 저장할 키를 위한 CString 변수 선언

	// 각 레이어에 대해 반복
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		// 각 채널에 대해 반복
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			// 각 요약 정보 인덱스에 대해 반복
			for (int sumIndex = 0; sumIndex < SUM_INFO_MAX; sumIndex++)
			{
				// 해당 요약 정보의 데이터를 비움
				m_summaryInfo[rack][layer][ch].m_sumData[sumIndex].Empty();
			}

			// 현재 랙, 레이어, 채널에 대한 키 생성
			skey.Format(_T("RACK%d_LAYER%d_CH%d"), rack + 1, layer + 1, ch + 1);
			// 다양한 요약 정보를 초기화
			Write_SummaryInfo(_T("FIRMWARE"), skey, _T("")); // 펌웨어 정보 초기화
			Write_SummaryInfo(_T("PID"), skey, _T("")); // PID 초기화
			Write_SummaryInfo(_T("START_TIME"), skey, _T("")); // 시작 시간 초기화
			Write_SummaryInfo(_T("AGN_IN_COMPLETE"), skey, _T("")); // AGN_IN_COMPLETE 초기화
			Write_SummaryInfo(_T("FAIL_MESSAGE"), skey, _T("")); // 실패 메시지 초기화
			Write_SummaryInfo(_T("FAIL_TIME"), skey, _T("")); // 실패 시간 초기화
			Write_SummaryInfo(_T("VCC"), skey, _T("")); // VCC 초기화
			Write_SummaryInfo(_T("ICC"), skey, _T("")); // ICC 초기화
			Write_SummaryInfo(_T("VBL"), skey, _T("")); // VBL 초기화
			Write_SummaryInfo(_T("IBL"), skey, _T("")); // IBL 초기화
		}
	}
}

void CHseAgingApp::Gf_sumSetSummaryInfo(int rack, int layer, int ch, int sumIndex, CString sdata) // 주어진 랙, 레이어, 채널, 요약 정보 인덱스 및 문자열 데이터를 인자로 받는다
{
	// 주어진 랙, 레이어, 채널, 요약 정보 인덱스에 대해 sdata를 포맷하여 저장
	m_summaryInfo[rack][layer][ch].m_sumData[sumIndex].Format(_T("%s"), sdata);
}

void CHseAgingApp::Gf_sumSetStartTime(int rack) // 주어진 랙에 대해 시작 시간을 설정
{
	CString startTime; // 시작 시간을 저장할 CString 변수 선언
	CTime time = CTime::GetCurrentTime(); // 현재 시간을 가져옴

	// 현재 시간을 포맷하여 startTime에 저장
	startTime.Format(_T("%04d%02d%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	// 각 레이어에 대해 반복
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		// 각 채널에 대해 반복
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			// 시작 시간을 요약 정보에 설정
			Gf_sumSetSummaryInfo(rack, layer, ch, SUM_START_TIME, startTime);
		}
	}
}

void CHseAgingApp::Gf_sumSetEndTime(int rack) // 주어진 랙에 대해 종료 시간을 설정
{
	CString startTime; // 종료 시간을 저장할 CString 변수 선언
	CTime time = CTime::GetCurrentTime(); // 현재 시간을 가져옴

	// 현재 시간을 포맷하여 startTime에 저장
	startTime.Format(_T("%04d%02d%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	// 각 레이어에 대해 반복
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		// 각 채널에 대해 반복
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			// 해당 채널의 MES 패널 ID가 비어있지 않은 경우에만 작업 수행
			if (lpInspWorkInfo->m_sMesPanelID[rack][layer][ch].IsEmpty() == TRUE)
				continue;

			// 종료 시간을 요약 정보에 설정
			Gf_sumSetSummaryInfo(rack, layer, ch, SUM_END_TIME, startTime);
		}
	}
}

void CHseAgingApp::Gf_sumSetFailedInfo(int rack, int layer, int ch, CString failMessage) // 특정 랙, 레이어, 채널에 대한 실패 정보를 기록하는 함수
{
	CString failTime; // 실패 시간을 저장할 CString 변수 선언
	CTime time = CTime::GetCurrentTime(); // 현재 시간을 가져옴

	// 실패 메시지를 요약 정보에 설정
	Gf_sumSetSummaryInfo(rack, layer, ch, SUM_FAILED_MESSAGE, failMessage);

	// 현재 시간을 포맷하여 failTime에 저장
	failTime.Format(_T("%04d%02d%02d %02d:%02d:%02d"), time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	// 실패 시간 정보를 요약 정보에 설정
	Gf_sumSetSummaryInfo(rack, layer, ch, SUM_FAILED_MESSAGE_TIME, failTime);
}

void CHseAgingApp::Gf_sumSetPowerMeasureInfo(int rack) // 특정 랙에 대한 전원 측정 정보를 수집하고 요약, 이를 저장하는 역할을 하는 함수
{
	CString vcc, vccLcm, vbl, icc, ibl, vdd, idd; // 전원 측정 정보를 저장할 CString 변수 선언
	// 각 레이어에 대해 반복
	for (int layer = 0; layer < MAX_LAYER; layer++)
	{
		// 각 채널에 대해 반복
		for (int ch = 0; ch < MAX_LAYER_CHANNEL; ch++)
		{
			// VCC 값을 포맷하여 vcc에 저장
			vcc.Format(_T("%.2f"), (float)(lpInspWorkInfo->m_nMeasVCC[rack][layer][ch] / 100.0));
			Gf_sumSetSummaryInfo(rack, layer, ch, SUM_MEAS_VCC, vcc); // VCC 정보를 요약 정보에 설정

			// ICC 값을 포맷하여 icc에 저장
			icc.Format(_T("%d"), lpInspWorkInfo->m_nMeasICC[rack][layer][ch]);
			Gf_sumSetSummaryInfo(rack, layer, ch, SUM_MEAS_ICC, icc); // ICC 정보를 요약 정보에 설정

			// VBL 값을 포맷하여 vbl에 저장
			vbl.Format(_T("%.2f"), (float)(lpInspWorkInfo->m_nMeasVBL[rack][layer][ch] / 100.0));
			Gf_sumSetSummaryInfo(rack, layer, ch, SUM_MEAS_VBL, vbl); // VBL 정보를 요약 정보에 설정

			// IBL 값을 포맷하여 ibl에 저장
			ibl.Format(_T("%d"), lpInspWorkInfo->m_nMeasIBL[rack][layer][ch]);
			Gf_sumSetSummaryInfo(rack, layer, ch, SUM_MEAS_IBL, ibl); // IBL 정보를 요약 정보에 설정
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CHseAgingApp::Gf_gmesInitServer(BOOL nServerType) // 서버 초기화 작업을 수행하는 함수
{
	if ((DEBUG_GMES_TEST_SERVER == TRUE) && (nServerType == SERVER_MES)) // DEBUG_GMES_TEST_SERVER가 TRUE이고, nServerType이 SERVER_MES일 경우의 조건을 확인
	{
		pCimNet->SetMachineName(lpSystemInfo->m_sEqpName); // CIM 네트워크 객체 pCimNet에 장비 이름을 설정합니다.
		pCimNet->SetLocalTest(nServerType); // nServerType에 따라 로컬 테스트 모드를 설정합니다.
	}
	else if ((DEBUG_GMES_TEST_SERVER == TRUE) && (nServerType == SERVER_EAS)) // DEBUG_GMES_TEST_SERVER가 TRUE이고, nServerType이 SERVER_EAS일 경우의 조건을 확인합니다.
	{
		pCimNet->SetLocalTest(nServerType); // nServerType에 따라 로컬 테스트 모드를 설정합니다. (이 경우 SERVER_EAS에 해당)
	}


	if (pCimNet->Init(nServerType) == TRUE) // pCimNet의 Init 메서드를 호출하여 서버 초기화를 시도하고, 성공 여부를 확인합니다.
	{
		return TRUE; // 성공하면 TRUE를 반환
	}

	return FALSE; // 실패하면 FALSE를 반환
}


BOOL CHseAgingApp::Gf_gmesConnect(int nServerType) // CIM 네트워크에 연결을 시도하는 함수.
{
	pCimNet->SetMachineName(lpSystemInfo->m_sEqpName); // CIM 네트워크 객체 pCimNet에 장비 이름을 설정합니다. 이 이름은 lpSystemInfo 구조체의 m_sEqpName 멤버에서 가져옵니다.

	if (DEBUG_GMES_TEST_SERVER) // DEBUG_GMES_TEST_SERVER가 TRUE일 경우의 조건을 확인합니다.
		pCimNet->SetLocalTest(nServerType); // 디버그 모드일 때, nServerType에 따라 로컬 테스트 모드를 설정합니다.

	if (pCimNet->ConnectTibRv(nServerType) == TRUE) // pCimNet의 ConnectTibRv 메서드를 호출하여 서버에 연결을 시도하고, 성공 여부를 확인합니다.
	{
		return TRUE; // 성공
	}

	return FALSE; // 실패
}

void CHseAgingApp::Gf_gmesSetValueAgcm(int rack, int layer, int ch) // MES 패널 ID와 채널 ID를 설정하는 함수
{
	CString strPanelID, strChannelID; // 패널 ID와 채널 ID를 저장할 CString 타입의 변수를 선언합니다.

	strPanelID.Format(_T("%s"), lpInspWorkInfo->m_sMesPanelID[rack][layer][ch]); // lpInspWorkInfo 구조체에서 주어진 rack, layer, ch에 해당하는 MES 패널 ID를 strPanelID에 포맷하여 저장합니다.
	strChannelID.Format(_T("%s"), lpSystemInfo->m_sChannelID[rack][layer][ch]); // lpSystemInfo 구조체에서 주어진 rack, layer, ch에 해당하는 채널 ID를 strChannelID에 포맷하여 저장합니다.

	if (lpSystemInfo->m_nMesIDType == MES_ID_TYPE_PID) // MES ID 타입이 PID인지 확인합니다.
	{
		pCimNet->SetPanelID(strPanelID); // pCimNet 객체에 패널 ID를 설정합니다.
		pCimNet->SetSerialNumber(_T("")); // 시리얼 번호를 빈 문자열로 설정합니다.
		pCimNet->SetChannelID(strChannelID); // 채널 ID를 설정합니다.
	}
	else if (lpSystemInfo->m_nMesIDType == MES_ID_TYPE_SERIAL) // MES ID 타입이 SERIAL인지 확인합니다.
	{
		pCimNet->SetPanelID(_T("")); // 패널 ID를 빈 문자열로 설정합니다.
		pCimNet->SetSerialNumber(strPanelID); // 시리얼 번호를 패널 ID로 설정합니다.
		pCimNet->SetChannelID(strChannelID); // 채널 ID를 설정합니다.
	}

	pCimNet->SetRwkCode(_T("")); // 작업 코드를 빈 문자열로 설정합니다.
}

void CHseAgingApp::Lf_gmesSetValueAPDR(int rack, int layer, int ch) // 특정 랙, 레이어, 채널에 대한 APD(Automatic Process Data) 정보를 설정하는 함수.
{
	SUMMARYINFO tempAPD; // APD 정보를 저장할 SUMMARYINFO 타입의 변수를 선언합니다.
	CString sdata, sValue, sAPDInfo; // 문자열 데이터를 저장할 CString 타입의 변수를 선언합니다.

	pCimNet->SetPanelID(lpInspWorkInfo->m_sMesPchkRtnPID[rack][layer][ch]); // lpInspWorkInfo 구조체에서 주어진 rack, layer, ch에 해당하는 패널 ID를 CIM 네트워크에 설정합니다.
	pCimNet->SetSerialNumber(_T("")); // 시리얼 번호를 빈 문자열로 설정합니다.


	tempAPD = m_summaryInfo[rack][layer][ch]; //  m_summaryInfo 배열에서 주어진 rack, layer, ch에 해당하는 APD 정보를 tempAPD에 저장합니다.

	tempAPD.m_sumData[SUM_SW_VER].Replace(_T(" "), _T("_")); // SW 버전 데이터의 공백을 언더바(_)로 변경합니다.
	tempAPD.m_sumData[SUM_SW_VER].Replace(_T("."), _T("_"));
	tempAPD.m_sumData[SUM_SW_VER].Replace(_T(":"), _T("_"));
	tempAPD.m_sumData[SUM_FW_VER].Replace(_T(":"), _T("_")); // FW 버전 데이터의 콜론(:)을 언더바(_)로 변경합니다.
	tempAPD.m_sumData[SUM_FW_VER].Replace(_T("."), _T("_"));
	tempAPD.m_sumData[SUM_FAILED_MESSAGE].Replace(_T(":"), _T("_")); // 실패 메시지 데이터의 콜론(:)을 언더바(_)로 변경합니다.
	tempAPD.m_sumData[SUM_FAILED_MESSAGE].Replace(_T("."), _T("_"));
	tempAPD.m_sumData[SUM_FAILED_MESSAGE].Replace(_T(" "), _T("_"));
	tempAPD.m_sumData[SUM_FAILED_MESSAGE].Replace(_T("."), _T("_"));
	tempAPD.m_sumData[SUM_FAILED_MESSAGE].Replace(_T("~"), _T("_"));
	tempAPD.m_sumData[SUM_FAILED_MESSAGE].Replace(_T("'"), _T(""));
	tempAPD.m_sumData[SUM_FAILED_MESSAGE].Replace(_T("\""), _T(""));
	tempAPD.m_sumData[SUM_START_TIME].Replace(_T(":"), _T("_")); // 시작 시간 데이터의 콜론(:)을 언더바(_)로 변경합니다.
	tempAPD.m_sumData[SUM_END_TIME].Replace(_T(":"), _T("_")); // 종료 시간 데이터의 콜론(:)을 언더바(_)로 변경합니다.


	sdata.Format(_T("INSPECTION_INFOR:SW_VER:%s"), tempAPD.m_sumData[SUM_SW_VER]);						sAPDInfo.Append(sdata); // SW 버전 정보를 포맷하여 sdata에 저장
	sdata.Format(_T(",INSPECTION_INFOR:FW_VER:%s"), tempAPD.m_sumData[SUM_FW_VER]);						sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:MODEL:%s"), tempAPD.m_sumData[SUM_MODEL]);						sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:EQP_NAME:%s"), tempAPD.m_sumData[SUM_EQP_NAME]);					sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:PID:%s"), tempAPD.m_sumData[SUM_PID]);							sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:GROUP:%s"), tempAPD.m_sumData[SUM_RACK]);						sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:CHANNEL:%s"), tempAPD.m_sumData[SUM_CHANNEL]);					sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:USER_ID:%s"), tempAPD.m_sumData[SUM_USER_ID]);					sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:START_TIME:%s"), tempAPD.m_sumData[SUM_START_TIME]);				sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:END_TIME:%s"), tempAPD.m_sumData[SUM_END_TIME]);					sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:RESULT:%s"), tempAPD.m_sumData[SUM_RESULT]);						sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:FAILED_MESSAGE:%s"), tempAPD.m_sumData[SUM_FAILED_MESSAGE]);		sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:FAILED_MESSAGE_TIME:%s"), tempAPD.m_sumData[SUM_FAILED_MESSAGE_TIME]);		sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:VCC:%s"), tempAPD.m_sumData[SUM_MEAS_VCC]);					sAPDInfo.Append(sdata);
	sValue.Format(_T("%.1f"), (float)_ttoi(tempAPD.m_sumData[SUM_MEAS_ICC]) / 1000.f);
	sdata.Format(_T(",INSPECTION_INFOR:ICC:%s"), sValue);											sAPDInfo.Append(sdata);
	sdata.Format(_T(",INSPECTION_INFOR:VBL:%s"), tempAPD.m_sumData[SUM_MEAS_VBL]);					sAPDInfo.Append(sdata);
	sValue.Format(_T("%.1f"), (float)_ttoi(tempAPD.m_sumData[SUM_MEAS_IBL]) / 1000.f);
	sdata.Format(_T(",INSPECTION_INFOR:IBL:%s"), sValue);											sAPDInfo.Append(sdata);

	sAPDInfo.Replace(_T("-"), _T("_"));				// 하이픈(-)은 언더바(_)로 변경
	sAPDInfo.Replace(_T(" "), _T("_"));				// 공백은 언더바(_)로 변경
	sAPDInfo.Replace(_T("("), _T("_"));				// 괄호는 언더바(_)로 변경
	sAPDInfo.Replace(_T(")"), _T("_"));				// 괄호는 언더바(_)로 변경
	sAPDInfo.MakeUpper();							// 대문자 변경

	pCimNet->SetAPDInfo(sAPDInfo); // 
}

void CHseAgingApp::Lf_setGmesValueAGN_INSP(int rack, int layer, int ch) // APD 정보를 설정하는 함수
{
	CString strPanelID; // 패널 ID를 저장할 CString 타입의 변수를 선언합니다.
	CString strRwkCD; // 작업 코드(Work Code)를 저장할 CString 타입의 변수를 선언합니다. (현재 사용되지 않음)

	strPanelID.Format(_T("%s"), lpInspWorkInfo->m_sMesPanelID[rack][layer][ch]); // lpInspWorkInfo 구조체에서 주어진 rack, layer, ch에 해당하는 패널 ID를 strPanelID에 저장합니다.
	if (lpSystemInfo->m_nMesIDType == MES_ID_TYPE_PID) // MES ID 타입이 PID인 경우
	{
		pCimNet->SetPanelID(strPanelID); // CIM 네트워크에 패널 ID를 설정합니다.
		pCimNet->SetSerialNumber(_T("")); // 시리얼 번호를 빈 문자열로 설정합니다.
	}
	else if (lpSystemInfo->m_nMesIDType == MES_ID_TYPE_SERIAL) // MES ID 타입이 SERIAL인 경우.
	{
		pCimNet->SetPanelID(_T("")); //  패널 ID를 빈 문자열로 설정합니다.
		pCimNet->SetSerialNumber(strPanelID); // CIM 네트워크에 시리얼 번호를 설정합니다.
	}
}

void CHseAgingApp::Lf_setGmesValueDSPM(int rack, int layer, int ch) // APD 정보를 설정하는 함수
{
	CString strPanelID;
	CString strRwkCD;

	strPanelID.Format(_T("%s"), lpInspWorkInfo->m_sMesPanelID[rack][layer][ch]);
	if (lpSystemInfo->m_nMesIDType == MES_ID_TYPE_PID)
	{
		pCimNet->SetPanelID(strPanelID);
		pCimNet->SetSerialNumber(_T(""));
	}
	else if (lpSystemInfo->m_nMesIDType == MES_ID_TYPE_SERIAL)
	{
		pCimNet->SetPanelID(_T(""));
		pCimNet->SetSerialNumber(strPanelID);
	}
	pCimNet->SetDurableID(lpInspWorkInfo->m_sRackID[rack]); // lpInspWorkInfo 구조체에서 주어진 rack에 해당하는 내구성 ID를 CIM 네트워크에 설정합니다.
	pCimNet->SetSlotNo(_T("1")); // 슬롯 번호를 "1"로 설정합니다.
	pCimNet->SetActFlag(_T("A")); // 작업 플래그를 "A"로 설정합니다.
}

void CHseAgingApp::Lf_setGmesValueDMIN(int rack)
{
	pCimNet->SetDurableID(lpInspWorkInfo->m_sRackID[rack]);
}

void CHseAgingApp::Lf_setGmesValueDMOU(int rack)
{
	pCimNet->SetDurableID(lpInspWorkInfo->m_sRackID[rack]);
}

CString CHseAgingApp::Gf_gmesGetUserID()
{
	pCimNet->GetFieldData(&m_pApp->m_sUserID, _T("USER_ID"));
	return m_pApp->m_sUserID;
}

CString CHseAgingApp::Gf_gmesGetUserName()
{
	pCimNet->GetFieldData(&m_pApp->m_sUserName, _T("USER_NAME"));
	return m_pApp->m_sUserName;
}

CString CHseAgingApp::Gf_gmesGetRTNCD()
{
	CString strBuff;

	pCimNet->GetFieldData(&strBuff, _T("RTN_CD"));
	return strBuff;
}

CString CHseAgingApp::Gf_gmesGetPlanInfo()
{
	CString strBuff;

	pCimNet->GetFieldData(&strBuff, _T("PLAN_INFO"));
	return strBuff;
}


void CHseAgingApp::Gf_gmesShowLocalErrorMsg()
{

	CString strMsg;
	CMessageError errDlg;

	pCimNet->GetFieldData(&strMsg, _T("ERR_MSG_ENG"));	//ERR_MSG_ENG	ERR_MSG_LOC
	errDlg.m_strEMessage.Format(_T("<MES> MES ERROR %s"), strMsg);

	errDlg.DoModal();
}

BOOL CHseAgingApp::Gf_gmesSendHost(int hostCMD, int rack, int layer, int ch)
{
	int nRtnCD;
	CString sLog, sdata = _T("");
	char Luc_PF = 0;

	if (m_pApp->m_bUserIdAdmin == TRUE)
	{
		return TRUE;
	}

Send_RETRY:

	if (hostCMD == HOST_EAYT)
	{
		nRtnCD = pCimNet->EAYT();
	}
	else if (hostCMD == HOST_UCHK)
	{
		nRtnCD = pCimNet->UCHK();
		if (nRtnCD == 0)
		{
			sdata = Gf_gmesGetUserID();
			m_pApp->m_sUserID = sdata;
			pCimNet->SetUserId(m_pApp->m_sUserID);

			sdata = Gf_gmesGetUserName();
			m_pApp->m_sUserName = sdata;
		}
	}
	else if (hostCMD == HOST_EDTI)
	{
		nRtnCD = pCimNet->EDTI();
	}
	else if (hostCMD == HOST_PCHK)
	{
		Gf_gmesSetValueAgcm(rack, layer, ch);
		nRtnCD = pCimNet->PCHK();
		if (nRtnCD == RTN_OK)
		{
			pCimNet->GetFieldData(&lpInspWorkInfo->m_sMesPchkRtnPID[rack][layer][ch], _T("RTN_PID"));
		}
	}
	else if (hostCMD == HOST_FLDR)
	{
	}
	else if (hostCMD == HOST_EICR)
	{
	}
	else if (hostCMD == HOST_AGN_IN)
	{
		Gf_gmesSetValueAgcm(rack, layer, ch);
		nRtnCD = pCimNet->AGN_IN();
	}
	else if (hostCMD == HOST_AGN_OUT)
	{
		Gf_gmesSetValueAgcm(rack, layer, ch);
		nRtnCD = pCimNet->AGN_OUT();
	}
	else if (hostCMD == HOST_APDR)
	{
		Lf_gmesSetValueAPDR(rack,layer, ch);
		nRtnCD = pCimNet->APDR();
	}
	else if (hostCMD == HOST_AGN_INSP)
	{
		Lf_setGmesValueAGN_INSP(rack, layer, ch);
		nRtnCD = pCimNet->AGN_INSP();
	}
	else if (hostCMD == HOST_DSPM)
	{
		Lf_setGmesValueDSPM(rack, layer, ch);
		nRtnCD = pCimNet->DSPM();
	}
	else if (hostCMD == HOST_DMIN)
	{
		Lf_setGmesValueDMIN(rack);
		nRtnCD = pCimNet->DMIN();
	}
	else if (hostCMD == HOST_DMOU)
	{
		Lf_setGmesValueDMOU(rack);
		nRtnCD = pCimNet->DMOU();
	}

	sLog.Format(_T("<HOST_R> %s"), pCimNet->GetHostRecvMessage());
	Gf_writeMLog(sLog);

	if (nRtnCD == RTN_OK)
	{
		return TRUE;
	}
	else if (nRtnCD == RTN_MSG_NOT_SEND)
	{
		CMessageQuestion msg_dlg;

		msg_dlg.m_strQMessage.Format(_T("Failed to send message. Do you want retry ?"));
		msg_dlg.m_strLButton = _T(" Retry");

		if (msg_dlg.DoModal() == IDOK)
			goto Send_RETRY;
		else
			return FALSE;
	}
	else if (nRtnCD == RTN_RCV_TIMEOUT)
	{
		CMessageQuestion msg_dlg;

		msg_dlg.m_strQMessage.Format(_T("No response frome MES Host. Do you want retry ?"));
		msg_dlg.m_strLButton = _T(" Retry");

		if (msg_dlg.DoModal() == IDOK)
			goto Send_RETRY;
		else
			return FALSE;
	}
	else
	{
		while (Gf_gmesGetRTNCD() == _T("2705"))
		{
			Gf_gmesShowLocalErrorMsg();

			return FALSE;
		}
		Gf_gmesShowLocalErrorMsg();
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void CHseAgingApp::Gf_clearAgingStatusError()
{
	memset(lpInspWorkInfo->m_ast_AgingChErrorType, 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorType));
	memset(lpInspWorkInfo->m_ast_AgingChErrorValue, 0, sizeof(lpInspWorkInfo->m_ast_AgingChErrorValue));
}


BOOL CHseAgingApp::Gf_initTempRecorder()
{
	BOOL bRet;

	bRet = m_pTemp2xxx->TempSDR100_Initialize(lpSystemInfo->m_nTempRecorderPort);

	return bRet;
}


