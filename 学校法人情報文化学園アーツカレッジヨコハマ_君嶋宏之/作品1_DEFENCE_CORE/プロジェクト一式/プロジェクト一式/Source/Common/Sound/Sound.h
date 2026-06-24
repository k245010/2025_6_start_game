#pragma once
#include "../../../Library/myDxLib.h"
#include <string>

/** 西 作
Sound.cpp,h
**/

class Sound
{
public:
	Sound();
	~Sound();

	void Update();

	void SoundData(int _soundHandle,int _volume);
	//通常再生
	void PlaySE();
	//bgmの再生
	void PlayBGM(bool _loop,bool firstPlay);

	//サウンドを止める
	void StopBGM();
	void StopSE();
	void StopSound();

	void BGMFeedIn(float time);
	void BGMFeedOut(float time);

	bool IsActive() { return active; }

	bool IsPlay() { return isPlay; }

private:

	/// <summary>
	///				サウンドが再生されてから初めのフレームか返す
	/// </summary>
	/// <returns>	true:初めのフレーム / false:初めのフレームではない	</returns>
	bool CheckPlaySoundFirstFrame();

	void ResetPlayInfo();

	int soundHandle;
	int volume;
	bool loop;
	bool firstPlay;
	int lastPlayintBgm;

	float feedInTime;
	float feedOutTime;
	float timeMax;

	bool active;

	bool isPlay			= false;// サウンドを再生しているかどうか
	float playSoundTime = 0.0f;	// 再生してからの経過時間
};