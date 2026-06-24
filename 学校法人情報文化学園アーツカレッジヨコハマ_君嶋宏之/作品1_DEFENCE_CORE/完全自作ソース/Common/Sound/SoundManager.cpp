#include "SoundManager.h"
#include "Sound.h"
#include "../Debug/Debug.h"
#include "../../../Library/DebugNew.h"

std::unordered_map<std::string, Sound*> SoundManager::sounds	= std::unordered_map<std::string, Sound*>{};
std::list<Sound*> SoundManager::feedInOutList					= std::list<Sound*>{};

void SoundManager::Init()
{
	Sound_ID::InitID();

	SoundIDToLoad(Sound_ID::TITLE_BGM,		255);
	SoundIDToLoad(Sound_ID::VICTORY_BGM,	140);
	SoundIDToLoad(Sound_ID::DEFEAT_BGM,		255);
	SoundIDToLoad(Sound_ID::SETUP_BGM,		100);
	SoundIDToLoad(Sound_ID::DEFENCE_BGM,	140);

	SoundIDToLoad(Sound_ID::VICTORY_JINGLE,		150);
	SoundIDToLoad(Sound_ID::DEFEAT_JINGLE,		150);
	SoundIDToLoad(Sound_ID::WAVE_START_JINGLE,	255);
	SoundIDToLoad(Sound_ID::WAVE_END_JINGLE,	255);

	SoundIDToLoad(Sound_ID::SELECT_PUSH_SE,				255);
	SoundIDToLoad(Sound_ID::TRAP_SELECTING_SE,			255);
	SoundIDToLoad(Sound_ID::CHANGE_START_STAGE_NUM_SE,	255);
	SoundIDToLoad(Sound_ID::DO_NOT_SE,					255);
	SoundIDToLoad(Sound_ID::SPIKE_PUT_SE,				150);
	SoundIDToLoad(Sound_ID::WALL_PUT_SE,				150);
	SoundIDToLoad(Sound_ID::TURRET_PUT_SE,				150);
	SoundIDToLoad(Sound_ID::NORMAL_BULLET_SHOT_SE,		150);
	SoundIDToLoad(Sound_ID::SLIME_BULLET_SHOT_SE,		255);
	SoundIDToLoad(Sound_ID::SLIME_BULLET_HIT_SE,		255);
	SoundIDToLoad(Sound_ID::SLIME_BULLET_ATTACH_SE,		255);
	SoundIDToLoad(Sound_ID::BULLET_EMPTY_SE,			255);
	SoundIDToLoad(Sound_ID::WALL_DAMAGE1_SE,			150);
	SoundIDToLoad(Sound_ID::WALL_DAMAGE2_SE,			150);
	SoundIDToLoad(Sound_ID::WALL_BROKEN_SE,				150);
	SoundIDToLoad(Sound_ID::CORE_DAMAGE_SE,				150);
	SoundIDToLoad(Sound_ID::CORE_BROKEN_SE,				150);
	SoundIDToLoad(Sound_ID::ENEMY_DAMAGE_SE,			150);
	SoundIDToLoad(Sound_ID::ENEMY_ATTACK_SE,			255);
	SoundIDToLoad(Sound_ID::ENEMY_DEATH_SE,				255);
	SoundIDToLoad(Sound_ID::PLAYER_WALK_SE,				255);
	SoundIDToLoad(Sound_ID::PLAYER_DAMAGE_SE,			255);
	SoundIDToLoad(Sound_ID::PLAYER_DEATH_SE,			255);
	SoundIDToLoad(Sound_ID::COIN_ADD_SE,				200);
	SoundIDToLoad(Sound_ID::COIN_GET_SE,				255);
	SoundIDToLoad(Sound_ID::COIN_SUB_SE,				255);
	SoundIDToLoad(Sound_ID::TRAP_UPGRADE_SE,			255);
	SoundIDToLoad(Sound_ID::TRAP_MAINTENANCE_SE,		150);
}

