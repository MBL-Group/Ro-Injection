#include "SerialPort.h"
#pragma once
class INJECTOR_PUMP
{
	public:
		INJECTOR_PUMP();
		~INJECTOR_PUMP();
		CSerialPort serial;// for serial port connection
		bool open();
		bool close();
		void start_pump();
		void set_pump_high_speed();
		void stop_pump();
		void start_injection();
};
