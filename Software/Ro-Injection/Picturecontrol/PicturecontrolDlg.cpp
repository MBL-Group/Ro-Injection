/*
 * PicturecontrolDlg.cpp : implementation file
 * TUBE SHOULD BE FIXED TO THE MICROSCOPE FRAME IN CASE DURING IMMOBILIZATION, DEVICE VIBRATION OR DEFLECTION OCCURS
 */
#include "stdafx.h"
#include <windows.h>
#include "Picturecontrol.h"
#include "PicturecontrolDlg.h"
#include <pylon/PylonIncludes.h>
#include <pylon/AviCompressionOptions.h>
#include <pylon/PylonGUI.h>
#include <pylon/1394/Basler1394InstantCamera.h>
#include "mx7600_utils.h"
#include "826api.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "opencv2\highgui\highgui.hpp"
#include "opencv.hpp"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "tip_detection.h"
#include "utils.h"
#include "stdio.h"
#include "cxcore.h"
#include <string>
#include "math.h"
#include "time.h"
#include "afxdialogex.h"
#include "mhi.h"
#include "mp285.h"
#include "xy_stage_proscan.h"
#include "injector_pump.h"
#include "camera.h"
#include "mx7600.h"
typedef Pylon::CBasler1394InstantCamera Camera_t;
using namespace Basler_IIDC1394CameraParams;

/* Namespace for using pylon objects. */
using namespace Pylon;
using namespace GenApi;
using namespace std;
using namespace cv;

MP285 *mp285;
XY_StageProScan *xy_stage;
INJECTOR_PUMP *injection_pump;
Camera *basler_camera;
MX7600 * mxx7600;



#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/* DWORD WINAPI Showimg(LPVOID lpParameter); */
DWORD WINAPI Showcam(LPVOID lpParameter);

/* DWORD WINAPI SENSORYFunction(LPVOID lpParameter); */
DWORD WINAPI SENSORYFunction_PID(LPVOID lpParameter);

DWORD WINAPI Autotracking(LPVOID lpParameter);

DWORD WINAPI Autotracking_speed(LPVOID lpParameter);

DWORD WINAPI SyrPump_manipulation(LPVOID lpParameter);

DWORD WINAPI Stage_manipulation(LPVOID lpParameter);

DWORD WINAPI Worm_rotation(LPVOID lpParameter);

DWORD WINAPI MicroSP(LPVOID lpParameter);

DWORD WINAPI wholemanipulation(LPVOID lpParameter);

/* CAboutDlg dialog used for App About */
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	/* Dialog Data */
	enum
	{
		IDD = IDD_ABOUTBOX
	};

protected:
	virtual void DoDataExchange(CDataExchange *pDX); /* DDX/DDV support */

	/* Implementation */
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
/* CPicturecontrolDlg dialog */
CPicturecontrolDlg::CPicturecontrolDlg(CWnd *pParent /*=NULL*/)
	: CDialogEx(CPicturecontrolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	/* Reorganize devices driver */
	mp285 = new MP285();
	xy_stage = new XY_StageProScan();
	injection_pump = new INJECTOR_PUMP();
	basler_camera = new Camera();
	mxx7600 = new MX7600();
}

void CPicturecontrolDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	/* DDX_Control(pDX, IDC_SCROLLBAR2, m_expesoure); */
	DDX_Control(pDX, IDC_SCROLLBAR1, m_gainraw);
	DDX_Control(pDX, IDC_SCROLLBAR3, m_exposure);
	DDX_Control(pDX, IDC_SCROLLBAR4, m_brightness);
}

BOOL CPicturecontrolDlg::PreTranslateMessage(MSG *pMsg)
{
	/* TODO: Add your specialized code here and/or call the base class */
	if (pMsg->message == WM_LBUTTONDOWN && GetDlgItem(IDC_MY_PIC)->GetSafeHwnd() == pMsg->hwnd)
		OnLButtonDown(MK_LBUTTON, pMsg->pt);
	return (CDialog::PreTranslateMessage(pMsg));
}

CPicturecontrolDlg::~CPicturecontrolDlg()
{
	if (m_hMod != NULL)
	{
		FreeLibrary(m_hMod);
	}
}

BEGIN_MESSAGE_MAP(CPicturecontrolDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_INITIALIZE, &CPicturecontrolDlg::OnClickedInitialize)
ON_BN_CLICKED(IDC_STOP, &CPicturecontrolDlg::OnClickedStop)
ON_BN_CLICKED(IDC_X_POS, &CPicturecontrolDlg::OnClickedXPos)
ON_BN_CLICKED(IDC_ZERO, &CPicturecontrolDlg::OnClickedZero)
ON_BN_CLICKED(IDC_X_NEG, &CPicturecontrolDlg::OnClickedXNeg)
ON_BN_CLICKED(IDC_CURRENT, &CPicturecontrolDlg::OnClickedCurrent)
ON_BN_CLICKED(IDC_CONNECT, &CPicturecontrolDlg::OnClickedConnect)
ON_BN_CLICKED(IDC_SETSPE, &CPicturecontrolDlg::OnClickedSetspe)
ON_BN_CLICKED(IDC_MOVE, &CPicturecontrolDlg::OnClickedMove)
ON_BN_CLICKED(IDC_STOP_MP, &CPicturecontrolDlg::OnClickedStopMp)
ON_BN_CLICKED(IDC_UP, &CPicturecontrolDlg::OnClickedUp)
ON_BN_CLICKED(IDC_DOWN, &CPicturecontrolDlg::OnClickedDown)
ON_BN_CLICKED(IDC_LEFT, &CPicturecontrolDlg::OnClickedLeft)
ON_BN_CLICKED(IDC_RIGHT, &CPicturecontrolDlg::OnClickedRight)
ON_BN_CLICKED(IDC_Y_POS, &CPicturecontrolDlg::OnClickedYPos)
ON_BN_CLICKED(IDC_Y_NEG, &CPicturecontrolDlg::OnClickedYNeg)
ON_BN_CLICKED(IDC_AUTO_SPEED, &CPicturecontrolDlg::OnClickedAutoSpeed)
ON_BN_CLICKED(IDC_AUTOSPEDZERO, &CPicturecontrolDlg::OnClickedAutospedzero)
ON_BN_CLICKED(IDC_FORCE, &CPicturecontrolDlg::OnClickedForce)
ON_BN_CLICKED(IDC_UPWARDS, &CPicturecontrolDlg::OnBnClickedUpwards)
ON_BN_CLICKED(IDC_DOWNWARDS, &CPicturecontrolDlg::OnBnClickedDownwards)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_STOP_CAM, &CPicturecontrolDlg::OnBnClickedStopCam)
ON_BN_CLICKED(IDC_STOP_CAM2, &CPicturecontrolDlg::OnBnClickedStopCam2)
ON_BN_CLICKED(IDC_INI_STAGE, &CPicturecontrolDlg::OnBnClickedIniStage)
ON_BN_CLICKED(IDC_CURRENTSTAGE, &CPicturecontrolDlg::OnBnClickedCurrentstage)
ON_BN_CLICKED(IDC_MOVESTAGE, &CPicturecontrolDlg::OnBnClickedMovestage)
ON_BN_CLICKED(IDC_STOP_STAGE, &CPicturecontrolDlg::OnBnClickedStopStage)
ON_BN_CLICKED(IDC_DIADOWN, &CPicturecontrolDlg::OnBnClickedDiadown)
ON_BN_CLICKED(IDC_DIAREP, &CPicturecontrolDlg::OnBnClickedDiarep)
ON_WM_LBUTTONDOWN()
ON_BN_CLICKED(IDC_BUT_ON, &CPicturecontrolDlg::OnBnClickedButOn)
ON_BN_CLICKED(IDC_BUT_OFF, &CPicturecontrolDlg::OnBnClickedButOff)
ON_BN_CLICKED(IDC_BUT_TRIGGER, &CPicturecontrolDlg::OnBnClickedButTrigger)
ON_BN_CLICKED(IDC_FORWARD_SPEED, &CPicturecontrolDlg::OnBnClickedForwardSpeed)
ON_BN_CLICKED(IDC_BACKWARD_SPEED, &CPicturecontrolDlg::OnBnClickedBackwardSpeed)
ON_BN_CLICKED(IDC_ZDOWNWARDS, &CPicturecontrolDlg::OnBnClickedZdownwards)
ON_BN_CLICKED(IDC_ZUPWARDS, &CPicturecontrolDlg::OnBnClickedZupwards)
ON_BN_CLICKED(IDC_RSESERVOIR1, &CPicturecontrolDlg::OnBnClickedRseservoir1)
ON_BN_CLICKED(IDC_BUTTON6, &CPicturecontrolDlg::OnBnClickedButton6)
ON_BN_CLICKED(IDC_BUTTON9, &CPicturecontrolDlg::OnBnClickedButton9)
ON_BN_CLICKED(IDC_BUTTONINJECTION, &CPicturecontrolDlg::OnBnClickedButtoninjection)
ON_BN_CLICKED(IDC_BUTTONPIPRES, &CPicturecontrolDlg::OnBnClickedButtonpipres)
ON_BN_CLICKED(IDC_BUTTONPIPINJ, &CPicturecontrolDlg::OnBnClickedButtonpipinj)
ON_BN_CLICKED(IDC_BUTTONPIPREC, &CPicturecontrolDlg::OnBnClickedButtonpiprec)
ON_BN_CLICKED(IDC_DOWN_IMMOBILIZATION, &CPicturecontrolDlg::OnBnClickedDownImmobilization)
ON_BN_CLICKED(IDC_LEFT_MP285, &CPicturecontrolDlg::OnBnClickedLeftMp285)
ON_BN_CLICKED(IDC_RIGHT_MP286, &CPicturecontrolDlg::OnBnClickedRightMp286)
ON_BN_CLICKED(IDC_BACKWARD_SPEED2, &CPicturecontrolDlg::OnBnClickedBackwardSpeed2)
ON_BN_CLICKED(IDC_FORWARD_SPEED2, &CPicturecontrolDlg::OnBnClickedForwardSpeed2)
ON_BN_CLICKED(IDC_BACKGROUND, &CPicturecontrolDlg::OnBnClickedBackground)
ON_BN_CLICKED(IDC_ZUPWARDS_QUICK, &CPicturecontrolDlg::OnBnClickedZupwardsQuick)
ON_BN_CLICKED(IDC_ENUMINTERFACE, &CPicturecontrolDlg::OnBnClickedEnuminterface)
ON_BN_CLICKED(IDC_OPENINTERFACE, &CPicturecontrolDlg::OnBnClickedOpeninterface)
ON_BN_CLICKED(IDC_CLOSE, &CPicturecontrolDlg::OnBnClickedClose)
ON_BN_CLICKED(IDC_LOGOUT, &CPicturecontrolDlg::OnBnClickedLogout)
ON_BN_CLICKED(IDC_LOGIN, &CPicturecontrolDlg::OnBnClickedLogin)
ON_BN_CLICKED(IDC_STATUSREADY, &CPicturecontrolDlg::OnBnClickedStatusready)
ON_BN_CLICKED(IDC_OB4, &CPicturecontrolDlg::OnBnClickedOb4)
ON_BN_CLICKED(IDC_OB_TEN, &CPicturecontrolDlg::OnBnClickedObTen)
ON_BN_CLICKED(IDC_OB_TWENTY, &CPicturecontrolDlg::OnBnClickedObTwenty)
ON_BN_CLICKED(IDC_OB_FOURTY, &CPicturecontrolDlg::OnBnClickedObFourty)
ON_BN_CLICKED(IDC_BUTTONFM, &CPicturecontrolDlg::OnBnClickedButtonfm)
ON_BN_CLICKED(IDC_BUTTONFMDOWN, &CPicturecontrolDlg::OnBnClickedButtonfmdown)
ON_BN_CLICKED(IDC_BUTTONOBST, &CPicturecontrolDlg::OnBnClickedButtonobst)
ON_BN_CLICKED(IDC_BUTTONOBSL, &CPicturecontrolDlg::OnBnClickedButtonobsl)
ON_BN_CLICKED(IDC_BUTTONOBRG, &CPicturecontrolDlg::OnBnClickedButtonobrg)
ON_BN_CLICKED(IDC_BUTTONOBFB, &CPicturecontrolDlg::OnBnClickedButtonobfb)
ON_BN_CLICKED(IDC_BUTTONBL, &CPicturecontrolDlg::OnBnClickedButtonbl)
ON_BN_CLICKED(IDC_BUTTONBL2, &CPicturecontrolDlg::OnBnClickedButtonbl2)
ON_BN_CLICKED(IDC_BUTTONBL4, &CPicturecontrolDlg::OnBnClickedButtonbl4)
ON_BN_CLICKED(IDC_BUTTONAF4, &CPicturecontrolDlg::OnBnClickedButtonaf4)
ON_BN_CLICKED(IDC_BUTTONAF5, &CPicturecontrolDlg::OnBnClickedButtonaf5)
ON_BN_CLICKED(IDC_AUTOFC, &CPicturecontrolDlg::OnBnClickedAutofc)
ON_BN_CLICKED(IDC_REAL, &CPicturecontrolDlg::OnBnClickedReal)
ON_BN_CLICKED(IDC_BUTTONMHI, &CPicturecontrolDlg::OnBnClickedButtonmhi)
ON_BN_CLICKED(IDC_BUTTONAFWORM, &CPicturecontrolDlg::OnBnClickedButtonafworm)
ON_BN_CLICKED(IDC_BUTTON_IMGORI, &CPicturecontrolDlg::OnBnClickedButtonImgori)
ON_BN_CLICKED(IDC_BUTTONMFINI, &CPicturecontrolDlg::OnBnClickedButtonmfini)
ON_BN_CLICKED(IDC_BUTTONTENSW, &CPicturecontrolDlg::OnBnClickedButtontensw)
ON_BN_CLICKED(IDC_BUTTONPIPMOVE, &CPicturecontrolDlg::OnBnClickedButtonpipmove)
ON_BN_CLICKED(IDC_BUTTONPIPMOVE2, &CPicturecontrolDlg::OnBnClickedButtonpipmove2)
ON_BN_CLICKED(IDC_BUTTON_WIBY, &CPicturecontrolDlg::OnBnClickedButtonWiby)
ON_BN_CLICKED(IDC_WIBN, &CPicturecontrolDlg::OnBnClickedWibn)
ON_BN_CLICKED(IDC_STEPSTAGEFIX, &CPicturecontrolDlg::OnBnClickedStepstagefix)
ON_BN_CLICKED(IDC_BUTTON_ORIRECORD, &CPicturecontrolDlg::OnBnClickedButtonOrirecord)
ON_BN_CLICKED(IDC_BUTTON_ORIRECORD2, &CPicturecontrolDlg::OnBnClickedButtonOrirecord2)
ON_BN_CLICKED(IDC_BUTTON_RC, &CPicturecontrolDlg::OnBnClickedButtonRc)
ON_BN_CLICKED(IDC_WORMTRANS, &CPicturecontrolDlg::OnBnClickedWormtrans)
ON_BN_CLICKED(IDC_BUTTONDEVHM, &CPicturecontrolDlg::OnBnClickedButtondevhm)
ON_BN_CLICKED(IDC_BUT_ON2, &CPicturecontrolDlg::OnBnClickedButOn2)
ON_BN_CLICKED(IDC_BUTTONALL, &CPicturecontrolDlg::OnBnClickedButtonall)
ON_BN_CLICKED(IDC_BUTTONPIPMOVE3, &CPicturecontrolDlg::OnBnClickedButtonpipmove3)
ON_BN_CLICKED(IDC_BUTTON_XH, &CPicturecontrolDlg::OnBnClickedButtonXh)
ON_BN_CLICKED(IDC_BUTTONAF40, &CPicturecontrolDlg::OnBnClickedButtonaf40)
ON_BN_CLICKED(IDC_BUTTON4, &CPicturecontrolDlg::OnBnClickedButton4)
ON_BN_CLICKED(IDC_BUTTONREL, &CPicturecontrolDlg::OnBnClickedButtonrel)
ON_BN_CLICKED(IDC_BUTTONPRE, &CPicturecontrolDlg::OnBnClickedButtonpre)
ON_BN_CLICKED(IDC_BUTTONALLHM, &CPicturecontrolDlg::OnBnClickedButtonallhm)
ON_BN_CLICKED(IDC_BUTTONPREBUT, &CPicturecontrolDlg::OnBnClickedButtonprebut)
ON_BN_CLICKED(IDC_BUTTONMHIIMG, &CPicturecontrolDlg::OnBnClickedButtonmhiimg)
ON_BN_CLICKED(IDC_BUTTONAUTO, &CPicturecontrolDlg::OnBnClickedButtonauto)
ON_BN_CLICKED(IDC_BUTTONRECOB, &CPicturecontrolDlg::OnBnClickedButtonrecob)
ON_BN_CLICKED(IDC_BUTTON_YH, &CPicturecontrolDlg::OnBnClickedButtonYh)
ON_BN_CLICKED(IDC_LARVAEDEC, &CPicturecontrolDlg::OnBnClickedLarvaedec)
//ON_EN_CHANGE(IDC_EDITSPEED, &CPicturecontrolDlg::OnEnChangeEditspeed)
//ON_BN_CLICKED(IDC_BUTTON_test_start, &CPicturecontrolDlg::OnBnClickedButtonteststart)
//ON_BN_CLICKED(IDC_BUTTON11_test_stop, &CPicturecontrolDlg::OnBnClickedButton11teststop)
ON_BN_CLICKED(IDC_BUTTONBI, &CPicturecontrolDlg::OnBnClickedButtonbi)
ON_BN_CLICKED(IDC_Z_POS, &CPicturecontrolDlg::OnBnClickedZPos)
ON_BN_CLICKED(IDC_Z_NEG, &CPicturecontrolDlg::OnBnClickedZNeg)
ON_BN_CLICKED(IDC_T_POS, &CPicturecontrolDlg::OnBnClickedTPos)
ON_BN_CLICKED(IDC_T_NEG, &CPicturecontrolDlg::OnBnClickedTNeg)
END_MESSAGE_MAP()

/* CMFCApplication3Dlg message handlers */
TCHAR szMsg_1[256];
CHAR szMsg_int[256];
int flag_pos_micro = 0;
int flag_pos_micro_handle = 0;
int pos_ob = 0;
CString objective_hm_ps;
int objective_hm_ps_flag = 0;
int objective_flag = 0;
int position_maximum = 0;
int worm_head_up = 0;
LARGE_INTEGER currentCount66; /*  */
LARGE_INTEGER startCount66;
LARGE_INTEGER endCount66; /*  */
LARGE_INTEGER freq66;	  /*  */
INT64 CurrentTime66;	  /*  */
INT64 frequency66;		  /*  */
double dbTime66;
ofstream myfile_TIME_ALL;
/* Helpful macros for DIOs */
#define DIO(C) ((uint64)1 << (C)) /* convert dio channel number to uint64 bit mask */
#define DIOMASK(N)                              \
	{                                           \
		(uint)(N) & 0xFFFFFF, (uint)((N) >> 24) \
	}																	/* convert uint64 bit mask to uint[2] array */
#define DIOSTATE(STATES, CHAN) ((STATES[CHAN / 24] >> (CHAN % 24)) & 1) /* extract dio channel's boolean state from uint[2] array */
/*
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * ERROR HANDLING
 * These examples employ very simple error handling: if an error is detected, the example functions will immediately return an error code.
 * This behavior may not be suitable for some real-world applications but it makes the code easier to read and understand. In a real
 * application, it's likely that additional actions would need to be performed. The examples use the following X826 macro to handle API
 * function errors; it calls an API function and stores the returned value in errcode, then returns immediately if an error was detected.
 */
#define X826(FUNC)                        \
	if ((errcode = FUNC) != S826_ERR_OK)  \
	{                                     \
		printf("\nERROR: %d\n", errcode); \
		return errcode;                   \
	}
/*
 * //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * Counter utility functions used by the demos.
 * PERIODIC TIMER --------------------
 * Counter mode: count down at 1MHz, preload upon start or when 0 reached, assert ExtOut when not 0
 */
#define TMR_MODE (S826_CM_K_1MHZ | S826_CM_UD_REVERSE | S826_CM_PX_ZERO | S826_CM_PX_START | S826_CM_OM_NOTZERO)
/* Configure a counter channel to operate as a periodic timer and start it running. */
static int PeriodicTimerStart(uint board, uint counter, uint period)
{
	int errcode;
	X826(S826_CounterStateWrite(board, counter, 0));						 /* halt channel if it's running */
	X826(S826_CounterModeWrite(board, counter, TMR_MODE));					 /* configure counter as periodic timer */
	X826(S826_CounterSnapshotConfigWrite(board, counter, 4, S826_BITWRITE)); /* capture snapshots at zero counts */
	X826(S826_CounterPreloadWrite(board, counter, 0, period));				 /* timer period in microseconds */
	X826(S826_CounterStateWrite(board, counter, 1));						 /* start timer */
	return (errcode);
}

/* Halt channel operating as periodic timer. */
static int PeriodicTimerStop(uint board, uint counter)
{
	return (S826_CounterStateWrite(board, counter, 0));
}

/* Wait for periodic timer event. */
static int PeriodicTimerWait(uint board, uint counter, uint *timestamp)
{
	uint counts, tstamp, reason;																		   /* counter snapshot */
	int errcode = S826_CounterSnapshotRead(board, counter, &counts, &tstamp, &reason, S826_WAIT_INFINITE); /* wait for timer snapshot */
	if (timestamp != NULL)
		*timestamp = tstamp;
	return (errcode);
}

