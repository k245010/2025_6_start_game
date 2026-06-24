#include "productionh.h"

#include "Screen.h"
#include "../Library/sceneManager.h"
#include "commonFind.h"
#include "fader.h"
#include "loadManager.h"
#include "soundManager.h"
Production::Production(SceneBase* _scene):GameObject(_scene)
{
	loadState = LoadState::STAY;
	nextScene = "NO";
	fade = _scene->FindGameObject<Fader>();
	loadManager = _scene->FindGameObject<LoadManager>();
	sound = _scene->FindGameObject<SoundManager>();

}

Production::~Production()
{
}

void Production::Draw()
{
#if _DEBUG
	if (loadState == LoadState::WaitForLoad) {
		float rate = loadManager->GetProgressRate();
		int percent = static_cast<int>(rate * 100);
		DrawFormatString(Screen::WIDTH - 400, Screen::HEIGHT - 130, GetColor(255, 255, 255), "Loading... %d%%", percent);

		// プログレスバー表示
		int barWidth = 400;
		int filled = static_cast<int>(rate * barWidth);
		DrawBox(Screen::WIDTH-400, Screen::HEIGHT - 100, Screen::WIDTH -400 + barWidth, Screen::HEIGHT - 130, GetColor(100, 100, 100), TRUE);
		DrawBox(Screen::WIDTH - 400, Screen::HEIGHT - 100, Screen::WIDTH - 400 + filled, Screen::HEIGHT - 130, GetColor(0, 255, 0), TRUE);
	}
#endif // _DEBUG

	
}

void Production::Update()
{

	switch (loadState)
	{
	case Production::LoadState::STAY:

		break;
	case Production::LoadState::FadeOut:
		if (fade->IsEnd()) {
			SceneManager::ChangeScene(nextScene);
		

			loadManager->SetAsync(true);
			loadManager->LoadHandleData(nextScene);
			loadState = LoadState::WaitForLoad;
			
		}
		break;

	case Production::LoadState::WaitForLoad:
		
		if (!loadManager->IsLoading()) {
			
			fade->FadeIn(60);
			loadState = LoadState::FIN;
			loadManager->SetAsync(false);
		}
		break;
	case Production::LoadState::FIN:
		loadState = LoadState::STAY;
		break;
	default:
		break;
	}
}

void Production::SceneProductionChange(const std::string _nextScene)
{	
	if (loadState != LoadState::STAY)
		return;
	
	nextScene = _nextScene;
	loadState = LoadState::FadeOut;
	
	fade->FadeOut(60);
	//sound->PlaySe("フェードアウト");


}
