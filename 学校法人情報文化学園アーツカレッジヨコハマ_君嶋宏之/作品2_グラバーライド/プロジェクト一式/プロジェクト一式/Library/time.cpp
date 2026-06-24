#include "time.h"
#include <Windows.h>

namespace {
	LARGE_INTEGER freq;
	LARGE_INTEGER current;
	LARGE_INTEGER last;
	float deltaTime;
	//static const float DELTA_MAX = 0.1f;
	const double FRAME_TIME = 1.0 / 60.0; // 60fps → 1フレームあたり約0.0166秒
};
void Time::Init()
{
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&current);
}

void Time::Refresh()
{
	
	last = current;
	QueryPerformanceCounter(&current);
	deltaTime = static_cast<float>(current.QuadPart - last.QuadPart) / freq.QuadPart;
	double elapsed = (double)(current.QuadPart - last.QuadPart) / freq.QuadPart;
	//if (elapsed < FRAME_TIME) {
	//	DWORD sleepMs = (DWORD)((FRAME_TIME - elapsed) * 1000.0);
	//	if (sleepMs > 0) {
	//		Sleep(sleepMs);
	//	}
	//	//continue; // Sleep後に再チェック
	//}
	

}

float Time::DeltaTime()
{
	return deltaTime;
}