int SetDacOutput(uint board, uint chan, double volts)
{
	uint setpoint;
	uint range;
	int errcode = S826_DacRead(board, chan, &range, &setpoint, 0); /* get DAC output range */
	switch (range)
	{
	case S826_DAC_SPAN_0_5:
		setpoint = (uint)(volts * 0xFFFF / 5);
		break; /* 0 to +5V */
	case S826_DAC_SPAN_0_10:
		setpoint = (uint)(volts * 0xFFFF / 10);
		break; /* 0 to +10V */
	case S826_DAC_SPAN_5_5:
		setpoint = (uint)(volts * 0xFFFF / 10) + 0x8000;
		break; /* -5V to +5V */
	case S826_DAC_SPAN_10_10:
		setpoint = (uint)(volts * 0xFFFF / 20) + 0x8000;
		break; /* -10V to +10V */
	}
	if (errcode == S826_ERR_OK)
		errcode = S826_DacDataWrite(board, chan, setpoint, 0);
	return (errcode);
}

/*
 * CPicturecontrolDlg message handlers
 * >>>>>>>>>>>definition of several funtions used in acquisition of force from force sensor<<<<<<<<<<<<//
 */
int CreateHwTimer(uint board, uint chan, uint period) /* period in microseconds */
{
	S826_CounterModeWrite(board, chan,							   /* Configure counter mode: */
						  S826_CM_K_1MHZ |						   /*   clock source = 1 MHz internal */
							  S826_CM_PX_START | S826_CM_PX_ZERO | /*   preload @start and counts==0 */
							  S826_CM_UD_REVERSE |				   /*   count down */
							  S826_CM_OM_NOTZERO);				   /*   ExtOut = (counts!=0) */
	S826_CounterPreloadWrite(board, chan, 0, period);			   /* Set period in microseconds. */
	return (S826_CounterStateWrite(board, chan, 1));			   /* Start the timer running. */
}

/* Use counter0 to periodically trigger ADC conversions ------------- */
void StartAdc16(uint period) /* Configure/start ADC and trigger generator */
{
	int i;
	for (i = 0; i < 16; i++)									/* Configure 3 timeslots: 1 slot per AIN; 20us/slot settling time. */
		S826_AdcSlotConfigWrite(15, i, i, 20, S826_ADC_GAIN_1); /* /-10v~+10v */
	S826_AdcSlotlistWrite(15, 0xFFFF, S826_BITWRITE);			/* Enable 3 timeslots. */
	S826_AdcTrigModeWrite(15, 0xB0);							/* Hardware triggered, source = counter0 ExtOut. */
	S826_AdcEnableWrite(15, 1);									/* Enable ADC conversions. */
	CreateHwTimer(15, 0, period);								/* Create and start the trigger generator. */
}

int ReadAdc16(int *adcbuf)
{
	uint slotlist = 0xFFFF; /* only the first three slot is of interest */
	return (S826_AdcRead(15, adcbuf, NULL, &slotlist, S826_WAIT_INFINITE));
}


BOOL CPicturecontrolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	/* IDM_ABOUTBOX must be in the system command range. */
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu *pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
	/*
	 * Set the icon for this dialog.  The framework does this automatically
	 *  when the application's main window is not a dialog
	 */
	SetIcon(m_hIcon, TRUE);	 /* Set big icon */
	SetIcon(m_hIcon, FALSE); /* Set small icon */
	/*
	 * TODO: Add extra initialization here
	 * //////////////////////////////////////////////////////olympus microscope //////////////////////
	 * TODO: Add extra initialization here
	 */
	int ret;
	/*
	 * -----------------------------------------------------
	 * move current directory
	 */
	_tchdir(theApp.m_Path);

	/*
	 * -----------------------------------------------------
	 * call initialize function
	 */
	m_bOpened = FALSE;
exit_freelib:
	m_gainraw.SetScrollRange(1, 1000);	 /* set the length of horizontal scroll bar */
	m_gainraw.SetScrollPos(507);		 /* set the initial position of horizontal bar is 810 */
	SetDlgItemInt(IDC_GAIN, 507);		 /* show the initial position of horizontal bar in edit control */
	m_exposure.SetScrollRange(1, 600);	 /* set the length of horizontal scroll bar */
	m_exposure.SetScrollPos(462);		 /* set the initial position of horizontal bar is 350 */
	SetDlgItemInt(IDC_EXP, 462);		 /* show the initial position of horizontal bar in edit control */
	m_brightness.SetScrollRange(1, 255); /* set the length of horizontal scroll bar */
	m_brightness.SetScrollPos(85);		 /* set the initial position of horizontal bar is 350 */
	SetDlgItemInt(IDC_BRIGHT, 85);		 /* show the initial position of horizontal bar in edit control */
	CRect rect;
	GetDlgItem(IDC_MY_PIC)->GetWindowRect(&rect); /* IDC_WAVE_DRAWÎªPicture ControlµÄID */
	ScreenToClient(&rect);
	GetDlgItem(IDC_MY_PIC)->MoveWindow(rect.left, rect.top, 656, 492, true); /* ¹Ì¶¨Picture Control¿Ø¼þµÄ´óÐ¡ */
	return (TRUE);															 /* return TRUE  unless you set the focus to a control */
}

void CPicturecontrolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

/*
 * If you add a minimize button to your dialog, you will need the code below
 *  to draw the icon.  For MFC applications using the document/view model,
 *  this is automatically done for you by the framework.
 */
void CPicturecontrolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); /* device context for painting */
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		/* Center icon in client rectangle */
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		/* Draw the icon */
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

/* ////////////////////// fucntion details///////////////////// */
#pragma region /* // (CALLBACK) CommandCallback //// */
/*
 * -----------------------------------------------------------------------------
 * COMMAND: call back entry from SDK port manager
 */
int CALLBACK CommandCallback(
	ULONG MsgId,	/* Callback ID. */
	ULONG wParam,	/* Callback parameter, it depends on callback event. */
	ULONG lParam,	/* Callback parameter, it depends on callback event. */
	PVOID pv,		/* Callback parameter, it depends on callback event. */
	PVOID pContext, /* This contains information on this call back function. */
	PVOID pCaller	/* This is the pointer specified by a user in the requirement function. */
)
{
	UNREFERENCED_PARAMETER(MsgId);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(pContext);
	UNREFERENCED_PARAMETER(pCaller);
	UNREFERENCED_PARAMETER(pv);
	CPicturecontrolDlg *dlg = (CPicturecontrolDlg *)pContext;

	return (0);
}

#pragma endregion
#pragma region /* // (CALLBACK) NotifyCallback //// */
/*
 * -----------------------------------------------------------------------------
 * NOTIFICATION: call back entry from SDK port manager
 */
int CALLBACK NotifyCallback(
	ULONG MsgId,	/* Callback ID. */
	ULONG wParam,	/* Callback parameter, it depends on callback event. */
	ULONG lParam,	/* Callback parameter, it depends on callback event. */
	PVOID pv,		/* Callback parameter, it depends on callback event. */
	PVOID pContext, /* This contains information on this call back function. */
	PVOID pCaller	/* This is the pointer specified by a user in the requirement function. */
)
{
	UNREFERENCED_PARAMETER(MsgId);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(pContext);
	UNREFERENCED_PARAMETER(pCaller);
	UNREFERENCED_PARAMETER(pv);
	CPicturecontrolDlg *dlg = (CPicturecontrolDlg *)pContext;
	/*
	 * ///////////////////////////////////////////////////////
	 * show notification
	 */
	TCHAR szMsg[256];
	_stprintf_s(szMsg, 256, _T( "Notifycation - %S" ), pv);
	dlg->MessageBox(szMsg, _T( "Notification" ), MB_OK + MB_ICONINFORMATION);
	return (0);
}

#pragma endregion
#pragma region /* // (CALLBACK) ErrorCallback //// */
/*
 * -----------------------------------------------------------------------------
 * ERROR NOTIFICATON: call back entry from SDK port manager
 */
int CALLBACK ErrorCall(
	ULONG MsgId,	/* Callback ID. */
	ULONG wParam,	/* Callback parameter, it depends on callback event. */
	ULONG lParam,	/* Callback parameter, it depends on callback event. */
	PVOID pv,		/* Callback parameter, it depends on callback event. */
	PVOID pContext, /* This contains information on this call back function. */
	PVOID pCaller	/* This is the pointer specified by a user in the requirement function. */
)
{
	UNREFERENCED_PARAMETER(MsgId);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(pContext);
	UNREFERENCED_PARAMETER(pCaller);
	UNREFERENCED_PARAMETER(pv);
	CPicturecontrolDlg *dlg = (CPicturecontrolDlg *)pContext;
	/*
	 * ///////////////////////////////////////////////////////
	 * show notification
	 */
	dlg->MessageBox(_T( "Disconnect interface ?" ), _T( "Notification" ), MB_OK + MB_ICONINFORMATION);
	dlg->OnBnClickedClose();
	return (0);
}

#pragma endregion
/*
 * The system calls this function to obtain the cursor to display while the user drags
 *  the minimized window.
 */
HCURSOR CPicturecontrolDlg::OnQueryDragIcon()
{
	return (static_cast<HCURSOR>(m_hIcon));
}

CRect rect;
CDC *pDC;
ToMove MyMovingPara;
void *ifData;
/* />>>>>>>>>>>>>assign value to adjust parameters of camera<<<<<<<<<<<<<<<<<<<<<<<<////// */
int exp_cam = 462;
int gain_cam = 507;
int brightness_micro = 85;
/*
 * />>>>>>>>>>>>>assign value to adjust parameters of camera<<<<<<<<<<<<<<<<<<<<<<<<//////

 * >>>>>>>>>>>>>>>>>>definie variables for robotic injection project<<<<<<<<<<<<<<<<///////
 */


int mx7600_autopositioning_x = 0;
int mx7600_autopositioning_y = 0;
int original_position = 0;
int inte = 0, pro;
int inte_y = 0, pro_y, inte_z = 0, pro_z = 0;
int diff[5] = {0};
int diff_y[5] = {0};
int diff_z[5] = { 0 };
int diff_t[5] = { 0 };
int diff_grabpicture[8] = {0};
int diff_grabpicture_y[8] = {0};
int diff_grabpicture_z[8] = { 0 };
int diff_grabpicture_t[8] = { 0 };
int num_diff_grabpicture = 0;
int num_diff_grabpicture_y = 0;
int num_diff_grabpicture_z = 0;
int num_diff_grabpicture_t = 0;
float kp = 10, ki, kd;
Mat image_show;
Mat image_movingdone;
int counter_ini;
int counter_ini_y;
int counter_ini_z;
int counter_ini_t;
int num_moving = 0;
int num_moving_y = 0;
int num_moving_z = 0;
int num_moving_t = 0;
int x_num = 0;
int y_num = 0;
int z_num = 0;
int t_num = 0;
int flag_step = 0;
int flag_step_y = 0;
int flag_step_z = 0;
int flag_step_t = 0;
/* int clear = 0; */
int num_counter = 0;
int num_counter_y = 0;
int num_counter_z = 0;
int num_counter_t = 0;

/* >>>>defenition of variables<<<<<// */
float m_xValue, m_yValue, m_zValue;										/* for MP285 position */
float m_xValue_home, m_yValue_home, m_zValue_home;						/* for MP285 position */
float m_xValue_MHI_home, m_yValue_MHI_home, m_zValue_MHI_home;			/* for MP285 position IN mhi PROCESSING */
float m_xValue_pipette_ori, m_yValue_pipette_ori, m_zValue_pipette_ori; /* for MP285 position in pipette positioning */
long m_xValuego, m_yValuego, m_zValuego;								/* for MP285 position */
long m_xValuemove, m_yValuemove, m_zValuemove;							/* for MP285 position */
int MP285_moving_up_flag = 0;
int seven_numer = 0;
int imgpro_flg = 0;
int imgpro_pip_flag = 1;
int imgpro_autofc_flag = 0;
int imgpro_pip_worm_contact = 0;
int solution_autodeposit_flag = 0;
int imgpro_num = 0;
int imgpro_MHI_pip = 0;
int imgpro_rotation_flag = 0;
int imgpro_rotation_file_stop = 0;
int imgpro_gray_file_stop = 0;
int pulses_have_moved_x = 0;
int pulses_to_move_sum_x;
int pulses_have_moved_y = 0;
int pulses_to_move_sum_y;
int manipulator_flg_x = 0;
int manipulator_flg_y = 0;
int pipetteflg = 0;
int a = 0;
float vol_mp285_x = 2.5; /* / used to control the mp285 by speed mode//2.5V euqals to the speed zero */
float vol_mp285_y = 2.5;
float vol_mp285_z = 2.5;
float vol_mp285_drive = 5;
int num_mp285 = 0;
int Mp285Conflg = 0;
int Mp285Spdflg = 0;
int Mp285Curflg = 0;

int rowstotal = 0;
int coloumstotal = 0;
double initial_vol = 0;
double initial_vol1 = 0;
double initial_vol2 = 0;
double vol_previous[5] = {0};
double vol1_previous[5] = {0};
double vol2_previous[5] = {0};
double vol_previous_dif[5] = {0};
double vol1_previous_dif[5] = {0};
double vol2_previous_dif[5] = {0};
double vol_to_finish[5] = {0};
double vol1_to_finish[5] = {0};
double vol2_to_finish[5] = {0};
double vol_inte_pid = 0;
double vol_diff_pid = 0;
double vol_diff_pid_temp[5] = {0};
int mx7600_moving_flag_mp285 = 0;
int mx7600_slow_flag = 0;
int pre_button_flag = 0;
int pre_button_y_flag = 0;
int pre_button_x_flag = 0;
int pre_button_z_flag = 0;
int pre_button_t_flag = 0;
int mx7600_start_num = 0;
int vol_diff_pid_temp_flag = 0;
int force_done_time = 0;
double inte_vol = 0;
int MP285_stop_moving = 1;			/* int MP285_stop_moving = 1; /// flag 1 indicates MP285 keeps rest */
int MP285_stop_moving_mode = 0;		/* /// 0 represents quick mode;1 represents slow mode */
int MP285_stop_moving_quick_fw = 0; /* int MP285_stop_moving_quick_fw = 0; /// flag 0 quickmovemnt is not realized */
int MP285_btnleft_moving_done = 0;	/* flag 1 indicates MP285 movement done when btnleft clicked */
int injection_flag = 0;
int injection_done_flag = 0;
int MP285_z_stage_forward_flag = 0;
int MP285_Pos_Initial_flag = 0;
int MP285_pOS_Initial_YN = 0;
int MP285_pOS_Initial_MHI_YN = 0;
int MP285_Pos_Initial_back_flag = 0;
int MP285_Pos_Initial_MHI_flag = 0;
int original_pulses = 0;
int realtime_pulses_y = 0;
int original_pulses_1 = 0;
int original_pulses_6 = 0;
int original_pulses_2 = 0;
int original_pulses_2_flag = 0; /* ////original_pulses_2 represents the real-time encoder value */
int original_pulses_y = -666435;
int original_pulses_y_flag = 0;
int original_pulses_x = 73711;
int original_pulses_x_flag = 0;
int original_pulses_t_flag = 0;
int original_pulses_z = -678298;
int original_pulses_z_flag = 0;
int original_pulses_t = -678298;
int original_pulses_fourth_t = 0;
int original_pulses_first_y = 0;
int num_img_final = 0;
int z_tophalf_flag = 0;
int z_bothalf_flag = 0;
int z_half_done_flag = 0;
int z_lower_done_flag = 0;
int z_lower_flag = 0;
int z_lower_done_injection_flag = 0;
int z_lower_injection_flag = 0;
int stage_mov_flag = 0;
int stage_stop_flag = 0;
int stage_cons_flag = 0;
int stage_move_center_flag = 0;
int Pump_negative_done = 0;
int immobilization_worm_flag = 0;
int stage_reservior_1 = 0, stage_reservior_2 = 0, stage_reservior_3 = 0, stage_reservior_4 = 0, stage_reservior_5 = 0, stage_reservior_6 = 0;
int stage_injection = 0, stage_recovery = 0, stage_recovery_areaflag = 0, worm_trans_all_flag = 0, device_trans_transfer_flag = 0, device_trans_loading_flag = 0, device_trans_lift_flag = 0, worm_rotation_all_flag = 0;
int stage_posflag = 0;
int stage_injection_part = 1;
int worm_lift_extract_flag = 0;
int worm_down_extract_flag = 0;
int Pre_inject_flag = 0;
int pre_button_area_flag = 0;
int a_stage = 0, b_stage = 0;
int stage_recovery_home_x = 12950;
int stage_recovery_home_y = -3255;
int stage_injection_home_x = 64260;
int stage_injection_home_y = 22432; /* ////  needs to be calibrated every time before each experiments */
int worm_release_all_flag = 0;
int device_all_home_flag = 0;
int device_trans_homex = 0;
int stage_release_areaflag = 0;
int stage_release_return_areaflag = 0;
CString X_pos, Y_pos;
Point head_pt_global;
Point tail_pt_global;
Point Pip_tip_global;
Point2f Pip_tip_global_ini;
Point Pip_tip_to_move;
int head_tail_x_delta = 0;
int head_tail_y_delta = 0;
int flag_tip_btndown = 0;
int injector_step_flag = 0;
int back_img_writing_flag = 0;
int back_img_pipette_MHI_writing_flag = 0;
int imgpro_pip_ini_flag = 0;
int imgpro_pip_ini_move_flag = 0;
int back_img_line_flag = 0;
int ten_obj_switch_flag = 0;
int err_num = 0;
int err_change_num = 0;
int initial_flag = 0;
int stable_flag = 0;
int vol_to_finish_flag = 0;
int data_temp_flag = 0;
//double data2_temp = 0;
int mx7600_moving_flag = 0;
int mx7600_movingdone_flag = 1;
int mx7600_movingdone_flag_y = 1;
int mx7600_movingdone_z_flag = 0;
int mx7600_movingdone_x_flag = 0;
int mx7600_movingdone_y_flag = 0;
int mx7600_movingdone_t_flag = 0;
int steps_to_move = 0;
int steps_to_move_tilt = 0;
/* /uint abs_puls = 0; */
int force_done_flag = 0;
int pid_flag = 0;
int max_vol_flag = 0;
int max_vol_flag_1 = 0;
int max_vol_flag_y = 0;
int max_vol_flag_z = 0;
int max_vol_flag_1_y = 0;
int max_vol_flag_1_z = 0;
int camera_flag = 0;
int camera_flag_num = 0;
int duration_time_inf = 0;
int duration_time_inf_one_tenth = 0;
int duration_time_wit = 0;
int duration_time_wit_one_tenth = 0;
int pump_flag_continuous[2] = {0};
int stage_flag_step = 0;
int stage_scanning_flag = 0;
int stage_flag_acce = 0;
Mat Frame_1;

HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);

/* >>>>>>>>>>>> this function is employed to control the manipulator of MP285 to the preset position<<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::moveto(long x, long y, long z)
{
	mp285->goto_position(x, y, z);
	OnClickedCurrent();
}

/* >>>>>>>>>>>> this function is employed to set motorized stage speed<<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::SetStageSpeed(unsigned short value)
{
	xy_stage->set_speed(value);
}

/* >>>>>>>>>>>> this function is employed to move the motorized stage to relative position <<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::MoveStageTo(int x, int y)
{
	xy_stage->goto_relative_position(x, y);
	/* GetStagePosition(a_stage, b_stage); */
	xy_stage->get_current_position(a_stage, b_stage);
	X_pos.Format(L"%d", a_stage);
	Y_pos.Format(L"%d", b_stage);
	SetDlgItemText(IDC_EDITXPOSSTAGE, X_pos);
	SetDlgItemText(IDC_EDITYPOSSTAGE, Y_pos);
}

/* >>>>>>>>>>>> this function is employed to move the motorized stage by absolute position <<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::MoveStageToabs(int x, int y)
{
	xy_stage->goto_absolute_position(x, y);
	/* GetStagePosition(a_stage, b_stage); */
	xy_stage->get_current_position(a_stage, b_stage);
	X_pos.Format(L"%d", a_stage);
	Y_pos.Format(L"%d", b_stage);
	SetDlgItemText(IDC_EDITXPOSSTAGE, X_pos);
	SetDlgItemText(IDC_EDITYPOSSTAGE, Y_pos);
}

/* >>>>>>>>>>>> this function is employed to get the motorized stage position <<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::GetStagePosition(int &x, int &y)
{
	xy_stage->get_current_position(x, y);
}

/* >>>>>>>>>>>> this function is employed to set the motorized stage acceleration <<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::GetStageAcceleration(int &x)
{
	xy_stage->get_accel(x);
}

/* >>>>>>>>>>>> this function is employed to move the motorized stage at a constant speed <<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::StageConstantMovement(int x, int y)
{
	xy_stage->constant_move(x, y);
	GetStagePosition(a_stage, b_stage);
	X_pos.Format(L"%d", a_stage);
	Y_pos.Format(L"%d", b_stage);
	SetDlgItemText(IDC_EDITXPOSSTAGE, X_pos);
	SetDlgItemText(IDC_EDITYPOSSTAGE, Y_pos);
}


/* >>>>>>>>>>>> this function is employed to stop the motorized stage movement <<<<<<<<<<<<<<<<// */
void CPicturecontrolDlg::StopStageMovement()
{
	xy_stage->stop();
}

