#pragma once
class MX7600
{
	public:
		MX7600();
		~MX7600();
		void initialize();
		int SetDacOutput(unsigned int board, unsigned int chan, unsigned int range, double volts);
		void set_x_speed(double speed);
		void set_y_speed(double speed);
		void set_z_speed(double speed);
		void set_t_speed(double speed);
		void stop_all();
		unsigned int get_x_counter();
		unsigned int get_y_counter();
		unsigned int get_z_counter();
		unsigned int get_t_counter();
		unsigned int counters[4]; // encoder counts when the snapshot was captured
		unsigned int timestamp[4]; // time the snapshot was captured
};
