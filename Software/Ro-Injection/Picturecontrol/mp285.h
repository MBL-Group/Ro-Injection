#include "SerialPort.h"
#pragma once
class MP285
{
	public:
		MP285();
		~MP285();
		bool open();
		void set_speed(unsigned short value);
		void get_current_position(float& m_xValue, float& m_yValue, float& m_zValue);
		void goto_position(long x, long y, long z);
		void stop();
		void initialize();
		int SetDacOutput(unsigned int board, unsigned int chan, unsigned int range, double volts);
		void set_x_speed(double speed);
		void set_y_speed(double speed);
		void set_z_speed(double speed);
		CSerialPort serial;// for serial port connection
};