/* />>>>>>>>>>> to process the message from windows like button click in a cycle<<<<<<<<<<<<<<///// */
BOOL PeekandPump()
{
	static MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			return (FALSE);
		}
	}
	return (TRUE);
}

/*



 * />>>>>>>>>>> this section is to initialize and create threads for the the manipulation of manipulator MX7600<<<<<<<<<<<<<</////
 */
void CPicturecontrolDlg::OnClickedInitialize()
{
	/* CreateThread(NULL, 0, SENSORYFunction, NULL, 0, NULL); */
	CreateThread(NULL, 0, SENSORYFunction_PID, NULL, 0, NULL);
	CreateThread(NULL, 0, wholemanipulation, NULL, 0, NULL);
	/*
	 * MP285_stop_moving = 1;/////////////////////////////////05022018
	 * imgpro_flg = 0;
	 */
	SetDlgItemText(IDC_EDIT_MX, L"connected");
}



 //>>>>>>>>>>> this section is to start showing images from camera and create a thread for the the image show (thread is necessary as image is continuously shown) <<<<<<<<<<<<<</////
void CPicturecontrolDlg::OnClickedStop()
{
	/* TODO: Add your control notification handler code here */
	HANDLE hthread_cam;
	hthread_cam = CreateThread(NULL, 0, Showcam, NULL, 0, NULL);
	CloseHandle(hthread_cam);
}

/*
 * />>>>>>>>>>> this section is to start showing images from camera and create a thread for the the image show (thread is necessary as image is continuously shown) <<<<<<<<<<<<<</////
 
 
 
 * //////////////// ******** Creation of thread for microscope manipulation *************** ///////////
 */
DWORD WINAPI MicroSP(LPVOID lpParameter)
{
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	pMwnd->SetDlgItemText(IDC_EDITSCOPE, L"connected");
	while (true)
	{
		if (ten_obj_switch_flag == 1) /* //////////////10x objective manipulation */
		{
			pMwnd->OnBnClickedObTen();
			Sleep(1000);
			pMwnd->OnBnClickedButtonbl2();
			Sleep(1000);
			pMwnd->OnBnClickedButtonaf5();
			Sleep(1000);
			ten_obj_switch_flag = 0;
		}
		if (immobilization_worm_flag == 1) /* //////if the womr immobilization succeeds, 10x objective manipulation  and pipette movement would be performed */
		{
			pMwnd->OnBnClickedObTen();
			Sleep(1500);
			pMwnd->OnBnClickedButtonbl2();
			Sleep(800);
			pMwnd->OnBnClickedButtonaf5();
			Sleep(500);
			immobilization_worm_flag = 0;
			back_img_writing_flag = 1; /* //// write the background image for the following worm microinjection */
			Sleep(100);
			pMwnd->OnBnClickedButtonpipmove(); /* /////////////////////////// move pipette to the FOV after worm is firmly immobilized */
		}
	}
}

/*
 * //////////////// ******** Creation of thread for microscope manipulation *************** ///////////
 */
 
 

 ////////////////////// This showcam thread is only used for  image show, video recording,and so on/////////////////////////////
DWORD WINAPI Showcam(LPVOID lpParameter)
{
	/* TODO: Add your control notification handler code here */
	PylonAutoInitTerm autoInitTerm;
	CImage myImage1;
	int exitCode = 0;
	int num = 0;
	/* //>>>>>>>>>>> this section is used to compresss the image which will be written in files<<<<<<<<<<<<<<<<<<<<//// */
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); /* choose jpeg type */
	compression_params.push_back(95);					   /* number indicates the quality of figure you want: 100 means the best quality. default value of jpeg compressed is 95 if this section is not employed */
	/* / *************Initialize the erosion element********* /// */
	int erosion_type;
	int erosion_size = 1;
	int erosion_elem = 0;
	if (erosion_elem == 0)
	{
		erosion_type = MORPH_RECT;
	}
	else if (erosion_elem == 1)
	{
		erosion_type = MORPH_CROSS;
	}
	else if (erosion_elem == 2)
	{
		erosion_type = MORPH_ELLIPSE;
	}
	Mat erosion_element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	/* / *************Initialize the dilation element********* /// */
	int dilatation_type;
	int dilatation_size = 1;
	int dilatation_elem = 0;
	if (dilatation_elem == 0)
	{
		dilatation_type = MORPH_RECT;
	}
	else if (dilatation_elem == 1)
	{
		dilatation_type = MORPH_CROSS;
	}
	else if (dilatation_elem == 2)
	{
		dilatation_type = MORPH_ELLIPSE;
	}
	long t1, t2;
	Mat dilatation_element1 = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilatation_element = getStructuringElement(erosion_type, Size(2, 2), Point(erosion_size, erosion_size));
	/* ///>>>>>>>>>> grabbing picture from camera<<<<<<<<<// */
	try
	{
		CvVideoWriter *video = NULL; /* Create an video object to store the images. */
		IplImage *frame_1;			 /* this type of variable is necessary in the funtion "cvWriteFrame()", use it to store the vonverted result from Mat variable */
		/* Only look for cameras supported by Camera_t */
		CDeviceInfo info;
		info.SetDeviceClass(Camera_t::DeviceClass());
		/* Create an instant camera object with the first found camera device that matches the specified device class. */
		Camera_t camera(CTlFactory::GetInstance().CreateFirstDevice(info));
		/* Print the model name of the camera. */
		cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
		/*
		 * ==========================================Parameter Configuration Start===================================
		 * Open the camera for accessing the parameters.
		 */
		camera.Open();
		camera.OffsetX.SetValue(camera.OffsetX.GetMin());
		camera.OffsetY.SetValue(camera.OffsetY.GetMin());
		camera.Width.SetValue(658);
		camera.Height.SetValue(490);
		camera.PixelFormat.SetValue(PixelFormat_YUV422Packed);
		camera.GainRaw.SetValue(510);
		camera.ExposureTimeAbs.SetValue(462);
		camera.BlackLevelRaw.SetValue(100);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
		camera.BalanceRatioAbs.SetValue(1.09375);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
		camera.BalanceRatioAbs.SetValue(1.421875);
		camera.BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
		camera.BalanceRatioAbs.SetValue(3.984375);
		/* **************setting parameters for video recording **************** // */
		CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
		CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));
		CEnumerationPtr pixelFormat(camera.GetNodeMap().GetNode("PixelFormat"));
		/*
		 * **************setting parameters for video recording **************** //
		 * ==========================================Parameter Configuration End===================================
		 * ==========================================Grabbing Start================================================
		 */
		camera.MaxNumBuffer = 10;
		camera.StartGrabbing();
		CGrabResultPtr ptrGrabResult;
		CPylonImage target;
		CImageFormatConverter converter;
		converter.OutputPixelFormat = PixelType_BGR8packed;
		converter.OutputBitAlignment = OutputBitAlignment_MsbAligned;
		CRect rect;
		CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
		CWnd *pWnd = pMwnd->GetDlgItem(IDC_MY_PIC);
		CDC *pDC = pWnd->GetDC();
		while (camera.IsGrabbing())
		{
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				/* Translate the message and dispatch it to WindowProc() */
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			/* If the message is WM_QUIT, exit the while loop */
			if (msg.message == WM_QUIT)
				break;
			PeekandPump();
			camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
			if (ptrGrabResult->GrabSucceeded())
			{
				/* ********start recording the video after the button of Video_STA is clicked******** // */
				if (camera_flag == 1)
				{
					if (camera_flag_num == 0)
					{
						video = cvCreateVideoWriter("C:/Users/panpe/Desktop/test.avi", CV_FOURCC('X', 'V', 'I', 'D'), 30, cvSize((uint32_t)width->GetValue(), (uint32_t)height->GetValue()));
						camera_flag_num++; /* the camera_flag_num value will increase after the  aviWriter.Open is open. This ensures the aviWriter.Open is just performed for one time in every video recording operation */
					}
				}
				/*
				 * ********start recording the video after the button of Video_STA is clicked******** //
				 * ******** finish recording the video after the button of Video_STOP is clicked******** //
				 */
				if (camera_flag == 2)
				{
					camera_flag = 0;	 /* // assign 0 to camera_flag is to indicate only image show is performed without video recording */
					camera_flag_num = 0; /* / this flag value is to ensure that every time when the button of Video_STA is clicked, aviwriter is open for the subsequent video recording */
					cvReleaseVideoWriter(&video);
				}
				/*
				 * ******** finish recording the video after the button of Video_STOP is clicked******** //
				 * /// ********convertion of the picture format to bgr******** //
				 */
				converter.Convert(target, ptrGrabResult);
				/*
				 * ///>>>>>>>>>convertion of the picture format to bgr<<<<<<//
				 * ///// ********Conversion of picture to Mat format******** /////
				 */
				Mat frame(target.GetHeight(), target.GetWidth(), CV_8UC3, target.GetBuffer(), Mat::AUTO_STEP);
				frame.copyTo(Frame_1);
				/* ///// ********Conversion of picture to Mat format******** ///// */
				num_img_final++;
				if (back_img_line_flag == 1)
				{
					line(frame, Point(420, 50), Point(420, 440), (255, 255, 255), 1);
					circle(frame, Point(410, 245), 2, (255, 255, 255), 2);
				}
				if (frame.empty())
				{
					cout << "No se pudo cargar la imagen Mat" << endl;
				}
				else
				{
					/* ///////////////////////Conversion of color image to grayscale mode  ////////////////////// */
					Mat frame_gry_ori, fin_img;
					Mat frame_temp = frame.clone();
					cvtColor(frame_temp, frame_gry_ori, CV_RGB2GRAY);
					vector<Mat> channels;
					channels.push_back(frame_gry_ori);
					channels.push_back(frame_gry_ori);
					channels.push_back(frame_gry_ori);
					merge(channels, fin_img);
					MatToCImage(fin_img, myImage1);
					/* ///////////////////////Conversion of color image to grayscale mode  ////////////////////// */
					pDC = pWnd->GetDC();
					pWnd->GetClientRect(&rect);
					pDC->SetStretchBltMode(STRETCH_HALFTONE);
					myImage1.Draw(pDC->m_hDC, rect);
					ReleaseDC(AfxGetMainWnd()->GetSafeHwnd(), pDC->m_hDC);
					myImage1.Destroy();
				}
				frame_1 = &IplImage(frame);
				if (camera_flag == 1)
				{
					cvWriteFrame(video, frame_1);
				}
				if (back_img_writing_flag == 1)
				{
					imwrite("C:/background.jpg", frame); /* //////background image for worm injection*/
					back_img_writing_flag = 0;
				}
				if (back_img_writing_flag == 3)
				{
					imwrite("C:/background_pipworm.jpg", frame); /* /////under 10x objective background image includes only the pipette tip for MHI detection */
					back_img_writing_flag = 0;
				}
				if (back_img_writing_flag == 4)
				{
					imwrite("C:/background_autofc.jpg", frame); /* /////under 10x objective background image includes only microfludic device for autofocusing */
					back_img_writing_flag = 0;
				}
				if (back_img_writing_flag == 5)
				{
					imwrite("C:/plain_image.jpg", frame); /* /////under 10x objective blank background image  */
					back_img_writing_flag = 0;
				}
			}
			else
			{
				cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
			}
		}
	}
	catch (GenICam::GenericException &e)
	{
		/* Error handling. */
		cerr << "An exception occurred." << endl
			 << e.GetDescription() << endl;
		exitCode = 1;
	}
	/* Comment the following two lines to disable waiting on exit. */
	cerr << endl
		 << "Press Enter to exit." << endl;
	while (cin.get() != '\n')
		;
	/*
	 * PylonTerminate();
	 * ReleaseMutex(hMutex);
	 */
	return (exitCode);
}

/*
 * ////////////////////// This showcam thread is only used for  image show, video recording,and so on/////////////////////////////
 
 
 
 
 * ////////////////////// This wholemanipulation thread is used for  manipulation of different hardwares/////////////////////////////
 */
DWORD WINAPI wholemanipulation(LPVOID lpParameter)
{
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	while (1)
	{
		if (worm_trans_all_flag == 1) /* /////////////////////this is the manipulation flow for automated worm pick up and transfer */
		{
			pMwnd->OnBnClickedStopStage();	  /* /// when worm is recognized by scanning, stage movement stops */
			pMwnd->OnBnClickedButtonpipinj(); /* ////device lower down for immobilization and transfer */
			while (true)
			{
				if (mx7600_movingdone_z_flag == 1)
				{
					mx7600_movingdone_z_flag = 0;
					break;
				}
			}
			pMwnd->OnBnClickedButOn(); /* ////pump power on */
			Sleep(1000);
			pMwnd->OnBnClickedButtoninjection(); /* ////stage movemnt to ensure injection reservoir is moved to the center of FOV */
			Sleep(1500);
			worm_trans_all_flag = 0;
		}
		if (worm_release_all_flag == 1) /* ///////after worm was injected, worm release was performed */
		{
			/* ///////////withdraw pipette//////////////// */
			pMwnd->OnBnClickedBackwardSpeed();
			Sleep(1000);
			/*
			 * ///////////withdraw pipette////////////////
			 * ///////////HOME position pipette////////////////
			 */
			pMwnd->OnBnClickedButtonpipmove3();
			/* ///////////HOME position pipette//////////////// */
			stage_release_areaflag = 1;		  /* ///stage moves to the recovery area; */
			pMwnd->OnBnClickedButtonpipres(); /* ////device are kind of lift up for worm release */
			while (true)
			{
				if (mx7600_movingdone_z_flag == 1)
				{
					mx7600_movingdone_z_flag = 0;
					break;
				}
			}
			while (true)
			{
				if (stage_release_areaflag == 0)
				{
					pMwnd->OnBnClickedButOff(); /* / pump release */
					break;
				}
			}
			worm_release_all_flag = 0;
		}
		if (worm_rotation_all_flag == 1) /* ///// if orientation after worm trasnfer is not good, perform rotation by clicking the W_IB_N button */
		{
			pMwnd->OnBnClickedButtonpipinj();
			while (true)
			{
				if (mx7600_movingdone_z_flag == 1)
				{
					mx7600_movingdone_z_flag = 0;
					break;
				}
			}
			pMwnd->OnBnClickedButOff();
			Sleep(500);
			/* //////pump release stops////// */

			stage_flag_step = 66; /* ///// when stage_flag_step == 66, all paramters of step mode for motorized stage are predetermined */
			worm_rotation_all_flag = 0;
		}
		if (z_half_done_flag == 1) /* ////// this flag is set for determining if the movement of securely immobilized worm completes or not */
		{
			/* CPicturecontrolDlg* pMwnd = (CPicturecontrolDlg*)AfxGetApp()->m_pMainWnd; */
			pMwnd->OnBnClickedButOn2();
			pMwnd->OnBnClickedDownImmobilization();
			z_half_done_flag = 0;
			z_lower_injection_flag = 1; /* /////this flas is used for the following pipette movement to FOV */
		}
		if (z_lower_done_injection_flag == 1) /* /////////this flag is set for manipulation of 10X objective and bringing the injection pipette in FOV under 10x objective */
		{
			z_lower_done_injection_flag = 0;
			pMwnd->OnBnClickedButtonWiby();
		}
		if (Pre_inject_flag == 1)
		{
			pMwnd->OnBnClickedButTrigger();
			Sleep(400);
			Pre_inject_flag = 0;
			pMwnd->OnBnClickedButTrigger();
		}
		/* /////////////////// ************ flag for automated solution deposition ************ /////////////////////// */
		if (injection_flag == 1)
		{
			pMwnd->OnBnClickedButTrigger();
			Sleep(5000);
			injection_flag = 0;
		}
		/*
		 * /////////////////// ************ flag for automated solution deposition ************ ///////////////////////
		 * /////////////////// ************ Return microfluidic device to HM position and release pump pressure ************ ///////////////////////
		 */
		if (device_all_home_flag == 1)
		{
			pMwnd->OnBnClickedButtonpipinj(); /* ////device lower down for pipette to withdraw from worm */
			while (true)
			{
				if (mx7600_movingdone_z_flag == 1)
				{
					mx7600_movingdone_z_flag = 0;
					break;
				}
			}
			pMwnd->OnBnClickedButtondevhm();
			while (true)
			{
				if (mx7600_movingdone_z_flag == 1)
				{
					mx7600_movingdone_z_flag = 0;
					break;
				}
			}
			device_all_home_flag = 0;
			pMwnd->OnBnClickedButOff(); /* /// PUMP RELEASE */
		}
		/*
		 * /////////////////// ************ Return microfluidic device to HM position and release pump pressure ************ ///////////////////////
		 * int num = 1;
		 */
	}
}
/*
 * ////////////////////// This wholemanipulation thread is used for  manipulation of different hardwares/////////////////////////////





 * ////////////// ********This thread is created  for all detailed image processing algorithms such as pipette tip tracking******** //////////////
 */
