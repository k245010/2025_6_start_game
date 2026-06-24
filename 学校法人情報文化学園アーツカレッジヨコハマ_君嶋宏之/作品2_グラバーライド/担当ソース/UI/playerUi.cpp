#include "playerUi.h"
#include "soundManager.h"
#include "loadManager.h"
#include "debugScreen.h"
#include "../ImGui/imgui.h"
#include "effectManager.h"
#include "gameManager.h"
#include "function.h"
#include "config.h"
#include <cmath>
#include <assert.h>

namespace 
{
	const float GRAVITY_GAUGE_MAX = 200.0f;

	const int UI_HP_DISTANCE = 131;
	const VECTOR2F UI_HP_SIZE = V2Get(150.0f, 25.0f);
	const VECTOR2F UI_HP_POSITION = V2Get(250.0f, 50.0f);
	const float UI_HP_SCALE_SUB_VALUE = 0.1f;
	const VECTOR2F UI_GRAVITY_GAUGE_POSITION = V2Get(150.0f, 150.0f);
	const VECTOR2F UI_GRAVITY_GAUGE_SIZE = V2Get(250.0f, 250.0f);
	const float UI_GRAVITY_GAUGE_CHANGE_ROT = 90 * DegToRad;			// 画像を90度回転させて、計算しやすいようにする
	const float UI_GRAVITY_GAUGE_ARROW_DEFAULT_DISTANCE = 55.5f;
	const float UI_GRAVITY_GAUGE_ARROW_LIGHT_DISTANCE = 65.5f;
	const float UI_GRAVITY_GAUGE_ARROW_LIGHT_INIT_RATE = 0.8f;
	const float UI_GRAVITY_GAUGE_ARROW_LIGHT_MAX_RATE = 1.1f;

	const float UI_GRAVITY_GAUGE_ARROW_LIGHTING_ERROR = DX_PI_F / 8;	// どれが一番プレイヤーの重力方向に近いか比べる時の誤差の値
	const float UI_GRAVITY_GAUGE_MIN_ROT = 45.0f * DegToRad;			// 計算する最小値の角度

	const float WARNING_GRAVITY_VALUE_VISION = 60.0f;					// 画面が赤くなる重力の値 この場合60%
	const float WARNING_GRAVITY_VALUE_GAUGE = 30.0f;					// ゲージが赤点滅する重力の値 この場合30%

	const Rgb VISION_COLOR_LEVEL[] =
	{
		{0,0,200},	// 重力ゲージ減少時
		{200,0,0},	// HP減少時
	};

	const float VISION_RATE_SPEED = 1.5f;

	const int GRAVITY_MOVE_GAUGE_NUM_MAX = 255;
}

