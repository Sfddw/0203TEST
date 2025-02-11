  
#include "pch.h"
#include "Rs232Port.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern HWND hCommWnd;

CRs232Port::CRs232Port()
{
	n = 0;
	m_hComm1 = (HANDLE) -1;
	m_hComm2 = (HANDLE) -1;
}

CRs232Port::~CRs232Port()
{
	m_boolConnected1 = FALSE;
	if (((HANDLE) -1) != m_hComm1)
		ClosePort1 ();

	if (((HANDLE) -1) != m_hComm2)
		ClosePort2 ();
}


BOOL CRs232Port::OpenPort1 (CString sPortName, unsigned long dwBaud, unsigned short wPortID)
{
	COMMTIMEOUTS timeouts;
	DCB dcb;
	DWORD dwThreadID;
	m_boolConnected1 = FALSE;
	m_wPortID1 = wPortID;
	m_osRead1.Offset = 0;
	m_osRead1.OffsetHigh = 0;
	m_osRead1.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (!m_osRead1.hEvent)
		return FALSE;
	m_osWrite1.Offset = 0;
	m_osWrite1.OffsetHigh = 0;
	m_osWrite1.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (!m_osWrite1.hEvent)
		return FALSE;
	m_strPortName1 = sPortName;
	m_hComm1 = CreateFile (sPortName, 
							GENERIC_READ | GENERIC_WRITE, 
							0, 
							NULL, 
							OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
							NULL);
  if (m_hComm1 == (HANDLE) -1)		return FALSE;
	SetCommMask (m_hComm1, EV_RXCHAR);
	SetupComm (m_hComm1, 4096, 4096);
	timeouts.ReadIntervalTimeout = 10;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts (m_hComm1, &timeouts);
//____________________________________________________________________
	dcb.DCBlength = sizeof (DCB);
	GetCommState (m_hComm1, &dcb);
	dcb.BaudRate = dwBaud;
	dcb.ByteSize = 8;	// Keyence default = Data Bits : 8
	dcb.Parity = 0;		// Keyence default = Parity Bits : 0(no) (0-4, 0:no, 1:odd, 2:even, 3:mark, 4:space)
	dcb.StopBits = 0;	// Keyence default = Stop Bits : 0(1Bits) (0-2, 0:1bit, 1:1.5bit, 2:2bit)

	dcb.fInX = dcb.fOutX = 0;
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;

	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = DTR_CONTROL_DISABLE;


	if (!SetCommState (m_hComm1, &dcb))		return FALSE;
	m_boolConnected1 = TRUE;

	m_hThreadWatchComm1 = CreateThread (NULL,
									   0,
									   (LPTHREAD_START_ROUTINE)ThreadWatchComm1,
									   this,
									   0,
									   &dwThreadID);

	if (!m_hThreadWatchComm1)
	{
		ClosePort1 ();
		return FALSE;
	}
	return TRUE;
}


BOOL CRs232Port::OpenPort2 (CString sPortName, unsigned long dwBaud, unsigned short wPortID)
{
	COMMTIMEOUTS timeouts;
	DCB dcb;
	DWORD dwThreadID;
	m_boolConnected2 = FALSE;
	m_wPortID2 = wPortID;
	m_osRead2.Offset = 0;
	m_osRead2.OffsetHigh = 0;
	m_osRead2.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (!m_osRead2.hEvent)
		return FALSE;
	m_osWrite2.Offset = 0;
	m_osWrite2.OffsetHigh = 0;
	m_osWrite2.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (!m_osWrite2.hEvent)
		return FALSE;
	m_strPortName2 = sPortName;
	m_hComm2 = CreateFile (sPortName, 
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
		NULL);
	if (m_hComm2 == (HANDLE) -1)		return FALSE;
	SetCommMask (m_hComm2, EV_RXCHAR);
	SetupComm (m_hComm2, 4096, 4096);
	timeouts.ReadIntervalTimeout = 10;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_57600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts (m_hComm2, &timeouts);
	//____________________________________________________________________
	dcb.DCBlength = sizeof (DCB);
	GetCommState (m_hComm2, &dcb);
	dcb.BaudRate = dwBaud;
	dcb.ByteSize = 8;	// Keyence default = Data Bits : 8
	dcb.Parity = 0;		// Keyence default = Parity Bits : 0(no) (0-4, 0:no, 1:odd, 2:even, 3:mark, 4:space)
	dcb.StopBits = 0;	// Keyence default = Stop Bits : 0(1Bits) (0-2, 0:1bit, 1:1.5bit, 2:2bit)
	dcb.fNull = FALSE;	// 0x00을 버리지 않도록 설정한다.

	dcb.fInX = dcb.fOutX = 0;
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;

	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;


	if (!SetCommState (m_hComm2, &dcb))		return FALSE;
	m_boolConnected2 = TRUE;

	m_hThreadWatchComm2 = CreateThread (NULL,
		0,
		(LPTHREAD_START_ROUTINE)ThreadWatchComm2,
		this,
		0,
		&dwThreadID);

	if (!m_hThreadWatchComm2)
	{
		ClosePort2 ();
		return FALSE;
	}
	return TRUE;
}

