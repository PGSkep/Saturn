#include "Clock.h"

Timer Clock2::timer;
double Clock2::deltaTime;
double Clock2::globalTime;
double Clock2::lastTime;

void Clock2::Init()
{
	timer.SetResolution(Timer::RESOLUTION_NANOSECONDS);
	timer.Play();

	lastTime = 0.0f;
	globalTime = 0.0f;
	deltaTime = 0.0f;
}

void Clock2::Update()
{
	globalTime = timer.GetTime();
	deltaTime = globalTime - lastTime;
	lastTime = globalTime;
}

double Clock2::GetGlobalTime()
{
	return timer.GetTime();
}

double Clock2::GetDeltaTime()
{
	return deltaTime;
}
