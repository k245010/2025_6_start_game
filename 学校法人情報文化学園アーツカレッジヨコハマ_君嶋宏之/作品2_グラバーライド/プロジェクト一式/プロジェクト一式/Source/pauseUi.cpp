#include "pauseUi.h"
#include "../ImGui/imgui.h"
#include "function.h"
#include "effectManager.h"
#include "debugScreen.h"

namespace
{
	const int INTERVEL_ALLOW = 200;
	const float BUTTON_LONG_DISTANCE = 50.0f;
	const float BUTTON_SHORT_DISTANCE = 30.0f;
	const float BASE_DISTANCE = 50.0f;
	const VECTOR2F BACK_POS = V2Get((float)SCREEN_WIDTH_CENTER, (float)550.0f);
	const VECTOR2F TITLE_POS = V2Get(835.0f, 200.0f);
	const VECTOR2F BUTTON_POS[2] = {
		V2Get(1157.0f, 990.0f),
		V2Get(1317.0f, 990.0f),
	};
	const VECTOR2F SELECT_POS[3] = {
		V2Get((float)850.0f, 460.0f * 1.0f),
		V2Get((float)840.0f, 460.0f * 2.0f),
		V2Get((float)850.0f, 460.0f * 3.0f),
	};
}

PauseUi::PauseUi(SceneBase* _scene) : UiSelectBase(_scene)
{
	
	backPos = V2Get((float)SCREEN_WIDTH_CENTER, (float)550.0f);
	backSize = V2Get((float)SCREEN_WIDTH_CENTER, (float)1000.0f);
	titlePos = V2Get(835.0f, 200.0f);
	titleSize = V2Get(50.0f, 50.0f);
	titleFontSize = V2Get(1.0f, 1.0f);

	for (int i = 0;i < 3;i++)
	{
		SelectData data;
		data.position = SELECT_POS[i];
		data.size = V2Get(100.0f, 100.0f);
		data.fontSize = V2Get(1.5f, 1.5f);
		select.emplace_back(data);
	}

	for (int i = 0; i < 2; i++) 
	{
		ButtonData data;
		data.position = V2Get(1177.0f + (i + 1) * 120.0f, 990.0f);
		data.fontSize = V2Get(1.2f, 1.2f);
		button.emplace_back(data);
	}
	
	distance = 145.0f;
	subDrawFontPos = V2Get(100,0);

	backColor = RGB_BLACK;
	titleColor = RGB_WHITE;
	for (int i = 0;i < 3;i++)
	{
		select[i].color = RGB_WHITE;
	}
	ResetMove();
	//aloow = LoadGraph("data\\texture\\YJ.png");
	gravityManager = _scene->FindGameObject<GravityManager>();
	effectManager = _scene->FindGameObject<EffectManager>();
	load = SceneManager::CommonScene()->FindGameObject<LoadManager>();

	for (int i = 0; i < UI_NUM; i++) {
		std::string c = "PAUSE_";
		c = c + std::to_string(i + 1);
		image.emplace_back(load->GetHandle(c));
	}

	PushReset();
	pushRateIndex = 0;
	
}

PauseUi::~PauseUi()
{
	//DeleteGraph(aloow);
}

