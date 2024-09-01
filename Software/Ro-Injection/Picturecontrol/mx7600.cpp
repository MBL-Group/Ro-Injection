#include "stdafx.h"
#include "mx7600.h"
#include "826api.h"
MX7600::MX7600()
{
	unsigned int counters[4];  /* encoder counts when the snapshot was captured */
	unsigned int timestamp[4]; /* time the snapshot was captured */
}

void MX7600::initialize()
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
	/* Config axis DAC for 7600 speed */
	S826_DacRangeWrite(15, 0, 1, 0); /* x */
	S826_DacRangeWrite(15, 2, 1, 0); /* y */
	S826_DacRangeWrite(15, 1, 1, 0); /* z */
	S826_DacRangeWrite(15, 3, 1, 0); /* t */
	/* Config 7600 axis direction using DIO, S826_BITSET=5V, S826_BITCLR=0V */
	uint dios0[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 0), /* DIOs 0 is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	S826_DioOutputWrite(15, dios0, S826_BITCLR); /* DIO 0 -> 0001 */
	uint dios2[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 2), /* DIOs 2 is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	S826_DioOutputWrite(15, dios2, S826_BITCLR); /* DIO 2 -> 0100 */
	uint dios1[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 1), /* DIOs 1 is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	S826_DioOutputWrite(15, dios1, S826_BITCLR); /* DIO 1 -> 0010 */
	uint dios3[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 3), /* DIOs 3 is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	S826_DioOutputWrite(15, dios3, S826_BITCLR); /* DIO 3 -> 1000 */
	/**** configure the counter channel******/
	S826_CounterModeWrite(15, 5, S826_CM_K_QUADX4); /* Configure counter 5 as incremental encoder interface. x-axis */
	S826_CounterStateWrite(15, 5, 1);				/* Start tracking encoder counts. */
	S826_CounterModeWrite(15, 1, S826_CM_K_QUADX4); /* Configure counter 1 as incremental encoder interface. y-axis */
	S826_CounterStateWrite(15, 1, 1);				/* Start tracking encoder counts. */
	S826_CounterModeWrite(15, 2, S826_CM_K_QUADX4); /* Configure counter 2 as incremental encoder interface. z-axis */
	S826_CounterStateWrite(15, 2, 1);				/* Start tracking encoder counts. */
	S826_CounterModeWrite(15, 3, S826_CM_K_QUADX4); /* Configure counter 3 as incremental encoder interface. t-axis */
	S826_CounterStateWrite(15, 3, 1);				/* Start tracking encoder counts. */
}

int MX7600::SetDacOutput(uint board, uint chan, uint range, double volts)
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

void MX7600::set_x_speed(double speed)
{
	uint dios[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 0), /* DIOs 0-x is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	if (speed > 0)
		S826_DioOutputWrite(15, dios, S826_BITSET);
	else
		S826_DioOutputWrite(15, dios, S826_BITCLR);
	SetDacOutput(15, 0, S826_DAC_SPAN_0_10, abs(speed));
}

void MX7600::set_y_speed(double speed)
{
	uint dios[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 2), /* DIOs 2-y is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	if (speed > 0)
		S826_DioOutputWrite(15, dios, S826_BITSET);
	else
		S826_DioOutputWrite(15, dios, S826_BITCLR);
	SetDacOutput(15, 2, S826_DAC_SPAN_0_10, abs(speed));
}

void MX7600::set_z_speed(double speed)
{
	uint dios[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 1), /* DIOs 1-z is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	if (speed > 0)
		S826_DioOutputWrite(15, dios, S826_BITSET);
	else
		S826_DioOutputWrite(15, dios, S826_BITCLR);
	SetDacOutput(15, 1, S826_DAC_SPAN_0_10, abs(speed));
}

void MX7600::set_t_speed(double speed)
{
	uint dios[] = {
		/* Specify DIOs that are to be turned on: */
		(1 << 3), /* DIOs 3-t is in first 24-bit mask (DIOs 0-23), */
		0		  /* //DIO 38 is in second 24-bit mask (DIOs 24-47). */
	};
	if (speed > 0)
		S826_DioOutputWrite(15, dios, S826_BITSET);
	else
		S826_DioOutputWrite(15, dios, S826_BITCLR);
	SetDacOutput(15, 3, S826_DAC_SPAN_0_10, abs(speed));
}

void MX7600::stop_all()
{
	SetDacOutput(15, 0, S826_DAC_SPAN_0_10, 0);
	SetDacOutput(15, 1, S826_DAC_SPAN_0_10, 0);
	SetDacOutput(15, 2, S826_DAC_SPAN_0_10, 0);
	SetDacOutput(15, 3, S826_DAC_SPAN_0_10, 0);
}

unsigned int MX7600::get_x_counter()
{
	S826_CounterSnapshot(15, 5);
	S826_CounterSnapshotRead(15, 5,								/* Read the snapshot: and configure the channel 2 of encoder as a feedback for the first stage */
							 &counters[0], &timestamp[0], NULL, /*  receive the snapshot info here */
							 0);								/*  no need to wait for snapshot; it's already been captured */
	return (counters[0]);
}

unsigned int MX7600::get_y_counter()
{
	S826_CounterSnapshot(15, 1);
	S826_CounterSnapshotRead(15, 1,								/* Read the snapshot: and configure the channel 2 of encoder as a feedback for the first stage */
							 &counters[0], &timestamp[0], NULL, /*  receive the snapshot info here */
							 0);								/*  no need to wait for snapshot; it's already been captured */
	return (counters[0]);
}

unsigned int MX7600::get_z_counter()
{
	S826_CounterSnapshot(15, 2);
	S826_CounterSnapshotRead(15, 2,								/* Read the snapshot: and configure the channel 2 of encoder as a feedback for the first stage */
							 &counters[0], &timestamp[0], NULL, /*  receive the snapshot info here */
							 0);								/*  no need to wait for snapshot; it's already been captured */
	return (counters[0]);
}

unsigned int MX7600::get_t_counter()
{
	S826_CounterSnapshot(15, 3);
	S826_CounterSnapshotRead(15, 3,								/* Read the snapshot: and configure the channel 2 of encoder as a feedback for the first stage */
							 &counters[0], &timestamp[0], NULL, /*  receive the snapshot info here */
							 0);								/*  no need to wait for snapshot; it's already been captured */
	return (counters[0]);
}

MX7600::~MX7600()
{
}
