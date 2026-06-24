#include "SoundID.h"
#include <unordered_map>
#include <assert.h>
#include "../Debug/Debug.h"

namespace 
{
	std::unordered_map<Sound_ID::SOUND_ID, std::string>* soundId;
	std::unordered_map<std::string, Sound_ID::SOUND_ID> soundIDStringFromEnumList;	// EFFEKSEER_KINDの文字列に応じた、EFFEKSEER_KINDのenumを管理するコンテナ
}

void Sound_ID::InitID()
{
	soundId = new std::unordered_map<Sound_ID::SOUND_ID, std::string>;

	for (const auto& soundIDStr : Sound_ID::SOUND_ID_STRING_LIST)
	{
		// 文字列とenumをpush
		soundIDStringFromEnumList.emplace(soundIDStr.second, soundIDStr.first);
	}
}

void Sound_ID::DeleteSoundID()
{
	soundId->clear();
	delete soundId;
}

std::string Sound_ID::GetSoundID(SOUND_ID _type)
{
	int type = (int)_type;
	if (type < 0 && type >= (int)SOUND_ID_END) {
		Debug::CreateMessageBox("soundHandleMiss", "miss");
	}
	return (*soundId)[_type];
}

void Sound_ID::SetSoundId(std::string _sound, SOUND_ID _type)
{
	int type = (int)_type;
	if (type < 0 && type >= (int)SOUND_ID_END) {
		Debug::CreateMessageBox("soundHandleSetMiss", "miss");
	}
	(*soundId)[_type] = _sound;
}

Sound_ID::SOUND_ID Sound_ID::GetFromEnum(std::string_view _kindStr)
{
	if (soundIDStringFromEnumList.find(std::string(_kindStr)) == soundIDStringFromEnumList.end())
		assert(false && "不正なのSOUND_ID文字列です");

	return soundIDStringFromEnumList.at(std::string(_kindStr));
}