PlayerUi::PlayerUi(SceneBase* _scene) :UiBase(_scene)
{
	LoadManager* load = SceneManager::CommonScene()->FindGameObject<LoadManager>();


	gravityGaugeSub = 0.0f;
	gravityMoveGaugeImage = LoadGraph("data\\texture\\A.png");

	gravityMoveGaugeNum = GRAVITY_GAUGE_MAX;
	gravityMoveGaugeNumMax = false;
	gravityMoveGaugeNumCount = 0;

	uiPosition.x = 0;
	uiPosition.y = 600;

	for (int i = 0;i < (int)VISION_KIND::MAX;i++)
	{
		vision[i].hImage = load->GetHandle("VISION");
		assert(vision[i].hImage >= 0);
		vision[i].onVision = false;
		vision[i].alpha = 0.0f;
		vision[i].time = 0.0f;
	}

	char uiFileName[70];
	for (int i = 0;i < UI_HP_IMAGE_MAX;i++)
	{
		sprintfDx(uiFileName, "data\\texture\\hp%d.png", i);
		hp.image[i] = LoadGraph(uiFileName);
		assert(hp.image[i] >= 0);
	}

	for (int i = 0;i < UI_HP_MAX;i++)
	{
		sprintfDx(uiFileName, "data\\texture\\hp_base%d.png", i);
		hp.backImage[i] = LoadGraph(uiFileName);
		assert(hp.backImage[i] >= 0);
	}
	hp.basePosition = UI_HP_POSITION;
	ResetUi();

	gGaugeUiGlobalPosition = UI_GRAVITY_GAUGE_POSITION;

	for (int i = 0;i < (int)GRAVITY_GAUGE::MAX;i++)
	{
		gGaugeChange[i] = false;
		gGaugeChangeNum[i] = 2;
		gGaugeChangeNumCopy[i] = gGaugeChangeNum[i];
	}

	for (int i = 0;i < (int)GRAVITY_GAUGE::MAX;i++)
	{
		gGaugeUiData[i].size = UI_GRAVITY_GAUGE_SIZE;
		char gGaugeFileName[70];

		switch (i)
		{
		case (int)GRAVITY_GAUGE::BASE:
			
			sprintfDx(gGaugeFileName, "data\\texture\\g_gauge_base%d.png", gGaugeChangeNum[i]);
			gGaugeUiData[i].hImage = LoadGraph(gGaugeFileName);
			break;
		case (int)GRAVITY_GAUGE::BASE_IN:

			gGaugeUiData[i].hImage = LoadGraph("data\\texture\\g_gauge_base_in.png");
			break;
		case (int)GRAVITY_GAUGE::CENTER:

			gGaugeUiData[i].hImage = LoadGraph("data\\texture\\g_gauge_base_center.png");
			break;
		case (int)GRAVITY_GAUGE::GAUGE:

			sprintfDx(gGaugeFileName, "data\\texture\\g_gauge%d.png", gGaugeChangeNum[i]);
			gGaugeUiData[i].hImage = LoadGraph(gGaugeFileName);
			break;
		default:
			break;
		}
		assert(gGaugeUiData[i].hImage >= 0);
		gGaugeUiData[i].brightColor.SetColor(255, 255, 255);
		gGaugeUiData[i].time = 0.0f;
	}

	for (int i = 0;i < (int)GRAVITY_GAUGE_ARROW::MAX;i++)
	{
		float initAngle = 0.0f;
		float addAngle = (360 / (UI_GRAVITY_GAUGE_ARROW_MAX / 2)) * DegToRad;	// 90度
		if (i % 2 != 0)
		{
			initAngle = (360 / UI_GRAVITY_GAUGE_ARROW_MAX) * DegToRad;			// 45度
			gArrowUiData[i].size = V2Get(40.0f,40.0f);
		}
		else
		{
			gArrowUiData[i].size = V2Get(50.0f, 60.0f);
		}
		for (int j = 0;j < UI_GRAVITY_GAUGE_ARROW_MAX / 2;j++)
		{
			float angle = addAngle * j;
			angle += initAngle;
			
			gArrowUiData[i].angle[j] = angle + UI_GRAVITY_GAUGE_CHANGE_ROT;
		}
		sprintfDx(uiFileName, "data\\texture\\g_gauge_arrow%d.png", i);
		gArrowUiData[i].hImage = LoadGraph(uiFileName);
		assert(gArrowUiData[i].hImage >= 0);
	}
	GravityGaugeUiOffset();

	arrowUiColorNormal.SetColor(255, 126, 0);
	arrowUiColorLight.SetColor(118, 0, 180);
	drawRateScaleInit = UI_GRAVITY_GAUGE_ARROW_LIGHT_INIT_RATE;
	drawRateScale = UI_GRAVITY_GAUGE_ARROW_LIGHT_MAX_RATE;
	light = 10;
	time = 0.0f;

	allowImage = LoadGraph("data\\texture\\YJ_2.png");
	allowPos = VGet(0, 0, 0);
	assert(allowImage >= 0);

	for (int i = 0;i < 8;i++)
		setAngle[i] = -1;

	
	gameManagerState = GameManager::GAME_STATE::PLAY_BEFORE;
	playerGravityState = nullptr;
	allowAngle = 0.0f;

	noGravity = false;

	inGoal = false;
}

void PlayerUi::ResetUi()
{
	ResetHpDataBlendAdd();
	for (int i = 0;i < UI_HP_MAX;i++)
	{
		hp.drawData[i].alpha = 255;
		hp.drawData[i].count = 0.0f;
		hp.drawData[i].scale = 1.0f;
		hp.drawData[i].onAlpha = false;
		hp.drawData[i].onDraw = true;
		hp.drawData[i].onUp = false;
		hp.drawData[i].onDown = false;
		hp.drawData[i].changingState = HP_CHANGING_STATE::NORMAL;
	}
	hp.changeState = HP_CHANGE_STATE::CHANGE;
	hp.oneHpState = HP_ONE::NORMAL;
	hp.num = PLAYER_HP_MAX;
	hp.oneHp = false;
	copyHpNum = hp.num;
	onChangeHpNum = false;
}

