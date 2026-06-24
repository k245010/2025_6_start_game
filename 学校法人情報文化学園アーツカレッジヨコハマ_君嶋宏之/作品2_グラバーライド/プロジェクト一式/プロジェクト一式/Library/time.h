#pragma once
#pragma once

#include <Windows.h>
namespace Time {



	void Init();
	void Refresh();

	float DeltaTime();
	/*LARGE_INTEGER& getCurrent();
	LARGE_INTEGER getLast();
	LARGE_INTEGER getFreq();*/

};
