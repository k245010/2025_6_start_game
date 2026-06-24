#include "Sound.h"
#include "../../../Library/myDxLib.h"
#include "../../../Library/Time.h"
#include "../Easing/Easing.h"

Sound::Sound()
{
	feedInTime		= 0.0f;
	feedOutTime		= 0.0f;
	active			= false;
	firstPlay		= false;
	loop			= false;
	volume			= 0;
	lastPlayintBgm	= 0;
	soundHandle		= -1;
	timeMax			= 0.0f;

}

Sound::~Sound()
{
	StopBGM();
}

void Sound::Update()
{
	if (isPlay)
	{
		// 再生が終わったら
		if (!CheckSoundMem(soundHandle))
		{
			ResetPlayInfo();
			return;	// 以下の処理を行わない
		}

		playSoundTime += Time::DeltaTime();
	}

	if (feedInTime > 0.0f) {
		float valume = Easing::EasingFlow<float>(&feedInTime, timeMax, (float)volume, 0, Easing::EaseIn<float>);
		ChangeVolumeSoundMem((int)valume, soundHandle);
	}
	if (feedOutTime > 0.0f) {
		float valume = Easing::EasingFlow<float>(&feedOutTime, timeMax, 0, (float)volume, Easing::EaseIn<float>);
		ChangeVolumeSoundMem((int)valume, soundHandle);
	}
}

void Sound::SoundData(int _soundHandle, int _volume)
{
	soundHandle = _soundHandle;
	volume = _volume;
	lastPlayintBgm = _soundHandle;
	ChangeVolumeSoundMem(volume,soundHandle);
}

void Sound::PlaySE()
{
	assert(soundHandle > 0 && "サウンドのハンドルが設定されていません");

	// サウンドが再生されてから初めのフレームだったら
	if (CheckPlaySoundFirstFrame())
		return;	// 二重に再生が行われないように、再生処理をしない

	PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK);
	loop = false;
	isPlay = true;
}

void Sound::PlayBGM(bool _loop, bool firstPlay)
{
	assert(soundHandle > 0 && "サウンドのハンドルが設定されていません");

	// サウンドが再生されてから初めのフレームだったら
	if (CheckPlaySoundFirstFrame())
		return;	// 二重に再生が行われないように、再生処理をしない

	loop = _loop;
	if (!loop) {
		PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK);
	}
	else {
		PlaySoundMem(soundHandle, DX_PLAYTYPE_LOOP,firstPlay);
	}
	isPlay = true;
}


void Sound::StopBGM()
{
	StopSoundMem(soundHandle);
	ResetPlayInfo();
}

void Sound::StopSE()
{
	StopSoundMem(soundHandle);
	ResetPlayInfo();
}

void Sound::StopSound()
{
	StopSoundMem(soundHandle);
	ResetPlayInfo();
}

void Sound::BGMFeedIn(float time)
{
	feedInTime	= time;
	timeMax		= time;
	active		= true;
}

void Sound::BGMFeedOut(float time)
{
	feedOutTime = time;
	timeMax		= time;
	active		= true;
}

bool Sound::CheckPlaySoundFirstFrame()
{
	// すでに再生していたら
	if (isPlay)
	{
		// 初めのフレームだったら
		if (playSoundTime <= 0.0f)
			return true;
	}
	return false;
}

void Sound::ResetPlayInfo()
{
	isPlay			= false;
	playSoundTime	= 0.0f;
}