DWORD WINAPI Worm_rotation(LPVOID lpParameter)
{
	/* TODO: Add your control notification handler code here */
	LARGE_INTEGER currentCount;
	LARGE_INTEGER startCount;
	LARGE_INTEGER endCount;
	LARGE_INTEGER freq;
	double dbTime, dbTime0, dbTime4, dbTime1, dbTime2, dbTime3, dbTime5, dbTime6, dbTime7, dbTime8, dbTime9, dbTime10; /* ³ÌÐòÔËÐÐµÄÊ±¼ä±£´æÔÚÕâÀï */
	double dbtime[20];
	double cTime;
	INT64 CurrentTime;
	INT64 frequency;
	QueryPerformanceFrequency(&freq);
	frequency = freq.QuadPart;
	
	Mat img, img_pipette, img_pipette_ini;
	Mat img_pdms_contact, img_pdms_sub_contact, img_pdms_bw_contact;
	Point2f pip_tip, pip_tip_ini;
	Mat template_img_back = imread("C:/background.jpg");
	cvtColor(template_img_back, template_img_back, CV_RGB2GRAY);
	/* ///////////////parameter definition for worm head and tail recognition//////////////////////// */
	Point head_pt;
	Point tail_pt;
	double dis_points_1 = 0;
	double dis_points_2 = 0;
	Mat img_bw, img_bw_2;
	vector<Point> found_worm_contours;
	Point2f corner_Point_intestine[2];
	vector<Point> found_worm_contours_2;
	Point2f corner_Point_head_tail[2];
	Point2f corner_Point_head[2];
	Point corner_Point_intestine_int[2];
	Point corner_Point_head_tail_int[2];
	Point corner_Point_head_int[2];
	Mat worm_img;
	int num_head = 0;
	int pip_num = 0;
	/* int autofc_num = 0; */
	Mat background_autofc;
	Mat background_pip_worm;
	Mat plain_image;
	Mat pip_worm_thresh;
	Mat pip_thresh;
	int pip_worm_contact_num = 0;
	Point2f pipette_tip[2] = {0};
	CString Sum_var, Gray_var;
	/* /////////////////////////>>>>>>>>>parameter initialization for worm rotation process<<<<<<<<<<<<<<<///// */
	Mat img_original_worm_rotation, img_original_worm_bw_rotation, img_original_worm_sub_bw_rotation, img_original_worm_sub_bw_1_rotation;
	Mat img_original_worm_bg_rotation;
	Mat img_original_worm_sub_rotation;
	Mat img_worm_clone_mark;
	int background_ave_pix = 0;
	int flag_conversion = 0;
	int idy_temp = 0;
	int idy_prev = 0;
	Point2f corner_Point_head_tail_rotation[2];
	Point2f corner_Point_head_tail_rotation_temp[2];
	Point2f corner_Point_head_tail_pevious_rotation[2];
	Point2f corner_Point_three_fourth;
	Point2f corner_Point_ele_twelveth;
	Point2f corner_Point_head_rotation[2];
	Point2f corner_Point_tail_rotation[2];
	int y_distance = 0; /* //// global variable */
	int flag_background = 0;
	/* int num_head = 0; */
	int gonad_pix_value = 0;
	int intestine_pix_value = 0;
	int worm_rotation_num = 0;
	int gonad_pix_value_pre[10] = {0};
	int intestine_pix_value_pre[10] = {0};
	double gonad_pix_value_ave = 0;
	double intestine_pix_value_ave = 0;
	int gonad_pix_value_sum = 0;
	int intestine_pix_value_sum = 0;
	/* int worm_head_up = 0; */
	int autofc_num = 0;
	Moments mu, mu_11, mu_pre, mu1_pre, mu21, mu22;
	Point2f mc, mc_11, mc_pre, mc1_pre, mc21, mc22;
	int num = 0;
	int direction_pip_worm = 0;

	/* /////////////////// paramter for autofocusing function///////////////// */
	double sum = 0;
	double sum_ave = 0;
	double gray_variance = 0;
	double gray_variance_ave = 0;
	double gray_variance_all[10] = {0};
	double maximum_variance = 0;
	double stop_variance = 0;
	int position_stop = 0;
	int maximum_variance_num = 0;
	
	/*************Initialize the erosion element*********/
	int erosion_type;
	int erosion_size = 1;
	int erosion_elem = 0;
	if (erosion_elem == 0)
	{
		erosion_type = MORPH_RECT;
	}
	else if (erosion_elem == 1)
	{
		erosion_type = MORPH_CROSS;
	}
	else if (erosion_elem == 2)
	{
		erosion_type = MORPH_ELLIPSE;
	}
	Mat erosion_element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	/*************Initialize the erosion element*********/
	/*************Initialize the dilation element*********/
	int dilatation_type;
	int dilatation_size = 1;
	int dilatation_elem = 0;
	if (dilatation_elem == 0)
	{
		dilatation_type = MORPH_RECT;
	}
	else if (dilatation_elem == 1)
	{
		dilatation_type = MORPH_CROSS;
	}
	else if (dilatation_elem == 2)
	{
		dilatation_type = MORPH_ELLIPSE;
	}
	long t1, t2;
	CString Pip_ini_Xpos, Pip_ini_Ypos;
	Mat dilatation_element1 = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilatation_element = getStructuringElement(erosion_type, Size(2, 2), Point(erosion_size, erosion_size));
	/*************Initialize the dilation element*********/
	/* ///>>>>>>>>>> grabbing picture from camera<<<<<<<<<// */
	while (1)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* Translate the message and dispatch it to WindowProc() */
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/* If the message is WM_QUIT, exit the while loop */
		if (msg.message == WM_QUIT)
			break;
		PeekandPump();
		num_img_final++;
		/* ///////// ***********Reset parameters for pipette tip tracking algorithm ************* ///////////// */
		if (imgpro_pip_flag == 1) /* /// initalize the parameters for pip_tip recognition */
		{
			pip_num = 0;
		}
		/*
		 * ///////// ***********Reset parameters for pipette tip tracking algorithm ************* /////////////
		 * ///////// ***********pipette tip tracking ************* /////////////
		 */
		if (imgpro_pip_flag == 2) 
		{
			pip_num++;
			img_pipette = Frame_1;
			rowstotal = Frame_1.rows;
			coloumstotal = Frame_1.cols;
			if (pip_num == 1)
			{
				Pipette_tip_detection(img_pipette, pip_tip);
				template_img_back = imread("C:/background.jpg");
				cvtColor(template_img_back, template_img_back, CV_RGB2GRAY); /* ////as the bakcground image at diff stages change, background image update is necessary */
			}
			else
			{
				Pipette_tip_detection_ROI(template_img_back, img_pipette, pip_tip);
			}
			Pip_tip_global = Point(pip_tip);
			Pip_ini_Xpos.Format(L"%f", pip_tip.x);
			Pip_ini_Ypos.Format(L"%f", pip_tip.y);
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->SetDlgItemText(IDC_EDITINIPIPX, Pip_ini_Xpos);
			pMwnd->SetDlgItemText(IDC_EDITINIPIPY, Pip_ini_Ypos);
		}
		/* ///////// ***********pipette tip tracking************* ///////////// */

		
		
		/* ///////// ***********Reset parameters for autofocusing algorithm ************* ///////////// */
		if (imgpro_autofc_flag != 1)
		{
			
			autofc_num = 0;
			maximum_variance_num = 0;
		}
		/*
		 * ///////// ***********Reset parameters for autofocusing algorithm ************* /////////////
		 * ///////// *********** autofocusing algorithm of securely immobilized worm************* /////////////
		 */
		if (imgpro_autofc_flag == 1)
		{
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			if (autofc_num == 0)
			{
				background_autofc = imread("C:/background_autofc.jpg");
				cvtColor(background_autofc, background_autofc, CV_RGB2GRAY);
				pMwnd->OnBnClickedButtonfm(); /* ///adding the fucntion to perfrom nosepiece manipulation */
				/* /// before this operation, please ensure microscope is controlled romotely and slow mode is chosen */
				Sleep(100);
			}
			Mat img_autofocusing = Frame_1.clone();
			
			cvtColor(img_autofocusing, img_autofocusing, CV_RGB2GRAY);
			Mat background_bw = background_autofc - img_autofocusing; /* //// this background is figure only including the mcirofluidic device under 10x */
			
			for (int idx = 0; idx < img_autofocusing.rows; idx++)
			{
				uchar *data = background_bw.ptr<uchar>(idx);
				for (int idy = 530; idy < 580; idy++)
				{
					sum = sum + (double)data[idy];
				}
			}
			sum_ave = (double)sum / (img_autofocusing.rows * 50); /* /// average pixel value */
			for (int idx = 0; idx < img_autofocusing.rows; idx++)
			{
				uchar *data = background_bw.ptr<uchar>(idx);
				for (int idy = 530; idy < 580; idy++)
				{
					gray_variance += (double)((data[idy] - sum_ave) * (data[idy] - sum_ave));
				}
			}
			gray_variance_ave = (double)(gray_variance / (img_autofocusing.rows * 50)); /* /// average pixel value */
			if (gray_variance_ave > maximum_variance)
			{
				maximum_variance = gray_variance_ave;
				maximum_variance_num = 0;
				/* CPicturecontrolDlg* pMwnd = (CPicturecontrolDlg*)AfxGetApp()->m_pMainWnd; */
				pMwnd->OnBnClickedButtonobfb(); /* /// get nosepiece position along z axis */
				position_maximum = pos_ob;
			}
			else if (gray_variance_ave < maximum_variance || gray_variance_ave == maximum_variance)
			{
				maximum_variance_num++;
			}
			if (maximum_variance_num > 30) /* 30 */
			{
				/* CPicturecontrolDlg* pMwnd = (CPicturecontrolDlg*)AfxGetApp()->m_pMainWnd; */
				pMwnd->OnBnClickedButtonobst(); /* ///stop moving upwards the nosepiece */
				Sleep(1000);
				pMwnd->OnBnClickedReal(); /* /////////// return nosepiece to the position where variance_value is maximum */
				Sleep(300);
				imgpro_autofc_flag = 0;
				pMwnd->SetDlgItemText(IDC_EDITAF, L"AF IS done");
				back_img_writing_flag = 1; /* ////once the autofocusing is done record the picture as background image for the follwoing worm injection */
				objective_hm_ps_flag = 1;
				objective_hm_ps.Format(_T( "FG %d\r\n" ), position_maximum); /* / RECORDING THE FOCUSING POSITION AS HOME POSITION UNDER 10X OBJECTIVE */
				pMwnd->OnBnClickedButtonobfb();								 /* /// update nosepiece position along z axis */
			}
			sum = 0;
			gray_variance = 0;
			autofc_num++;
		}
		/*
		 * ///////// *********** autofocusing algorithm of securely immobilized worm************* /////////////
		 * ////////////////////// **********************MHI image processing ********************* /////////////////////////////////////////////
		 */
		if (imgpro_pip_worm_contact == 1) /* ///////////////////////MHI detection between worm and pipette */
		{
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			if (pip_worm_contact_num == 0)
			{
				background_pip_worm = imread("C:/background_pipworm.jpg"); /* // background image only inclduing the injection pipette is needed */
				plain_image = imread("C:/plain_image.jpg");
				
				Mat background_pip_gray, plain_image_gray;
				
				cvtColor(background_pip_worm, background_pip_gray, COLOR_BGR2GRAY);
				cvtColor(plain_image, plain_image_gray, COLOR_BGR2GRAY);
				Mat img_diff_pip = plain_image_gray - background_pip_gray;
			
				
				Mat img_temp_bw = Mat::zeros(background_pip_worm.size(), CV_32FC1);
				threshold(img_diff_pip, pip_worm_thresh, 10, 255, 0);
				
				
				 ////////// ************remove small contours and just leave the pipette tip************ ////////////
				 
				vector<vector<Point>> dif_contours;
				vector<Vec4i> hierarchy;
				findContours(pip_worm_thresh, dif_contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
				double area_temp[4] = {0};
				double area[4] = {0};
				double area_comp;
				int count_cont = 0;
				for (int i = 0; i < dif_contours.size(); i++)
				{
					area_comp = contourArea(dif_contours[i], false);
					if (area_comp > area[0])
					{
						area[0] = area_comp;
						count_cont = i;
					}
				}
				drawContours(img_temp_bw, dif_contours, count_cont, Scalar(255), FILLED, 8);
				pip_thresh = img_temp_bw.clone();
				/* ////////// ************remove small contours and just leave the pipette tip************* //////////// */
				findpipettetip(img_temp_bw, pipette_tip);
				CString pre_mhi_pip_x, pre_mhi_pip_y;
				pre_mhi_pip_x.Format(L"%d", int(pipette_tip[0].x));
				pMwnd->SetDlgItemText(IDC_EDITXPIP, pre_mhi_pip_x);
				pre_mhi_pip_y.Format(L"%d", int(pipette_tip[0].y));
				pMwnd->SetDlgItemText(IDC_EDITYPIP, pre_mhi_pip_y);
				pip_worm_contact_num++;
				MP285_stop_moving = 77; /* ///////////////////////////////////////////////////////////lower down the MP285 slowly */
			}
			Mat img_pip_worm_ori = Frame_1.clone();
			use_MHI(img_pip_worm_ori, background_pip_worm, pip_thresh, (int)(pipette_tip[0].x), (int)(pipette_tip[0].y));
			if (right_diretcion_flag == 1 && left_number > 0)
			{
				pMwnd->OnClickedAutospedzero(); /* //////////////////////////////stop MP285 movement */
				num_local_1 = -1;
				num_local_2 = -1;
				right_diretcion_flag = 0;
				right_number = 0;
				left_number = 0;
				imgpro_pip_worm_contact = 0;
				pip_worm_contact_num = 0;
			}
		}

		/* ////////////////////// **********************MHI image processing ********************* ///////////////////////////////////////////// */
	}
	return (true);
}

/*
 * ////////////// ********This thread is created for all detailed image processing algorithms such as pipette tip tracking******** //////////////




 * ////======================this section is used to stop manipulator MX7600======================//
 */
