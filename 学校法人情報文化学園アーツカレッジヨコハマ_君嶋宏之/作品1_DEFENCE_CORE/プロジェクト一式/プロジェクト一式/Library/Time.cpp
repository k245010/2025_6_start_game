#include "time.h"
#include <Windows.h>
#include <DxLib.h>
namespace {
	LARGE_INTEGER freq;
	LARGE_INTEGER current;
	float deltaTime;
	float gameDeltaTimeRate;								// deltaTimeとは別のゲーム用deltaTimeを設定するためのdeltaTime倍率
	constexpr float DEFAULT_GAME_DELTA_TIME_RATE = 1.0f;	// 初期のgameDeltaTimeRateの値
	float refreshRate;
	// 直近の処理時間を保存
	const int BUF_SIZE = 30;
	float timeBuf[BUF_SIZE];
	int readP;
	int writeP;

	constexpr float ONE_SECOND = 1.0f;

	float sumFps;			// １秒間のFPS合計値
	float oneSecondCount;	// １秒たったかカウント
	float frameCount;		// １秒間でUpdate関数を通ったカウント
	float averageFps;		// １秒間の返金FPS
	float worstFps;			// 指定時間間の最低FPS
	float bestFps;			// 指定時間間の最高FPS
};

void Time::Init()
{
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&current);

	HDC hdc = GetDC(GetMainWindowHandle());	// デバイスコンテキストの取得
	refreshRate = (float)GetDeviceCaps(hdc, VREFRESH);	// リフレッシュレートの取得
	ReleaseDC(GetMainWindowHandle(), hdc);	// デバイスコンテキストの解放

	readP = 0;
	writeP = 0;
	gameDeltaTimeRate = DEFAULT_GAME_DELTA_TIME_RATE;

	sumFps			= 0.0f;
	oneSecondCount	= 0.0f;
	frameCount		= 0.0f;
	averageFps		= 0.0f;
	worstFps		= 3.402823466e+38F;	// floatの最大値で初期化
	bestFps			= 0.0f;
}

void Time::Refresh()
{
	LARGE_INTEGER last = current;
	QueryPerformanceCounter(&current);
	float dt = static_cast<float >(current.QuadPart - last.QuadPart) / freq.QuadPart;
	timeBuf[writeP] = dt;
	writeP = (writeP+1) % BUF_SIZE;
	if (writeP == readP) {
		readP = (readP + 1) % BUF_SIZE;
	}
	float sum = 0;
	int num = 0;
	for (int i = readP; i != writeP; i = (i + 1) % BUF_SIZE) {
		sum += timeBuf[i];
		num++;
	}
	if (num>BUF_SIZE/2)
		deltaTime = sum / num;
	else
		deltaTime = dt;
	//int frames = (int)((dt * 1.2f) * refreshRate);
	//if (frames >= 2)
	//{
	//	frames = 2;
	//}
	//deltaTime = frames / refreshRate;

	// １秒たったら
	if ((oneSecondCount += Time::DeltaTime()) > ONE_SECOND)
	{
		// 1秒の平均FPSを計算
		averageFps		= sumFps / frameCount;

		// リセット //
		oneSecondCount	-= ONE_SECOND;
		sumFps			= 0.0f;
		frameCount		= 0.0f;
	}
	frameCount++;
	sumFps += GetFPS();

	// 最低FPSの更新
	if (worstFps > GetFPS())
		worstFps = GetFPS();
}

float Time::DeltaTime()
{
	return deltaTime;
}

float Time::GameDeltaTime()
{
	return deltaTime * gameDeltaTimeRate;
}

void Time::SetGameDeltaTimeRate(float _rate)
{
	gameDeltaTimeRate = _rate;
}

float Time::GetGameDeltaTimeRate()
{
	return gameDeltaTimeRate;
}

void Time::ResetGameDeltaTime()
{
	gameDeltaTimeRate = DEFAULT_GAME_DELTA_TIME_RATE;
}

void Time::Release()
{
}

float Time::GetAverageFPS()
{
	return averageFps;
}

float Time::GetWorstFPS()
{
	return worstFps;
}

void Time::ResetSaveFPS()
{
	averageFps		= 0.0f;
	oneSecondCount	= 0.0f;
	sumFps			= 0.0f;
	frameCount		= 0.0f;
	worstFps		= 3.402823466e+38F;	// floatの最大値でリセット
	bestFps			= 0.0f;
}
