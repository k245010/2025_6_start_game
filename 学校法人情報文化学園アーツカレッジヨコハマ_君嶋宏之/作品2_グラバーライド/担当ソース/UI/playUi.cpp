#include "playUi.h"
#include "stageData.h"
#include "font.h"
#include "commonFind.h"
#include "../Library/sceneManager.h"
#include "../ImGui/imgui.h"
#include "config.h"
#include "function.h"
#include "loadManager.h"
#include "soundManager.h"
#include "commonData.h"
#include "../ImGui/imgui.h"

namespace
{
	// コインを全て取得したときの、全取得音がなるまでの時間
	const int SOUND_PLAY_WAIT_TIME = 30;		

	// 8方向にしたときの一単位の角度
	const float DEG_MIN = 45.0f;				
	// 8方向にしたときの一単位の角度　Rad
	const float RAD_MIN = DEG_MIN * DegToRad;	

	// ステージの文字の最初から最終地点までの距離
	const VECTOR2F START_INFO_POS = V2Get((float)SCREEN_WIDTH_CENTER * 1.2f, (float)SCREEN_HEIGHT_CENTER * 1.2f);
	// ステージの文字の最初の座標
	const VECTOR2F STAGE_FONT_STARTPOS[] = 
	{
		V2Get(START_INFO_POS.x * cosf(0 * RAD_MIN),START_INFO_POS.y * sinf(0 * RAD_MIN)),
		V2Get(START_INFO_POS.x * cosf(1 * RAD_MIN),START_INFO_POS.y * sinf(1 * RAD_MIN)),
		V2Get(START_INFO_POS.x * cosf(2 * RAD_MIN),START_INFO_POS.y * sinf(2 * RAD_MIN)),
		V2Get(START_INFO_POS.x * cosf(3 * RAD_MIN),START_INFO_POS.y * sinf(3 * RAD_MIN)),
		V2Get(START_INFO_POS.x * cosf(4 * RAD_MIN),START_INFO_POS.y * sinf(4 * RAD_MIN)),
		V2Get(START_INFO_POS.x * cosf(5 * RAD_MIN),START_INFO_POS.y * sinf(5 * RAD_MIN)),
		V2Get(START_INFO_POS.x * cosf(6 * RAD_MIN),START_INFO_POS.y * sinf(6 * RAD_MIN)),
		V2Get(START_INFO_POS.x * cosf(7 * RAD_MIN),START_INFO_POS.y * sinf(7 * RAD_MIN)),
	};
	// ステージの文字が止まる最終座標
	const VECTOR2F STAGE_FONT_ENDPOS = V2Get((float)SCREEN_WIDTH_CENTER + 140.0f, (float)SCREEN_HEIGHT_CENTER);

	// コインの文字座標
	const VECTOR2F COIN_TEXT_POS = V2Get((float)SCREEN_WIDTH - 208.0f, 85.0f + 20.0f);
	// コインの画像座標
	const VECTOR2F COIN_IMAGE_POS = V2Get(1857.0f, 120.0f);
	// コインの後ろの画像の最大拡大サイズ
	const float COIN_MAX_BACK_SCALE_MAX = 1.5f;
	// コインの後ろの画像の最小サイズ
	const float COIN_MAX_BACK_SCALE_MIN = 1.0f;

	// タイムの文字座標
	const VECTOR2F TIME_TEXT_POS = V2Get((float)SCREEN_WIDTH - 43.0f, 31.0f + 20.0f);
}

PlayUi::PlayUi(SceneBase* _scene):GameObject(_scene)
{
	CommonFind* commonFind = SceneManager::CommonScene()->FindGameObject<CommonFind>();
	commonData = commonFind->CommonFindObject<CommonData>();
	stageScore=_scene->FindGameObject<StageScore>();
	gameManager=_scene->FindGameObject<GameManager>();
	sound = SceneManager::CommonScene()->FindGameObject<SoundManager>();

	timeFontImage = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("TIME_NUM");
	coinFontImage = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("NUMBER_TEXT");
	coinImage = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("COIN");
	crossImage = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("CROSS");
	
	coinTextPos = COIN_TEXT_POS;
	timeTextPos = TIME_TEXT_POS;

	coinMaxData.position = COIN_IMAGE_POS;
	coinMaxData.onMax = false;
	coinMaxData.angle = 0.0f;
	coinMaxData.scale = COIN_MAX_BACK_SCALE_MIN;
	coinMaxData.time = 0.0f;
	coinMaxData.image = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("OBJ_BACK");

	allCoinGetSound = false;
	waitTime = SOUND_PLAY_WAIT_TIME;

	state = READY_STATE::STAY;

	// ランダムな方向からステージ数文字を出すように乱数を初期化
	randIndex = GetRand(7);
	stageFont.position = V2Get(0.0f, 0.0f);
	stageFont.rate = 0.0f;
	stageFont.time = 0;
	stageFont.scale = 1.0f;
	stageFont.draw = false;
	stageFont.state = RATE_STATE::START;
	stageFont.image[0] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("STAGE_FONT");
	stageFont.image[1] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("NUMBER_TEXT_1");

	onDraw = true;

	stageFontColor[0] = 255.0f;
	stageFontColor[1] = 0.0f;
	stageFontColor[2] = 255.0f;
}

PlayUi::~PlayUi()
{
}

