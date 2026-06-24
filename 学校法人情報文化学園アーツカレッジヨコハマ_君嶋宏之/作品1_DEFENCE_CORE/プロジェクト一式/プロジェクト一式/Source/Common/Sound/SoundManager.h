#pragma once
#include "../../../Library/GameObject.h"
#include "SoundID.h"
#include <unordered_map>
#include <set>


class Sound;
class SoundManager
{
public:

	/// <summary>
	/// サウンドのロードと初期化
	/// </summary>
	static void Init();

	/// <summary>
	/// サウンドの削除
	/// </summary>
	static void Release();

	static void Update();
	static void Draw();

	static void SoundLoad(Sound_ID::SOUND_ID _id, std::string _path, int _volume);

	/// <summary>
	/// SoundIDからサウンドをロードする関数
	/// </summary>
	/// <param name="_id"> SoundID </param>
	/// <param name="_volume"> 0～255を指定 </param>
	static void SoundIDToLoad(Sound_ID::SOUND_ID _id, int _volume);

	/// <summary>
	/// SEを再生させる
	/// </summary>
	/// <param name="_type"></param>
	static void PlaySE(Sound_ID::SOUND_ID _type);

	/// <summary>
	///								引数で渡されたサウンドをランダムに一つ再生する
	/// </summary>
	/// <param name="_soundIDList">	サウンドID					</param>
	/// <returns>					再生されたサウンドID		</returns>
	static Sound_ID::SOUND_ID PlaySERandom(const std::set<Sound_ID::SOUND_ID>& _soundIDList);

	//BGMを再生させる
	static void PlayBGM(Sound_ID::SOUND_ID _type, bool _loop, bool firstPlay);
	//BGMを止める
	static void StopBGM(Sound_ID::SOUND_ID _type);
	static void StopSE(Sound_ID::SOUND_ID _type);
	static void StopAllSound();

	/// <summary>
	///							サウンドが再生されているかどうか
	/// </summary>
	/// <param name="_type">	サウンドID										</param>
	/// <returns>				true:再生されている / false:再生されていない	</returns>
	static bool IsPlaySound(Sound_ID::SOUND_ID _type);

	/// <summary>
	/// データのハンドルを削除する
	/// </summary>
	static void AllDeleteSound();


	static void FeedInStart(Sound_ID::SOUND_ID _id,float _time);
	static void FeedInOut(Sound_ID::SOUND_ID _id, float _time);

	static void SetFeedInOutList(Sound* sound);

private:
	
	static std::unordered_map<std::string,Sound*> sounds;

	static std::list<Sound*> feedInOutList;
};