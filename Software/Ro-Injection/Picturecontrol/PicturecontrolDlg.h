// PicturecontrolDlg.h : header file
//
#pragma once
#include<windows.h>
#include <iostream>
#include "SerialPort.h"
#include "afxwin.h"

// CPicturecontrolDlg dialog
class CPicturecontrolDlg : public CDialogEx
{
// Construction
public:
	CPicturecontrolDlg(CWnd* pParent = NULL);	// standard constructor
	CPicturecontrolDlg:: ~ CPicturecontrolDlg();
// Dialog Data
	enum { IDD = IDD_PICTURECONTROL_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
// Implementation
protected:
	HICON m_hIcon;
	HMODULE						m_hMod;
	//fn_MSL_PM_Initialize		pfn_InifInterface;
	//fn_MSL_PM_GetInterfaceInfo	pfn_GetInterfaceInfo;
	//fn_MSL_PM_EnumInterface		pfn_EnumInterface;
	//fn_MSL_PM_OpenInterface		pfn_OpenInterface;
	//fn_MSL_PM_CloseInterface	pfn_CloseInterface;
	//fn_MSL_PM_SendCommand		pfn_SendCommand;
	//fn_MSL_PM_RegisterCallback	pfn_RegisterCallback;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedInitialize();
	afx_msg void OnClickedStop();
	afx_msg void OnEnChangeSpeedX2();
	afx_msg void OnClickedXPos();
	afx_msg void OnClickedZero();
	afx_msg void OnClickedXNeg();
public:
	void moveto(long x, long y, long z);
	void Pump_Manipulation(char* buf_pump, int num_pump);
	void OnhomePS_MP_up();
	void OnhomePS_MP_down();
	void OnhomePS_MP_down2();
	void OnhomePS_MP_down3();
	CSerialPort m_MP285;// for serial port connection
	CSerialPort syringe_pump;// for serial port connection of syringe pump
	CSerialPort m_Stage;// for stage regulation
	CSerialPort m_Pressure;// for stage regulation
	BOOL	m_bOpened;
	//MDK_MSL_CMD	m_Cmd;
	void SetStageSpeed(unsigned short value);
	void MoveStageTo(int x, int y);
	void MoveStageToabs(int x, int y);
	void GetStagePosition( int &x,  int &y);
	void GetStageAcceleration(int &x);
	void stagescanning();
	void CPicturecontrolDlg::StopStageMovement();
	void CPicturecontrolDlg::OnhomePS_MP_up2();
	void CPicturecontrolDlg::OnhomePS_MP_up3();
	void CPicturecontrolDlg::OnhomePS_MP_up6();
	void CPicturecontrolDlg::OnBnClickedButtonytophalfmclick();
	void CPicturecontrolDlg::fourtyobjectivebrightness();
	void CPicturecontrolDlg::twentyobjectivebrightness();
	void CPicturecontrolDlg::inimicroscope();
	void StageConstantMovement(int x, int y);
	afx_msg void OnClickedCurrent();
	afx_msg void OnClickedConnect();
	afx_msg void OnClickedSetspe();
	afx_msg void OnClickedMove();
	afx_msg void OnClickedStopMp();
	afx_msg void OnClickedUp();
	afx_msg void OnClickedDown();
	afx_msg void OnClickedLeft();
	afx_msg void OnClickedRight();
	afx_msg void OnClickedAutoMp();
	afx_msg void OnClickedTracking7600();
	afx_msg void OnClickedYPos();
	afx_msg void OnClickedYNeg();
	afx_msg void OnClickedReturning7600();
	afx_msg void OnClickedImgprocee();
	afx_msg void OnClickedLarvaedec();
	afx_msg void OnClickedAutoSpeed();
	afx_msg void OnClickedAutospedzero();
	afx_msg void OnClickedForce();
	afx_msg void OnBnClickedOriginal();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedUpwards();
	afx_msg void OnBnClickedDownwards();
	afx_msg void OnStnClickedStatic286();
	afx_msg void OnBnClickedIniPump();
	afx_msg void OnBnClickedInfuse();
	afx_msg void OnBnClickedWithdraw();
	afx_msg void OnBnClickedStopPump();
	CScrollBar m_expesoure;
	CScrollBar m_gainraw;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CScrollBar m_exposure;
	CScrollBar m_brightness;
	afx_msg void OnBnClickedStopCam();
	afx_msg void OnBnClickedStopCam2();
	afx_msg void OnBnClickedInfSpeed();
	afx_msg void OnBnClickedWithSpeed();
	afx_msg void OnBnClickedInfSpeed2();
	afx_msg void OnBnClickedWithDuration();
	afx_msg void OnBnClickedPush();
	afx_msg void OnBnClickedPull();
	afx_msg void OnBnClickedWormMa();
	afx_msg void OnBnClickedIniStage();
	afx_msg void OnBnClickedSetspestage();
	afx_msg void OnBnClickedCurrentstage();
	afx_msg void OnBnClickedMovestage();
	afx_msg void OnBnClickedStopStage();
	afx_msg void OnBnClickedMovecons();
	afx_msg void OnBnClickedStep();
	afx_msg void OnBnClickedSetaccestage();
	afx_msg void OnBnClickedDiaup();
	afx_msg void OnBnClickedDiadown();
	afx_msg void OnBnClickedDiarep();
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButOn();
	afx_msg void OnBnClickedButOff();
	afx_msg void OnBnClickedButTrigger();
	afx_msg void OnStnClickedStatic7();
	afx_msg void OnBnClickedForwardSpeed();
	//afx_msg void OnBnClickedRepetition();
	afx_msg void OnBnClickedBackwardSpeed();
	afx_msg void OnBnClickedZdownwards();
	afx_msg void OnBnClickedZupwards();
	afx_msg void OnBnClickedRseservoir1();
	//afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	//afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	//afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButtoninjection();
	afx_msg void OnBnClickedButtonrec();
	afx_msg void OnBnClickedButtonrec2();
	afx_msg void OnBnClickedButtonpipres();
	afx_msg void OnBnClickedButtonpipinj();
	afx_msg void OnBnClickedButtonpiprec();
	afx_msg void OnBnClickedWormrotation();
	afx_msg void OnBnClickedPenetration();
	afx_msg void OnBnClickedDownImmobilization();
	afx_msg void OnBnClickedLeftMp285();
	afx_msg void OnBnClickedRightMp286();
	afx_msg void OnBnClickedBackwardSpeed2();
	afx_msg void OnBnClickedForwardSpeed2();
	//afx_msg void OnBnClickedDiagonalMp285();
	afx_msg void OnBnClickedBackground();
	afx_msg void OnStnClickedStatic14();
	afx_msg void OnBnClickedPipIniPos();
	afx_msg void OnEnChangeEditPipIniX();
	afx_msg void OnBnClickedZupwardsQuick();
	afx_msg void OnBnClickedZdownwardsQuick();
	afx_msg void OnBnClickedMovestage2();
	afx_msg void OnBnClickedEnuminterface();
	afx_msg void OnBnClickedOpeninterface();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedLogout();
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedStatusready();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedObjective4();
	afx_msg void OnBnClickedObjective20();
	afx_msg void OnBnClickedObjective40();
	afx_msg void OnBnClickedOb4();
	afx_msg void OnBnClickedObTen();
	afx_msg void OnBnClickedObTwenty();
	afx_msg void OnBnClickedObFourty();
	afx_msg void OnBnClickedButtonfm();
	afx_msg void OnBnClickedButtonfmdown();
	afx_msg void OnBnClickedButtonobst();
	afx_msg void OnBnClickedButtonobsl();
	afx_msg void OnBnClickedButtonobrg();
	afx_msg void OnBnClickedButtonobfb();
	afx_msg void OnBnClickedButtonbl();
	afx_msg void OnBnClickedButtonbl2();
	afx_msg void OnEnChangeBright();
	afx_msg void OnBnClickedBackground2();
	afx_msg void OnBnClickedButtonbl4();
	afx_msg void OnBnClickedButtonaf4();
	afx_msg void OnBnClickedButtonSz();
	afx_msg void OnBnClickedButtonSz2();
	afx_msg void OnBnClickedButtonSz3();
	afx_msg void OnBnClickedButtonaf5();
	afx_msg void OnBnClickedAutofc();
	afx_msg void OnBnClickedReal();
	//afx_msg void OnBnClickedAutomhi();
	afx_msg void OnBnClickedButtonmhi();
	afx_msg void OnBnClickedButtonafworm();
	afx_msg void OnBnClickedButtonImgori();
	afx_msg void OnBnClickedButtonmfini();
	afx_msg void OnBnClickedButtontensw();
	afx_msg void OnBnClickedButtonytophalf();
	afx_msg void OnBnClickedButtonybothalf();
	afx_msg void OnBnClickedButtoniniall();
	afx_msg void OnNMThemeChangedScrollbar3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonpipmove();
	afx_msg void OnBnClickedButtonpipmove2();
	afx_msg void OnBnClickedButtonWiby();
	afx_msg void OnBnClickedWibn();
	afx_msg void OnBnClickedButtonzdown();
	afx_msg void OnBnClickedButtonzup();
	afx_msg void OnBnClickedStepstagefix();
	//afx_msg void OnBnClickedButtonrecarea();
	afx_msg void OnBnClickedButtonOrirecord();
	afx_msg void OnBnClickedButtonOrirecord2();
	//afx_msg void OnBnClickedButtonrecarea();
	//afx_msg void OnBnClickedButtonRc();
	afx_msg void OnBnClickedButtonRc();
	//afx_msg void OnBnClickedButtonmanithr();
	afx_msg void OnBnClickedWormtrans();
	afx_msg void OnBnClickedButtondevhm();
	afx_msg void OnBnClickedButOn2();
	afx_msg void OnBnClickedButtonall();
	afx_msg void OnBnClickedButtonpipmove3();
	afx_msg void OnBnClickedButtonXh();
	afx_msg void OnBnClickedButtonaf20();
	afx_msg void OnBnClickedButtonaf40();
	afx_msg void OnBnClickedButtonor();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButtonrel();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButtonpre();
	afx_msg void OnBnClickedButtonallhm();
	afx_msg void OnBnClickedButtoncd();
	afx_msg void OnBnClickedButtoncwr();
	afx_msg void OnBnClickedButtonprebut();
	afx_msg void OnBnClickedButtonmhiimg();
	afx_msg void OnBnClickedButtonauto();
	afx_msg void OnBnClickedButtonrecob();
	afx_msg void OnBnClickedPumpFull();
	afx_msg void OnBnClickedButtonpipaf();
	afx_msg void OnBnClickedButtonYh();
	afx_msg void OnBnClickedLarvaedec();
	afx_msg void OnBnClickedAutoMp();
	afx_msg void OnBnClickedButtoncloall2();
	afx_msg void OnEnChangeEditspeed();
	afx_msg void OnBnClickedButtonteststart();
	afx_msg void OnBnClickedButton11teststop();
	afx_msg void OnBnClickedButtonbi();
	afx_msg void OnBnClickedZPos();
	afx_msg void OnBnClickedZNeg();
	afx_msg void OnBnClickedTPos();
	afx_msg void OnBnClickedTNeg();
};