void PlayerUi::ResetHpDataBlendAdd()
{
	for (int i = 0;i < UI_HP_MAX;i++)
	{
		hp.drawData[i].add = 0.0f;
		hp.drawData[i].addScale = 0.01f;
		hp.drawData[i].addLevel = 1;
		hp.drawData[i].onAddDraw = false;
	}
}

void PlayerUi::SetHpOneInfo(bool one)
{
	hp.oneHpState = one ? HP_ONE::ONE : HP_ONE::NORMAL;
}

void PlayerUi::SetHpChangeInfo(const HP_CHANGING_STATE& _state, int index)
{
	switch (_state)
	{
	case PlayerUi::HP_CHANGING_STATE::ADD:

		hp.drawData[index].onDraw = true;
		hp.drawData[index].onAddDraw = false;
		hp.drawData[index].alpha = 255;
		ResetHpDataBlendAdd();
		break;
	case PlayerUi::HP_CHANGING_STATE::SUB:
		
		hp.drawData[index].onDraw = false;
		hp.drawData[index].scale = 1.0f;
		hp.drawData[index].alpha = 0;
		break;
	default:

		assert(false);
		break;
	}
	onChangeHpNum = false;
	copyHpNum = hp.num;
	hp.drawData[index].changingState = HP_CHANGING_STATE::NORMAL;
}

void PlayerUi::SetHpChangeState(int index)
{	
	if (hp.num < copyHpNum && !(copyHpNum <= 1) && copyHpNum == index + 1)
		hp.drawData[index].changingState = HP_CHANGING_STATE::SUB;

	if (hp.num > copyHpNum && copyHpNum == index + 1)
		hp.drawData[index + 1].changingState = HP_CHANGING_STATE::ADD;
}

void PlayerUi::GravityGaugeUiOffset()
{
	for (int i = 0;i < (int)GRAVITY_GAUGE::MAX;i++)
		gGaugeUiData[i].position = gGaugeUiGlobalPosition;

	for (int i = 0;i < (int)GRAVITY_GAUGE_ARROW::MAX;i++)
	{
		for (int j = 0;j < UI_GRAVITY_GAUGE_ARROW_MAX / 2;j++)
		{
			gArrowUiData[i].position[j] = gGaugeUiGlobalPosition;
			float dis = UI_GRAVITY_GAUGE_ARROW_DEFAULT_DISTANCE;
			if (gameManagerState == GameManager::GAME_STATE::PLAYING && LigthingGravityArrow(gArrowUiData[i].angle[j]) && *playerModeState != Player::PlayerMode::NORMAL_MODE)
				dis = UI_GRAVITY_GAUGE_ARROW_LIGHT_DISTANCE;
			gArrowUiData[i].position[j].x += dis * cosf(gArrowUiData[i].angle[j] - UI_GRAVITY_GAUGE_CHANGE_ROT);
			gArrowUiData[i].position[j].y += dis * sinf(gArrowUiData[i].angle[j] - UI_GRAVITY_GAUGE_CHANGE_ROT);
		}
	}

	/*arrowUiColorLight.SetColor(0, 0, 0);
	arrowUiColorNormal.SetColor(0, 0, 0);*/
}

bool PlayerUi::IsWaringGravityValue(WARNING_GRAVITY_VALUE_KIND _kind)
{
	switch (_kind)
	{
	case PlayerUi::WARNING_GRAVITY_VALUE_KIND::VISION:
		if (WARNING_GRAVITY_VALUE_VISION > 100 * (gravityMoveGaugeNum / GRAVITY_GAUGE_MAX))
			return true;
		break;
	case PlayerUi::WARNING_GRAVITY_VALUE_KIND::GAUGE:
		if (WARNING_GRAVITY_VALUE_GAUGE > 100 * (gravityMoveGaugeNum / GRAVITY_GAUGE_MAX))
			return true;
		break;
	}
	return false;
}

void PlayerUi::GravityMoveGaugeNumInit()
{
	gravityMoveGaugeNumMax = true;
	gravityMoveGaugeNumInfo = true;
	gravityMoveGaugeNumCount = GRAVITY_MOVE_GAUGE_NUM_MAX;
}