void SoundManager::Release()
{
	// サウンドのSOUND_ID列挙体と音量のペアコンテナを削除する
	Sound_ID::DeleteSoundID();

	AllDeleteSound();
}

void SoundManager::Update()
{
	for (const auto& sound : sounds)
	{
		sound.second->Update();
	}
}

void SoundManager::Draw()
{
}

void SoundManager::SoundLoad(Sound_ID::SOUND_ID _id, std::string _path, int _volume)
{

}

void SoundManager::SoundIDToLoad(Sound_ID::SOUND_ID _id, int _volume)
{
	char path[CHAR_MAX];
	sprintfDx(path, "data/sound/%.4d.wav", _id);
	
	Sound* soundData = new Sound();
	soundData->SoundData(LoadSoundMem(path), _volume);
	Sound_ID::SetSoundId(std::to_string(_id), _id);
	sounds.emplace(Sound_ID::GetSoundID(_id), soundData);
}

void SoundManager::PlaySE(Sound_ID::SOUND_ID _type)
{
	sounds[Sound_ID::GetSoundID(_type)]->PlaySE();
}

Sound_ID::SOUND_ID SoundManager::PlaySERandom(const std::set<Sound_ID::SOUND_ID>& _soundIDList)
{
	assert(!_soundIDList.empty() && "コンテナが空です。再生するサウンドIDを入れてください。");

	int rand = GetRand((int)_soundIDList.size() - 1);	// 乱数値	この値分イテレータを進める

	std::set<Sound_ID::SOUND_ID>::iterator soundIdItr = _soundIDList.begin();	// 先頭のイテレータ

	// 乱数値分イテレータを進める
	std::advance(soundIdItr, rand);

	// サウンドの再生
	PlaySE(*soundIdItr);

	return *soundIdItr;	// 再生したサウンドIDを返すことで、StopSEを使えるようにする
}

void SoundManager::PlayBGM(Sound_ID::SOUND_ID _type, bool _loop, bool firstPlay)
{
	sounds[Sound_ID::GetSoundID(_type)]->PlayBGM(_loop,firstPlay);
}

void SoundManager::StopBGM(Sound_ID::SOUND_ID _type)
{
	sounds[Sound_ID::GetSoundID(_type)]->StopBGM();
}

void SoundManager::StopSE(Sound_ID::SOUND_ID _type)
{
	sounds[Sound_ID::GetSoundID(_type)]->StopSE();
}

void SoundManager::StopAllSound()
{
	for (const auto& sound : sounds)
	{
		sound.second->StopSound();
	}
}

bool SoundManager::IsPlaySound(Sound_ID::SOUND_ID _type)
{
	return sounds[Sound_ID::GetSoundID(_type)]->IsPlay();
}
void SoundManager::AllDeleteSound()
{
	feedInOutList.clear();
	for (auto& sound : sounds) {
		delete sound.second;
	}
	sounds.clear();
}

void SoundManager::FeedInStart(Sound_ID::SOUND_ID _id, float _time)
{
	sounds[Sound_ID::GetSoundID(_id)]->BGMFeedIn(_time);
	SetFeedInOutList(sounds[Sound_ID::GetSoundID(_id)]);
}

void SoundManager::FeedInOut(Sound_ID::SOUND_ID _id, float _time)
{
	sounds[Sound_ID::GetSoundID(_id)]->BGMFeedOut(_time);
	SetFeedInOutList(sounds[Sound_ID::GetSoundID(_id)]);
}

void SoundManager::SetFeedInOutList(Sound* sound)
{
	for (auto itr = feedInOutList.begin(); itr != feedInOutList.end();) {
		if ((*itr) == sound) {
			itr = feedInOutList.erase(itr);
		}
		else {
			itr++;
		}
	}
	feedInOutList.push_back(sound);
}