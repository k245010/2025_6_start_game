#include "selectUi.h"
#include "function.h"
#include "loadManager.h"
#include <string>
#include "../ImGui/imgui.h"
#include "../Library/sceneManager.h"

namespace
{
	const VECTOR2F FONT_STAR_SIZE = V2Get(1.0f, 1.0f);
	const VECTOR2F FONT_COIN_SIZE = V2Get(1.0f, 1.0f);
	const VECTOR2F FONT_TIME_SIZE = V2Get(1.0f, 1.0f);
	const float RATE_SPEED = 0.03f;
	const float PICTURE_DEFAULT_SCALE = 800.0f;
	const float PICTURE_BIG_SCALE = 900.0f;
	const float PICTURE_CHANGE_SCALE = 10.0f;
	const VECTOR FONT_CHANGE_POS = VGet(-200.0f, 400.0f, 0.0f);
	const float FONT_DISTANCE = 148.0f;
	const float SCORE_DISTANCE = 94.0f;
	const int COLOR_DOWN_LEVEL = (int)((255.0f / 100) * 
	/* Lv-> */  70);// セレクトのオブジェクトの色が暗くなるレベル %
}

SelectUi::SelectUi(SceneBase* _scene) : UiSelectBase(_scene)
{
	back.hImage = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("OBJ_BACK");
	back.scale = 1.0f;
	back.copyScale = back.scale;
	back.angle = 0.0f;

	pic.image = -1;
	pic.scale = PICTURE_DEFAULT_SCALE;

	rate = 0.0f;
	selectIndex = 0;
	meIndex = 0;

	scoreFontImage[TIME_STAR] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("RESULT_4");
	scoreFontImage[COIN_STAR] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("RESULT_3");
	scoreFontImage[CLEAR_STAR] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("RESULT_2");

	scoreImage[COIN_STAR] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("NUMBER_TEXT");
	scoreImage[TIME_STAR] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("TIME_NUM");
	scoreImage[CLEAR_STAR] = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("STAR");

	stage.position = V2Get(0.0f, 0.0f);
	stage.scale = 1.8f;
	stage.time = 0.0f;
	stage.image = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("STAGE_FONT");
	stage.color = Rgb(255, 0, 255);
	distance = SCORE_DISTANCE;

	stageFontImage = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("NUMBER_TEXT_1");
		
	coinMax = -1;

	for (int s = 0;s < 3;s++)
		data.starNum[s] = 0;

	data.bestTime = 0.0f;
	data.coinNum = 0;
	clear = false;

	for (int i = 0;i < 3;i++)
	{
		SelectData data;
		data.color = RGB_WHITE;
		select.emplace_back(data);
	}

	selecting = false;
}

SelectUi::~SelectUi()
{
}

void SelectUi::Init(const int& _index, const int& frameImage)
{
	meIndex = _index;

	std::string str = "SELECT_PICTURE_";
	str = str + std::to_string(_index + 1);
	
	pic.image = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle(str);

	if (pic.image < 0)
		pic.image = SceneManager::CommonScene()->FindGameObject<LoadManager>()->GetHandle("SELECT_PICTURE");

	pic.frameImage = frameImage;
}

void SelectUi::Update()
{
	distance = SCORE_DISTANCE;

	// thisが選択されていたら
	if (selecting)
	{
		rate += RATE_SPEED;
		back.scale = EaseIn(1.0f, 2.0f, sinf(rate));

		pic.scale += PICTURE_CHANGE_SCALE;
		if (pic.scale >= PICTURE_BIG_SCALE)
			pic.scale = PICTURE_BIG_SCALE;
	}
	// thisが選択されていなかったら
	else
	{
		if (back.scale > 1.0f)
		{
			back.scale -= RATE_SPEED;
		}
		else
		{
			back.scale = 0.0f;
		}

		pic.scale -= PICTURE_CHANGE_SCALE;
		if (pic.scale <= PICTURE_DEFAULT_SCALE)
			pic.scale = PICTURE_DEFAULT_SCALE;
	}
}

