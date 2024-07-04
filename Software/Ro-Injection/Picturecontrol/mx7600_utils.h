#ifndef MX7600_UTILS_H
#define MX7600_UTILS_H
#include <array>
#include <cmath>
class ToMove
{
	public:
		volatile int pulses_to_move_x;
		volatile float voltage_x;
		volatile int stopped_and_ready_2_move_x;
		volatile int pulses_to_move_history_x[5];
		volatile int IsJustSet_x;
		volatile int IsJustSet_history_x[5];
		volatile int pulse_abs_history_x[5];

		int pulses_to_move_y;
		float voltage_y;
		int stopped_and_ready_2_move_y;
		int pulses_to_move_history_y[5];
		int IsJustSet_y;
		int IsJustSet_history_y[5];
		int pulse_abs_history_y[5];

		int pulses_to_move_z;
		float voltage_z;
		int stopped_and_ready_2_move_z;
		int pulses_to_move_history_z[5];
		int IsJustSet_z;
		int IsJustSet_history_z[5];
		int pulse_abs_history_z[5];

		int pulses_to_move_4th_axis;
		float voltage_4th_axis;
		int stopped_and_ready_2_move_4th_axis;
		int pulses_to_move_history_4th_axis[5];
		int IsJustSet_4th_axis;
		int IsJustSet_history_4th_axis[5];
		int pulse_abs_history_4th_axis[5];
};
int sign_weize(float number);
float min_weize(float a, float b);
float max_weize(float a, float b);
float min_abs_weize(float a, float b);
float max_abs_weize(float a, float b);
int MoveBy_x_steps(float pulses_to_move, ToMove* MovingPara);
int MoveBy_x(float voltage, float pulses_to_move, ToMove* MovingPara);
int Check_If_Need_Stop_x(ToMove* MovingPara);
int MoveBy_y_steps(float pulses_to_move, ToMove* MovingPara);
int MoveBy_y(float voltage, float pulses_to_move, ToMove* MovingPara);
int Check_If_Need_Stop_y(ToMove* MovingPara);
int MoveBy_z(float voltage, float pulses_to_move, ToMove* MovingPara);
int Check_If_Need_Stop_z(ToMove* MovingPara);
int MoveBy_4th_axis(float voltage, float pulses_to_move, ToMove* MovingPara);
int Check_If_Need_Stop_4th_axis(ToMove* MovingPara);
int Comparevolatges(int voltages);

int MoveBy_z_steps(float pulses_to_move, ToMove *MovingPara);
int MoveBy_t_steps(float pulses_to_move, ToMove *MovingPara);

#endif // TOMOVE_H