void CPicturecontrolDlg::OnClickedZero()
{
	mx7600_slow_flag = 1;
	MyMovingPara.pulses_to_move_x = 0;
	MyMovingPara.voltage_x = 0;
	MyMovingPara.stopped_and_ready_2_move_x = 1;
	inte = 0;
	num_counter = 0;
	pro = 0;
	pro_y = 0;
	for (int index = 0; index < 5; index++)
	{
		diff[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulses_to_move_history_x[index] = 0;
	}
	MyMovingPara.IsJustSet_x = 0;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.IsJustSet_history_x[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulse_abs_history_x[index] = 0;
	}
	MyMovingPara.pulses_to_move_y = 0;
	MyMovingPara.voltage_y = 0;
	MyMovingPara.stopped_and_ready_2_move_y = 1;
	inte_y = 0;
	for (int index = 0; index < 5; index++)
	{
		diff_y[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulses_to_move_history_y[index] = 0;
	}
	MyMovingPara.IsJustSet_y = 0;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.IsJustSet_history_y[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulse_abs_history_y[index] = 0;
	}
	MyMovingPara.pulses_to_move_z = 0;
	MyMovingPara.voltage_z = 0;
	MyMovingPara.stopped_and_ready_2_move_z = 1;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulses_to_move_history_z[index] = 0;
	}
	MyMovingPara.IsJustSet_z = 0;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.IsJustSet_history_z[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulse_abs_history_z[index] = 0;
	}
}

/*
 * ////====================== this section is used to stop manipulator MX7600======================//



 * ////====================== positive X axis movement of  MX7600 by preset number of steps======================//
 */
void CPicturecontrolDlg::OnClickedXPos()
{
	/* TODO: Add your control notification handler code here */
	double vol1, step1;
	step1 = GetDlgItemInt(IDC_DISTANCE_X);
	num_moving = 0;
	flag_step = 0;
	x_num++;
	mx7600_slow_flag = 0;
	MoveBy_x_steps(step1, &MyMovingPara);
}

/*
 * ////====================== positive X axis movement of  MX7600 by preset number of steps======================//



 * ////======================  negative of X axis movement of manipulator MX7600 by preset number of steps======================//
 */
void CPicturecontrolDlg::OnClickedXNeg()
{
	/* TODO: Add your control notification handler code here */
	double vol1, step1;
	num_moving = 0;
	step1 = GetDlgItemInt(IDC_DISTANCE_X);
	x_num++;
	flag_step = 0;
	mx7600_slow_flag = 0;
	MoveBy_x_steps(-step1, &MyMovingPara);
}

/*
 * ////======================  negative of X axis movement of manipulator MX7600 by preset number of steps======================//



 * ////====================== positive Y axis movement of  MX7600 by preset number of steps======================//
 */
void CPicturecontrolDlg::OnClickedYPos()
{
	/* TODO: Add your control notification handler code here */
	double vol1, step1;
	step1 = GetDlgItemInt(IDC_DISTANCE_Y);
	num_moving_y = 0;
	flag_step_y = 0;
	y_num++;
	mx7600_slow_flag = 0;
	MoveBy_y_steps(step1, &MyMovingPara);
}

/*
 * ////====================== positive Y axis movement of  MX7600 by preset number of steps======================//



 * ////====================== negative Y axis movement of  MX7600 by preset number of steps======================//
 */
void CPicturecontrolDlg::OnClickedYNeg()
{
	/* TODO: Add your control notification handler code here */
	double vol1, step1;
	step1 = GetDlgItemInt(IDC_DISTANCE_Y);
	num_moving_y = 0;
	flag_step_y = 0;
	y_num++;
	mx7600_slow_flag = 0;
	MoveBy_y_steps(-step1, &MyMovingPara);
}
////====================== negative Y axis movement of  MX7600 by preset number of steps======================//


////////////====================== positive Z axis movement of  MX7600 by preset number of steps======================//
void CPicturecontrolDlg::OnBnClickedZPos()
{
	/* TODO: Add your control notification handler code here */
	double vol1, step1;
	step1 = GetDlgItemInt(IDC_DISTANCE_Z);
	num_moving_z = 0;
	flag_step_z = 0;
	z_num++;
	mx7600_slow_flag = 0;
	MoveBy_z_steps(step1, &MyMovingPara);
}

////////////====================== Negative Z axis movement of  MX7600 by preset number of steps======================//
void CPicturecontrolDlg::OnBnClickedZNeg()
{
	// TODO: Add your control notification handler code here
	/* TODO: Add your control notification handler code here */
	double vol1, step1;
	step1 = GetDlgItemInt(IDC_DISTANCE_Z);
	num_moving_z = 0;
	flag_step_z = 0;
	z_num++;
	mx7600_slow_flag = 0;
	MoveBy_z_steps(-step1, &MyMovingPara);
}

////////////====================== Positive T axis movement of  MX7600 by preset number of steps======================//
void CPicturecontrolDlg::OnBnClickedTPos()
{
	// TODO: Add your control notification handler code here
	double vol1, step1;
	step1 = GetDlgItemInt(IDC_DISTANCE_T);
	num_moving_t = 0;
	flag_step_t = 0;
	t_num++;
	mx7600_slow_flag = 0;
	MoveBy_t_steps(step1, &MyMovingPara);
}

////////////====================== Negative T axis movement of  MX7600 by preset number of steps======================//
void CPicturecontrolDlg::OnBnClickedTNeg()
{
	// TODO: Add your control notification handler code here
	double vol1, step1;
	step1 = GetDlgItemInt(IDC_DISTANCE_T);
	num_moving_t = 0;
	flag_step_t = 0;
	t_num++;
	mx7600_slow_flag = 0;
	MoveBy_t_steps(-step1, &MyMovingPara);
}


/* ////////////// ********This thread is created for MX7600 manipulation******** ////////////// */
DWORD WINAPI SENSORYFunction_PID(LPVOID lpParameter)
{
	uint counter_x[2] = {0}; /* encoder counts when the snapshot was captured */
	uint counter_y[2] = {0}; /* encoder counts when the snapshot was captured */
	uint counter_z[2] = {0}; /* encoder counts when the snapshot was captured */
	uint counter_t[2] = {0}; /* encoder counts when the snapshot was captured */
	uint timestamp;
	uint timestamp_y; /* time the snapshot was captured */
	float vol_in = 0;
	float vol_in_y = 0;
	float vol_in_z = 0;
	float vol_in_t = 0;

	float data2[4] = {0};				  /* parameter to store voltage in four directions */
	uint data_digital_direction[4] = {0}; /* parameter to store values of four direction */
	/* uint     up_mot = 10; */
	char filenm_x[100] = {0};
	char filenm_y[100] = {0};
	int ii = 0;
	int ii_max = 99999999999999;

	initial_flag = 1;
	LARGE_INTEGER currentCount;										
	LARGE_INTEGER startCount;										
	LARGE_INTEGER endCount;											
	LARGE_INTEGER freq;												
	double dbTime, dbTime0, dbTime1_1, dbTime1, dbTime2, dbTime2_1; 
	double dbtime[20];
	double cTime;
	INT64 CurrentTime;
	INT64 frequency;
	QueryPerformanceFrequency(&freq);
	frequency = freq.QuadPart;

	/****initialize parameters of Class MyMovingPara****/
	/* MyMovingPara.pulses_to_move_x = 0; */
	MyMovingPara.voltage_x = 0;
	MyMovingPara.stopped_and_ready_2_move_x = 1;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulses_to_move_history_x[index] = 0;
	}
	MyMovingPara.IsJustSet_x = 0;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.IsJustSet_history_x[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulse_abs_history_x[index] = 0;
	}
	/* MyMovingPara.pulses_to_move_y = 0; */
	MyMovingPara.voltage_y = 0;
	MyMovingPara.stopped_and_ready_2_move_y = 1;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulses_to_move_history_y[index] = 0;
	}
	MyMovingPara.IsJustSet_y = 0;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.IsJustSet_history_y[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulse_abs_history_y[index] = 0;
	}
	
	/*MyMovingPara.pulses_to_move_z = 0;*/
	MyMovingPara.voltage_z = 0;
	MyMovingPara.stopped_and_ready_2_move_z = 1;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulses_to_move_history_z[index] = 0;
	}
	MyMovingPara.IsJustSet_z = 0;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.IsJustSet_history_z[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulse_abs_history_z[index] = 0;
	}
	/*MyMovingPara.pulses_to_move_4th_axis = 0;*/

	MyMovingPara.voltage_4th_axis = 0;
	MyMovingPara.stopped_and_ready_2_move_4th_axis = 1;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulses_to_move_history_4th_axis[index] = 0;
	}
	MyMovingPara.IsJustSet_4th_axis = 0;
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.IsJustSet_history_4th_axis[index] = 0;
	}
	for (int index = 0; index < 5; index++)
	{
		MyMovingPara.pulse_abs_history_4th_axis[index] = 0;
	}
	
	
	if (a != 32768)
	{
		a = S826_SystemOpen();
	}
	
	/*************range of analog output*******/
	S826_DacRangeWrite(15, 0, 1, 0); /* x when I set the range from 0 to 1, in the following stepI can  provide higher voltages */
	S826_DacRangeWrite(15, 2, 1, 0); /* y when I set the range from 0 to 1, in the following stepI can  provide higher voltages */
	S826_DacRangeWrite(15, 1, 1, 0); /* z when I set the range from 0 to 1, in the following stepI can  provide higher voltages */
	S826_DacRangeWrite(15, 3, 1, 0); /* t when I set the range from 0 to 1, in the following stepI can  provide higher voltages */

	/**** configure the counter channel******/
	S826_CounterModeWrite(15, 5, 0x00000070); /* Configure counter 2 as incremental encoder interface. */
	S826_CounterStateWrite(15, 5, 1);		  /* Start tracking encoder counts. */
	S826_CounterModeWrite(15, 1, 0x00000070); /* Configure counter 1 as incremental encoder interface. */
	S826_CounterStateWrite(15, 1, 1);		  /* Start tracking encoder counts. */

	S826_CounterModeWrite(15, 2, 0x00000070);  // Configure counter 2 as incremental encoder interface.
	S826_CounterStateWrite(15, 2, 1);          // Start tracking encoder counts.
	S826_CounterModeWrite(15, 3, 0x00000070);  // Configure counter 3 as incremental encoder interface.
	S826_CounterStateWrite(15, 3, 1);          // Start tracking encoder counts.


	while (1)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* Translate the message and dispatch it to WindowProc() */
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/* If the message is WM_QUIT, exit the while loop */
		if (msg.message == WM_QUIT)
			break;
		PeekandPump();
		/*
		 * ****peek finished
		 * ///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>the following code is to realize the position control of mx7600<<<<<<<<<<<<<<<<<<<<<<<///
		 */
		S826_CounterSnapshot(15, 5);
		S826_CounterSnapshot(15, 1);
		S826_CounterSnapshot(15, 2);
		S826_CounterSnapshot(15, 3);

		S826_CounterSnapshotRead(15, 5,							/* Read the snapshot: and configure the channel 2 of encoder as a feedback for the first stage */
								 counter_x, &timestamp, NULL,	/*  receive the snapshot info here */
								 0);							/*  no need to wait for snapshot; it's already been captured */
		S826_CounterSnapshotRead(15, 1,							/* Read the snapshot: */
								 counter_y, &timestamp_y, NULL, /*  receive the snapshot info here */
								 0);							/*  no need to wait for snapshot; it's already been captured */
		S826_CounterSnapshotRead(15, 2,							/* Read the snapshot: and configure the channel 2 of encoder as a feedback for the first stage */
			counter_z, &timestamp, NULL,	/*  receive the snapshot info here */
			0);							/*  no need to wait for snapshot; it's already been captured */
		S826_CounterSnapshotRead(15, 3,							/* Read the snapshot: */
			counter_t, &timestamp_y, NULL, /*  receive the snapshot info here */
			0);							/*  no need to wait for snapshot; it's already been captured */



		if (mx7600_slow_flag == 0)
		{
			
			
			for (int jj = 0; jj < 4; jj++)
			{
				MyMovingPara.pulses_to_move_history_x[4 - jj] = MyMovingPara.pulses_to_move_history_x[3 - jj];
				diff[4 - jj] = diff[3 - jj];
				MyMovingPara.pulse_abs_history_x[4 - jj] = MyMovingPara.pulse_abs_history_x[3 - jj];
			}

			MyMovingPara.pulse_abs_history_x[0] = counter_x[0];
			original_position = counter_x[0]; /* / this parameter is used to move the tiltling stage in force-control section */
			original_pulses_6 = counter_x[0];
			if (original_pulses_x_flag == 1) /* //////recording the encoder value when device is in center along y-axis */
			{
				original_pulses_x = counter_x[0];
				original_pulses_x_flag = 0;
				CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
				CString X_pos_HOME;
				X_pos_HOME.Format(L"%d", original_pulses_x);
				pMwnd->SetDlgItemText(IDC_EDITXH, X_pos_HOME);
			}



			for (int jj = 0; jj < 4; jj++)
			{
				MyMovingPara.pulses_to_move_history_y[4 - jj] = MyMovingPara.pulses_to_move_history_y[3 - jj];
				diff_y[4 - jj] = diff_y[3 - jj];
				MyMovingPara.pulse_abs_history_y[4 - jj] = MyMovingPara.pulse_abs_history_y[3 - jj];
			}
			MyMovingPara.pulse_abs_history_y[0] = counter_y[0];
			original_pulses_first_y = counter_t[0];
			if (original_pulses_y_flag==1)////////recording the encoder value when device is away from the substrate
			{
			  original_pulses_y = counter_y[0];
			  original_pulses_y_flag = 0;
			  CPicturecontrolDlg* pMwnd = (CPicturecontrolDlg*)AfxGetApp()->m_pMainWnd;
			  CString Y_pos_FAR;
			  Y_pos_FAR.Format(L"%d", original_pulses_y);
			  pMwnd->SetDlgItemText(IDC_EDITYF, Y_pos_FAR);
			}



			for (int jj = 0; jj < 4; jj++)
			{
				MyMovingPara.pulses_to_move_history_z[4 - jj] = MyMovingPara.pulses_to_move_history_z[3 - jj];
				diff_z[4 - jj] = diff_z[3 - jj];
				MyMovingPara.pulse_abs_history_z[4 - jj] = MyMovingPara.pulse_abs_history_z[3 - jj];
			}

			if (original_pulses_z_flag == 1) /* //////recording the encoder value when microlfuidic device is close the substrate */
			{
				original_pulses_z = counter_z[0];
				original_pulses_z_flag = 0;
				CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
				CString Z_pos_CLOSE;
				Z_pos_CLOSE.Format(L"%d", original_pulses_z);
				pMwnd->SetDlgItemText(IDC_EDITZC, Z_pos_CLOSE);
			}

			MyMovingPara.pulse_abs_history_z[0] = counter_z[0];
			original_pulses_2 = counter_z[0];


			for (int jj = 0; jj < 4; jj++)
			{
				MyMovingPara.pulses_to_move_history_4th_axis[4 - jj] = MyMovingPara.pulses_to_move_history_4th_axis[3 - jj];
				diff_t[4 - jj] = diff_t[3 - jj];
				MyMovingPara.pulse_abs_history_4th_axis[4 - jj] = MyMovingPara.pulse_abs_history_4th_axis[3 - jj];
			}
			if (original_pulses_t_flag == 1) /* //////recording the encoder value when microlfuidic device is close the substrate */
			{
				original_pulses_t = counter_t[0];
				original_pulses_t_flag = 0;
				CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
				CString T_pos_CLOSE;
				T_pos_CLOSE.Format(L"%d", original_pulses_t);
				pMwnd->SetDlgItemText(IDC_EDITTH, T_pos_CLOSE);
			}
			MyMovingPara.pulse_abs_history_4th_axis[0] = counter_t[0];
			original_pulses_fourth_t = counter_t[0];

			

			///////////////////>>>>>>>>>>>>>>>>>>>   pid control for X-axis motor<<<<<<<<<<<<<<<<<<<<<<<<<//////////////////////
			if (num_counter > 10)
			{
				MyMovingPara.pulses_to_move_x -= (MyMovingPara.pulse_abs_history_x[0] - MyMovingPara.pulse_abs_history_x[1]); /* /////// Update the steps to move in real-time */
				diff[0] = MyMovingPara.pulse_abs_history_x[1] - MyMovingPara.pulse_abs_history_x[0];						  /* ////////////// Update the diff in real time */
				pro = MyMovingPara.pulses_to_move_x;																		  /* /////// Update the ERROR to move in real-time */
				MyMovingPara.pulses_to_move_history_x[0] = MyMovingPara.pulses_to_move_x;									  /* /////// Update the steps to move in real-time */
			}
			if (abs(pro) > 20000)
			{
				vol_in = 5; /* / */
				max_vol_flag = 1;
			}
			if (abs(pro) < 20001 && abs(pro) > 6000)
			{
				inte += MyMovingPara.pulses_to_move_x;
				vol_in = 0.6 * pro * 0.001 + 0.0000005 * inte + 0.25 * diff[0];
				vol_in = abs(vol_in);
				if (abs(vol_in) > 5)
					vol_in = 5;
			}
			if (abs(pro) > 100 && abs(pro) < 6001) 
			{
				vol_in = 5 * pro * 0.001 + 0.25 * diff[0];
				vol_in = abs(vol_in);
				if (abs(vol_in) > 5)
				{
					vol_in = 5;
				}
			}

			if (abs(pro) < 100) 
			{
				vol_in = 0; 
			}
			data2[0] = vol_in;
			data_digital_direction[0] = max_weize(-sign_weize(MyMovingPara.pulses_to_move_x), 0);

			if (data_digital_direction[0] == 0)
				mxx7600->set_x_speed(data2[0]);
			else
				mxx7600->set_x_speed(-data2[0]);

		

			if (num_counter == 0)
			{
				counter_ini = counter_x[0];
			}

			
			if ((counter_x[0] != counter_ini) && (abs(MyMovingPara.pulses_to_move_x) < 100) && (vol_in == 0)) 
			{
				num_moving++;
				inte = 0;
				MyMovingPara.pulses_to_move_x = 0;
				MyMovingPara.voltage_x = 0;
				MyMovingPara.stopped_and_ready_2_move_x = 1;
				for (int index = 0; index < 5; index++)
				{
					diff[index] = 0;
				}
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.pulses_to_move_history_x[index] = 0;
				}
				MyMovingPara.IsJustSet_x = 0;
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.IsJustSet_history_x[index] = 0;
				}
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.pulse_abs_history_x[index] = 0;
				}
				num_counter = 0;

				
				if (z_tophalf_flag == 1)
				{
					z_half_done_flag = 1; /* /// this flag is used to start the following operation of device lowering down */
					z_tophalf_flag = 0;
				}
				if (z_bothalf_flag == 1)
				{
					z_half_done_flag = 1;
					z_bothalf_flag = 0;
				}
				if (pre_button_x_flag == 1)
				{
					mx7600_movingdone_x_flag = 1;
					pre_button_x_flag = 0;
				}
			}



			// y pid


			if (num_counter_y > 10)
			{
				MyMovingPara.pulses_to_move_y -= (MyMovingPara.pulse_abs_history_y[0] - MyMovingPara.pulse_abs_history_y[1]); /* /////// Update the steps to move in real-time */
				diff_y[0] = MyMovingPara.pulse_abs_history_y[1] - MyMovingPara.pulse_abs_history_y[0];						  /* ////////////// Update the diff in real time */
				pro_y = MyMovingPara.pulses_to_move_y;																		  /* /////// Update the ERROR to move in real-time */
				MyMovingPara.pulses_to_move_history_y[0] = MyMovingPara.pulses_to_move_y;									  /* /////// Update the steps to move in real-time */
			}
			if (abs(pro_y) > 20000)
			{
				vol_in_y = 5; /* / */
				max_vol_flag_y = 1;
			}
			if (abs(pro_y) < 20001 && abs(pro_y) > 6000)
			{
				inte_y += MyMovingPara.pulses_to_move_y;
				vol_in_y = 0.6 * pro_y * 0.001 + 0.0000005 * inte_y + 0.25 * diff_y[0];
				vol_in_y = abs(vol_in_y);
				if (abs(vol_in_y) > 5)
					vol_in_y = 5;
			}
			if (abs(pro_y) > 100 && abs(pro_y) < 6001) 
			{
				vol_in_y = 5 * pro_y * 0.001 + 0.25 * diff_y[0];
				vol_in_y = abs(vol_in_y);
				if (abs(vol_in_y) > 5)
				{
					vol_in_y = 5;
				}
			}

			{
				vol_in_y = 0; 
			}

			data2[1] = vol_in_y;
			data_digital_direction[1] = max_weize(-sign_weize(MyMovingPara.pulses_to_move_y), 0);

			if (data_digital_direction[1]==0)
				mxx7600->set_y_speed(data2[1]);
			else
				mxx7600->set_y_speed(-data2[1]);

			

			if (num_counter_y == 0)
			{
				counter_ini_y = counter_y[0];
			}

			if ((counter_y[0] != counter_ini_y) && (abs(MyMovingPara.pulses_to_move_y) < 100) && (vol_in_y == 0)) 
			{
				num_moving_y++;
				inte_y = 0;

				MyMovingPara.pulses_to_move_y = 0;
				MyMovingPara.voltage_y = 0;
				MyMovingPara.stopped_and_ready_2_move_y = 1;
				for (int index = 0; index < 5; index++)
				{
					diff_y[index] = 0;
				}
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.pulses_to_move_history_y[index] = 0;
				}
				MyMovingPara.IsJustSet_y = 0;
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.IsJustSet_history_y[index] = 0;
				}
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.pulse_abs_history_y[index] = 0;
				}
				num_counter_y = 0;

				if (z_tophalf_flag == 1)
				{
					z_half_done_flag = 1; /* /// this flag is used to start the following operation of device lowering down */
					z_tophalf_flag = 0;
				}
				if (z_bothalf_flag == 1)
				{
					z_half_done_flag = 1;
					z_bothalf_flag = 0;
				}
				if (pre_button_x_flag == 1)
				{
					mx7600_movingdone_x_flag = 1;
					pre_button_x_flag = 0;
				}
			}



			/*****************manipulation of mx7600r in z direction starts****************************/
			if (num_counter_z > 10)
			{
				MyMovingPara.pulses_to_move_z -= (MyMovingPara.pulse_abs_history_z[0] - MyMovingPara.pulse_abs_history_z[1]);
				diff_z[0] = MyMovingPara.pulse_abs_history_z[1] - MyMovingPara.pulse_abs_history_z[0];
				pro_z = MyMovingPara.pulses_to_move_z;
				MyMovingPara.pulses_to_move_history_z[0] = MyMovingPara.pulses_to_move_z;
			}
			if (abs(pro_z) > 20000)
			{
				vol_in_z = 5;
				max_vol_flag_z = 1;
			}
			if (abs(pro_z) < 20001 && abs(pro_z) > 6000)
			{
				max_vol_flag_1_z = 1;
				inte_z += MyMovingPara.pulses_to_move_z;
				vol_in_z = 0.6 * pro_z * 0.001 + 0.0000005 * inte_z + 0.25 * diff_z[0];
				vol_in_z = abs(vol_in_z);
				if (abs(vol_in_z) > 5)
					vol_in_z = 5;
			}
			if (abs(pro_z) > 100 && abs(pro_z) < 6001) 
			{
				vol_in_z = 5 * pro_z * 0.001 + 0.25 * diff_z[0];
				vol_in_z = abs(vol_in_z);
				if (abs(vol_in_z) > 5)
				{
					vol_in_z = 5;
				}
			}

			if (abs(pro_z) < 101) 
			{
				vol_in_z = 0;
			}
			data2[2] = vol_in_z;
			data_digital_direction[2] = max_weize(-sign_weize(MyMovingPara.pulses_to_move_z), 0);
			if (data_digital_direction[2] == 0)
				mxx7600->set_z_speed(data2[2]);
			else
				mxx7600->set_z_speed(-data2[2]);

		
			if (num_counter_z == 0)
			{
				counter_ini_z = counter_z[0];
			}

			if ((counter_z[0] != counter_ini_z) && (abs(MyMovingPara.pulses_to_move_z) < 100) && (vol_in_z == 0))
			{
				num_moving_z++;
				
				inte_z = 0;
				MyMovingPara.pulses_to_move_z = 0;
				MyMovingPara.voltage_z = 0;
				MyMovingPara.stopped_and_ready_2_move_z = 1;
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.pulses_to_move_history_z[index] = 0;
				}
				MyMovingPara.IsJustSet_z = 0;
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.IsJustSet_history_z[index] = 0;
				}
				for (int index = 0; index < 5; index++)
				{
					MyMovingPara.pulse_abs_history_z[index] = 0;
				}
				num_counter_z = 0;
				
				if (worm_trans_all_flag == 1 && device_trans_loading_flag == 1) /* /////during microlfuidic device automatically lowering down, see whether the z-axis movement is done */
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_loading_flag = 0;
				}
				if (device_trans_lift_flag == 1 && worm_trans_all_flag == 1) /* /////during microlfuidic device automatically lifting up, see whether the z-axis movement is done */
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_lift_flag = 0;
				}
				if (worm_lift_extract_flag == 1 && device_trans_lift_flag == 1)
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_lift_flag = 0;
				}
				if (device_all_home_flag == 1 && device_trans_loading_flag == 1)
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_loading_flag = 0;
				}
				if (device_all_home_flag == 1 && device_trans_homex == 1)
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_homex = 0;
				}
				if (worm_rotation_all_flag == 1 && device_trans_loading_flag == 1)
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_loading_flag = 0;
				}
				if (device_trans_transfer_flag == 1 && worm_release_all_flag == 1)
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_loading_flag = 0;
				}


				if (z_lower_injection_flag == 1) /* /////// this flag is used for the follwoing automated pipette movement to FOV */
				{
					z_lower_injection_flag = 0;
					z_lower_done_injection_flag = 1;
				}
				if (device_trans_transfer_flag == 1)
				{
					mx7600_movingdone_z_flag = 1;
					device_trans_transfer_flag = 0;
				}
				if (pre_button_z_flag == 1)
				{
					mx7600_movingdone_z_flag = 1;
					pre_button_z_flag = 0;
				}
			}
			num_counter++;
			num_counter_y++;
			num_counter_z++;
		}


		if (mx7600_slow_flag == 1) /* ///////////////Stopping mx7600 movement */
		{
			MyMovingPara.pulses_to_move_x = 0;
			MyMovingPara.voltage_x = 0;
			MyMovingPara.stopped_and_ready_2_move_x = 1;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulses_to_move_history_x[index] = 0;
			}
			MyMovingPara.IsJustSet_x = 0;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.IsJustSet_history_x[index] = 0;
			}
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulse_abs_history_x[index] = 0;
			}
			inte = 0;
			num_counter = 0;
			flag_step = 0;


			MyMovingPara.pulses_to_move_y = 0;
			MyMovingPara.voltage_y = 0;
			MyMovingPara.stopped_and_ready_2_move_y = 1;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulses_to_move_history_y[index] = 0;
			}
			MyMovingPara.IsJustSet_y = 0;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.IsJustSet_history_y[index] = 0;
			}
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulse_abs_history_y[index] = 0;
			}
			inte_y = 0;
			num_counter_y = 0;
			flag_step_y = 0;


			MyMovingPara.pulses_to_move_z = 0;
			MyMovingPara.voltage_z = 0;
			MyMovingPara.stopped_and_ready_2_move_z = 1;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulses_to_move_history_z[index] = 0;
			}
			MyMovingPara.IsJustSet_z = 0;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.IsJustSet_history_z[index] = 0;
			}
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulse_abs_history_z[index] = 0;
			}
			inte_z = 0;
			num_counter_z = 0;
			flag_step_z = 0;

			MyMovingPara.pulses_to_move_4th_axis = 0;
			MyMovingPara.voltage_4th_axis = 0;
			MyMovingPara.stopped_and_ready_2_move_4th_axis = 1;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulses_to_move_history_4th_axis[index] = 0;
			}
			MyMovingPara.IsJustSet_4th_axis = 0;
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.IsJustSet_history_4th_axis[index] = 0;
			}
			for (int index = 0; index < 5; index++)
			{
				MyMovingPara.pulse_abs_history_4th_axis[index] = 0;
			}

			vol_in = 0;
			vol_in_y = 0;
			vol_in_z = 0;
		
			data2[0] = 0;				   /* parameter to store voltage in four directions */
			data2[1] = 0;				   /* parameter to store voltage in four directions */
			data2[2] = 0;				   /* parameter to store voltage in four directions */
			data2[3] = 0;				   /* parameter to store voltage in four directions */
			data_digital_direction[0] = 0; /* parameter to store values of four direction */
			data_digital_direction[1] = 0; /* parameter to store values of four direction */
			SetDacOutput(15, 2, 0);
			SetDacOutput(15, 3, 0); /* ////0v upwards */
			SetDacOutput(15, 0, 0); /* ///////////////////////////////////////////////////////////////////////////////////////////////////// */
			SetDacOutput(15, 1, 0);
			mx7600_slow_flag = 0;
		}
	}
	return (true);
}

/* ///////////////////>>>>>>>>>>>>>>>>>>>>>>>>MP285 CONTROL MANIPULATION<<<<<<<<<<<<<<<<<<<<<<<<<<</// */
void CPicturecontrolDlg::OnClickedCurrent()
{
	mp285->get_current_position(m_xValue, m_yValue, m_zValue);
	CString X_pos, Y_pos, Z_pos;
	X_pos.Format(L"%f", m_xValue);
	Y_pos.Format(L"%f", m_yValue);
	Z_pos.Format(L"%f", m_zValue);
	SetDlgItemText(IDC_EDITXPOS2, X_pos);
	SetDlgItemText(IDC_EDITYPOS2, Y_pos);
	SetDlgItemText(IDC_EDITZPOS2, Z_pos);
	Mp285Curflg = 1;
	if (MP285_Pos_Initial_back_flag == 1)
	{
		m_xValue_home = m_xValue;
		m_yValue_home = m_yValue;
		m_zValue_home = m_zValue;
		MP285_Pos_Initial_back_flag = 0;
	}
	if (MP285_Pos_Initial_MHI_flag == 1)
	{
		m_xValue_MHI_home = m_xValue;
		m_yValue_MHI_home = m_yValue;
		m_zValue_MHI_home = m_zValue;
		MP285_Pos_Initial_MHI_flag = 0;
	}
}

void CPicturecontrolDlg::OnClickedConnect() /* ////////////////Initialize MP285 for position mode */
{
	bool mp285_status = false;
	mp285_status = mp285->open();
	if (mp285_status)
	{
		SetDlgItemText(IDC_EDITSTATUS, L"connexted");
		Mp285Conflg = 1;
	}
	else
	{
		SetDlgItemText(IDC_EDITSTATUS, L" not connexted");
	}
	CString speed_value, move_step;
	speed_value.Format(L"%d", 1000);
	move_step.Format(L"%d", 25);
	SetDlgItemText(IDC_EDITSPEED, speed_value);
	SetDlgItemText(IDC_STEP1, move_step);
	OnClickedSetspe();
	OnClickedCurrent();
}

void CPicturecontrolDlg::OnClickedSetspe()
{
	unsigned short value;
	value = unsigned short(GetDlgItemInt(IDC_EDITSPEED));
	mp285->set_speed(value);
	Mp285Spdflg = 1;
}

void CPicturecontrolDlg::OnClickedMove()
{
	/* TODO: Add your control notification handler code here */
	long x_move = long(GetDlgItemInt(IDC_EDITXPOS));
	long y_move = long(GetDlgItemInt(IDC_EDITYPOS));
	long z_move = long(GetDlgItemInt(IDC_EDITZPOS));
	moveto(x_move, y_move, z_move);
}

void CPicturecontrolDlg::OnClickedStopMp()
{
	mp285->stop();
}

