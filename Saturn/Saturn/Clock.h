#ifndef	CLOCK2_H
#define CLOCK2_H

#include "../../Common/Utility/Timer.h"

class Clock2
{
	static Timer timer;

	static double	deltaTime;
	static double	globalTime;
	static double	lastTime;

public:
	static void Init();
	static void Update();
	static double GetGlobalTime();
	static double GetDeltaTime();
};

#endif