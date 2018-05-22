#ifndef	CLOCK_H
#define CLOCK_H

#include "Timer.h"

namespace Clock
{
	Timer timer;

	double	deltaTime;
	double	globalTime;
	double	lastTime;

	static void Init()
	{
		timer.SetResolution(Timer::RESOLUTION_NANOSECONDS);
		timer.Play();

		lastTime = 0.0f;
		globalTime = 0.0f;
		deltaTime = 0.0f;
	}

	static void Update()
	{
		globalTime = timer.GetTime();
		deltaTime = globalTime - lastTime;
		lastTime = globalTime;
	}

	static double GetGlobalTime()
	{
		return timer.GetTime();
	}
};

#endif