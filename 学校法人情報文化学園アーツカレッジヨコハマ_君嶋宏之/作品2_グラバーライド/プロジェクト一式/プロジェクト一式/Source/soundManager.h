#pragma once
#include "../Library/gameObject.h"
#include <string>
class StageData;
class LoadManager;
class SoundManager:public GameObject
{
public:
	SoundManager(SceneBase* _scene);
	~SoundManager();
	/// <summary>
	/// se再生
	/// </summary>
	/// <param name="_sound"></param>
	void PlaySe(std::string _sound);
	/// <summary>
	/// bgm再生
	/// </summary>
	/// <param name="_sound"></param>
	void PlayBgm(std::string _sound);
	/// <summary>
	/// bgmとめるよー
	/// </summary>
	void StopBgm();
	/// <summary>
	/// SEを停止
	/// </summary>
	/// <param name="_sound"></param>
	void StopSdSe(std::string _sound);

	/// <summary>
	/// 現在再生しているbgmを取得
	/// </summary>
	/// <returns></returns>
	std::string GetPlayingSDBgm()const { return playingBgm; }
	/// <summary>
	/// 全ての音を削除
	/// </summary>
	void AllStopSound();

	void Update()override;

	/// <summary>
	/// デバック用指定したサウンドの音量を変更
	/// </summary>
	/// <param name="type"></param>
	/// <param name="volume"></param>
	void SetSoundTypeVolume(std::string type, int volume);

	/// <summary>
	/// 一時停止していたbgmを再開する
	/// </summary>
	void RestartBgm();

	

	
private:
	StageData* stageData;
	LoadManager* loadManager;
	std::string playingBgm;
	std::string playingSe;
	std::string lastPlayBgm;

};