void CRs232Port::ClosePort1 ()
{
	m_boolConnected1 = FALSE;
	SetCommMask ((void *)(m_hComm1), (unsigned long)0);
	PurgeComm (m_hComm1, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR | EV_RXFLAG | EV_TXEMPTY | EV_ERR | EV_BREAK);
	CloseHandle (m_hComm1);
	m_hComm1 = (HANDLE) -1;
}

void CRs232Port::ClosePort2 ()
{
	m_boolConnected2 = FALSE;
	SetCommMask ((void *)(m_hComm2), (unsigned long)0);
	PurgeComm (m_hComm2, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR | EV_RXFLAG | EV_TXEMPTY | EV_ERR | EV_BREAK);
	CloseHandle (m_hComm2);
	m_hComm2 = (HANDLE) -1;
}

DWORD CRs232Port::WritePort1 (BYTE* pBuff, DWORD nToWrite)
{
	DWORD dwWritten, dwError, dwErrorFlags;
	COMSTAT comstat;
	if (!WriteFile (m_hComm1, pBuff, nToWrite, &dwWritten, &m_osWrite1))
	{
		//		DWORD dwErrorCode = GetLastError ();
		if (GetLastError () == ERROR_IO_PENDING)
		{
			while (!GetOverlappedResult (m_hComm1, &m_osWrite1, &dwWritten, TRUE))
			{
				dwError = GetLastError ();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError (m_hComm1, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError (m_hComm1, &dwErrorFlags, &comstat);
		}
	}
	return dwWritten;
}

DWORD CRs232Port::WritePort2 (BYTE* pBuff, DWORD nToWrite)
{
	DWORD dwWritten, dwError, dwErrorFlags;
	COMSTAT comstat;
	if (!WriteFile (m_hComm2, pBuff, nToWrite, &dwWritten, &m_osWrite2))
	{
		//		DWORD dwErrorCode = GetLastError ();
		if (GetLastError () == ERROR_IO_PENDING)
		{
			while (!GetOverlappedResult (m_hComm2, &m_osWrite2, &dwWritten, TRUE))
			{
				dwError = GetLastError ();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError (m_hComm2, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError (m_hComm2, &dwErrorFlags, &comstat);
		}
	}
	return dwWritten;
}

DWORD CRs232Port::ReadPort1 (BYTE* pBuff, DWORD nToRead)
{
	DWORD dwRead, dwError, dwErrorFlags;
	COMSTAT comstat;
	ClearCommError (m_hComm1, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	if (dwRead > 0)
	{
		Sleep (100);
		if (!ReadFile (m_hComm1, pBuff, nToRead, &dwRead, &m_osRead1))
		{
			if (GetLastError () == ERROR_IO_PENDING)
			{
				while (!GetOverlappedResult (m_hComm1, &m_osRead1, &dwRead, TRUE))
				{
					dwError = GetLastError ();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError (m_hComm1, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError (m_hComm1, &dwErrorFlags, &comstat);
			}
		}
	}
	return dwRead;
}

DWORD CRs232Port::ReadPort2 (BYTE* pBuff, DWORD nToRead)
{
	DWORD dwRead, dwError, dwErrorFlags;
	COMSTAT comstat;
	ClearCommError (m_hComm2, &dwErrorFlags, &comstat);
	dwRead = comstat.cbInQue;
	if (dwRead > 0)
	{
		Sleep (100);
		if (!ReadFile (m_hComm2, pBuff, nToRead, &dwRead, &m_osRead2))
		{
			if (GetLastError () == ERROR_IO_PENDING)
			{
				while (!GetOverlappedResult (m_hComm2, &m_osRead2, &dwRead, TRUE))
				{
					dwError = GetLastError ();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError (m_hComm2, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError (m_hComm2, &dwErrorFlags, &comstat);
			}
		}
	}
	return dwRead;
}

DWORD ThreadWatchComm1 (CRs232Port* port_controller)
{
	DWORD dwEvent;
	OVERLAPPED os;
	BOOL bOK = TRUE;
	BYTE buff [BUFF_SIZE] = {0,};
	DWORD dwsize;

	memset (&os, 0, sizeof (OVERLAPPED));
	memset (port_controller->szCommRxBuff1, 0, sizeof(port_controller->szCommRxBuff1));

	os.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (!os.hEvent)
		bOK = FALSE;

	if (!SetCommMask (port_controller->m_hComm1, EV_RXCHAR))
		bOK = FALSE;

	if (!bOK)
	{
		AfxMessageBox (_T("Error while creating ThreadWatchComm 'COM1'"));
		return FALSE;
	}
	while (port_controller->m_boolConnected1)
	{
		dwEvent = 0;
		WaitCommEvent (port_controller->m_hComm1, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			dwsize = port_controller->ReadPort1 (buff, BUFF_SIZE);
 			if (dwsize > 0)
			{
//*********************************************************************
				AfxGetApp()->GetMainWnd()->SendMessage(WM_RS232_RECEIVED1, (WPARAM)buff, NULL);
//*********************************************************************
				Sleep(100);
				memset (buff, 0x00, BUFF_SIZE);
			}
		}
		else
		{
			dwsize = port_controller->ReadPort1 (buff, BUFF_SIZE);
			if (dwsize > 0)
			{
				memset (buff, 0x00, BUFF_SIZE);
			}
		}
	}
	CloseHandle (os.hEvent);
	port_controller->m_hThreadWatchComm1 = NULL;
	return TRUE;
}


DWORD ThreadWatchComm2 (CRs232Port* port_controller)
{
	DWORD dwEvent;
	OVERLAPPED os;
	BOOL bOK = TRUE;
	BYTE buff [BUFF_SIZE] = {0,};
	DWORD dwsize;

	memset (&os, 0, sizeof (OVERLAPPED));
	memset (port_controller->szCommRxBuff2, 0, sizeof(port_controller->szCommRxBuff2));

	os.hEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (!os.hEvent)
		bOK = FALSE;

	if (!SetCommMask (port_controller->m_hComm2, EV_RXCHAR))
		bOK = FALSE;

	if (!bOK)
	{
		AfxMessageBox (_T("Error while creating ThreadWatchComm 'COM2'"));
		return FALSE;
	}
	while (port_controller->m_boolConnected2)
	{
		dwEvent = 0;
		WaitCommEvent (port_controller->m_hComm2, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			dwsize = port_controller->ReadPort2 (buff, BUFF_SIZE);
 			if (dwsize > 0)
			{
//*********************************************************************
				AfxGetApp()->GetMainWnd()->SendMessage(WM_RS232_RECEIVED2, (WPARAM)buff, NULL);
//*********************************************************************
				Sleep(100);
				memset (buff, 0x00, BUFF_SIZE);
			}
		}
		else
		{
			dwsize = port_controller->ReadPort2 (buff, BUFF_SIZE);
			if (dwsize > 0)
			{
				memset (buff, 0x00, BUFF_SIZE);
			}
		}
	}
	CloseHandle (os.hEvent);
	port_controller->m_hThreadWatchComm2 = NULL;
	return TRUE;
}
