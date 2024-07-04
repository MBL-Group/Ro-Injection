#include "SerialPort.h"
#pragma once
class XY_StageProScan
{
public:
	XY_StageProScan();
	~XY_StageProScan();
	CSerialPort serial;// for serial port connection
	bool open();
	void set_speed(unsigned short value);
	void goto_relative_position(int x, int y);
	void goto_absolute_position(int x, int y);
	void get_current_position(int &x, int &y);
	void get_accel(int &x);
	void constant_move(int x, int y);
	void stop();
};
