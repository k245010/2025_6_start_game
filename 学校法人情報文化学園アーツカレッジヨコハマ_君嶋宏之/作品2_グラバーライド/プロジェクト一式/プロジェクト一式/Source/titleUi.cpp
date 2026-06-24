#include "titleUi.h"
#include "commonFind.h"
#include "../Library/sceneManager.h"
#include "font.h"
#include "Screen.h"
#include "config.h"
#include "movie.h"
#include "function.h"
#include "../ImGui/imgui.h"
namespace {
	//補完開始地点
	//const VECTOR2F TITLE_BEGIN_POS= VECTOR2F(((float)Screen::WIDTH/2) + 50.0f, -200.0f);
	const VECTOR2F TITLE_BEGIN_POS[] =
	{
		VECTOR2F(50.0f, -200.0f),
		VECTOR2F(-380.0f, -20.0f),
		//VECTOR2F(SCREEN_WIDTH_CENTER,-500.0f),
	};
	//終点
	//const VECTOR2F TITLE_END_POS = VECTOR2F(((float)Screen::WIDTH / 2) + 50.0f, 330.0f);
	const VECTOR2F TITLE_END_POS[] =
	{
		VECTOR2F(((float)Screen::WIDTH / 2) + 30.0f, 370.0f),
		VECTOR2F(((float)Screen::WIDTH / 2) - 430.0f, 350.0f),
	};
	//終点
	const VECTOR2F SELECT_ITEM_END_POS = VECTOR2F(((float)Screen::WIDTH / 2) -260.0f, 1200.0f);
	//補完開始地点
	//const VECTOR2F SELECT_ITEM_BEGIN_POS = VECTOR2F(((float)Screen::WIDTH / 2) - 260.0f, 650.0f);
	const VECTOR2F SELECT_ITEM_BEGIN_POS = VECTOR2F(((float)Screen::WIDTH / 2) - 260.0f, 750.0f);

	const float ADD_RATE = 0.01f; // rate系の変数の加算量
	const float PUSH_FONT_DEFAULT_SCALE = 1.5f * 1.15f;
	const float PUSH_FONT_MIN_SCALE = 1.0f;
	const float PUSH_FONT_STATE_WAIT_TIME = 20.0f;
}

TitleUi::TitleUi(SceneBase* _scene):UiBase(_scene)
{
	for (int i = 0;i < (int)TITLE_KIND::MAX;i++)
	{
		title[i].hImage = -1;
	}
	font = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<Font>();
	movie = _scene->FindGameObject<Movie>();
	Init();
}

TitleUi::~TitleUi()
{
	for (int i = 0;i < (int)TITLE_KIND::MAX;i++)
	{
		DeleteGraph(title[i].hImage);
	}
	DeleteGraph(titleSelectImage);
}
void TitleUi::Init()
{
	for (int i = 0;i < (int)TITLE_KIND::MAX;i++)
	{
		title[i].position = TITLE_BEGIN_POS[i];
		title[i].angle = 0.0f;
		title[i].rate = 0.0f;
	}
	title[(int)TITLE_KIND::FONT].scale = 0.75f;
	
	BackCircleInit();
	
	/*titlePosition[(int)TITLE_KIND::FONT] = TITLE_BEGIN_POS[(int)TITLE_KIND::FONT];
	titlePosition[(int)TITLE_KIND::BACK_CIRCLE] = TITLE_BEGIN_POS[(int)TITLE_KIND::BACK_CIRCLE];*/

	//titlePosition[(int)TITLE_KIND::BACK_CIRCLE] = TITLE_END_POS;

	titleUiState = TitleUiState::BEGIN;
	beginProRate = 0.0f;
	selectItemPosition = SELECT_ITEM_BEGIN_POS;
	itemSelectScaleF = 1.5f;
	itemSelectWaitTime = 0.0f;

	beginState = BeginState::TITLE;
	endState = END_STATE::SMALL;
	for (int i = 0; i != ItemSelectRate::MAX; i++) {
		itemSelectRate.emplace_back(0.0f);
	}
	itemSelectBlendValue = 0;
	itemSelectScale = 1.5f;
	goProEnd = false;
	for (int i = 0;i < (int)TITLE_KIND::MAX;i++)
	{
		switch (i)
		{
		case (int)TITLE_KIND::FONT:

			title[i].hImage = LoadGraph("data\\texture\\title_letter.png");
			break;
		case (int)TITLE_KIND::BACK_CIRCLE:

			title[i].hImage = LoadGraph("data\\texture\\title_back.png");
			break;
		}
	}
	titleSelectImage = LoadGraph("data\\texture\\push.png");
}

