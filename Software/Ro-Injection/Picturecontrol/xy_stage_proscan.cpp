#include "stdafx.h"
#include "xy_stage_proscan.h"
#include <string>
#include "opencv.hpp"
#include <opencv2/core/core.hpp>
using namespace std;
using namespace cv;
XY_StageProScan::XY_StageProScan()
{
}

bool XY_StageProScan::open()
{
	serial.Open(3);
	if (serial.IsOpen())
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

void XY_StageProScan::set_speed(unsigned short value)
{
	/* set speed */
	string buf;
	String str = to_string(value); /* conver int to string */
	buf = "SMS " + str + '\r';
	/* printf("%s total", buf); */
	DWORD dwWritten, dwWTrans;
	char lpBuf[2];
	unsigned long nRead;
	DWORD byteTrans;
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 15, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	serial.Read(lpBuf, 2, osRead, &nRead);
	serial.GetOverlappedResult(osRead, byteTrans, FALSE);
}

void XY_StageProScan::goto_relative_position(int x, int y)
{
	string buf;
	string xstring, ystring;
	int a = 0, b = 0;
	CString X_pos, Y_pos;
	xstring = to_string(x); /* conver int to string */
	ystring = to_string(y); /* conver int to string */
	buf = "GR " + xstring + " " + ystring + '\r';
	DWORD dwWritten, dwWTrans;
	char lpBuf[4];
	unsigned long nRead;
	DWORD byteTrans;
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 18, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
}

void XY_StageProScan::goto_absolute_position(int x, int y)
{
	string buf;
	string xstring, ystring;
	int a = 0, b = 0;
	CString X_pos, Y_pos;
	xstring = to_string(x); /* conver int to string */
	ystring = to_string(y); /* conver int to string */
	buf = "G " + xstring + " " + ystring + '\r';
	DWORD dwWritten, dwWTrans;
	char lpBuf[4] = {0};
	unsigned long nRead;
	DWORD byteTrans;
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 18, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
}

void XY_StageProScan::get_current_position(int &x, int &y)
{
	char xchar[10], ychar[10];
	bool change = false;
	int k = 0, n = 0;
	char buf[3];
	buf[0] = 'P';
	buf[1] = '\r';
	DWORD dwWritten, dwWTrans;
	char pos[20];
	unsigned long nRead;
	DWORD byteTrans;
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(buf, 2, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	Sleep(30);
	serial.Read(pos, 13, osRead, &nRead);
	serial.GetOverlappedResult(osRead, byteTrans, TRUE);
	for (int i = 0; i < 13; i++)
	{
		if ((change == false) && (pos[i] != ','))
		{
			xchar[i] = pos[i];
			k++;
		}
		else if (i == k)
		{
			change = true;
			xchar[k] = '\0';
		}
		else if ((change == true) && (pos[i] != '\r'))
		{
			ychar[i - k - 1] = pos[i];
		}
		else if (pos[i] == '\r')
		{
			n = i;
			ychar[n - k - 1] = '\0';
			break;
		}
	}
	x = atoi(xchar);
	y = atoi(ychar);
}

void XY_StageProScan::get_accel(int &x)
{
	char xchar[10], ychar[10];
	bool change = false;
	int k = 0, n = 0;
	string buf = "SAS u\r";
	const char *cstr = buf.c_str();
	DWORD dwWritten, dwWTrans;
	char pos[20];
	unsigned long nRead;
	DWORD byteTrans;
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 7, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	Sleep(30);
	serial.Read(pos, 13, osRead, &nRead);
	serial.GetOverlappedResult(osRead, byteTrans, TRUE);
	for (int i = 0; i < 13; i++)
	{
		if ((pos[i] != '\r'))
		{
			xchar[i] = pos[i];
			k++;
		}
		else if (pos[i] == '\r')
		{
			n = i;
			xchar[n] = '\0';
			break;
		}
	}
	x = atoi(xchar);
}

void XY_StageProScan::constant_move(int x, int y)
{
	string buf;
	string xstring, ystring;
	int a = 0, b = 0;
	CString X_pos, Y_pos;
	xstring = to_string(x); /* conver int to string */
	ystring = to_string(y); /* conver int to string */
	buf = "VS " + xstring + " " + ystring + '\r';
	DWORD dwWritten, dwWTrans;
	char lpBuf[4];
	unsigned long nRead;
	DWORD byteTrans;
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 18, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
}

void XY_StageProScan::stop()
{
	char buf[3];
	buf[0] = 'I';
	buf[1] = '\r';
	DWORD dwWritten, dwWTrans;
	char lpBuf[4];
	unsigned long nRead;
	DWORD byteTrans;
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(buf, 2, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
}

XY_StageProScan::~XY_StageProScan()
{
}
