#include "soundManager.h"
#include "../Library/sceneManager.h"
#include "commonFind.h"
#include "loadManager.h"
#include "../ImGui/imgui.h"
#include "LoadData.h"
#include "stageData.h"
SoundManager::SoundManager(SceneBase* _scene):GameObject(_scene)
{
	 loadManager= SceneManager::CommonScene()->FindGameObject<LoadManager>();
	 stageData = SceneManager::CommonScene()->FindGameObject<StageData>();
	 playingSe = "NO";
	 playingBgm = "NO";
	 lastPlayBgm = "NO";
	

	 ///SetVolumeInitialize();
	 //ChangeVolumeSoundMem(bgmVolume -50, loadManager->GetHandle("1stagebgm"));
}

SoundManager::~SoundManager()
{
}

void SoundManager::PlaySe(std::string _sound)
{
	if (playingSe == "NO") {
		playingSe = _sound;
	}
	PlaySoundMem(loadManager->GetHandle(_sound), DX_PLAYTYPE_BACK);
	
}

void SoundManager::PlayBgm(std::string _sound)
{
	if (playingBgm!= "NO") {

		StopSoundMem(loadManager->GetHandle(playingBgm));
		std::string stageBgm = std::to_string(stageData->GetStageConfigData().bgmData) + "bgmStage";
		StopSoundMem(loadManager->GetHandle(stageBgm));
		playingBgm ="NO";
	}
	playingBgm = _sound;
	lastPlayBgm = _sound;
	PlaySoundMem(loadManager->GetHandle(_sound), DX_PLAYTYPE_LOOP);
}

void SoundManager::StopBgm()
{
	if (playingBgm != "NO") {
		StopSoundMem(loadManager->GetHandle( playingBgm));
		std::string stageBgm = std::to_string(stageData->GetStageConfigData().bgmData) + "bgmStage";
		StopSoundMem(loadManager->GetHandle(stageBgm));
	}
	
	playingBgm = "NO";
}
//
//void SoundManager::PlayTdSe(VECTOR position, std::string _sound)
//{
//	// ‰¹‚ÌÄ¶ˆÊ’u‚ðÝ’è
//	Set3DPositionSoundMem(position, loadManager->GetHandle(_sound));
//	PlaySoundMem(loadManager->GetHandle(_sound), DX_PLAYTYPE_BACK);
//}

void SoundManager::StopSdSe(std::string _sound)
{
	StopSoundMem(loadManager->GetHandle(_sound));	
}

void SoundManager::AllStopSound()
{
	for (auto& s : ResoData::SOUND) {
		StopSoundMem(loadManager->GetHandle(s.data.key));
	}
	std::string stageBgm = std::to_string(stageData->GetStageConfigData().bgmData) + "bgmStage";
	StopSoundMem(loadManager->GetHandle(stageBgm));
}

void SoundManager::Update()
{
#if _DEBUG
	ImGui::Begin("sound");
	/*if (ImGui::DragInt("SEVOLUME", &seVolume, 1.0f, 0, 255)) {
		SetSoundTypeVolume("SE", seVolume);
	}

	if (ImGui::DragInt("BGMVOLUME", &bgmVolume, 1.0f, 0, 255)) {
		SetSoundTypeVolume("BGM", bgmVolume);
	}*/
	if (ImGui::Button("PauseBGM")) {
		StopBgm();
	}
	if (ImGui::Button("RestartBGM")) {
		RestartBgm();
	}
	ImGui::End();
#endif // _DEBUG	
}





void SoundManager::RestartBgm()
{
	PlaySoundMem(loadManager->GetHandle(lastPlayBgm), DX_PLAYTYPE_LOOP, false);
	playingBgm = lastPlayBgm;
}