void PauseUi::Update()
{
	int keepSelect;
	if (gameManager->GetGameState() == GameManager::MENU)
	{
		switch (state)
		{
		case PauseUi::Wait:
			state = Move;
			EffectManager::AloowEffectData data;
			data = effectManager->GetEffectData(gravityManager, gravityManager->GetWorldGravityState());
			moveInitPosition.x = data.position.x;
			moveInitPosition.y = data.position.y;
			buttonCounter = 0.0f;
			countNum = 0.015f;
			finalMove = false;
			aloowDistance = 50;
			rateIndex = 0;
			break;
		case PauseUi::Move:
			backPos.x = EaseOutBack(moveInitPosition.x, BACK_POS.x, buttonCounter);
			backPos.y = EaseOutBack(moveInitPosition.y, BACK_POS.y, buttonCounter);
			titlePos.x = EaseOutBack(moveInitPosition.x, TITLE_POS.x, buttonCounter);
			titlePos.y = EaseOutBack(moveInitPosition.y, TITLE_POS.y, buttonCounter);
			for (int i = 0; i < 2; i++) {
				button[i].position.x = EaseOutBack(moveInitPosition.x, BUTTON_POS[i].x, buttonCounter);
				button[i].position.y = EaseOutBack(moveInitPosition.y, BUTTON_POS[i].y, buttonCounter);
			}
			for (int i = 0; i < 3; i++) {
				select[i].position.x = EaseOutBack(moveInitPosition.x, SELECT_POS[i].x,buttonCounter);
				select[i].position.y = EaseOutBack(moveInitPosition.y, SELECT_POS[i].y,buttonCounter);
			}
			for (int i = 0; i < 3; i++) {
				if (i == rateIndex) {
					//selectFontSize[i].x = Lerp(1.5f, 1.5f, waitCounter);
					//selectFontSize[i].y = Lerp(1.0f, 1.5f, waitCounter);
				}
				else {
					select[i].fontSize.x = Lerp(1.5f, 1.0f, buttonCounter);
					select[i].fontSize.y = Lerp(1.5f, 1.0f, buttonCounter);
				}
			}
			if (finalMove) {
				buttonCounter -= countNum;
				if (buttonCounter <= 0.0f) {
					//state = None;
					//finalMove = false;
					buttonCounter = 0.0f;
				}
				
				FontScaling(&pushCount, &pushRate, 0.1f, 0.1f);
				button[pushRateIndex].fontSize = EaseIn(1.0f, 1.5f, pushCount);
			}
			else {
				buttonCounter += countNum;
				if (buttonCounter >= 1.0f) {
					waitCounter = 0.0f;
					state = MoveWait;
				}
			}
			
			break;
		case MoveWait:
			waitCounter += 1.0f;
			if (waitCounter >= 1.0f) {
				state = Menu;
				ResetMove();
				gameManager->CursolMoveOn();
			}
			
			break;
		case PauseUi::Menu:
		
			FontScaling(&buttonCounter, &buttonMove, 0.02f, 0.2f);
			keepSelect = rateIndex;
			RateSelect();
			if (rateIndex != keepSelect) {
				ResetMove();
			}
			aloowDistance = Lerp(BUTTON_SHORT_DISTANCE, BUTTON_LONG_DISTANCE, buttonCounter);


			for (int i = 0; i < 3; i++) {
				if (i == rateIndex) {
					select[i].fontSize.x = Lerp(1.8f, 1.5f, buttonCounter);
					select[i].fontSize.y = Lerp(1.8f, 1.5f, buttonCounter);
				}
				else {
					select[i].fontSize = V2Get(1.0f, 1.0f);
				}
			}
			if (gameManager->GetCountStart()) {
				data = effectManager->GetEffectData(gravityManager, gravityManager->GetReverceGravity(gravityManager->GetWorldGravityState()));
				moveInitPosition.x = data.position.x;
				moveInitPosition.y = data.position.y;
				finalMove = true;
				buttonCounter = 1.0f;
				countNum = 1.0f / 30.0f;
				state = Move;
				PushReset();
				PushIndex();
			}
			break;
		default:
			break;
		}
		
		
	}
	else {
		state = Wait;
	}
}

