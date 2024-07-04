#include "stdafx.h"
#include "injector_pump.h"
#include <string>
using namespace std;
INJECTOR_PUMP::INJECTOR_PUMP()
{
}

bool INJECTOR_PUMP::open()
{
	serial.Open(8);
	if (serial.IsOpen())
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool INJECTOR_PUMP::close()
{
	serial.Close();
	if (serial.IsOpen())
	{
		return (false);
	}
	else
	{
		return (true);
	}
}

void INJECTOR_PUMP::start_pump()
{
	serial.Open(8);
	string buf;
	CString str;
	DWORD dwWritten, dwWTrans;
	char lpBuf[1];
	unsigned long nRead;
	DWORD byteTrans;
	/* write 'A' */
	buf = 'A'; /* /// flag for the injector */
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 1, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	serial.Close();
}

void INJECTOR_PUMP::stop_pump()
{
	serial.Open(8);
	string buf;
	CString str;
	DWORD dwWritten, dwWTrans;
	char lpBuf[1];
	unsigned long nRead;
	DWORD byteTrans;
	/* write 'A' */
	buf = 'C'; /* // flag for vacuum pump releases the object */
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 1, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	serial.Close();
}

void INJECTOR_PUMP::start_injection()
{
	serial.Open(8);
	string buf;
	CString str;
	DWORD dwWritten, dwWTrans;
	char lpBuf[1];
	unsigned long nRead;
	DWORD byteTrans;
	/* write 'A' */
	buf = 'D'; /* /// flag for the injector */
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 1, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	serial.Close();
}

void INJECTOR_PUMP::set_pump_high_speed()
{
	serial.Open(8);
	string buf;
	CString str;
	DWORD dwWritten, dwWTrans;
	char lpBuf[1];
	unsigned long nRead;
	DWORD byteTrans;
	buf = 'E'; /* // flag for vacuum pump starts working */
	const char *cstr = buf.c_str();
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(cstr, 1, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	serial.Close();
}

INJECTOR_PUMP::~INJECTOR_PUMP()
{
}
