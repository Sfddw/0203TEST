﻿#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _ST_MODELINFO_
{
	int		m_nTimingHorTotal;
	int		m_nTimingHorActive;
	int		m_nTimingHorWidth;
	int		m_nTimingHorBP;
	int		m_nTimingHorFP;
	int		m_nTimingVerTotal;
	int		m_nTimingVerActive;
	int		m_nTimingVerWidth;
	int		m_nTimingVerBP;
	int		m_nTimingVerFP;
	float	m_fTimingMainClock;

	int		m_nLcmSignalType;
	int		m_nLcmPixelType;
	int		m_nLcmOddEven;
	int		m_nLcmSignalBit;
	int		m_nLcmBitSwap;
	int		m_nLcmLvdsRsSel;

	int		m_nDimmingSel;
	int		m_nPwmFreq;
	int		m_nPwmDuty;
	float	m_fVbrVolt;

	int		m_nFuncCableOpen;

	int		m_nPowerOnSeq1;
	int		m_nPowerOnSeq2;
	int		m_nPowerOnSeq3;
	int		m_nPowerOnSeq4;
	int		m_nPowerOnSeq5;
	int		m_nPowerOnSeq6;
	int		m_nPowerOnSeq7;
	int		m_nPowerOnSeq8;
	int		m_nPowerOnSeq9;
	int		m_nPowerOnSeq10;
	int		m_nPowerOnSeq11;
	int		m_nPowerOnDelay1;
	int		m_nPowerOnDelay2;
	int		m_nPowerOnDelay3;
	int		m_nPowerOnDelay4;
	int		m_nPowerOnDelay5;
	int		m_nPowerOnDelay6;
	int		m_nPowerOnDelay7;
	int		m_nPowerOnDelay8;
	int		m_nPowerOnDelay9;
	int		m_nPowerOnDelay10;
	int		m_nPowerOffSeq1;
	int		m_nPowerOffSeq2;
	int		m_nPowerOffSeq3;
	int		m_nPowerOffSeq4;
	int		m_nPowerOffSeq5;
	int		m_nPowerOffSeq6;
	int		m_nPowerOffSeq7;
	int		m_nPowerOffSeq8;
	int		m_nPowerOffSeq9;
	int		m_nPowerOffSeq10;
	int		m_nPowerOffSeq11;
	int		m_nPowerOffDelay1;
	int		m_nPowerOffDelay2;
	int		m_nPowerOffDelay3;
	int		m_nPowerOffDelay4;
	int		m_nPowerOffDelay5;
	int		m_nPowerOffDelay6;
	int		m_nPowerOffDelay7;
	int		m_nPowerOffDelay8;
	int		m_nPowerOffDelay9;
	int		m_nPowerOffDelay10;

	int		m_nPowerOffDelay;

	float	m_fVccVolt;
	float	m_fVccVoltOffset;
	float	m_fVccLimitVoltLow;
	float	m_fVccLimitVoltHigh;
	float	m_fVccLimitCurrLow;
	float	m_fVccLimitCurrHigh;

	float	m_fVblVolt;
	float	m_fVblVoltOffset;
	float	m_fVblLimitVoltLow;
	float	m_fVblLimitVoltHigh;
	float	m_fVblLimitCurrLow;
	float	m_fVblLimitCurrHigh;

	int		m_nAgingTimeHH;
	int		m_nAgingTimeMM;
	int		m_nAgingTimeMinute;

	CString m_sPanelIDCode;

}MODELINFO, *LPMODELINFO;


typedef struct _ST_SYSTEMINFO_{
	// STATION 정보 설정 //
	CString m_sChamberNo;
	CString m_sEqpName;
	int m_nMesIDType;

	CString	m_sLastModelName[MAX_RACK];			// 마지막 M/C 모델명
	CString m_sChannelID[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	int m_nTempRecorderPort;
	int m_nTempLogInterval;

	// MES 정보 설정 //
	CString m_sMesServicePort;
	CString m_sMesNetWork;
	CString m_sMesDaemonPort;
	CString m_sMesLocalSubject;
	CString m_sMesRemoteSubject;

	// EAS 정보 설정 //
	int		m_nEasUse;
	CString m_sEasServicePort;
	CString m_sEasNetWork;
	CString m_sEasDaemonPort;
	CString m_sEasLocalSubject;
	CString m_sEasRemoteSubject;

}SYSTEMINFO, *LPSYSTEMINFO;


typedef struct _ST_INSPWORKINFO_
{
	int m_nConnectInfo[CONNECT_MAX];

	int m_nMainEthConnect[MAX_RACK][MAX_LAYER];
	float m_fTempReadVal[MAX_TEMP_SENSOR];
	CString m_sMainFWVersion[MAX_RACK][MAX_LAYER];
	BOOL m_nFwVerifyResult[MAX_RACK];

	ULONGLONG m_nAgingStartTick[MAX_RACK];
	int	m_nAgingStatus[MAX_RACK];
	int m_nAgingSetTime[MAX_RACK];
	int m_nAgingRunTime[MAX_RACK];
	int m_nAgingOperatingMode[MAX_RACK];

	BOOL m_nRecvACK_Rack[MAX_RACK][MAX_LAYER];

	CString m_sRackID[MAX_RACK];
	CString m_sMesPanelID[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	CString m_sMesPchkRtnPID[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	BOOL m_bMesChannelUse[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];

	BOOL m_nMesDspmOK[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];

	int m_nMeasVCC[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	int m_nMeasICC[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	int m_nMeasVBL[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	int m_nMeasIBL[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];

	BOOL m_ast_AgingLayerError[MAX_RACK][MAX_LAYER];
	BOOL m_ast_AgingStartStop[MAX_RACK][MAX_LAYER];
	BOOL m_ast_AgingChOnOff[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	BOOL m_ast_ChUseUnuse[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	BOOL m_ast_CableOpenCheck[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	BOOL m_ast_AgingChErrorResult[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	BOOL m_ast_AgingChErrorType[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	int  m_ast_AgingChErrorValue[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];

	int m_nChMainUiStatusOld[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];
	int m_nChErrorStatusOld[MAX_RACK][MAX_LAYER][MAX_LAYER_CHANNEL];

	int m_nDioNeedInitial;
	int m_nDioOutputData;
	int m_nDioOutputMode;
	int m_nDioInputData[20];
	int m_nTowerLampStatus;
	int m_nDoorOpenClose[DOOR_MAX];

	CString m_sDioFWVersion;

	BOOL m_bAlarmOccur;
	CString m_sAlarmMessage;

}INSPWORKINFO, * LPINSPWORKINFO;

enum
{
	SUM_SW_VER = 0,
	SUM_FW_VER,
	SUM_MODEL,
	SUM_EQP_NAME,
	SUM_PID,
	SUM_RACK,
	SUM_LAYER,
	SUM_CHANNEL,
	SUM_USER_ID,
	SUM_START_TIME,
	SUM_END_TIME,
	SUM_RESULT,
	SUM_FAILED_MESSAGE,
	SUM_FAILED_MESSAGE_TIME,
	SUM_MEAS_VCC,
	SUM_MEAS_ICC,
	SUM_MEAS_VBL,
	SUM_MEAS_IBL,
	SUM_INFO_MAX
};

typedef struct _ST_SUMMARY_
{
	CString	m_sumData[SUM_INFO_MAX];

} SUMMARYINFO, * LPSUMMARYINFO;