void PlayUi::Update()
{


#if _DEBUG
	ImGui::Begin("PlayUI");
	ImGui::SliderFloat2("coinTextPos", &coinTextPos.x, 0, SCREEN_WIDTH);
	ImGui::SliderFloat2("timeTextPos", &timeTextPos.x, 0, SCREEN_WIDTH);
	ImGui::Text("Coin×%01d", stageScore->GetCoinNum());
	ImGui::Text("%02d:%02d.%01d", (int)(stageScore->GetGameTime() / 60), (int)stageScore->GetGameTime() % 60,	(int)((stageScore->GetGameTime() - (int)stageScore->GetGameTime()) * 10));
	ImGui::SliderFloat2("CoinBack", &coinMaxData.position.x, 0, 2500);
	ImGui::SliderFloat2("StageFontPos", &stageFont.position.x, SCREEN_WIDTH_CENTER, SCREEN_WIDTH_CENTER * 2);

	ImGui::Begin("StageFont");
	ImGui::ColorEdit3("color", &stageFontColor[0], 0);
	ImGui::End();
	ImGui::End();

#endif // _DEBUG


#ifdef _DEBUG

	ImGui::Begin("PlayUi");
	if (ImGui::SmallButton("onDraw"))
		onDraw = !onDraw;
	ImGui::End();
#endif


	switch (state)
	{
	case PlayUi::READY_STATE::STAY:

		if (gameManager->GetGameState() == GameManager::GAME_STATE::PLAY_BEFORE)
		{
			state = READY_STATE::STAGE;
		}
		break;
	case PlayUi::READY_STATE::STAGE:

		switch (stageFont.state)
		{
		case PlayUi::RATE_STATE::START:

			stageFont.rate += 0.01f;

			stageFont.position = EaseIn(V2Get(STAGE_FONT_ENDPOS.x + STAGE_FONT_STARTPOS[randIndex].x, STAGE_FONT_ENDPOS.y + STAGE_FONT_STARTPOS[randIndex].y), STAGE_FONT_ENDPOS, stageFont.rate);
			stageFont.draw = true;

			if (stageFont.rate > 1.0f)
			{
				stageFont.rate = 0.0f;
				stageFont.state = RATE_STATE::STOP;
			}
			break;
		case PlayUi::RATE_STATE::STOP:

			if (stageFont.time++ > 60 * 1.5f)
			{
				stageFont.state = RATE_STATE::END;
				stageFont.time = 0;
			}
			break;
		case PlayUi::RATE_STATE::END:

			stageFont.draw = false;
			state = READY_STATE::START;
			break;
		default:
			break;
		}
		break;
	case PlayUi::READY_STATE::START:

#if 0
		if (startFont.scale < 3.0f)
		{
			startFont.scale += 0.01f;
			startFont.draw = true;
		}
		else
		{
			state = READY_STATE::END;
			startFont.draw = false;
		}
#else
		state = READY_STATE::END;
#endif
		break;
	case PlayUi::READY_STATE::END:


		break;
	default:
		break;
	}

	if (coinMaxData.onMax)
	{
		coinMaxData.time += 0.05f;
		coinMaxData.scale = EaseIn(COIN_MAX_BACK_SCALE_MIN, COIN_MAX_BACK_SCALE_MAX, sinf(coinMaxData.time));
	}
}

void PlayUi::Draw()
{
	//__ ステージ(ステージ数) __//
	if (stageFont.draw)
	{
		SetDrawBright((int)stageFontColor[0] * 255, (int)stageFontColor[1] * 255, (int)stageFontColor[2] * 255);
		// 文字
		DrawRotaGraphF(stageFont.position.x - 170.0f, stageFont.position.y - 50.0f, 1.8f, 0.0f, stageFont.image[0], true);
		// 数字
		DrawRotaNum(stageFont.position.x + 40.0f, stageFont.position.y - 50.0f, commonData->GetData().stage + 1, 47, stageFont.image[1], 39, 45, 1.6f, 0.0f, 1);
		SetDrawBright(255, 255, 255);
	}

#if 0	
	if (!onDraw)
		return;
#else
	// UI描画フラグ
	if (!onDraw || gameManager->GetGameState() != (int)GameManager::PLAYING || gameManager->GetGameState() == (int)GameManager::RESULT_DRAW)
		return;
	
#endif	

	
	//__ COIN __//
	if (stageScore->GetCoinNum() >= stageScore->GetTargetCoin())
	{
		coinMaxData.onMax = true;
		// 数字の後ろの光
		DrawRotaGraphF(coinMaxData.position.x, coinMaxData.position.y, coinMaxData.scale, coinMaxData.angle, coinMaxData.image, true);
		if (!allCoinGetSound)
		{
			if (waitTime-- < 0)
			{
				sound->PlaySe("ALLコイン取得");
				allCoinGetSound = true;
				waitTime = SOUND_PLAY_WAIT_TIME;
			}
		}
		SetDrawBright(0, 255, 0);
	}																													
	else
	{
		SetDrawBright(240, 255, 0);
	}
	// 数字
	DrawRotaNum(coinTextPos.x + 170, coinTextPos.y + 20, stageScore->GetCoinNum(), 47, coinFontImage, 60, 65, 0.9f, 0.0f, 2);
	SetDrawBright(255, 255, 255);
	// 文字
	DrawRotaGraphF(coinTextPos.x + 13.0f, coinTextPos.y + 20.0f, 0.25f, 0.0f, coinImage, true);
	// ×
	DrawRotaGraphF(coinTextPos.x + 75.0f, coinTextPos.y + 20.0f, 0.6f, 0.0f, crossImage, true);


	//__ TIME __//
	DrawTimeAsImageM(timeTextPos, stageScore->GetGameTime(), 48, timeFontImage, 1.0f, V2Get(60, 56), V2Get(60, 64), V2Get(145, 16), V2Get(15, 31), V2Get(161, 35), V2Get(18, 20));
}