bool PlayerUi::VisionValueInfo(float& value, const float rate, const float finishValue, int _kind)
{
	vision[_kind].onVision = true;
	value += rate;
	float copyValue = rate < 0 ? -value : value;

	if (copyValue > finishValue)
	{
		value = finishValue;
		return true;
	}
	return false;
}

void PlayerUi::PlayerGravityStateInit(GravityState& _state)
{
	playerGravityState = &_state;
}

PlayerUi::~PlayerUi()
{
	DeleteGraph(allowImage);
	DeleteGraph(gravityMoveGaugeImage);

	for (int i = 0;i < (int)VISION_KIND::MAX;i++)
		DeleteGraph(vision[i].hImage);

	for (int i = 0;i < UI_HP_IMAGE_MAX;i++)
		DeleteGraph(hp.image[i]);

	for (int i = 0;i < UI_HP_MAX;i++)
		DeleteGraph(hp.backImage[i]);

	for (int i = 0;i < (int)GRAVITY_GAUGE::MAX;i++)
		DeleteGraph(gGaugeUiData[i].hImage);
}

void PlayerUi::Update()
{
	UiBase::Update();

	ObjectPosisionMove(&uiPosition, 1.0f);
	GravityGaugeUiOffset();

#ifdef _DEBUG
	ImGui::Begin("BLENDMODE_ADD");
	ImGui::SliderFloat("rateInit", &drawRateScaleInit, 0.0f, 1.0f);
	ImGui::SliderFloat("rate", &drawRateScale, 0.0f, 0.9f);
	ImGui::SliderInt("light", &light, 0, 255);
	ImGui::SliderInt("texGauge Back", &gGaugeChangeNum[0], 0, 2);
	ImGui::SliderInt("texGauge Front", &gGaugeChangeNum[2], 0, 2);
	ImGui::SliderInt3("arrowNorm", &arrowUiColorNormal.r, 0, 255);
	ImGui::SliderInt3("arrowLight", &arrowUiColorLight.r, 0, 255);
	ImGui::End();



#endif // _DEBUG

#ifdef _DEBUG
	ImGui::Begin("PlayerUi");
	if (ImGui::SmallButton("onDraw"))
		onDraw = !onDraw;
	ImGui::End();
#endif


	for (int i = 0;i < (int)GRAVITY_GAUGE_ARROW::MAX;i++)
	{
		for (int j = 0;j < UI_GRAVITY_GAUGE_ARROW_MAX / 2;j++)
		{
			if (LigthingGravityArrow(gArrowUiData[i].angle[j]) && *playerModeState != Player::PlayerMode::NORMAL_MODE)
			{
				arrowRate = EaseIn(0.0f, UI_GRAVITY_GAUGE_ARROW_LIGHT_DISTANCE - UI_GRAVITY_GAUGE_ARROW_DEFAULT_DISTANCE, fabsf(sinf(time)));
				gArrowUiData[i].position[j].x += arrowRate * cosf(gArrowUiData[i].angle[j] - UI_GRAVITY_GAUGE_CHANGE_ROT);
				gArrowUiData[i].position[j].y += arrowRate * sinf(gArrowUiData[i].angle[j] - UI_GRAVITY_GAUGE_CHANGE_ROT);
			}
		}
	}
	
	//__ VISION __//
	int kind = (int)VISION_KIND::GRAVITY_VALUE;

	if (IsWaringGravityValue(WARNING_GRAVITY_VALUE_KIND::GAUGE))
	{
		VisionValueInfo(vision[kind].alpha, VISION_RATE_SPEED, 150.0f, kind);
	}
	else
	{
		if (VisionValueInfo(vision[kind].alpha, -VISION_RATE_SPEED * 2, 0.0f, kind))	// 減る速度は早く
			vision[kind].onVision = false;
	}

	kind = (int)VISION_KIND::HP_VALUE;

	if (hp.oneHpState == HP_ONE::ONE)
	{
		VisionValueInfo(vision[kind].alpha, VISION_RATE_SPEED, 150.0f, kind);
	}
	else
	{
		if (VisionValueInfo(vision[kind].alpha, -VISION_RATE_SPEED * 2, 0.0f, kind))	// 減る速度は早く
			vision[kind].onVision = false;
	}

	time += 0.05f;
}

VECTOR2F PlayerUi::GetDrawPosition(const VECTOR2F& vec)
{
	return uiGlobalPosition + vec;
}