void SelectUi::Draw(const VECTOR& pos, const int& _selectIndex)
{
	selectIndex = _selectIndex;
	selecting = (meIndex == _selectIndex);

	VECTOR baseDrawPos = ConvWorldPosToScreenPos(pos);

	VECTOR2F scoreAddPos = selecting ? V2Get(-40.0f,-160.0f) : V2Get(-40.0f,-150.0f);
	VECTOR2F drawPos = V2Get(baseDrawPos.x + scoreAddPos.x, baseDrawPos.y + scoreAddPos.y);
	VECTOR framePos = VGet(pos.x + 0.0f, pos.y - 14.0f, pos.z);

	// ステージ画像裏のフレーム
	DrawBillboard3D(framePos, 0.5f, 0.5f, pic.scale * 1.15f, 0.0f, pic.frameImage, true);
	// ステージ画像
	DrawBillboard3D(pos, 0.5f, 0.5f, pic.scale, 0.0f, pic.image, true);

	float subDis = 0.0f;
	for (int i = 0;i < 3;i++)
	{
		select[i].position = V2Get(drawPos.x - 100.0f, drawPos.y - ((int)StarType::STAR_NUM - (i + 1) * distance) + 328.0f);
		float scoreNum = 0.0f; 
		float fontScale = 1.5f;

		if (selecting)
		{
			switch (i)
			{
			case StarType::CLEAR_STAR:

				subDis = 7.9f;

				// 下のfor文で回すと、星が隠れる
				for (int s = 0;s < 3;s++)
				{
					// 星裏の枠
					DrawRectRotaGraphF(select[i].position.x + 423.0f, select[i].position.y + ((distance - subDis) * s), 0, 0, 128, 128, 0.75f, 0.0f, scoreImage[i], true);
				}

				for (int s = 0;s < 3;s++)
				{
					if (data.starNum[s])
					{
						float dis = (distance - subDis) * s;
						// 星 
						DrawRectRotaGraphF(select[i].position.x + 420.0f, select[i].position.y + dis, 128, 0, 128, 128, 0.75f, 0.0f, scoreImage[i], true);
					}
				}
				break;
			case StarType::COIN_STAR:

				scoreNum = (float)data.coinNum;
				
				if (scoreNum >= coinMax && coinMax > 0)
					SetDrawBright(0, 255, 0);
				// コイン
				DrawRotaNum(select[i].position.x + 320.0f, select[i].position.y - 10.0f, scoreNum, 48, scoreImage[i], 60, 65, 0.9f, 0.0f,2);
				SetDrawBright(255, 255, 255);
				break;
			case StarType::TIME_STAR:

				if (clear)
					select[i].color = RGB_GREEN;
				// タイム
				DrawTimeAsImageM(V2Get(select[i].position.x + 320.0f,select[i].position.y - 2.0f), data.bestTime, 48, scoreImage[i], 0.9f, V2Get(60, 56), V2Get(60, 64), V2Get(145, 16), V2Get(15, 31), V2Get(161, 35), V2Get(18, 20));
				break;
			}

			VECTOR fontPos = VGet(pos.x - 175.0f, pos.y - (FONT_DISTANCE * (((int)StarType::STAR_NUM - i)) + 850.0f * 1.0f),-400.0f);
			fontPos += FONT_CHANGE_POS;
			float picScale = pic.scale;

			if (i == StarType::CLEAR_STAR)
			{
				picScale = PICTURE_DEFAULT_SCALE * 2.45f;
				fontPos.x += 135.0f;
				fontPos.y = pos.y - ((FONT_DISTANCE * (4) + 850.0f * 1.0f) - (FONT_DISTANCE * (1) + 905.0f * 1.0f) + 60.0f);
			}
			// スコア文字
			DrawBillboard3D(fontPos, 0.5f, 0.5f, picScale * 0.25f, 0.0f, scoreFontImage[i], true);
		}

		VECTOR2F stageAddPos = selecting ? V2Get(-40.0f, -250.0f) : V2Get(-40.0f, -180.0f);
		float stageFontScale = 2.0f;
		
		SetDrawBright(stage.color.r, stage.color.g, stage.color.b);
		// ステージ文字
		DrawRotaGraphF(baseDrawPos.x + stageAddPos.x, baseDrawPos.y + stageAddPos.y, stage.scale, 0.0f, stage.image, true);
		// ステージ数字
		DrawRotaNum(baseDrawPos.x + stageAddPos.x + 220.0f, baseDrawPos.y + stageAddPos.y, meIndex + 1, 47, stageFontImage, 39, 45, 1.6f, 0.0f, 1);
		SetDrawBright(255, 255, 255);
	}
}



