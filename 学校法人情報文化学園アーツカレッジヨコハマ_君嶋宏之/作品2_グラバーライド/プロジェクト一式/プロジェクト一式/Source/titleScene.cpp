#include "titleScene.h"
#include "../Library/sceneManager.h"
#include "DebugScreen.h"
#include "titleUi.h"
#include "commonFind.h"
#include "movie.h"
#include "productionh.h"
#include "soundManager.h"
#include "loadManager.h"
TitleScene::TitleScene()
{
	movie = CreateGameObject<Movie>();
	titleUi=CreateGameObject<TitleUi>();
	CommonFind* commonFind = SceneManager::CommonScene()->FindGameObject<CommonFind>();
	input = commonFind->CommonFindObject<InputManager>();
	pro = commonFind->CommonFindObject<Production>();
	sound = commonFind->CommonFindObject<SoundManager>();
	load = commonFind->CommonFindObject<LoadManager>();
	load->LoadHandleData("TitleScene");
	sound->PlayBgm("タイトル音楽");
	onPro = false;

	SetDrawOrder(movie, 0);
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	/*if (CheckHitKey(KEY_INPUT_P)) {
		SceneManager::ChangeScene("PlayScene");
	}*/
#ifdef _DEBUG

	if (CheckHitKey(KEY_INPUT_S)) {
		//SceneManager::ChangeScene("SelectScene");
		pro->SceneProductionChange("SelectScene");
	}
#endif // _DEBUG

	if (titleUi->GetTitleUiStateReady()) {
		if (input->GetAnyKeyPut(false) || input->GetIsKeyOrButtonPutNow(KEY_INPUT_Z, XINPUT_BUTTON_A)) {
			sound->PlaySe("決定");
			titleUi->GoTitleUiState();
		}
	}
	if (titleUi->GetGoProEnd() && !onPro) {
		//SceneManager::ChangeScene("SelectScene");
		onPro = true;
		pro->SceneProductionChange("SelectScene");
	}

	movie->PlayMovie();

	SceneBase::Update();
}

void TitleScene::Draw()
{
	//movie->Draw();

	SceneBase::Draw();

	

	/*DrawString(0, 0, "TITLE SCENE", GetColor(255,255,255));
	DrawString(100, 400, "Push [S]Key To SelectScene", GetColor(255, 255, 255));*/

}