void PlayerUi::Draw()
{
#if 0
	if (!onDraw)
		return;
#else
	if (!onDraw || gameManagerState != (int)GameManager::PLAYING || gameManagerState == (int)GameManager::RESULT_DRAW)
		return;
#endif
	//__ HP __//
	for (int i = 0;i < UI_HP_MAX;i++)
	{
		VECTOR2F drawPos = GetDrawPosition(V2Get(hp.basePosition.x + i * UI_HP_DISTANCE, hp.basePosition.y));
		HpDrawData& draw = hp.drawData[i];

		switch (hp.changeState)
		{
		case PlayerUi::HP_CHANGE_STATE::CHANGE:

			if (draw.changingState == HP_CHANGING_STATE::NORMAL)
				SetHpChangeState(i);
			break;
		case PlayerUi::HP_CHANGE_STATE::SET:

			draw.changingState = HP_CHANGING_STATE::NORMAL;
			SetHpChangeState(i);

			if (draw.changingState != HP_CHANGING_STATE::NORMAL)
				SetHpChangeInfo(draw.changingState, i);
			break;
		default:
			break;
		}
			
		// BackFrame
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		DrawRectGraphF(drawPos.x, drawPos.y, 0, 0, (int)UI_HP_SIZE.x, (int)UI_HP_SIZE.y, hp.backImage[i], true);
		SetDrawBright(255, 255, 255);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// HP
		int hImageIndex = i;
		float rate = 0.0f;
		float t = 0.0f;
		int setIndex = i;

		switch (draw.changingState)
		{
		case PlayerUi::HP_CHANGING_STATE::NORMAL:
			break;
		case PlayerUi::HP_CHANGING_STATE::ADD:

			// 現在のHP数未満のHPゲージは瞬時にalpha値リセット
			setIndex = max(i - 1, 0);
			hp.drawData[setIndex].alpha = 255;

			draw.onDraw = true;
			draw.onAddDraw = true;
			draw.alpha += 5;

			rate = draw.alpha / 255.0f;
			t = EaseOutElastic(0.0f, 1.0f, rate);
			draw.scale = t;
			draw.addLevel = (int)(10 + (1 - t) * 10);
			draw.add = 10.0f + rate;

			if (draw.alpha > 255)
			{
				SetHpChangeInfo(draw.changingState, i);
				ResetHpDataBlendAdd();
			}
			break;
		case PlayerUi::HP_CHANGING_STATE::SUB:

			// HP減少時
			if (i == hp.num)		
			{
				draw.alpha -= 4;

				rate = draw.alpha / 255.0f;
				t = EaseIn(0.0f, 180.0f * DegToRad, rate);		// return 0.0 ~ 180.0fのラジアン値	☆☆☆
				draw.scale = sinf(t) * 2.0f;					// 0.0fと180.0fのラジアン値の時scaleが0　☆☆☆
				if (draw.alpha < 0)
				{
					SetHpChangeInfo(draw.changingState,i);
				}
			}
			break;
		default:
			break;
		}
		
		// 死亡時は表示しない
		if (hp.num == 0)		
			draw.onDraw = false;

		// HP1の時の点滅
		if (copyHpNum == 1)		
		{
			if (i == copyHpNum - 1)
			{
				if (!draw.onAlpha)
				{
					draw.alpha += 2.5f;
					if (draw.alpha > 255)
						draw.onAlpha = true;
				}
				else
				{
					draw.alpha -= 3.5f;
					if (draw.alpha < 50)
						draw.onAlpha = false;
				}
				if (hp.num == 1)
					hImageIndex = 3;
			}
			SetHpOneInfo(true);
		}
		else
		{
			SetHpOneInfo(false);
		}

		// 描画フラグがtrueの時に描画
		if (draw.onDraw)
		{
			for (int a = 0;a < draw.addLevel;a++)
			{
				float scale = draw.scale;
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)draw.alpha);
				if (draw.onAddDraw)
				{
					SetDrawBlendMode(DX_BLENDMODE_ADD, (int)draw.add);
					scale += (a * draw.addScale);
				}
				DrawRectRotaGraphF(drawPos.x + UI_HP_SIZE.x / 2, drawPos.y + UI_HP_SIZE.y / 2, 0, 0, (int)UI_HP_SIZE.x, (int)UI_HP_SIZE.y, scale, 0.0f, hp.image[hImageIndex], true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	}
	hp.changeState = HP_CHANGE_STATE::CHANGE;

	//__ GAUGE __//
	float gValue = 100 * (gravityMoveGaugeNum / GRAVITY_GAUGE_MAX);	// 重力ゲージ%
	Rgb color = 0xffffff;

	if (noGravity)
		SetDrawBright(100, 100, 100);

	for (int i = 0;i < (int)GRAVITY_GAUGE::MAX;i++)
	{
		switch (i)
		{
		case (int)GRAVITY_GAUGE::BASE:

			DrawRectRotaGraphF(GetDrawPosition(gGaugeUiData[i].position).x, GetDrawPosition(gGaugeUiData[i].position).y, 0, 0, (int)gGaugeUiData[i].size.x, (int)gGaugeUiData[i].size.y, 1, 0, gGaugeUiData[i].hImage, true, false);
			break;
		case (int)GRAVITY_GAUGE::BASE_IN:

			// イージングを使った色付き点滅
			if (IsWaringGravityValue(WARNING_GRAVITY_VALUE_KIND::GAUGE))
			{
				float rate = 20.0f * DegToRad;
				gGaugeUiData[i].time += rate;
				gGaugeUiData[i].brightColor.SetColor(0, (int)EaseIn(0.0f, 255.0f, sinf(gGaugeUiData[i].time)), 0);
			}
			else
			{
				gGaugeUiData[i].brightColor.SetColor(255, 255, 255);
				gGaugeUiData[i].time = 0.0f;
			}
			color = gGaugeUiData[i].brightColor;

			if (!inGoal && !noGravity)
			{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)EaseIn(0.0f, 255.0f, sinf(gGaugeUiData[i].time)));
				SetDrawBright(color.r, color.g, color.b);
				DrawRectRotaGraphF(GetDrawPosition(gGaugeUiData[i].position).x, GetDrawPosition(gGaugeUiData[i].position).y, 0, 0, (int)gGaugeUiData[i].size.x, (int)gGaugeUiData[i].size.y, 1, 0, gGaugeUiData[i].hImage, true, false);
				SetDrawBright(255, 255, 255);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			break;
		case (int)GRAVITY_GAUGE::CENTER:

			DrawRectRotaGraphF(GetDrawPosition(gGaugeUiData[i].position).x, GetDrawPosition(gGaugeUiData[i].position).y, 0, 0, (int)gGaugeUiData[i].size.x, (int)gGaugeUiData[i].size.y, 1, 0, gGaugeUiData[i].hImage, true, false);
			break;
		case (int)GRAVITY_GAUGE::GAUGE:

			// 加算合成
			bool bre = false;
			for (int b = 0;b < 15 && !bre;b++)
			{
				float div = 50;
				float init = 1.0f;	// 初期の大きさ
				
				if (gravityMoveGaugeNumInfo && !noGravity)
				{
					if (gravityMoveGaugeNumMax)
					{
						gravityMoveGaugeNumCount--;
						SetDrawBright(255, 0, 150);
						int add = gravityMoveGaugeNumCount < 10 ? add = 10 : add = gravityMoveGaugeNumCount;
						init = 1.0f - (5 / div);
						SetDrawBlendMode(DX_BLENDMODE_ADD, add);

						if (gravityMoveGaugeNumCount < 10)
							gravityMoveGaugeNumMax = false;
					}
					else
					{
						gravityMoveGaugeNumCount++;
						int alpha = gravityMoveGaugeNumCount > 255 ? alpha = 255 : alpha = gravityMoveGaugeNumCount;
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
						
						if (gravityMoveGaugeNumCount > 255)
							gravityMoveGaugeNumInfo = false;
					}
				}
				else
				{
					bre = true;
				}
				DrawCircleGaugeF(GetDrawPosition(gGaugeUiData[i].position).x + 1.05f, GetDrawPosition(gGaugeUiData[i].position).y, gValue, gGaugeUiData[i].hImage, 0.0f, init = gravityMoveGaugeNumMax ? init + (b / div) : 1.0f, 1, 0);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			SetDrawBright(255, 255, 255);
			break;
		}
	}
	
	//__ GAUGE ARROW __//
	SetDrawArrowAngle();
	int cn = 0;
	for (int i = 0;i < (int)GRAVITY_GAUGE_ARROW::MAX;i++)
	{
		for (int j = 0;j < UI_GRAVITY_GAUGE_ARROW_MAX / 2;j++)
		{
			float addBaseRate = 0.2f;
			
			// 重力方向の矢印は加算合成で光らせる
			if (LigthingGravityArrow(gArrowUiData[i].angle[j]) && *playerModeState != Player::PlayerMode::NORMAL_MODE)
			{
				DrawRectRotaGraphF(GetDrawPosition(gArrowUiData[i].position[j]).x, GetDrawPosition(gArrowUiData[i].position[j]).y, 0, 0, (int)gArrowUiData[i].size.x, (int)gArrowUiData[i].size.y, drawRateScale + addBaseRate, gArrowUiData[i].angle[j], gArrowUiData[i].hImage, true, false);
				
				SetDrawBright(arrowUiColorLight.r, arrowUiColorLight.g, arrowUiColorLight.b);
				DrawRectRotaGraphF(GetDrawPosition(gArrowUiData[i].position[j]).x, GetDrawPosition(gArrowUiData[i].position[j]).y, 0, 0, (int)gArrowUiData[i].size.x, (int)gArrowUiData[i].size.y, drawRateScale, gArrowUiData[i].angle[j], gArrowUiData[i].hImage, true, false);
				SetDrawBright(255, 255, 255);
				continue;
			}
			DrawRectRotaGraphF(GetDrawPosition(gArrowUiData[i].position[j]).x, GetDrawPosition(gArrowUiData[i].position[j]).y, 0, 0, (int)gArrowUiData[i].size.x, (int)gArrowUiData[i].size.y, drawRateScaleInit + addBaseRate, gArrowUiData[i].angle[j], gArrowUiData[i].hImage, true, false);
			
			SetDrawBright(arrowUiColorNormal.r, arrowUiColorNormal.g, arrowUiColorNormal.b);
			DrawRectRotaGraphF(GetDrawPosition(gArrowUiData[i].position[j]).x, GetDrawPosition(gArrowUiData[i].position[j]).y, 0, 0, (int)gArrowUiData[i].size.x, (int)gArrowUiData[i].size.y, drawRateScaleInit, gArrowUiData[i].angle[j], gArrowUiData[i].hImage, true, false);
			SetDrawBright(255, 255, 255);
		}
	}
	SetDrawBright(255, 255, 255);

	//__ VISION __//
	for (int k = 0;k < (int)VISION_KIND::MAX;k++)
	{
		if (vision[k].onVision && !inGoal)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)vision[k].alpha);
			SetDrawBright(VISION_COLOR_LEVEL[k].r, VISION_COLOR_LEVEL[k].g, VISION_COLOR_LEVEL[k].b);
			DrawRectRotaGraphF(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.2f, 0.0f, vision[k].hImage, true, false);
			SetDrawBright(255, 255, 255);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}

