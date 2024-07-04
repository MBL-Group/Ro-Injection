#include "stdafx.h"
#include "mx7600_utils.h"
int sign_weize(float number)
{
	if (number > 0)
	{
		return (5);
	}
	else if (number < 0)
	{
		return (-5);
	}
	else
	{
		return (0);
	}
}

float min_weize(float a, float b)
{
	if (a >= b)
	{
		return (b);
	}
	else
	{
		return (a);
	}
}

float max_weize(float a, float b)
{
	if (a >= b)
	{
		return (a);
	}
	else
	{
		return (b);
	}
}

float min_abs_weize(float a, float b)
{
	if (abs(a) >= abs(b))
	{
		return (b);
	}
	else
	{
		return (a);
	}
}

float max_abs_weize(float a, float b)
{
	if (abs(a) >= abs(b))
	{
		return (a);
	}
	else
	{
		return (b);
	}
}

int MoveBy_x_steps(float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->pulses_to_move_x = pulses_to_move;
	return (1);
}

int MoveBy_x(float voltage, float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->IsJustSet_x = 1;
	if (MovingPara->stopped_and_ready_2_move_x == 1)
	{
		MovingPara->pulses_to_move_x = pulses_to_move;
		MovingPara->voltage_x = voltage;
		MovingPara->stopped_and_ready_2_move_x = 0;
	}
	return (1);
}

int Check_If_Need_Stop_x(ToMove *MovingPara)
{
	if (sign_weize(MovingPara->pulses_to_move_history_x[1]) * sign_weize(MovingPara->pulses_to_move_history_x[0]) <= 0 && MovingPara->IsJustSet_history_x[1] * MovingPara->IsJustSet_history_x[0] > 0)
	{
		MovingPara->pulses_to_move_x = 0;
		MovingPara->voltage_x = 0;
		MovingPara->stopped_and_ready_2_move_x = 1;
		MovingPara->IsJustSet_x = 0;
	}
	return (1);
}

int MoveBy_y_steps(float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->pulses_to_move_y = pulses_to_move;
	return (1);
}

int MoveBy_y(float voltage, float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->IsJustSet_y = 1;
	if (MovingPara->stopped_and_ready_2_move_y == 1)
	{
		MovingPara->pulses_to_move_y = pulses_to_move;
		MovingPara->voltage_y = voltage;
		MovingPara->stopped_and_ready_2_move_y = 0;
	}
	return (1);
}

int Check_If_Need_Stop_y(ToMove *MovingPara)
{
	if (sign_weize(MovingPara->pulses_to_move_history_y[1]) * sign_weize(MovingPara->pulses_to_move_history_y[0]) <= 0 && MovingPara->IsJustSet_history_y[1] * MovingPara->IsJustSet_history_y[0] > 0)
	{
		MovingPara->pulses_to_move_y = 0;
		MovingPara->voltage_y = 0;
		MovingPara->stopped_and_ready_2_move_y = 1;
		MovingPara->IsJustSet_y = 0;
	}
	return (1);
}

int MoveBy_z(float voltage, float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->IsJustSet_z = 1;
	if (MovingPara->stopped_and_ready_2_move_z == 1)
	{
		MovingPara->pulses_to_move_z = pulses_to_move;
		MovingPara->voltage_z = voltage;
		MovingPara->stopped_and_ready_2_move_z = 0;
	}
	return (1);
}

int MoveBy_z_steps(float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->pulses_to_move_z = pulses_to_move;
	return (1);
}

int MoveBy_t_steps(float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->pulses_to_move_4th_axis = pulses_to_move;
	return (1);
}


int Check_If_Need_Stop_z(ToMove *MovingPara)
{
	if (sign_weize(MovingPara->pulses_to_move_history_z[1]) * sign_weize(MovingPara->pulses_to_move_history_z[0]) <= 0 && MovingPara->IsJustSet_history_z[1] * MovingPara->IsJustSet_history_z[0] > 0)
	{
		MovingPara->pulses_to_move_z = 0;
		MovingPara->voltage_z = 0;
		MovingPara->stopped_and_ready_2_move_z = 1;
		MovingPara->IsJustSet_z = 0;
	}
	return (1);
}

int MoveBy_4th_axis(float voltage, float pulses_to_move, ToMove *MovingPara)
{
	MovingPara->IsJustSet_4th_axis = 1;
	if (MovingPara->stopped_and_ready_2_move_4th_axis == 1)
	{
		MovingPara->pulses_to_move_4th_axis = pulses_to_move;
		MovingPara->voltage_4th_axis = voltage;
		MovingPara->stopped_and_ready_2_move_4th_axis = 0;
	}
	return (1);
}

int Check_If_Need_Stop_4th_axis(ToMove *MovingPara)
{
	if (sign_weize(MovingPara->pulses_to_move_history_4th_axis[1]) * sign_weize(MovingPara->pulses_to_move_history_4th_axis[0]) <= 0 && MovingPara->IsJustSet_history_4th_axis[1] * MovingPara->IsJustSet_history_4th_axis[0] > 0)
	{
		MovingPara->pulses_to_move_4th_axis = 0;
		MovingPara->voltage_4th_axis = 0;
		MovingPara->stopped_and_ready_2_move_4th_axis = 1;
		MovingPara->IsJustSet_4th_axis = 0;
	}
	return (1);
}

int Comparevolatges(int voltages)
{
	if (voltages > 10)
		voltages = 10;
	if (voltages < -10)
		voltages = -10;
	return (voltages);
}