void TitleUi::BackCircleInit()
{
	title[(int)TITLE_KIND::BACK_CIRCLE].scale = 0.0f;
	title[(int)TITLE_KIND::BACK_CIRCLE].angle = 0.0f;
	title[(int)TITLE_KIND::BACK_CIRCLE].rate = 0.0f;
}

void TitleUi::Update()
{
#ifdef _DEBUG
	ImGui::Begin("title");
	if (ImGui::SmallButton("Restart")) {
		Init();
	}
	ImGui::DragFloat2("title", &title[(int)TITLE_KIND::FONT].position.x);
	ImGui::DragFloat2("titleBack", &title[(int)TITLE_KIND::BACK_CIRCLE].position.x);
	ImGui::DragFloat2("select", &selectItemPosition.x);
	ImGui::DragFloat2("selectScale", &itemSelectScale.x);
	ImGui::End();
#endif // _DEBUG

	
	switch (titleUiState)
	{
	case TitleUi::TitleUiState::BEGIN:
		switch (beginState)
		{
		case TitleUi::BeginState::TITLE:
			if (beginProRate < 1.0f) {
				beginProRate += ADD_RATE;
				for (int i = 0;i < (int)TITLE_KIND::MAX;i++)
				{
					title[i].position = EaseOutElastic(TITLE_BEGIN_POS[i], TITLE_END_POS[i], beginProRate);
					//selectItemPosition= EaseOutBack(SELECT_ITEM_BEGIN_POS, SELECT_ITEM_END_POS, beginProRate);
				}

				//title[(int)TITLE_KIND::BACK_CIRCLE].scale = EaseIn(0.0f, 0.75f, beginProRate);
				//title[(int)TITLE_KIND::BACK_CIRCLE].angle = EaseIn(0.0f, -DX_PI_F, beginProRate);

			}
			else {
				beginProRate = 0.0f;
				beginState = BeginState::ITEMSELECT;
			}
			break;
		case TitleUi::BeginState::ITEMSELECT:
			if (itemSelectRate[BEGIN] < 1.0f) {
				itemSelectRate[BEGIN] += ADD_RATE;
				itemSelectBlendValue = SmoothStep(0, 255, itemSelectRate[BEGIN]);
				
			}
			else {
				titleUiState = TitleUiState::READY;
			}
			break;
		default:
			break;
		}
		break;
	case TitleUi::TitleUiState::READY:
		if (itemSelectRate[READY] < 1.0f) {
			itemSelectRate[READY] += ADD_RATE;
			itemSelectBlendValue = EaseInOut(0, 255, itemSelectRate[READY]);
		}
		else {
			itemSelectRate[READY] = 0.0f;
		}
	
		break;
	case TitleUiState::GO:

		switch (endState)
		{
		case END_STATE::SMALL:

			itemSelectScaleF -= 0.1f;
			if (itemSelectScaleF < PUSH_FONT_MIN_SCALE)
			{
				itemSelectScaleF = PUSH_FONT_MIN_SCALE;
				endState = END_STATE::BIG;
			}
			break;
		case END_STATE::BIG:

			itemSelectScaleF += 0.1f;
			if (itemSelectScaleF > PUSH_FONT_DEFAULT_SCALE)
			{
				itemSelectScaleF = PUSH_FONT_DEFAULT_SCALE;

				if (itemSelectWaitTime++ > PUSH_FONT_STATE_WAIT_TIME)
				{
					itemSelectWaitTime = 0.0f;
					endState = END_STATE::END;
				}
			}
			break;
		default:

			if (itemSelectRate[GOMOVE] < 1.0f) {
				itemSelectRate[GOMOVE] += ADD_RATE * 2.0f;
				selectItemPosition = EaseInBack(SELECT_ITEM_BEGIN_POS, SELECT_ITEM_END_POS, itemSelectRate[GOMOVE]);
			}
			if (beginProRate < 1.0f) {
				beginProRate += ADD_RATE * 2.0f;
				for (int i = 0;i < (int)TITLE_KIND::MAX;i++)
				{
					title[i].position = EaseInBack(TITLE_END_POS[i], TITLE_BEGIN_POS[i], beginProRate);
				}
			}
			else {
				goProEnd = true;
			}
			break;
		}
		break;
	default:
		break;
	}

	//if (beginState != BeginState::TITLE)
	//{
	//	int kind = (int)TITLE_KIND::BACK_CIRCLE;
	//	float& rate = title[kind].rate;
	//	/*if (title[kind].rate == 0.0f)
	//	{
	//		title[kind].angle =
	//	}*/

	//	if (beginState != BeginState::TITLE)
	//	if (rate < 1.0f)
	//	{
	//		rate += 0.01f;

	//		title[kind].scale = EaseInOut(0.75f, 1.2f, rate);
	//		title[kind].angle = EaseInOut(0.0f, -360.0f * DegToRad, rate);
	//	}
	//	else
	//	{
	//		title[kind].rate = 0.0f;
	//	}
	//}

	if (!movie->GetIsDraw())
	{
		BackCircleInit();
	}

	int kind = (int)TITLE_KIND::BACK_CIRCLE;
	float& rate = title[kind].rate;
	maxScale = 1.01f;
	minScale = 0.75f;
	float subScale = 0.05f;//誤差 
	float maxAngle = -360.0f * DegToRad;
	
#if 1
	maxScale = 0.75f;
	minScale = 0.0f;

	if (rate < 1.0f)
	{
		rate += 0.01f;

		title[kind].scale = EaseInOut(minScale, maxScale, rate);
		title[kind].angle = EaseInOut(0.0f, maxAngle, rate);
	}
	
#else
	rate += ADD_RATE;

	// タイトルが動いているときの丸い渦の拡大回転
	if (beginState == BeginState::TITLE)
	{
		maxScale = 0.75f;
		minScale = 0.0f;

		if (rate < 1.0f)
		{
			//rate += 0.01f;

			title[kind].scale = EaseInOut(minScale, maxScale, rate);
			title[kind].angle = EaseInOut(0.0f, maxAngle, rate);
		}
		else
		{
			rate = 0.0f;
		}
		initScale[0] = false;
	}
	else
	{
#if 0
		if (!initScale[0])
		{
			copyScale = title[kind].scale;
			initScale[0] = true;
		}

		title[kind].scale = EaseInOut(minScale, maxScale, sinf(rate));
		title[kind].angle = EaseInOut(0.0f, maxAngle, rate);

		if (!initScale[1] && title[kind].scale - subScale > copyScale)
		{
			title[kind].scale = copyScale;
		}
		else
		{
			initScale[1] = true;
		}
#endif

#if 0
		if (!initScale[0])
		{
			copyScale = title[kind].scale;
			initScale[0] = true;
		}
		title[kind].angle = EaseInOut(0.0f, maxAngle, rate);

#endif
	}
#endif
	/*rate += 0.01f;

	title[kind].scale = EaseInOut(minScale, maxScale, sinf(rate));
	title[kind].angle = EaseInOut(0.0f, maxAngle, rate);*/
}


void TitleUi::Draw()
{
	//font->FontDraw(titlePosition,V2Get(3.0f), FONT_KIND::IKA, RGB_RED,"title");
	for (int i = (int)TITLE_KIND::MAX - 1;i >= 0;i--)
	{
		/*if (i == 1)
			continue;*/
		DrawRotaGraphF(title[i].position.x, title[i].position.y, title[i].scale, title[i].angle, title[i].hImage, true, false);
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, itemSelectBlendValue);
#if 0
	font->FontDraw(selectItemPosition, itemSelectScale, FONT_KIND::IKA, RGB_RED, "Push To Z In Game");
#else
	const VECTOR2I imageSize = V2Get(600, 100);
	DrawRectRotaGraphF(selectItemPosition.x + (float)imageSize.x / 2, selectItemPosition.y + (float)imageSize.y / 2, 0, 0, imageSize.x, imageSize.y, itemSelectScaleF, 0.0f, titleSelectImage, true, false);
#endif
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

bool TitleUi::GetTitleUiStateReady() const
{
	return titleUiState == TitleUiState::READY;
}