bool PlayerUi::LigthingGravityArrow(const float& _angle)
{
	float angle = fmodf(_angle, DX_PI_F * 2);
	float copyArrowAngle = allowAngle;

	if (copyArrowAngle < 0)
	{
		copyArrowAngle = (DX_PI_F - fabsf(copyArrowAngle)) + DX_PI_F;
	}

	float angleSize = copyArrowAngle - angle;
	if (fabsf(angleSize) < UI_GRAVITY_GAUGE_MIN_ROT / 2)
	{
		return true;
	}
	return false;
}

bool PlayerUi::LigthingGravityArrow(const float& _angle, int i, int j)
{
	float angle = fmodf(_angle, DX_PI_F * 2);
	float copyArrowAngle = allowAngle;

	if (copyArrowAngle < 0)
	{
		copyArrowAngle = (DX_PI_F - fabsf(copyArrowAngle)) + DX_PI_F;
	}
	gArrowUiData[i].test[j] = copyArrowAngle;
	float angleSize = copyArrowAngle - angle;
	if (fabsf(angleSize) < UI_GRAVITY_GAUGE_MIN_ROT / 2)
	{
		return true;
	}
	return false;
}

void PlayerUi::PlayerGaugeAdd(float _add)
{
	if (gravityMoveGaugeNum < GRAVITY_GAUGE_MAX) 
	{
		gravityMoveGaugeNum += _add;
	}
	else 
	{
		gravityMoveGaugeNum = GRAVITY_GAUGE_MAX;
		GravityMoveGaugeNumInit();
		Shaking(UI_GRAVITY_GAUGE_POSITION, &gGaugeUiGlobalPosition, V2Get(5.0f, 5.0f), UI_MOVE_INFO_KIND::NORMAL, 10);
	}
}