void CPicturecontrolDlg::OnClickedUp() /* ///// for x-axis movemnt */
{
	long x_step = long(GetDlgItemInt(IDC_STEP1));
	if (MP285_stop_moving == 4)
	{
		if (x_step > 45)
		{
			x_step = 45;
		}
	}
	m_xValuego = long((m_xValue + x_step));
	m_yValuego = long(m_yValue);
	m_zValuego = long(m_zValue);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnhomePS_MP_up() /* /// withdraw pipette to home position and record the final home position */
{
	if (MP285_Pos_Initial_flag == 1) /* //// this flag is used to get the initial home position of pipette */
	{
		MP285_Pos_Initial_back_flag = 1;
		MP285_Pos_Initial_flag = 0;
	}
	m_xValuego = long((m_xValue - 1000));
	m_yValuego = long(m_yValue);
	m_zValuego = long(m_zValue - 1000);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnhomePS_MP_up2() /* ///  withdraw pipette to the finally recorded home position */
{
	m_xValuego = long((m_xValue_home));
	m_yValuego = long(m_yValue_home);
	m_zValuego = long(m_zValue_home);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnhomePS_MP_up6() /* /// withdraw pipette to the home position predetermined by MHI */
{
	m_xValuego = long((m_xValue_MHI_home));
	m_yValuego = long(m_yValue_MHI_home);
	m_zValuego = long(m_zValue_MHI_home);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnhomePS_MP_up3() /* /// lift up pipette by 200um based on home position of pipette/// */
{
	m_xValuego = long((m_xValue_home));
	m_yValuego = long(m_yValue_home);
	m_zValuego = long(m_zValue_home - 200);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

/*void CPicturecontrolDlg::OnhomePS_MP_down() 
{
	m_xValuego = long((m_xValue + 1000));
	m_yValuego = long(m_yValue);
	m_zValuego = long(m_zValue + 1000);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}*/

void CPicturecontrolDlg::OnhomePS_MP_down2() /* ///move pipette foward to FOV based on the the home position   20210216///based on home position of pipette */
{
	m_xValuego = long((m_xValue_home + 1000));
	m_yValuego = long(m_yValue_home);
	m_zValuego = long(m_zValue_home + 1000);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnhomePS_MP_down3() /* ///move pipette foward to the the home position   20210216///based on home position of pipette */
{
	m_xValuego = long((m_xValue_home));
	m_yValuego = long(m_yValue_home);
	m_zValuego = long(m_zValue_home);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnClickedDown()//////MP285 RIGHT MOVEMNT 
{
	/* TODO: Add your control notification handler code here */
	OnClickedCurrent();
	Sleep(200);
	/* / */
	long x_step = long(GetDlgItemInt(IDC_STEP1));
	m_xValuego = long((m_xValue - x_step));
	m_yValuego = long(m_yValue);
	m_zValuego = long(m_zValue);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnClickedLeft()//////MP285 LEFT MOVEMNT 
{
	/* TODO: Add your control notification handler code here */
	OnClickedCurrent();
	Sleep(200);
	long y_step = long(GetDlgItemInt(IDC_STEP1));
	m_xValuego = long(m_xValue);
	m_yValuego = long((m_yValue - y_step));
	m_zValuego = long(m_zValue);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnClickedRight()//////MP285 RIGHT MOVEMNT 
{
	/* TODO: Add your control notification handler code here */
	OnClickedCurrent();
	Sleep(200);
	/* / */
	long y_step = long(GetDlgItemInt(IDC_STEP1));
	m_xValuego = long(m_xValue);
	m_yValuego = long((m_yValue + y_step));
	m_zValuego = long(m_zValue);
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnBnClickedUpwards()//////MP285 UPWARD MOVEMNT 
{
	/* TODO: Add your control notification handler code here */
	OnClickedCurrent();
	Sleep(200);
	long z_step = long(GetDlgItemInt(IDC_STEP1));
	m_xValuego = long(m_xValue);
	m_yValuego = long(m_yValue);
	m_zValuego = long((m_zValue - z_step));
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnBnClickedDownwards()//////MP285 DOWNWARD MOVEMNT 
{
	/* TODO: Add your control notification handler code here */
	OnClickedCurrent(); /* /// */
	Sleep(200);
	long z_step = long(GetDlgItemInt(IDC_STEP1));
	m_xValuego = long(m_xValue);
	m_yValuego = long(m_yValue);
	m_zValuego = long((m_zValue + z_step));
	moveto(m_xValuego, m_yValuego, m_zValuego);
}

void CPicturecontrolDlg::OnBnClickedDiadown()
{
	/* TODO: Add your control notification handler code here */
	if (MP285_pOS_Initial_MHI_YN == 1)
	{
		MP285_stop_moving = 99;
	}
}

void CPicturecontrolDlg::OnBnClickedDiarep() 
{
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	MP285_Pos_Initial_MHI_flag = 1;
	MP285_pOS_Initial_MHI_YN = 1; /* //IN cASE THE HOME POSITION IS NOT SET */
	OnClickedCurrent();			  /* /// OBTAIN THE current tip position and set it for the home position for MHI processing */
	Sleep(200);
	pMwnd->SetDlgItemText(IDC_EDITSNUM, L"HM SET");
}


/* /////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Creation of the thread for speed control of injection pipette starts<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//////// */
void CPicturecontrolDlg::OnClickedAutoSpeed()
{
	CreateThread(NULL, 0, Autotracking_speed, NULL, 0, NULL);
	MP285_stop_moving = 1;
}

/* /////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Creation of the thread for speed control of injection pipette starts<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//////// */
DWORD WINAPI Autotracking_speed(LPVOID lpParameter) /* ///////////////////////For speed control mode of the injection pipette */
{
	float x_moving = 0;
	float x_objectivespeed = 0;
	float x_voltage_control_feedforward;
	float y_moving = 0;
	float y_objectivespeed = 0;
	float y_voltage_control_feedforward;
	int iii = 0;
	int num_counter_Mp285 = 0;
	int lerp_marker_x = 0;
	int lerp_marker_y = 0;
	float sina_1 = 0;
	float cosa_1 = 0;
	float delta_x_pip = 0;
	float delta_y_pip = 0;
	float delta_x_pip_original = 0;
	float delta_y_pip_original = 0;
	float dis_verification[10] = {0};
	float delta_x_pip_pre = 0;
	float delta_y_pip_pre = 0;
	float delta_x_pip_inte = 0;
	float delta_x_pip_diff = 0;
	float delta_y_pip_inte = 0;
	float delta_y_pip_diff = 0;
	int num_ini = 0;
	float dis_1;
	LARGE_INTEGER currentCount;		  /* MOST ACCURATE TIMER METHOD */
	LARGE_INTEGER startCount;		  /*  */
	LARGE_INTEGER endCount;			  /*  */
	LARGE_INTEGER freq;				  /*  */
	double dbTime;					  /*  */
	double cTime;					  /*  */
	INT64 CurrentTime;				  /*  */
	INT64 frequency;				  /*  */
	QueryPerformanceFrequency(&freq); /*  */
	frequency = freq.QuadPart;

	/* ////MP285 calibration */
	float cal_voltage[69] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.01, 1.02, 1.03, 1.04, 1.05, 1.06, 1.07, 1.08, 1.09, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4, 4.01, 4.02, 4.03, 4.04, 4.05, 4.06, 4.07, 4.08, 4.09, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 5};
	float cal_speed[69] = {-1265, -1265, -1270, -1265, -1269, -1263, -1263, -1277, -1267, -1146, -489, -292, -206, -163, -133, -112, -96, -86, -76, -69, -36, -24, -19, -27, -25, -20, -19, -16, -15, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 9, 10, 11, 13, 17, 20, 29, 46, 123, 150, 186, 253, 410, 874, 1277, 1280, 1281, 1266, 1266, 1288, 1281, 1263, 1270, 1261, 1266, 1289, 1282, 1264};
	float k_control_scaling = 10; /*  */
	if (a != 32768)
	{
		a = S826_SystemOpen();
	}
	SetDacOutput(15, 4, 2.5);
	SetDacOutput(15, 5, vol_mp285_y);
	SetDacOutput(15, 6, vol_mp285_z);
	SetDacOutput(15, 7, vol_mp285_drive);
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	while (true)
	{
		if (MP285_stop_moving == 0) /* ////// for speed control mode */
		{
			if (flag_tip_btndown == 1)
			{
				sina_1 = sina(Pip_tip_global.x, Pip_tip_to_move.x, Pip_tip_global.y, Pip_tip_to_move.y);
				cosa_1 = cosa(Pip_tip_global.x, Pip_tip_to_move.x, Pip_tip_global.y, Pip_tip_to_move.y);
				delta_y_pip = sina_1 * caldistance(Pip_tip_global.x, Pip_tip_to_move.x, Pip_tip_global.y, Pip_tip_to_move.y);
				delta_x_pip = cosa_1 * caldistance(Pip_tip_global.x, Pip_tip_to_move.x, Pip_tip_global.y, Pip_tip_to_move.y) - 40; /* //// let the pipette moves to the position,  40um away from the position determined by the screen click */	
				dis_1 = sqrt(delta_y_pip * delta_y_pip + delta_x_pip * delta_x_pip);
				if (num_ini == 0)
				{
					delta_x_pip_pre = delta_x_pip;
					delta_y_pip_pre = delta_y_pip;
					delta_x_pip_original = delta_x_pip;
					delta_y_pip_original = delta_y_pip;
					for (int i = 0; i < 10; i++)
					{
						dis_verification[i] = dis_1;
					}
				}
				else
				{
					for (int i = 0; i < 9; i++)
					{
						dis_verification[i + 1] = dis_verification[i];
					}
					dis_verification[0] = dis_1;
				}
				delta_y_pip_inte += delta_y_pip;
				delta_x_pip_inte += delta_x_pip;
				delta_x_pip_diff = delta_x_pip - delta_x_pip_pre;
				delta_y_pip_diff = delta_y_pip - delta_y_pip_pre;
				num_ini++;
				delta_x_pip_pre = delta_x_pip;
				delta_y_pip_pre = delta_y_pip;
			}

			if ((abs(dis_verification[0]) <= 5))
			{
				vol_mp285_y = 2.5;
				vol_mp285_x = 2.5;
				MP285_stop_moving = 1;		   /* // This flag indicates that the Mp285  STOP MOVING */
				MP285_btnleft_moving_done = 1; /* // This flag indicates that the Mp285 movemnt of btnleft clicked is done */
				delta_y_pip_inte = 0;
				delta_x_pip_inte = 0;
				flag_tip_btndown = 0;
			}
			if (dis_verification[0] > 5)
			{
				if (abs(delta_x_pip) > 100)
				{
					k_control_scaling = 0.5;
					x_moving = delta_x_pip * 1;																				 /* each pixel is recalibrated as 2.5um under 4x objective 1um under 10x */
					x_objectivespeed = x_moving * k_control_scaling + 0.01 * delta_x_pip_diff + 0.000001 * delta_x_pip_inte; 
					if (x_objectivespeed > 1264)
						x_objectivespeed = 1264;
					if (x_objectivespeed < -1265)
						x_objectivespeed = -1265;
					for (iii = 0; iii < 68; iii++)
					{
						if ((cal_speed[iii] <= x_objectivespeed) && (cal_speed[iii + 1] >= x_objectivespeed))
						{
							lerp_marker_x = iii;
							vol_mp285_x = cal_voltage[iii] + (cal_voltage[iii + 1] - cal_voltage[iii]) * (x_objectivespeed - cal_speed[iii]) / (cal_speed[iii + 1] - cal_speed[iii]);
							break;
						}
					}
				}
				if (abs(delta_x_pip) > 50 && abs(delta_x_pip) < 100)
				{
					k_control_scaling = 0.8;
					x_moving = delta_x_pip * 1;
					x_objectivespeed = x_moving * k_control_scaling + 0.01 * delta_x_pip_diff + 0.000001 * delta_x_pip_inte;
					if (x_objectivespeed > 1264)
						x_objectivespeed = 1264;
					if (x_objectivespeed < -1265)
						x_objectivespeed = -1265;
					for (iii = 0; iii < 68; iii++)
					{
						if ((cal_speed[iii] <= x_objectivespeed) && (cal_speed[iii + 1] >= x_objectivespeed))
						{
							lerp_marker_x = iii;
							vol_mp285_x = cal_voltage[iii] + (cal_voltage[iii + 1] - cal_voltage[iii]) * (x_objectivespeed - cal_speed[iii]) / (cal_speed[iii + 1] - cal_speed[iii]);
							break;
						}
					}
				}
				if (abs(delta_x_pip) <= 50 && abs(delta_x_pip) > 5)
				{
					k_control_scaling = 2;
					x_moving = delta_x_pip * 1;
					x_objectivespeed = x_moving * k_control_scaling + 0.01 * delta_x_pip_diff + 0.000001 * delta_x_pip_inte;
					if (x_objectivespeed > 1264)
						x_objectivespeed = 1264;
					if (x_objectivespeed < -1265)
						x_objectivespeed = -1265;
					for (iii = 0; iii < 68; iii++)
					{
						if ((cal_speed[iii] <= x_objectivespeed) && (cal_speed[iii + 1] >= x_objectivespeed))
						{
							lerp_marker_x = iii;
							vol_mp285_x = cal_voltage[iii] + (cal_voltage[iii + 1] - cal_voltage[iii]) * (x_objectivespeed - cal_speed[iii]) / (cal_speed[iii + 1] - cal_speed[iii]);
							break;
						}
					}
				}
				if (abs(delta_x_pip) <= 5)
				{
					vol_mp285_x = 2.5;
				}
				if (abs(delta_y_pip) > 100) /*  */
				{
					y_moving = delta_y_pip * 1; /*  */
					k_control_scaling = 0.5;	/* / */
					y_objectivespeed = y_moving * k_control_scaling + 0.01 * delta_y_pip_diff + 0.000001 * delta_y_pip_inte;
					if (y_objectivespeed > 1264)
						y_objectivespeed = 1264;
					if (y_objectivespeed < -1265)
						y_objectivespeed = -1265;
					for (iii = 0; iii < 68; iii++)
					{
						if ((cal_speed[iii] <= y_objectivespeed) && (cal_speed[iii + 1] >= y_objectivespeed))
						{
							lerp_marker_y = iii;
							vol_mp285_y = cal_voltage[iii] + (cal_voltage[iii + 1] - cal_voltage[iii]) * (y_objectivespeed - cal_speed[iii]) / (cal_speed[iii + 1] - cal_speed[iii]);
							break;
						}
					}
				}
				if (abs(delta_y_pip) > 50 && abs(delta_y_pip) < 100)
				{
					y_moving = delta_y_pip * 1; 
					k_control_scaling = 0.8;
					y_objectivespeed = y_moving * k_control_scaling + 0.01 * delta_y_pip_diff + 0.000001 * delta_y_pip_inte; /* +0.0001*delta_y_pip_inte;// +0.1*delta_y_pip_diff; */
					if (y_objectivespeed > 1264)
						y_objectivespeed = 1264;
					if (y_objectivespeed < -1265)
						y_objectivespeed = -1265;
					for (iii = 0; iii < 68; iii++)
					{
						if ((cal_speed[iii] <= y_objectivespeed) && (cal_speed[iii + 1] >= y_objectivespeed))
						{
							lerp_marker_y = iii;
							vol_mp285_y = cal_voltage[iii] + (cal_voltage[iii + 1] - cal_voltage[iii]) * (y_objectivespeed - cal_speed[iii]) / (cal_speed[iii + 1] - cal_speed[iii]);
							break;
						}
					}
				}
				if (abs(delta_y_pip) > 5 && abs(delta_y_pip) <= 50)
				{
					y_moving = delta_y_pip * 1;																				 /* each pixel is recalibrated as 25um */
					k_control_scaling = 2;																					 /* 1 */
					y_objectivespeed = y_moving * k_control_scaling + 0.01 * delta_y_pip_diff + 0.000001 * delta_y_pip_inte; /* + 0.001*delta_y_pip_inte;// +0.1*delta_y_pip_diff; */
					if (y_objectivespeed > 1264)
						y_objectivespeed = 1264;
					if (y_objectivespeed < -1265)
						y_objectivespeed = -1265;
					for (iii = 0; iii < 68; iii++)
					{
						if ((cal_speed[iii] <= y_objectivespeed) && (cal_speed[iii + 1] >= y_objectivespeed))
						{
							lerp_marker_y = iii;
							vol_mp285_y = cal_voltage[iii] + (cal_voltage[iii + 1] - cal_voltage[iii]) * (y_objectivespeed - cal_speed[iii]) / (cal_speed[iii + 1] - cal_speed[iii]);
							break;
						}
					}
				}
				if (abs(delta_y_pip) <= 5)
				{
					vol_mp285_y = 2.5;
				}
			}
			SetDacOutput(15, 4, vol_mp285_x);
			SetDacOutput(15, 5, vol_mp285_y);
		}
		if (MP285_stop_moving == 1) /* //////////////this flag indicates that following worm penetration will be performed */
		{
			vol_mp285_x = 2.5;
			vol_mp285_y = 2.5; 
			vol_mp285_z = 2.5;
			SetDacOutput(15, 4, vol_mp285_x);
			SetDacOutput(15, 6, vol_mp285_z);
			SetDacOutput(15, 5, vol_mp285_y);
			if (MP285_btnleft_moving_done == 1)
			{
				imgpro_pip_flag = 1; /* ////image processing for pip-tip is terminated */
				MP285_stop_moving = 4;
				MP285_btnleft_moving_done = 0;
			}
		}
		if (MP285_stop_moving == 44) /* lift up pipette */
		{
			
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->OnClickedCurrent();
			Sleep(200);
			pMwnd->OnhomePS_MP_down2();
			Sleep(1500);

			/* /////displacement control///// */
			MP285_stop_moving_quick_fw = 1;
			/* } */
			MP285_stop_moving = 1;
			imgpro_pip_flag = 2;
			pMwnd->SetDlgItemText(IDC_EDITPIPMOVEMENT, L"mouse click prepared");
		}

		if (MP285_stop_moving == 444) /* ///// lift up pipette */
		{
			/* /////displacement control///// */
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->OnClickedCurrent();
			Sleep(200);
			pMwnd->OnhomePS_MP_down3();
			Sleep(1500);
			MP285_stop_moving = 1;
			imgpro_pip_flag = 1; /* ////image processing is terminated */
								 
		}

		/* ///controller box: <2.5v move backwards */
		if (MP285_stop_moving == 4) /* && imgpro_flg ==0 ///// */
		{
			QueryPerformanceCounter(&startCount);
			SetDacOutput(15, 4, 3.9);																		   /*  */
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
			while (dbTime < 1300)
			{
				QueryPerformanceCounter(&endCount);																   
				dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
			}
			SetDacOutput(15, 4, 2.5);
			vol_mp285_x = 2.5; /* 2.5 */
			MP285_stop_moving = 1;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->OnClickedCurrent();
			Sleep(200);
			pMwnd->OnClickedUp();
			Sleep(300);
			/* ///// the following part is to withdraw the pipette a little bit when the pipette tip indents the worm body too much. This would contribute to the solution depositon */
			QueryPerformanceCounter(&startCount);
			SetDacOutput(15, 4, 2.1);
			QueryPerformanceCounter(&endCount);																   /* Í£Ö¹¼ÆÊ± */
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; /* »ñÈ¡Ê±¼ä²î ms */
			while (dbTime < 500)
			{
				QueryPerformanceCounter(&endCount);																   /* Í£Ö¹¼ÆÊ± */
				dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; /* »ñÈ¡Ê±¼ä²î ms */
			}
			SetDacOutput(15, 4, 2.5);
			vol_mp285_x = 2.5; /* 2.5 */
			MP285_stop_moving = 1;
			
			if (solution_autodeposit_flag == 1) /* //////this function is enabled by pressing the button"Autodep" in pipette section */
			{
				injection_flag = 1;
			}
			
		}




		if (MP285_stop_moving == 66)  ///// x-axis moves backwards with a fixed distance to a position recorded as HM for worm injection*/
		{
			imgpro_pip_flag = 1; /* ////image processing for pip-tip is terminated */
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->OnClickedCurrent();
			Sleep(200);
			pMwnd->OnhomePS_MP_up();
			Sleep(300);
			pMwnd->SetDlgItemText(IDC_EDITHMMP, L"HM PS SET");
			MP285_stop_moving = 1;
			MP285_stop_moving_quick_fw = 0;
		}
		if (MP285_stop_moving == 88) /* x-axis moves backwards based on the predetermined home position */
		{
			imgpro_pip_flag = 1; /* ////image processing for pip-tip is terminated */
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->OnClickedCurrent();
			Sleep(200);
			pMwnd->OnhomePS_MP_up2();
			Sleep(300);
			MP285_stop_moving = 1;
			MP285_stop_moving_quick_fw = 0;
		}
		if (MP285_stop_moving == 99) /* && imgpro_flg ==0 ///// pipette tip move to the initial home position determined by MHI */
		{
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->OnhomePS_MP_up6();
			Sleep(300);
			MP285_stop_moving = 1;
			
		}
		if (MP285_stop_moving == 28)
		{
			imgpro_pip_flag = 1; /* ////image processing for pip-tip is terminated */
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->OnClickedCurrent();
			Sleep(200);
			pMwnd->OnhomePS_MP_up3();
			Sleep(300);
			MP285_stop_moving = 1;
			MP285_stop_moving_quick_fw = 0;
		}
		if (MP285_stop_moving == 6) /* ///// x-axis moves backwards with a certain time period */
		{
			QueryPerformanceCounter(&startCount);
			SetDacOutput(15, 4, 1.02);
			QueryPerformanceCounter(&endCount);
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
			while (dbTime < 500)
			{
				QueryPerformanceCounter(&endCount);
				dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
			}
			SetDacOutput(15, 4, 2.5);
			vol_mp285_x = 2.5; /* 2.5 */
			MP285_stop_moving = 1;
		}
		if (MP285_stop_moving == 7) /* z-axis downwards movement at different speed mode */
		{
			if (MP285_stop_moving_mode == 0) /* ///quick mode */
			{
				vol_mp285_z = 4.0;
				SetDacOutput(15, 6, vol_mp285_z);
			}
			else
			{
				vol_mp285_z = 3.0;
				SetDacOutput(15, 6, vol_mp285_z);
				/* MP285_stop_moving = 1; */
			}
		}
		if (MP285_stop_moving == 77) /* ////this MP285 mode is used for the pipette movement enabling MHI between pipette and worm contact */
		{
			vol_mp285_z = 2.91;
			SetDacOutput(15, 6, vol_mp285_z);
		}

		if (MP285_stop_moving == 8) /* z-axis upwards movement at different speed mode */
		{
			if (MP285_stop_moving_mode == 0) /* ///quick mode */
			{
				vol_mp285_z = 1.0;
				SetDacOutput(15, 6, vol_mp285_z);
				
			}
			else
			{
				vol_mp285_z = 2.0;
				SetDacOutput(15, 6, vol_mp285_z);
				
			}
		}
		if (MP285_stop_moving == 10) /* y-axis left movement at different speed mode */
		{
			if (MP285_stop_moving_mode == 0)
			{
				vol_mp285_y = 1.1; 
				SetDacOutput(15, 5, vol_mp285_y);
				
			}
			else
			{
				vol_mp285_y = 1.5; 
				SetDacOutput(15, 5, vol_mp285_y);
				
			}
		}
		if (MP285_stop_moving == 11) /* y-axis right movement at different speed mode */
		{
			if (MP285_stop_moving_mode == 0)
			{
				vol_mp285_y = 3.9; 
				SetDacOutput(15, 5, vol_mp285_y);

			}
			else
			{
				vol_mp285_y = 3.0; 
				SetDacOutput(15, 5, vol_mp285_y);
				
			}
		}
		if (MP285_stop_moving == 12) /* x-axis  backward movement at different speed mode */
		{
			if (MP285_stop_moving_mode == 0)
			{
				vol_mp285_x = 1.1; 
				SetDacOutput(15, 4, vol_mp285_x);
			}
			else if (MP285_stop_moving_mode == 1)
			{
				vol_mp285_x = 1.5; 
				SetDacOutput(15, 4, vol_mp285_x);
			}
			else if (MP285_stop_moving_mode == 2)
			{
				vol_mp285_x = 2.12;
				SetDacOutput(15, 4, vol_mp285_x);
			}
		}
		if (MP285_stop_moving == 13) /* x-axis  forward movement at different speed mode*/
		{
			if (MP285_stop_moving_mode == 0)
			{
				vol_mp285_x = 3.9; 
				SetDacOutput(15, 4, vol_mp285_x);
			}
			else if (MP285_stop_moving_mode == 1)
			{
				vol_mp285_x = 3.0; 
				SetDacOutput(15, 4, vol_mp285_x);
			}
			else if (MP285_stop_moving_mode == 2)
			{
				vol_mp285_x = 2.9105;
				SetDacOutput(15, 4, vol_mp285_x);
			}
		}
		if (MP285_stop_moving == 14) /* try to move the pipette along the pipette direction */
		{
			sina_1 = sina(Pip_tip_global.x, Pip_tip_to_move.x, Pip_tip_global.y, Pip_tip_to_move.y);
			cosa_1 = cosa(Pip_tip_global.x, Pip_tip_to_move.x, Pip_tip_global.y, Pip_tip_to_move.y);
			if (sina_1 > 0)
			{
				y_objectivespeed = 29;
				vol_mp285_y = 3.8; 
				SetDacOutput(15, 5, vol_mp285_y);
			}
			else if ((sina_1 < 0))
			{
				vol_mp285_y = 1.2; 
				SetDacOutput(15, 5, vol_mp285_y);
				y_objectivespeed = -24;
			}
			else if (sina_1 == 0)
			{
				vol_mp285_y = 2.5; 
				SetDacOutput(15, 5, vol_mp285_y);
			}
			if (sina_1 == 0)
			{
				if (cosa_1 > 0)
				{
					vol_mp285_x = 3.8; 
					SetDacOutput(15, 4, vol_mp285_x);
				}
				else if ((cosa_1 < 0))
				{
					vol_mp285_x = 1.2; 
					SetDacOutput(15, 4, vol_mp285_x);
				}
			}
			else if (sina_1 != 0)
			{
				x_objectivespeed = y_objectivespeed * cosa_1 / sina_1; 
				if (x_objectivespeed > 1264)
					x_objectivespeed = 1264;
				if (x_objectivespeed < -1265)
					x_objectivespeed = -1265;
				for (iii = 0; iii < 68; iii++)
				{
					if ((cal_speed[iii] <= x_objectivespeed) && (cal_speed[iii + 1] >= x_objectivespeed))
					{
						lerp_marker_x = iii;
						vol_mp285_x = cal_voltage[iii] + (cal_voltage[iii + 1] - cal_voltage[iii]) * (x_objectivespeed - cal_speed[iii]) / (cal_speed[iii + 1] - cal_speed[iii]);
						break;
					}
				}
				SetDacOutput(15, 4, vol_mp285_x);
			}
		}
	}
	return (true);
}

/* /////>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> STOP MP285 MOVEMENT WHEN IT MOVES AT SPEED MODE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//////// */
void CPicturecontrolDlg::OnClickedAutospedzero()
{
	/* TODO: Add your control notification handler code here */
	if (a != 32768)
	{
		a = S826_SystemOpen();
	}
	SetDacOutput(15, 4, 2.5);
	SetDacOutput(15, 5, 2.5);
	SetDacOutput(15, 6, 2.5);
	SetDacOutput(15, 7, 5);
	MP285_stop_moving = 1;
}

/* /////////////// ******** Image processing thread creation******************* ////////////// */
void CPicturecontrolDlg::OnClickedForce()
{
	/* TODO: Add your control notification handler code here */
	CreateThread(NULL, 0, Worm_rotation, NULL, 0, NULL);
}



void CPicturecontrolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	/* TODO: Add your message handler code here and/or call default */
	CScrollBar *scroll_bar = (CScrollBar *)pScrollBar;
	/* /////////////// ******** Obtain the position of scroll bar for setting value of gain******************* ////////////// */
	if (scroll_bar == &m_gainraw)
	{
		int pos = m_gainraw.GetScrollPos();
		switch (nSBCode)
		{
		case SB_LINELEFT:
			pos -= 1;
			break;
		case SB_LINERIGHT:
			pos += 1;
			break;
		case SB_PAGELEFT:
			pos -= 10;
			break;
		case SB_PAGERIGHT:
			pos += 10;
			break;
		case SB_LEFT:
			pos = 1;
			break;
		case SB_RIGHT:
			pos = 1000;
			break;
		case SB_THUMBPOSITION:
			pos = nPos;
			break;
		default:
			SetDlgItemInt(IDC_GAIN, pos);
			return;
		}
		gain_cam = pos;
		m_gainraw.SetScrollPos(pos);
	}
	/*
	 * /////////////// ******** Obtain the position of scroll bar for setting value of gain******************* //////////////
	 * /////////////// ******** Obtain the position of scroll bar for setting value of exposure******************* //////////////
	 */
	if (scroll_bar == &m_exposure)
	{
		int pos = m_exposure.GetScrollPos();
		switch (nSBCode)
		{
		case SB_LINELEFT:
			pos -= 1;
			break;
		case SB_LINERIGHT:
			pos += 1;
			break;
		case SB_PAGELEFT:
			pos -= 10;
			break;
		case SB_PAGERIGHT:
			pos += 10;
			break;
		case SB_LEFT:
			pos = 1;
			break;
		case SB_RIGHT:
			pos = 500;
			break;
		case SB_THUMBPOSITION:
			pos = nPos;
			break;
		default:
			SetDlgItemInt(IDC_EXP, pos);
			return;
		}
		exp_cam = pos;
		m_exposure.SetScrollPos(pos);
	}
	/*
	 * /////////////// ******** Obtain the position of scroll bar for set value of exposure******************* //////////////
	 * /////////////// ******** Obtain the position of scroll bar for set value of brightness******************* //////////////
	 */
	if (scroll_bar == &m_brightness)
	{
		int pos = m_brightness.GetScrollPos();
		switch (nSBCode)
		{
		case SB_LINELEFT:
			pos -= 1;
			break;
		case SB_LINERIGHT:
			pos += 1;
			break;
		case SB_PAGELEFT:
			pos -= 10;
			break;
		case SB_PAGERIGHT:
			pos += 10;
			break;
		case SB_LEFT:
			pos = 1;
			break;
		case SB_RIGHT:
			pos = 500;
			break;
		case SB_THUMBPOSITION:
			pos = nPos;
			break;
		default:
			SetDlgItemInt(IDC_BRIGHT, pos);
			return;
		}
		brightness_micro = pos;
		m_brightness.SetScrollPos(pos);
		CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
		pMwnd->OnBnClickedButtonbl();
	}
	/* /////////////// ******** Obtain the position of scroll bar for set value of brightness******************* ////////////// */
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

/*
 * /////////////// ******** Obtain the position of different scroll bars and set value******************* //////////////



 * /////////////// ******** Video recording starts******************* //////////////
 */
void CPicturecontrolDlg::OnBnClickedStopCam()
{
	/* TODO: Add your control notification handler code here */
	camera_flag = 1;
}
/*
 * /////////////// ******** Video recording starts******************* //////////////



 * /////////////// ******** Video recording ends******************* //////////////
 */
void CPicturecontrolDlg::OnBnClickedStopCam2()
{
	/* TODO: Add your control notification handler code here */
	camera_flag = 2;
}

/*
 * /////////////// ******** Video recording ends******************* //////////////



 * ///////////===================Creation of thread for motorized stage manipulation==================/////////
 */
void CPicturecontrolDlg::OnBnClickedIniStage() /* ///////////////intialize motorized stage */
{
	/* TODO: Add your control notification handler code here */


	bool status;
	status = xy_stage->open();
	if (status)
	{
		SetDlgItemText(IDC_EDIT_STAGESTATUS, L"connected");
	}
	else
	{
		SetDlgItemText(IDC_EDIT_STAGESTATUS, L" not connected");
	}
	CString speed_x_value, speed_y_value;
	speed_x_value.Format(L"%d", 0);
	speed_y_value.Format(L"%d", 250);
	SetDlgItemText(IDC_EDITXPOSSPEED, speed_x_value);
	SetDlgItemText(IDC_EDITYPOSSPEED, speed_y_value);
	CreateThread(NULL, 0, Stage_manipulation, NULL, 0, NULL);
}

/*
 * ///////////===================Creation of thread for motorized stage manipulation==================/////////




 * ///////////=================== thread for motorized stage manipulation==================/////////
 */
DWORD WINAPI Stage_manipulation(LPVOID lpParameter)
{
	while (true)
	{
		long int a = 0, b = 0;
		/*
		 * ****peek message
		 * Check to see if any messages are waiting in the queue
		 */
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* Translate the message and dispatch it to WindowProc() */
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/* If the message is WM_QUIT, exit the while loop */
		if (msg.message == WM_QUIT)
			break;
		PeekandPump();
		if (stage_posflag == 1)
		{
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->GetStagePosition(a_stage, b_stage);
			X_pos.Format(L"%d", a_stage);
			Y_pos.Format(L"%d", b_stage);
			pMwnd->SetDlgItemText(IDC_EDITXPOSSTAGE, X_pos);
			pMwnd->SetDlgItemText(IDC_EDITYPOSSTAGE, Y_pos);
			stage_posflag = 0;
		}


		
		if (stage_mov_flag == 2)
		{
			int x_p, y_p;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			x_p = int(pMwnd->GetDlgItemInt(IDC_EDITXPOSSTAGE1));
			y_p = int(pMwnd->GetDlgItemInt(IDC_EDITYPOSSTAGE1));
			pMwnd->MoveStageTo(x_p, y_p);
			stage_mov_flag = 0;
		}
		if (stage_stop_flag == 1)
		{
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->StopStageMovement();
			stage_stop_flag = 0;
		}

		if (stage_flag_acce == 1)
		{
			int a = 0, b = 0;
			CString X_pos, Y_pos;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->GetStageAcceleration(a);
			X_pos.Format(L"%d", a);
			pMwnd->SetDlgItemText(IDC_EDITACCESTAGE, X_pos);
		}

		if (stage_flag_step == 66) /* / step mode */
		{
			int x_s, y_s, w_time, r_time, one_tenth_w_time, one_tenth_r_time;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			x_s = 250;
			y_s = 0;
			w_time = 300;
			r_time = 200;
			one_tenth_w_time = int(w_time / 10);
			one_tenth_r_time = int(r_time / 10);
			for (int i = 0; i < 100; i++)
			{
				if (stage_flag_step == 66)
				{
					pMwnd->StageConstantMovement(x_s, y_s);
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					pMwnd->StopStageMovement();
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
					if (stage_flag_step == 66)
						Sleep(one_tenth_w_time); /* // decrease time period for each sleep in case there is a delay to respond to stop command */
				}
			}
			stage_flag_step = 0;
		}
		if (stage_reservior_1 == 1) /* ////////////move the center OF reservoir loading area_1 to the center of field of view */
		{
			CString part_num;
			int x_p_1 = 0, y_p_1 = 0;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			x_p_1 = stage_injection_home_x - 8000;
			y_p_1 = stage_injection_home_y - 2500;
			pMwnd->MoveStageToabs(x_p_1, y_p_1);
			stage_reservior_1 = 0;
			stage_injection_part = 1;
			part_num.Format(L"%d", stage_injection_part);
			pMwnd->SetDlgItemText(IDC_EDITINJPART, part_num);
		}
		if (stage_reservior_3 == 1) /* ////////////move the center OF reservoir loading area_2 to the center of field of view  */
		{
			CString part_num;
			int x_p_3 = 0, y_p_3 = 0;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			x_p_3 = stage_injection_home_x - 8000;
			y_p_3 = stage_injection_home_y - 13000;
			pMwnd->MoveStageToabs(x_p_3, y_p_3);
			stage_reservior_3 = 0;
			stage_injection_part = 2;
			part_num.Format(L"%d", stage_injection_part);
			pMwnd->SetDlgItemText(IDC_EDITINJPART, part_num);
		}
		if (stage_reservior_5 == 1) /* ////////////move the center OF reservoir loading area_3 to the center of field of view  */
		{
			CString part_num;
			int x_p_5 = 0, y_p_5 = 0;
			x_p_5 = stage_injection_home_x - 8000;
			y_p_5 = stage_injection_home_y - 24500;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;

			pMwnd->MoveStageToabs(x_p_5, y_p_5);
			stage_reservior_5 = 0;
			stage_injection_part = 3;
			part_num.Format(L"%d", stage_injection_part);
			pMwnd->SetDlgItemText(IDC_EDITINJPART, part_num);
		}
		if (stage_injection == 1) /* ////////////move the  reservoir injection area to the center of field of view */
		{
			CString part_num;
			int x_i = 0, y_i = 0;
			if (stage_injection_part == 1)
			{
				x_i = stage_injection_home_x - 18000;
				y_i = stage_injection_home_y - 4500;
			}
			else if (stage_injection_part == 2)
			{
				x_i = stage_injection_home_x - 18000;
				y_i = stage_injection_home_y - 16000;
			}
			else if (stage_injection_part == 3)
			{
				x_i = stage_injection_home_x - 18000;
				y_i = stage_injection_home_y - 27500;
			}
			
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;

			pMwnd->MoveStageToabs(x_i, y_i);
			stage_injection = 0;
		}
		if (stage_recovery_areaflag == 1) /* ////////////move  reservoir recovery area_1 to the center of field of view */
		{
			int x_r = 0, y_r = 0;
			if (stage_injection_part == 1)
			{
				x_r = stage_injection_home_x - 30000;
				y_r = stage_injection_home_y - 16000;
			}
			else if (stage_injection_part == 2)
			{
				x_r = stage_injection_home_x - 30000;
				y_r = stage_injection_home_y - 16000;
			}
			else if (stage_injection_part == 3)
			{
				x_r = stage_injection_home_x - 30000;
				y_r = stage_injection_home_y - 16000;
			}
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;

			pMwnd->MoveStageToabs(x_r, y_r);
			stage_recovery_areaflag = 0;
		}
		if (stage_release_areaflag == 1) /* ////////////move the worm to recovery area///*/
		{
			int x_r = 0, y_r = 0;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			x_r = stage_injection_home_x - 25000;
			y_r = stage_injection_home_y - 16000;
			pMwnd->MoveStageToabs(x_r, y_r);
			Sleep(3000);
			stage_release_areaflag = 0;
		}

		/*if (stage_release_return_areaflag == 1) 
		{
			int x_r = 0, y_r = 0;
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			x_r = stage_injection_home_x - 20000;
			y_r = stage_injection_home_y - 16000;
			pMwnd->MoveStageToabs(x_r, y_r);
			Sleep(2000);
			stage_injection = 1;
			stage_release_return_areaflag = 0;
		}*/

		if (stage_flag_step == 88)
		{
			CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
			pMwnd->stagescanning();
		}
	}
	return (true);
}

/* ///////////=================== thread for motorized stage manipulation==================///////// */
void CPicturecontrolDlg::OnBnClickedCurrentstage()
{
	stage_posflag = 1;
}

void CPicturecontrolDlg::OnBnClickedMovestage()
{
	stage_mov_flag = 1;
	imgpro_flg = 0;
}

void CPicturecontrolDlg::OnBnClickedStopStage()
{
	stage_stop_flag = 1;
	stage_flag_step = 0;
}

void CPicturecontrolDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	/* TODO: Add your message handler code here and/or call default */
	CDialogEx::OnLButtonDown(nFlags, point);
	CString str;
	CDialogEx::OnLButtonDown(nFlags, point);
	CRect lRect;
	GetDlgItem(IDC_MY_PIC)->GetWindowRect(&lRect);
	ScreenToClient(&lRect);
	if ((point.x >= lRect.left && point.x <= lRect.right) && (point.y >= lRect.top && point.y <= lRect.bottom))
	{
		point.x -= lRect.left;
		point.y -= lRect.top;
		Pip_tip_to_move.x = point.x;
		Pip_tip_to_move.y = point.y;

		/* /////// ******the flags is set for movement of immobilzied worm to FOV center along x-axis of MX7600  ****** //////////// */
		if (Pump_negative_done == 1) /* ///worm  immobilization is done and then movement  */
		{
			OnBnClickedButtonytophalfmclick();
		}
		/*
		 * /////// ******the flags is set for movement of immobilzied worm to FOV center along x-axis of MX7600 ****** ////////////


		 * /////// ******the flags is set for movement of injection pipette for worm penetration ****** ////////////
		 */
		if (Pump_negative_done == 0) 
		{
			flag_tip_btndown = 1;  /* /////control pipette movement */
			MP285_stop_moving = 0; /* /////speed control mode for pipette movement */
		}
		/* /////// ******the flags is set for movement of injection pipette for wor penetration ****** //////////// */
	}
}

void CPicturecontrolDlg::OnBnClickedButOn()
{
	stage_stop_flag = 1;
	stage_flag_step = 0; /* ////when pump starts working, the stage movement stops */

	injection_pump->start_pump();

	Pump_negative_done = 1;
	SetDlgItemText(IDC_EDITWM, L"mc pre for WM");
}

void CPicturecontrolDlg::OnBnClickedButOff()
{
	injection_pump->stop_pump();
	Pump_negative_done = 0;
}

void CPicturecontrolDlg::OnBnClickedButTrigger()
{
	if (Pre_inject_flag == 0)
	{
		injector_step_flag++;
	}
	CString Step_name;
	injection_pump->start_injection();
}

void CPicturecontrolDlg::OnBnClickedForwardSpeed()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 4;
}

void CPicturecontrolDlg::OnBnClickedBackwardSpeed()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 6;
}

void CPicturecontrolDlg::OnBnClickedZdownwards()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 7;
}

void CPicturecontrolDlg::OnBnClickedZupwards()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 8;
}

void CPicturecontrolDlg::OnBnClickedRseservoir1()
{
	/* TODO: Add your control notification handler code here */
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	MP285_stop_moving = 28;	  
	stage_scanning_flag = 1;  
	if (pre_button_flag == 0)   /* ////only for the stage movement */
	{
		stage_reservior_1 = 1;
	}
	else                     /* //////////inclduing the 7600 micromanipulator  and microscope control */
	{ 
		if (objective_flag == 2)
		{
			pMwnd->OnBnClickedButtonbl4();
			Sleep(500);
			pMwnd->OnBnClickedOb4();
		}
		pre_button_z_flag = 1;
		pre_button_x_flag = 1;
		pMwnd->OnBnClickedButtonpiprec(); /* ///device lift up */
		pMwnd->OnBnClickedButtondevhm();  
		while (true)
		{
			if (mx7600_movingdone_z_flag == 1 && mx7600_movingdone_x_flag == 1)
			{
				mx7600_movingdone_z_flag = 0;
				mx7600_movingdone_x_flag = 0;
				break;
			}
		}
		stage_reservior_1 = 1;
	}
	/* stagescanning(); */
}

void CPicturecontrolDlg::OnBnClickedButton6()
{
	/* TODO: Add your control notification handler code here */
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	MP285_stop_moving = 28;	 
	stage_scanning_flag = 1; 
	if (pre_button_flag == 0)        /* ////only for the stage movement */
	{
		stage_reservior_3 = 1;
	}
	else                            /* //////////inclduing the 7600 micromanipulator  and microscope control */
	{
		if (objective_flag == 2)
		{
			pMwnd->OnBnClickedButtonbl4();
			Sleep(500);
			pMwnd->OnBnClickedOb4();
		}
		pre_button_z_flag = 1;
		pre_button_x_flag = 1;
		pMwnd->OnBnClickedButtonpiprec(); /* ///device lift up */
		pMwnd->OnBnClickedButtondevhm();  
		while (true)
		{
			if (mx7600_movingdone_z_flag == 1 && mx7600_movingdone_x_flag == 1)
			{
				mx7600_movingdone_z_flag = 0;
				mx7600_movingdone_x_flag = 0;
				break;
			}
		}
		stage_reservior_3 = 1;
	}
}

void CPicturecontrolDlg::OnBnClickedButton9()
{
	/* TODO: Add your control notification handler code here */
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	MP285_stop_moving = 28;	 
	stage_scanning_flag = 1; 
	if (pre_button_flag == 0)                 /* ////only for the stage movement */
	{
		stage_reservior_5 = 1;
	}
	else                      /* //////////inclduing the 7600 micromanipulator  and microscope control */
	{
		if (objective_flag == 2)
		{
			pMwnd->OnBnClickedButtonbl4();
			Sleep(500);
			pMwnd->OnBnClickedOb4();
		}
		pre_button_z_flag = 1;
		pre_button_x_flag = 1;
		pMwnd->OnBnClickedButtonpiprec(); /* ///device lift up */
		pMwnd->OnBnClickedButtondevhm();  
		while (true)
		{
			if (mx7600_movingdone_z_flag == 1 && mx7600_movingdone_x_flag == 1)
			{
				mx7600_movingdone_z_flag = 0;
				mx7600_movingdone_x_flag = 0;
				break;
			}
		}
		stage_reservior_5 = 1;
	}
}

void CPicturecontrolDlg::OnBnClickedButtoninjection()
{
	stage_injection = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonpipres() /* //////device lift up a little bit for worm release */
{
	/* TODO: Add your control notification handler code here */
	double step2;
	step2 = int(original_pulses_z - original_pulses_2 + 5000); 
	device_trans_transfer_flag = 1;
	num_moving_z = 0;
	flag_step_z = 0;
	z_num++;
	MoveBy_z_steps(step2, &MyMovingPara);
}

void CPicturecontrolDlg::OnBnClickedButtonpipinj() /* //////vertical home position of device */
{
	/* TODO: Add your control notification handler code here */
	double step2;
	step2 = int(original_pulses_z - original_pulses_2); 
	device_trans_loading_flag = 1;
	num_moving_z = 0;
	flag_step_z = 0;
	z_num++;
	MoveBy_z_steps(step2, &MyMovingPara);
}

void CPicturecontrolDlg::OnBnClickedDownImmobilization() /* ///worm further immobilization against the substrate for further injection */
{
	/* TODO: Add your control notification handler code here */
	double step2;
	step2 = int(original_pulses_z - original_pulses_2 - 3000); /* / here orignal_pulses should be replaced by a real value//////-137868//157512  ///////20210403 -10000 is used to enhance the worm immobilization for transfer /////40000-0121-2021 */
	num_moving_z = 0;
	flag_step_z = 0;
	z_num++;
	MoveBy_z_steps(step2, &MyMovingPara);
}

void CPicturecontrolDlg::OnBnClickedButtonpiprec() /* /////device lift up eanbling stage scanning */
{
	/* TODO: Add your control notification handler code here */
	double step2;
	step2 = int(original_pulses_z - original_pulses_2 + 30000); /* / here orignal_pulses should be replaced by a real value */
	num_moving_z = 0;
	flag_step_z = 0;
	z_num++;
	MoveBy_z_steps(step2, &MyMovingPara);
}

void CPicturecontrolDlg::OnBnClickedLeftMp285()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 10;
}

void CPicturecontrolDlg::OnBnClickedRightMp286()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 11;
}

void CPicturecontrolDlg::OnBnClickedBackwardSpeed2()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 12;
}

void CPicturecontrolDlg::OnBnClickedForwardSpeed2()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 13;
}

void CPicturecontrolDlg::OnBnClickedBackground()
{
	/* TODO: Add your control notification handler code here */
	back_img_writing_flag = 1;
}

void CPicturecontrolDlg::OnBnClickedZupwardsQuick() /* //// different speed mode of MP285 */
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving_mode++;
	if (MP285_stop_moving_mode % 3 == 1)
	{
		SetDlgItemText(IDC_EDIT_MPMODE, L"SLOW");
	}
	else if (MP285_stop_moving_mode % 3 == 2)
	{
		SetDlgItemText(IDC_EDIT_MPMODE, L"EX slow");
	}
	else if (MP285_stop_moving_mode % 3 == 0)
	{
		MP285_stop_moving_mode = 0;
		SetDlgItemText(IDC_EDIT_MPMODE, L"QUICK");
	}
}





////////////////////////=========================== As requested by Olympus, the SDK for microscope control is non-disclosable. However it can be obtained from Olympus upon request=====================//////////////
////////////////////////=========================== If a different microscope is used, the same functionality can be achieved using the SDK provided by that microscope manufacturer=====================//////////////
void CPicturecontrolDlg::OnBnClickedEnuminterface() /* ////// Initialize Microscope for remote control: Step 1 */
{
	/* TODO: Add your control notification handler code here */
	int i = 0, count;
	bool rc;

	SetDlgItemText(IDC_EDITSTATE, L"step 1");
}

void CPicturecontrolDlg::OnBnClickedOpeninterface() /* ////// Initialize Microscope for remote control: Step 2 */
{
	SetDlgItemText(IDC_EDITSTATE, L"step 2");
}

void CPicturecontrolDlg::OnBnClickedClose() /* ////// Close  microscope: Step 6 */
{
	/* TODO: Add your control notification handler code here */
	bool rc;
	if (ifData != 0)
	{
		//if (pfn_CloseInterface == NULL)
		//	return;
		//rc = (*pfn_CloseInterface)(ifData);
	}
	m_bOpened = FALSE;
	SetDlgItemText(IDC_EDITSTATE, L"step 6");
}

void CPicturecontrolDlg::OnBnClickedLogout() /* ///Log out remote control mode of microscope: Step 5 */
{
	/*
	 * TODO: Add your control notification handler code here
	 * TODO: Add your control notification handler code here
	 */
}

void CPicturecontrolDlg::OnBnClickedLogin() /* ///Log in remote control mode of microscope: Step 3 */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedStatusready() /* ///Enabling remote control  of microscope: Step 4 */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedOb4() /* /////////////////// switch to 4x objective */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedObTen() /* /////////////////// switch to 10x objective */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedObTwenty() /* /////////////////// switch to 20x objective */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedObFourty() /* /////////////////// switch to 40x objective */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonfm() /* / move the nosepiece upwards */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonfmdown() /* ////move the nosepiece downwards */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonobst() /* ////////stop movement of  the nosepiece */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonobsl() /* ////// slow speed mode of the movement of nosepiece */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonobrg() /* ////// regular speed mode of the movement of nosepiece */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonobfb() /* ////POSITION INQUIRY of the nosepiece of the microscope */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonbl() /* ////BRIGHTNESS ADJUSTMENT by moving the scrollbar named brightness */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonbl2() /* ////BRIGHTNESS ADJUSTment for imaging under 10X objectibe */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonbl4() /* ////BRIGHTNESS ADJUSTment for imaging under 4X objectibe */
{
	/*
	 * TODO: Add your control notification handler code here
	 * TODO: Add your control notification handler code here
	 */
}

void CPicturecontrolDlg::fourtyobjectivebrightness() /* ////BRIGHTNESS ADJUSTment for imaging under 40X objectibe */
{
	/*
	 * TODO: Add your control notification handler code here
	 * TODO: Add your control notification handler code here
	 */
}

void CPicturecontrolDlg::twentyobjectivebrightness() /* ////BRIGHTNESS ADJUSTment for imaging under 4X objectibe */
{
	/*
	 * TODO: Add your control notification handler code here
	 * TODO: Add your control notification handler code here
	 */
}

void CPicturecontrolDlg::OnBnClickedButtonaf4() /* ///////////////movement of nosepiece for focusing under 4X objective */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonaf5() /* ///////////////movement of nosepiece to determined position by algorithm for autofocusing under 10X objective */
{
	/* TODO: Add your control notification handler code here */
}

void CPicturecontrolDlg::OnBnClickedButtonaf40() /* ////for microscope focusing of glass filament for pipette break */
{
	/*
	* TODO: Add your control notification handler code here
	* TODO: Add your control notification handler code here
	*/
}

void CPicturecontrolDlg::OnBnClickedReal() /* //////Movement of nosepiece to the position determined by AF algorithm. This fucntion will be called by the "worm_rotation thread" */
{
	/* TODO: Add your control notification handler code here */
}
////////////////////////=========================== As requested by Olympus, the SDK for microscope control is non-disclosable. However it can be obtained from Olympus upon request=====================//////////////
////////////////////////=========================== If a different microscope is used, the same functionality can be achieved using the SDK provided by that microscope manufacturer=====================//////////////



void CPicturecontrolDlg::OnBnClickedAutofc() /* //////////////flag for autofocusing */
{
	/* TODO: Add your control notification handler code here */
	if (imgpro_autofc_flag % 2 == 0)
	{
		imgpro_autofc_flag = 1;
	}
	else
	{
		imgpro_autofc_flag = 0;
		/* imgpro_gray_file_stop = 1; */
	}
}



/*void CPicturecontrolDlg::OnBnClickedAutomhi()
 * {
 * // TODO: Add your control notification handler code here
 * imgpro_pip_worm_contact = 1;
 * }*/
void CPicturecontrolDlg::OnBnClickedButtonmhi()
{
	/* TODO: Add your control notification handler code here */
	back_img_writing_flag = 3;
}

void CPicturecontrolDlg::OnBnClickedButtonafworm()
{
	/* TODO: Add your control notification handler code here */
	back_img_writing_flag = 4;
}

void CPicturecontrolDlg::OnBnClickedButtonImgori() /* ///////Line indicating where the predetermined x-axis position of microfluidic device is */
{
	/* TODO: Add your control notification handler code here */
	if (back_img_line_flag == 0)
	{
		back_img_line_flag = 1;
	}
	else
	{
		back_img_line_flag = 0;
	}
}


/* ////////////////////==============Creation of thread for Microscope remote control===============/////////////// */
void CPicturecontrolDlg::OnBnClickedButtonmfini()
{
	/* TODO: Add your control notification handler code here */
	HANDLE hthread_microscope;
	hthread_microscope = CreateThread(NULL, 0, MicroSP, NULL, 0, NULL);
	CloseHandle(hthread_microscope);
}
/* ////////////////////==============Creation of thread for Microscope remote control===============/////////////// */



void CPicturecontrolDlg::OnBnClickedButtontensw() /* ///////////////////////////flag for the manipulation of 10x objective */
{
	/* TODO: Add your control notification handler code here */
	ten_obj_switch_flag = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonytophalfmclick()///////upon mouse click on the screen, the microfluidic device will be moved to the center of FOV along x direction of MX7600
{
	/* TODO: Add your control notification handler code here */
	double vol1, step1;
	/* vol1 = GetDlgItemInt(IDC_SPEED_X); */
	step1 = (double)(2.5 * (245 - Pip_tip_to_move.y) / 0.004); /* /////THIS PARAMTER SHOULD BE CALIBRATED 30000*0.004=120UM */
	num_moving = 0;
	flag_step = 0;
	x_num++;
	/* MoveBy_x(vol1, step1, &MyMovingPara); */
	MoveBy_x_steps(step1, &MyMovingPara);
	mx7600_slow_flag = 0;
	z_tophalf_flag = 1; /* /this flag is used to indicate whether tophalf movemnt is done or not. Indication for the following device lowering down for injection */
}



void CPicturecontrolDlg::inimicroscope()
{
	/* TODO: Add your control notification handler code here */
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	pMwnd->OnBnClickedEnuminterface();
	Sleep(1000);
	pMwnd->OnBnClickedOpeninterface();
	Sleep(1000);
	pMwnd->OnBnClickedLogin();
	Sleep(5000);
	pMwnd->OnBnClickedStatusready();
	/* Sleep(5000); */
}

void CPicturecontrolDlg::OnBnClickedButtonpipmove()
{
	/* TODO: Add your control notification handler code here */
	if (MP285_pOS_Initial_YN == 1) //// in case that pipette home position was not set
	{
		MP285_stop_moving = 44;
	}
}

void CPicturecontrolDlg::OnBnClickedButtonpipmove2()
{
	/* TODO: Add your control notification handler code here */
	MP285_stop_moving = 66;
	MP285_Pos_Initial_flag = 1;
	MP285_pOS_Initial_YN = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonWiby()
{
	/* TODO: Add your control notification handler code here */
	if (Pump_negative_done == 1)
	{
		immobilization_worm_flag = 1;
		Pump_negative_done = 0;
	}
}

void CPicturecontrolDlg::OnBnClickedWibn()
{
	/* TODO: Add your control notification handler code here */
	worm_rotation_all_flag = 1;
	
}


void CPicturecontrolDlg::OnBnClickedStepstagefix() /* ////fixed step rotation for worm injection */
{
	/* TODO: Add your control notification handler code here */
	stage_flag_step = 66;
}

void CPicturecontrolDlg::OnBnClickedButtonOrirecord() /* //////////////this flag used to record the home position of mx7600 along certain axis */
{
	/* TODO: Add your control notification handler code here */
	original_pulses_y_flag = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonOrirecord2() /* //////////////this flag used to record the home position of mx7600 along certain axis */
{
	/* TODO: Add your control notification handler code here */
	original_pulses_z_flag = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonRc() /* ////////this flag is used for moving the stage to recovery area */
{
	/* TODO: Add your control notification handler code here */
	stage_recovery_areaflag = 1;
}

void CPicturecontrolDlg::OnBnClickedWormtrans() /* /////Manipulation of worm including pick up, immobilization, and trasnfer */
{
	/* TODO: Add your control notification handler code here */
	worm_trans_all_flag = 1;
	/* /imgpro_flg = 1;////worm head recognition2021-0223 */
	if (stage_scanning_flag == 1) /* ///when the worm transfer process starts, the pipette start returing to the hoMe position */
	{
		MP285_stop_moving = 444;
		stage_scanning_flag = 0;
	}
}

void CPicturecontrolDlg::OnBnClickedButtondevhm() /* //// return microfluidic device to the home position along x-axis of mx7600 */
{
	/* TODO: Add your control notification handler code here */
	double step1;
	device_trans_homex = 1;
	step1 = int(original_pulses_x - original_pulses_6 + 3000); 
	num_moving = 0;
	flag_step = 0;
	x_num++;
	MoveBy_x_steps(step1, &MyMovingPara);
	mx7600_slow_flag = 0;
}

void CPicturecontrolDlg::OnBnClickedButOn2() /* ////////vacuum pump works at high speed */
{
	/*
	 * TODO: Add your control notification handler code here
	 * TODO: Add your control notification handler code here
	 */
	stage_stop_flag = 1;
	stage_flag_step = 0; /* ////when pump starts working, the stage movement stops */
	injection_pump->set_pump_high_speed();
	Pump_negative_done = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonall() /* /////////////////// Initialize all hardwares and create all threads */
{
	/* TODO: Add your control notification handler code here */
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	CreateThread(NULL, 0, Showcam, NULL, 0, NULL);
	CreateThread(NULL, 0, Worm_rotation, NULL, 0, NULL);
	CreateThread(NULL, 0, MicroSP, NULL, 0, NULL);
	pMwnd->OnBnClickedIniStage();
	Sleep(500);
	pMwnd->OnClickedInitialize();
	Sleep(500);
	pMwnd->OnClickedAutoSpeed();
	Sleep(500);
	pMwnd->OnClickedConnect();
	Sleep(500);
	pMwnd->OnBnClickedButtonprebut();
	Sleep(500);
	pMwnd->inimicroscope();
}

void CPicturecontrolDlg::OnBnClickedButtonpipmove3() /* //////////////////Return the pipette to home position */
{
	/* TODO: Add your control notification handler code here */
	if (MP285_pOS_Initial_YN == 1)
	{
		MP285_stop_moving = 88;
	}
}

void CPicturecontrolDlg::OnBnClickedButtonXh() /* //////////////this flag used to record the home position of mx7600 along certain axis */
{
	/* TODO: Add your control notification handler code here */
	original_pulses_x_flag = 1;
}



void CPicturecontrolDlg::stagescanning() /* //////stage scanning */
{
	LARGE_INTEGER currentCount; /*  */
	LARGE_INTEGER startCount;
	LARGE_INTEGER endCount;			  /*  */
	LARGE_INTEGER freq;				  /*  */
	INT64 CurrentTime;				  /*  */
	INT64 frequency;				  /*  */
	QueryPerformanceFrequency(&freq); /*  */
	frequency = freq.QuadPart;
	double dbTime;
	int x_s, y_s;
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	QueryPerformanceCounter(&startCount);
	QueryPerformanceCounter(&endCount);																   
	dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(800, 0); /* //->1st cycle */
		while (dbTime < 6000)
		{
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(0, -1000);
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 700)
		{
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(-800, 0); /* //->2nd cycle */
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 6000)
		{
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(0, -1000);
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 700)
		{
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(800, 0); /* //->3rd cycle */
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 6000)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																  
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(0, -1000);
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 700)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(-800, 0); /* //->4th cycle */
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 6000)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																  
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(0, -1000);
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 700)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(800, 0); /* //->5th cycle */
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 6000)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(0, -1000);
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 700)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(-800, 0); /* //->6th cycle */
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 6000)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(0, -1000);
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 700)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	if (stage_flag_step == 88)
	{
		pMwnd->StageConstantMovement(800, 0); /* //->7th cycle */
		QueryPerformanceCounter(&startCount);
		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000;
		while (dbTime < 6000)
		{
			if (stage_flag_step != 88)
			{
				pMwnd->StageConstantMovement(0, 0);
				break;
			}
			QueryPerformanceCounter(&endCount);																   
			dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart * 1000; 
		}
	}
	pMwnd->StageConstantMovement(0, 0); /* //stage movement stop */
	stage_flag_step = 0;
}