void PauseUi::Draw()
{
	if (gameManager->GetGameState() == GameManager::MENU)
	{
		
		
#ifdef _DEBUG

		ImGui::Begin("PAUSE");
		
		ImGui::SliderFloat2("backPos", &backPos.x, 0.0f, 10000.0f);
		ImGui::SliderFloat2("backSize", &backSize.x, 0.0f, 10000.0f);
		ImGui::SliderInt3("backColor", &backColor.r, 0,255);

		ImGui::SliderFloat2("titlePos", &titlePos.x, 0.0f, 10000.0f);
		ImGui::SliderFloat2("titleSize", &titleSize.x, 0.0f, 10000.0f);
		ImGui::SliderFloat2("titleFontSize", &titleFontSize.x, 0.0f, 10.0f);
		ImGui::SliderInt3("titleColor", &titleColor.r, 0, 255);

		ImGui::SliderFloat2("selectPos", &select[0].position.x, 0.0f, 10000.0f);
		ImGui::SliderFloat2("selectSize", &select[0].size.x, 0.0f, 10000.0f);
		ImGui::SliderFloat2("selectFontSize", &select[0].fontSize.x, 0.0f, 10.0f);
		ImGui::SliderInt3("selectColor", &select[0].color.r, 0, 255);
		
		//ImGui::SliderFloat2("buttonPos", &buttonPos.x, 0.0f, 10000.0f);
		//ImGui::SliderFloat2("buttonSize", &buttonFontSize.x, 0.0f, 10.0f);

		ImGui::SliderFloat("distance", &distance, 0.0f, 10000.0f);
		ImGui::SliderFloat2("subDrawFontPos", &subDrawFontPos.x, 0.0f, 10000.0f);

		ImGui::End();
#endif // DEBUG

		/*selectSize[1] = selectSize[0];
		selectSize[2] = selectSize[0];

		selectPos[1] = V2Get(selectPos[0].x, selectPos[0].y + distance);
		selectPos[2] = V2Get(selectPos[0].x, selectPos[1].y + distance);

		selectFontSize[1] = selectFontSize[0];
		selectFontSize[2] = selectFontSize[1];*/

		for (int i = 0;i < 3;i++)
		{
			//selectSize[i] = selectSize[0];
			//selectFontSize[i] = selectFontSize[0];

			if (i == 0)
				continue;
			int index = max(i - 1, 0);
			select[i].position = V2Get(select[i].position.x, select[index].position.y + distance);
		}

		VECTOR2F drawPos1 = V2Get(backPos.x - backSize.x / 2, backPos.y - backSize.y / 2);
		VECTOR2F drawPos2 = V2Get(backPos.x + backSize.x / 2, backPos.y + backSize.y / 2);
		
		if (state != Move) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawFillBox((int)drawPos1.x, (int)drawPos1.y, (int)drawPos2.x, (int)drawPos2.y, backColor);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		else if (buttonCounter != 0.0f) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			DrawFillBox((int)drawPos1.x, (int)drawPos1.y, (int)drawPos2.x, (int)drawPos2.y, backColor);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		
		

		drawPos1 = V2Get(titlePos.x - titleSize.x / 2, titlePos.y - titleSize.y / 2);
		drawPos2 = V2Get(titlePos.x + titleSize.x / 2, titlePos.y + titleSize.y / 2);
		//DrawFillBox(drawPos1.x, drawPos1.y, drawPos2.x, drawPos2.y,titleColor);

		//SetFontSize(titleSize.x / 5);
		////DrawString(drawPos1.x, drawPos1.y, "タイトル", 0xffffff, true);
		//SetFontSize(DEFAULT_FONT_SIZE);

		for (int i = 0;i < 3;i++)
		{
			drawPos1 = V2Get(select[i].position.x - select[i].size.x / 2, select[i].position.y - select[i].size.y / 2);
			drawPos2 = V2Get(select[i].position.x + select[i].size.x / 2, select[i].position.y + select[i].size.y / 2);
			int color = select[0].color;
			//DrawFillBox(drawPos1.x, drawPos1.y, drawPos2.x, drawPos2.y, color);
		}
	}
	FONT_KIND kind = FONT_KIND::NORMAL;
	float drawFontPosX = subDrawFontPos.x;
	switch (gameManager->GetGameState())
	{
	case GameManager::PLAY_BEFORE:
		break;
	case GameManager::PLAYING:
		break;
	case GameManager::GOAL_FRONT_ANI:
		//DrawFormatString(500, 500, 0xff00000, "GOAL!!!!!!");
		break;
	case GameManager::MENU:
		/*SetFontSize(fontSize);
		drawFontPosX = selectPos[0].x - subDrawFontPos.x;
		DrawFormatString(titlePos.x - subDrawFontPos.x, titlePos.y - subDrawFontPos.y, 0xff00000, "ポーズ");
		DrawFormatString(drawFontPosX, selectPos[0].y - subDrawFontPos.y, 0xff0000, "中間ポイントから");
		DrawFormatString(drawFontPosX, selectPos[1].y - subDrawFontPos.y, 0xff0000, "初めから");
		DrawFormatString(drawFontPosX, selectPos[2].y - subDrawFontPos.y, 0xff0000, "セレクトシーンへ");
		DrawFormatString(drawFontPosX - 100, (selectPos[0].y + distance * (gameManager->GetPauseNum() - 1)) - subDrawFontPos.y, 0xff0000, "→");
		DrawFormatString(drawFontPosX * 1.9f, selectPos[2].y + distance, 0xff0000, "A : 決定　B : 戻る");
		SetFontSize(DEFAULT_FONT_SIZE);*/

		kind = FONT_KIND::POP;
		drawFontPosX = select[0].position.x - subDrawFontPos.x;
		/*font->FontDraw(V2Get(titlePos.x - subDrawFontPos.x, titlePos.y - subDrawFontPos.y), titleFontSize, 0.0f, kind, RGB_RED, "ポーズ");
		font->FontDraw(V2Get(drawFontPosX, select[0].position.y - subDrawFontPos.y), select[0].fontSize, 0.0f, kind, RGB_RED, "中間ポイントから");
		font->FontDraw(V2Get(drawFontPosX, select[1].position.y - subDrawFontPos.y), select[1].fontSize, 0.0f, kind, RGB_RED, "初めから");
		font->FontDraw(V2Get(drawFontPosX, select[2].position.y - subDrawFontPos.y), select[2].fontSize, 0.0f, kind, RGB_RED, "セレクトシーンへ");
		font->FontDraw(V2Get(drawFontPosX - aloowDistance, (select[0].position.y + distance * rateIndex) - subDrawFontPos.y), V2Get(1.5f,1.5f), 0.0f, kind, RGB_RED, "→");
		font->FontDraw(button[0].position, button[0].fontSize, 0.0f, kind, RGB_RED, "A:決定");
		font->FontDraw(button[1].position, button[1].fontSize, 0.0f, kind, RGB_RED, "B:戻る");*/
		UIDraw(V2Get(titlePos.x - subDrawFontPos.x, titlePos.y - subDrawFontPos.y), titleFontSize, 0.0f, image[0]);
		UIDraw(V2Get(drawFontPosX, select[0].position.y - subDrawFontPos.y), select[0].fontSize, 0.0f,image[1]);
		UIDraw(V2Get(select[1].position.x - subDrawFontPos.x, select[1].position.y - subDrawFontPos.y), select[1].fontSize, 0.0f,image[2]);
		UIDraw(V2Get(drawFontPosX, select[2].position.y - subDrawFontPos.y), select[2].fontSize, 0.0f,image[3]);
		UIDraw(V2Get(drawFontPosX - BASE_DISTANCE - aloowDistance, (select[0].position.y + distance * rateIndex) - subDrawFontPos.y + 20.0f), V2Get(0.8f, 0.8f), 0.0f,image[4]);
		UIDraw(button[0].position, button[0].fontSize, 0.0f,image[5]);
		UIDraw(button[1].position, button[1].fontSize, 0.0f,image[6]);
		break;
	case GameManager::LOSE:
		break;
	case GameManager::PLAY_AFTER:
		//printfDx("222");
		break;
	case GameManager::SCENE_CHANGE:
		break;
	}
}

void PauseUi::RateSelect()
{
	rateIndex = (gameManager->GetPauseNum() - 1);
}

void PauseUi::ResetMove()
{
	buttonCounter = 0.0f;
	buttonMove = true;
}

void PauseUi::PushReset()
{
	pushRate = true;;
	pushCount = 0.0f;
}

void PauseUi::PushIndex()
{
	pushRateIndex =  gameManager->GetPushButton();
}

void PauseUi::FontScaling(float* _counter, bool* rate, float _up, float _down)
{
	if (*rate) {
		*_counter += _up;
		if (*_counter >= 1.0f) {
			*_counter = 1.0f;
			*rate = false;
		}
	}
	else {
		*_counter -= _down;
		if (*_counter <= 0.0f) {
			*_counter = 0.0f;
			*rate = true;
		}
	}
}

void PauseUi::UIDraw(VECTOR2F _pos,VECTOR2F _size,float _angle,int _himage)
{
	DrawRotaGraph3F(_pos.x, _pos.y, 0.0f, 0.0f, _size.x, _size.y, _angle, _himage, true, false, false);
}