void PlayerUi::PlayerGaugeUse(float _sub)
{
	gravityMoveGaugeNum -= _sub;
	if (gravityMoveGaugeNum <= 0) 
	{
		gravityMoveGaugeNum = 0;
	}
	gravityUse = true;
}

void PlayerUi::SetHp(int _hp)
{
	if (hp.num != _hp)
	{
		if (onChangeHpNum)
		{
			hp.changeState = HP_CHANGE_STATE::SET;
			for (int i = 0;i < 3;i++)
			{
				if (hp.drawData[i].changingState != HP_CHANGING_STATE::NORMAL)
					SetHpChangeInfo(hp.drawData[i].changingState, i);
			}
		}
		onChangeHpNum = true;

		if (hp.num > _hp)
		{
			Shaking(UI_HP_POSITION, &hp.basePosition, V2Get(5.0f, 5.0f), UI_MOVE_INFO_KIND::NORMAL, 30);
		}
	}
	hp.num = _hp;
}

void PlayerUi::GravityGaugeMax()
{
	gravityMoveGaugeNum = GRAVITY_GAUGE_MAX;
}

void PlayerUi::PlayerUiShake()
{
	Shaking(V2Get(0.0f, 0.0f), &uiGlobalPosition, V2Get(5.0f, 5.0f), UI_MOVE_INFO_KIND::NORMAL, 20);
}

