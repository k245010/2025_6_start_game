#include "TitleScene.h"
#include "PlayScene.h"
#include "../../Camera/Camera.h"
#include "../../Stage/StageManager/StageManager.h"
#include "../../Screen.h"
#include "../../Common/Sky/Sky.h"
#include "../../Common/Input/Input.h"
#include "../../Common/Sound/SoundManager.h"
#include "../../Common/Sound/SoundID.h"
#include "../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../Common/Fade/Fade.h"
#include "../../../Library/DebugNew.h"
#include <assert.h>

namespace
{
	constexpr float MAX_SELECT_KEY_UI_SCALE_CHANGE_TIME = 0.15f;	// ステージ選択キーUIの大きさの最大変化時間
}

TitleScene::TitleScene()
{
	SetCameraPositionAndTarget_UpVecY(VZero, VECTOR3(0, 0, 100));
	Transform transform;
	transform.scale = VOne * 20.0f;
	new Sky(transform, MV1LoadModel("data/models/sky/sky_2.mv1"));

	uiImage[IMAGE_TITLE_LOGO]	= LoadGraph("data/texture/letter/title_logo.png");
	uiImage[IMAGE_A_KEY]		= LoadGraph("data/texture/a_key.png");
	uiImage[IMAGE_D_KEY]		= LoadGraph("data/texture/d_key.png");

	input	= FindGameObject<Input>();
	fade	= FindGameObject<Fade>();

	// フェードアウトしていたら
	if (fade->GetLevel() > 0)
	{
		// フェードインする
		fade->FadeIn(30);
	}

	lastStageNumber = PlayScene::startStageNumber;

	SoundManager::PlayBGM(Sound_ID::TITLE_BGM, true, true);

}

TitleScene::~TitleScene()
{
	// エフェクトの削除
	EffekseerObjectManager::DeleteAllEffect();
}

void TitleScene::Update()
{
	if (CheckHitKey(KEY_INPUT_ESCAPE)) 
		SceneManager::Exit();
	
	if (!isFirstFrame)
	{
		if (input->GetKeyConfigPut("NEXT"))
		{
#ifdef _DEBUG
			if (CheckHitKey(KEY_INPUT_LCONTROL))
			{
				// ナビメッシュを一から構築せずに、csvから読み込む
				PlayScene::isNavMeshCreate = false;
			}
			else
			{	
				// ナビメッシュを一から構築
				PlayScene::isNavMeshCreate = true;
			}
#endif // _DEBUG

			SoundManager::PlaySE(Sound_ID::SELECT_PUSH_SE);

			// フェードアウト
			fade->FadeOut(60 * 1);

			isSceneChange = true;
		}
	}

	if (isSceneChange)
	{
		// フェードアウトが終了したら
		if (fade->IsEnd())
		{
			// プレイシーンの開始
			PlaySceneStart();
		}
	}

	//_ ステージの選択処理 _//

	// 1～ステージ最大数まで回す
	for (int i = 1;i <= StageManager::GetStageNumberMax();i++)
	{
		if (PlayScene::startStageNumber == i)
			continue;	// ナンバーが現在選択しているナンバーだったら continue
	
		std::string str = "_D_SELECT_STAGE_NUM_" + std::to_string(i);

		if (input->GetKeyConfigPut(str))
		{
			// ステージ選択変更サウンド再生
			SoundManager::PlaySE(Sound_ID::CHANGE_START_STAGE_NUM_SE);

			// ステージナンバーを設定
			PlayScene::startStageNumber = i;
		}
	}

	if (PlayScene::startStageNumber > 1)
	{
		if (input->GetKeyConfigPut("MOVE_LEFT"))
		{
			PlayScene::startStageNumber--;
			selectKeyUIScaleChangeTime = MAX_SELECT_KEY_UI_SCALE_CHANGE_TIME;
			isPressedAKey = true;
			// ステージ選択変更サウンド再生
			SoundManager::PlaySE(Sound_ID::CHANGE_START_STAGE_NUM_SE);
		}
	}

	if (PlayScene::startStageNumber < StageManager::GetStageNumberMax())
	{
		if (input->GetKeyConfigPut("MOVE_RIGHT"))
		{
			PlayScene::startStageNumber++;
			selectKeyUIScaleChangeTime = MAX_SELECT_KEY_UI_SCALE_CHANGE_TIME;
			isPressedDKey = true;
			// ステージ選択変更サウンド再生
			SoundManager::PlaySE(Sound_ID::CHANGE_START_STAGE_NUM_SE);
		}
	}

	if ((selectKeyUIScaleChangeTime -= Time::GameDeltaTime()) < 0)
	{
		isPressedAKey = false;
		isPressedDKey = false;
	}

	isFirstFrame = false;
}

void TitleScene::Draw()
{
	DrawRotaGraph(Screen::WIDTH_CENTER, Screen::HEIGHT_CENTER,1.5f,0.0f, uiImage[IMAGE_TITLE_LOGO], true);
	//DrawRotaGraph(Screen::WIDTH_CENTER, Screen::HEIGHT_CENTER,1.0f,0.0f, uiImage[IMAGE_TITLE], true);

	SetFontSize(55);

	VECTOR2I stageLetterPos		= VECTOR2I(Screen::WIDTH_CENTER - 85, Screen::HEIGHT - 200);
	VECTOR2I stageSelectAKeyPos	= VECTOR2I(stageLetterPos.x - 100,stageLetterPos.y + 25);
	VECTOR2I stageSelectDKeyPos	= VECTOR2I(stageLetterPos.x + 290,stageLetterPos.y + 25);
	float stageSelectAKeyScale	= 1.8f;
	float stageSelectDKeyScale	= 1.8f;
	VECTOR2I startLetterPos		= VECTOR2I(Screen::WIDTH - 400, Screen::HEIGHT - 70);

	// ToDo:関数化する

	if (selectKeyUIScaleChangeTime > 0)
	{
		if (isPressedAKey)
		{
			stageSelectAKeyScale = 1.5f;
		}

		if (isPressedDKey)
		{
			stageSelectDKeyScale = 1.5f;
		}
	}

	DrawRectRotaGraph(stageSelectAKeyPos.x, stageSelectAKeyPos.y, 0, 0, 32, 32, stageSelectAKeyScale, 0.0, uiImage[IMAGE_A_KEY], true);
	DrawRectRotaGraph(stageSelectDKeyPos.x, stageSelectDKeyPos.y, 0, 0, 32, 32, stageSelectDKeyScale, 0.0, uiImage[IMAGE_D_KEY], true);

	DrawFormatString(stageLetterPos.x + 4, stageLetterPos.y + 4, 0x000000, "Stage %d", PlayScene::startStageNumber);
	DrawFormatString(stageLetterPos.x, stageLetterPos.y, 0xffffff, "Stage %d", PlayScene::startStageNumber);
	
	SetFontSize(60);

	DrawString(startLetterPos.x + 4, startLetterPos.y + 4, "Start [ENTER]",0x000000);
	DrawString(startLetterPos.x, startLetterPos.y, "Start [ENTER]",0xffffff);

	SetFontSize(DEFAULT_FONT_SIZE);
}

void TitleScene::PlaySceneStart()
{
	SoundManager::StopBGM(Sound_ID::TITLE_BGM);

	SceneManager::ChangeScene("PLAY");
}