void CPicturecontrolDlg::OnBnClickedButton4() /* ///Stage scanning flag */
{
	/* TODO: Add your control notification handler code here */
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	stage_flag_step = 88; /* //for stage scanning */
						 
						  
}

void CPicturecontrolDlg::OnBnClickedButtonrel() /* ////////////// worm release after injection */
{
	/* TODO: Add your control notification handler code here */
	worm_release_all_flag = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonpre() /* ////flag for microinjector */
{
	/* TODO: Add your control notification handler code here */
	Pre_inject_flag = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonallhm() /* //////// device returning to home position for worm injection project */
{
	/* TODO: Add your control notification handler code here */
	device_all_home_flag = 1;
}

void CPicturecontrolDlg::OnBnClickedButtonprebut() /* /////flag for motorized stage movement */
{
	/* TODO: Add your control notification handler code here */
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	CString pre_but_num;
	if (pre_button_flag == 0)
	{
		pre_button_flag = 1;
	}
	else if (pre_button_flag == 1)
	{
		pre_button_flag = 0;
	}
	pre_but_num.Format(L"%d", pre_button_flag);
	pMwnd->SetDlgItemText(IDC_EDITPREBUT, pre_but_num);
}

void CPicturecontrolDlg::OnBnClickedButtonmhiimg() /* //// THIS SECTION IS USED TO RELAZIE MHI image processing and pipette lowering down at the same time */
{
	if (imgpro_pip_worm_contact % 2 == 0)
	{
		imgpro_pip_worm_contact = 1; /* /////to enable the image processing for MHI///// this can alos be obtained by pressing the button(Pip_w contact). */
	}
	else
	{
		imgpro_pip_worm_contact = 0;
	}
}

void CPicturecontrolDlg::OnBnClickedButtonauto() /* ///////////////flag for autodeposition of solution */
{
	/* TODO: Add your control notification handler code here */
	CString num_injection;
	if (solution_autodeposit_flag % 2 == 0)
	{
		solution_autodeposit_flag = 1;
	}
	else
	{
		solution_autodeposit_flag = 0;
	}
	CPicturecontrolDlg *pMwnd = (CPicturecontrolDlg *)AfxGetApp()->m_pMainWnd;
	num_injection.Format(L"%d", solution_autodeposit_flag);
	pMwnd->SetDlgItemText(IDC_EDITAUTODEP, num_injection);
}

void CPicturecontrolDlg::OnBnClickedButtonrecob() /* /////////RECORD THE CURRENT OBJECTIVE POSITION AS HOME POSITION */
{
	/* TODO: Add your control notification handler code here */
	objective_hm_ps_flag = 1;
	objective_hm_ps.Format(_T( "FG %d\r\n" ), pos_ob);
}

void CPicturecontrolDlg::OnBnClickedButtonYh() /* //////////////////////this flag used to record the home position of mx7600 along certain axis */
{
	/* TODO: Add your control notification handler code here */
	original_pulses_t_flag = 1;
}

/* ////////////////==============this section is used to set a flag for IF the automatic tracking of pipette tip is stopped or not============/////////// */
void CPicturecontrolDlg::OnBnClickedLarvaedec()
{
	/* TODO: Add your control notification handler code here */
	if (imgpro_pip_flag % 2 == 1)
	{
		imgpro_pip_flag = 2;
	}
	else
	{
		imgpro_pip_flag = 1;
	}
	imgpro_num++;
}



/*void CPicturecontrolDlg::OnBnClickedButtoncloall2()
 * {
 * // TODO: Add your control notification handler code here
 * }*/
//void CPicturecontrolDlg::OnEnChangeEditspeed()
//{
	/*
	 * TODO:  If this is a RICHEDIT control, the control will not
	 * send this notification unless you override the CDialogEx::OnInitDialog()
	 * function and call CRichEditCtrl().SetEventMask()
	 * with the ENM_CHANGE flag ORed into the mask.
	 * TODO:  Add your control notification handler code here
	 */
//}




void CPicturecontrolDlg::OnBnClickedButtonbi()
{
	// TODO: Add your control notification handler code here
	back_img_writing_flag = 5;
}