void PlayerUi::SetDrawArrowAngle()
{
	
#if 1

	//___ 重力ゲージ矢印UIのアングル設定 switch文を使わず45度ずつ増える性質を利用 ___//
	int state = *playerGravityState - 1;	// 角度のはじめのステートが1なので0にする

	if (setAngle[0] == -1)
	{
		for (int i = 0;i < 15;i++)
		{
			int s = i;
			if (s % 2 != 0)
				continue;
			int index = i > 0 ? i - ((i * 1) / 2) : i;
			setAngle[index] = ((((((int)GravityState::DOWN_LEFT_GRAVITY_START - 1) / 2) - (s / 2))) * UI_GRAVITY_GAUGE_MIN_ROT) - (135 * DegToRad);
		}
	}

	if (state % 2 != 0)
		return;
	allowAngle = ((((((int)GravityState::DOWN_LEFT_GRAVITY_START - 1) / 2) - (state / 2))) * UI_GRAVITY_GAUGE_MIN_ROT) - (135 * DegToRad);
	
#else
	switch (*playerGravityState)
	{
	case DOWN_GRAVITY_START:
		allowAngle = DX_PI_F;	//180 = 45 * 4
		break;

	case DOWN_RIGHT_GRAVITY_START:
		allowAngle = DX_PI_F - DX_PI_F / 4; //135 = 45 * 3
		break;

	case RIGHT_GRAVITY_START:
		allowAngle = DX_PI_F / 2; //90 = 45 * 2
		break;

	case UP_RIGHT_GRAVITY_START:
		allowAngle = DX_PI_F / 4; //45 = 45 * 1
		break;

	case UP_GRAVITY_START:
		allowAngle = 0.0f; //0 = 45 * 0
		break;

	case UP_LEFT_GRAVITY_START:
		allowAngle = -DX_PI_F / 4; //-45 = 45 * -1
		break;

	case LEFT_GRAVITY_START:
		allowAngle = -DX_PI_F / 2; //-90 = 45 * -2
		break;

	case DOWN_LEFT_GRAVITY_START:
		//allowAngle = DX_PI_F - -DX_PI_F / 4;//225 = 45 + 180
 		allowAngle = -DX_PI_F / 4 + -DX_PI_F / 2; //-135 = 45 * -3
		break;

	default:
		break;
	}
#endif // 0
}

void PlayerUi::AloowDraw(VECTOR _screenPosition,int animeNum)
{
	if (!onDraw)
		return;
	allowPos = _screenPosition;
	float DrawAngle = allowAngle - UI_GRAVITY_GAUGE_CHANGE_ROT;
	allowPos.x += cosf(DrawAngle) * 200.0f;
	allowPos.y += sinf(DrawAngle) * 200.0f;
	animeCounter += animeNum;
	int anime = animeCounter / 64 % 3;
	DrawRectRotaGraphF(allowPos.x, allowPos.y, anime * 100, 0, 100, 150, 1.0f, allowAngle, allowImage, true);
}




