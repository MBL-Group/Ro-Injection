#include "stdafx.h"
#include "mp285.h"
#include "826api.h"
/* ////MP285 calibration */
float cal_voltage[69] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.01, 1.02, 1.03, 1.04, 1.05, 1.06, 1.07, 1.08, 1.09, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2, 2.1, 2.2, 2.3, 2.4, 2.5, 2.5, 2.6, 2.7, 2.8, 2.9, 3, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4, 4.01, 4.02, 4.03, 4.04, 4.05, 4.06, 4.07, 4.08, 4.09, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8, 4.9, 5};
float cal_speed[69] = {-1265, -1265, -1270, -1265, -1269, -1263, -1263, -1277, -1267, -1146, -489, -292, -206, -163, -133, -112, -96, -86, -76, -69, -36, -24, -19, -27, -25, -20, -19, -16, -15, -13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 9, 10, 11, 13, 17, 20, 29, 46, 123, 150, 186, 253, 410, 874, 1277, 1280, 1281, 1266, 1266, 1288, 1281, 1263, 1270, 1261, 1266, 1289, 1282, 1264};
MP285::MP285()
{
}

void MP285::initialize()
{
	/* Open the 826 API and list all boards -------------------- */
	int id; /* board ID (determined by switch settings on board) */
	int flags = S826_SystemOpen();
	if (flags < 0)
		printf("S826_SystemOpen returned error code %d", flags);
	else if (flags == 0)
		printf("No boards were detected");
	else
	{
		printf("Boards were detected with these IDs:");
		for (id = 0; id < 16; id++)
		{
			if (flags & (1 << id))
				printf(" %d", id);
		}
	}
	/* Config axis DAC for MP285 speed */
	S826_DacRangeWrite(15, 4, S826_DAC_SPAN_0_5, 0); /* x */
	S826_DacRangeWrite(15, 5, S826_DAC_SPAN_0_5, 0); /* y */
	S826_DacRangeWrite(15, 6, S826_DAC_SPAN_0_5, 0); /* z */
}

int MP285::SetDacOutput(uint board, uint chan, uint range, double volts)
{
	uint setpoint;
	switch (range) /* conversion is based on dac output range: */
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
	return (S826_DacDataWrite(board, chan, setpoint, 0)); /* program DAC output */
}

bool MP285::open()
{
	serial.Open(6);
	if (serial.IsOpen())
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

void MP285::set_speed(unsigned short value)
{
	DWORD dwWritten, dwWTrans;
	char lpBuf[1];
	unsigned long nRead;
	DWORD byteTrans;
	char CommOut[10];
	CommOut[0] = 'V';
	strncpy(CommOut + 1, (char *)&value, 2);
	CommOut[3] = '\r';
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(CommOut, 4, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	serial.Read(lpBuf, 1, osRead, &nRead);
	serial.GetOverlappedResult(osRead, byteTrans, FALSE);
}

void MP285::get_current_position(float &m_xValue, float &m_yValue, float &m_zValue)
{
	DWORD dwWritten, dwWTrans;
	char buf[2];
	char lpBuf[14];
	unsigned long nRead;
	long x, y, z;
	DWORD byteTrans;
	buf[0] = 'c';
	buf[1] = '\r';
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(buf, 2, osWrite, &dwWritten);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	serial.Read(lpBuf, 13, osRead, &nRead);
	Sleep(30);
	serial.GetOverlappedResult(osRead, byteTrans, TRUE);
	/*
	 * strncpy_s((char*)&x,4, lpBuf, 3);
	 * strncpy_s((char*)&y, 4,lpBuf + 4,3);
	 * strncpy_s((char*)&z, 4,lpBuf + 8,3);
	 */
	strncpy((char *)&x, lpBuf, 4);
	strncpy((char *)&y, lpBuf + 4, 4);
	strncpy((char *)&z, lpBuf + 8, 4);
	m_xValue = x / 25;
	m_yValue = y / 25;
	m_zValue = z / 25;
}

void MP285::goto_position(long x, long y, long z)
{
	long m_xValuemove, m_yValuemove, m_zValuemove;
	char Buf[20];
	char lpBuf[1];
	DWORD dwWritten, dwWTrans;
	DWORD byteTrans;
	unsigned long nRead;
	m_xValuemove = long(x * 25);
	m_yValuemove = long(y * 25);
	m_zValuemove = long(z * 25);
	Buf[0] = 'm';
	strncpy(Buf + 1, (char *)&m_xValuemove, 4);
	strncpy(Buf + 5, (char *)&m_yValuemove, 4);
	strncpy(Buf + 9, (char *)&m_zValuemove, 4);
	Buf[13] = '\r';
	OVERLAPPED osWrite = {0};
	osWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osRead = {0};
	osRead.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	Sleep(50);
	serial.Write(Buf, 14, osWrite, &dwWritten);
	Sleep(50);
	serial.GetOverlappedResult(osWrite, dwWTrans, TRUE);
	Sleep(50);
	serial.Read(lpBuf, 1, osRead, &nRead);
	serial.GetOverlappedResult(osRead, byteTrans, FALSE);
}

void MP285::stop()
{
	DWORD dwWritten, dwWTrans;
	char Buf[1];
	char lpBuf[1];
	unsigned long nRead;
	OVERLAPPED osWriteC = {0};
	osWriteC.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	OVERLAPPED osReader = {0};
	osReader.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	Buf[0] = 3;
	serial.ClearReadBuffer();
	serial.ClearWriteBuffer();
	serial.Write(Buf, 1, osWriteC, &dwWritten);
	serial.GetOverlappedResult(osWriteC, dwWTrans, TRUE);
	serial.Read(lpBuf, 1, osReader, &nRead);
}

void MP285::set_x_speed(double speed)
{
	SetDacOutput(15, 4, S826_DAC_SPAN_0_5, speed);
}

void MP285::set_y_speed(double speed)
{
	SetDacOutput(15, 5, S826_DAC_SPAN_0_5, speed);
}

void MP285::set_z_speed(double speed)
{
	SetDacOutput(15, 6, S826_DAC_SPAN_0_5, speed);
}

MP285::~MP285()
{
}
