#include "selectScene.h"
#include "function.h"
#include "stageData.h"
#include "CsvReader.h"
#include "loadManager.h"

#include "../Library/time.h"
#include "../ImGui/imgui.h"
#include "productionh.h"
#include "selectUi.h"
#include <set>
#include "../Library/sceneManager.h"

namespace { 
	
	const float INPUT_WAIT_TIME = 10.0f;
	const float STAGE_POP_POS = 400.0f;
	const float LERP_SPEED = 0.03f;
	const float SELECT_PICTURE_DISTANCE = 1200.0f;

	//__ ステージ選択 __//
	const int subSeLv = 1;								// 配列番号とステージ数を合わせる
	const int DEFAULT_SELECT_MAX_LEVEL = 17 - subSeLv;	// デフォルトで選択できる最大ステージ数
	const int OVER_SELECT_MIN_LEVEL = 17 - subSeLv;		// 特定のキーを選択した際に、選択できるDEFAULT_SELECT_MAX_LEVELを除いた最小ステージ数
	const int OVER_SELECT_MAX_LEVEL = 30 - subSeLv;		// 特定のキーを選択した際に、選択できる最大ステージ数
	
	const std::vector<std::vector<int>> STAGE_PUT_DATA	// ステージオブジェクトの配置 数値 - 1がステージ数
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 2, 0, 0, 0, 0, 0, 0, 0,10, 0, 0,13, 0, 0, 0, 0, 0, 0, 0, 0,22,23,24,25,26,27,28,29,30},
		{1, 0, 3, 0, 0, 0, 7, 8, 9, 0,11,12, 0,14, 0, 0, 0, 0, 0,20,21, 0},
		{0, 0, 0, 4, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0,18,19,},
		{0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,16,17, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	};
	const int STAGE_PUT_DATA_INDEX_INFO_NUM = -1;		// STAGE_PUT_DATAを使った計算時にこの定数を加算し0を-1として計算する
	
	//__ 背景 __//
	const VECTOR2F BACKGROUND_IMAGE_SIZE = V2Get(8470.0f, (float)SCREEN_HEIGHT);
	const float BACKGROUND_IMAGE_POS_X_START = BACKGROUND_IMAGE_SIZE.x / 2;
	const float BACKGROUND_IMAGE_POS_X_END = BACKGROUND_IMAGE_SIZE.x / 2;
	// 背景が一度に動く距離
	const float BACKGROUND_IMAGE_MOVE_POS_LEVEL = (BACKGROUND_IMAGE_SIZE.x - SCREEN_WIDTH) / DEFAULT_SELECT_MAX_LEVEL;

	//__ スコア __//
	const int BEST_COIN_INDEX = 0;
	const int BEST_TIME_INDEX = 1;
}
SelectScene::SelectScene()
{
	CommonFind* commonFind = SceneManager::CommonScene()->FindGameObject<CommonFind>();
	input = commonFind->CommonFindObject<InputManager>();
	commonData = commonFind->CommonFindObject<CommonData>();
	stageData = commonFind->CommonFindObject<StageData>();
	pro = commonFind->CommonFindObject<Production>();

	camera = CreateGameObject<SelectCamera>();
	CreateGameObject<Light>();


	sound = SceneManager::CommonScene()->FindGameObject<SoundManager>();
	load = commonFind->CommonFindObject<LoadManager>();

	sound->PlayBgm("セレクト音楽");

	backImage.image = load->GetHandle("SELECT_BACKGROUND");
	GraphFilter(backImage.image, DX_GRAPH_FILTER_GAUSS, 16, 140);
	backImage.scale = 1.05f;
	backImage.position = V2Get(BACKGROUND_IMAGE_POS_X_START,(float)SCREEN_HEIGHT_CENTER);
	backImage.startPos = V2Get(0.0f, 0.0f);
	backImage.endPos = V2Get(0.0f, 0.0f);

	int frameImage = load->GetHandle("SELECT_PICTURE_FRAME");
	std::set<int> searchMultNum;
	int creatCn = 0;
	for (int i = 0; i < stage::MAX_STAGE; i++) {

		for (int y = 0;y < STAGE_PUT_DATA.size();y++)
		{
			for (int x = 0;x < STAGE_PUT_DATA[y].size();x++)
			{
				int putDataIndex = STAGE_PUT_DATA[y][x] + STAGE_PUT_DATA_INDEX_INFO_NUM;

				if (putDataIndex > -1 && i == 0)
				{
					if (searchMultNum.insert(putDataIndex).second == false)
					{
						CreateMessageBox("STAGE_PUT_DATAに１以上の値で同じものを初期化で来ません", "selectSceneのコンストラクタでエラー");
					}
				}

				if (putDataIndex == i)
				{
					int sub = (int)STAGE_PUT_DATA.size() / 2;
					SelectData s = { {VECTOR3(800.0f + (x * SELECT_PICTURE_DISTANCE),SELECT_PICTURE_DISTANCE * -(y - sub) + SELECT_PICTURE_DISTANCE,100.0f)}, {RGB_WHITE} };
					select.emplace_back(s);
					SelectUi* uiObj = CreateGameObject<SelectUi>();
					ui.emplace_back(uiObj);
					ui[i]->Init(i,frameImage);
					creatCn++;
				}
			}
		}
	}

	CsvReader& csv = stageData->csvReader;
	csv.Load("data\\score\\score.csv");
	for (int h = 0;h < csv.GetLines();h++)
	{
		std::vector<float> line;
		for (int w = 0;w < csv.GetColums(h);w++)
		{
			float c = csv.GetFloat(h, w);
			line.push_back(c);
		}
		score.push_back(line);
	}

	//__ 全てのステージが作られたか __//
	if (stage::MAX_STAGE > creatCn)
	{
		
		CreateMessageBox("いくつかのステージが作成されませんでした", "selectSceneのコンストラクタでエラー");
	}
	
	isLerp = false;
	selectIndex = commonData->GetData().stage;
	onSelect = false;
	selectLerp = false;
	lerpVelocity = 0.0f;
	inputWaitTime = 0.0f;

	selectLevel = DEFAULT_SELECT_MAX_LEVEL;

	
	InitPosition(selectIndex,true);
	
	state = SELECTSCENE_STATE::READY;
	if (commonData->GetData().clear)
	{
		clearedState = CLEARED_SELECT_STATE::CLEAR;
		commonData->SetClear(false);
	}
	else
	{
		clearedState = CLEARED_SELECT_STATE::NO_CLEAR;
	}
	stateWaitTime = 0.0f;

	stageFontColor[0] = 255.0f;
	stageFontColor[1] = 0.0f;
	stageFontColor[2] = 255.0f;
	isDebug = false;
}

SelectScene::~SelectScene()
{
	select.clear();

}

void SelectScene::Draw()
{
	SceneBase::Draw();

	DrawRotaGraphF(backImage.position.x, backImage.position.y, backImage.scale,0.0f, backImage.image, true,false);

#if _DEBUG
	DrawFormatString(100, 100, 0xffffff, "ステージ[%d]", selectIndex);
	DrawFormatString(100, 130, 0xffffff, "右へ移動[RIGHT]");
	DrawFormatString(100, 160, 0xffffff, "左へ移動[RIGHT]");
	DrawFormatString(800, 190, 0xffffff, "%f", Time::DeltaTime());
	DrawFormatString(800, 220, 0xffffff, "%f", GetFPS());
	Vector2Draw(V2Get(800, 300), backImage.position, nullptr);

	

#endif // _DEBUG

	for (int i = 0; i < stageData->csvReader.GetLines(); i++) {
		
		float bestCoin = score[i][BEST_COIN_INDEX];
		float bestTime = score[i][BEST_TIME_INDEX];
		
		if (bestTime > 0.0f)
		{
			ui[i]->Clear();
			ui[i]->SetBestTime(bestTime);
			ui[i]->SetCoinNum(bestCoin);
			for (int n = 0;n < 3;n++)
			{
				int star = score[i][BEST_TIME_INDEX + 1 + n];
				ui[i]->SetStarNum(star, n);
			}
		}
		ui[i]->Draw(select[i].position, selectIndex);
		ui[i]->SetStageFontColor(Rgb(stageFontColor[0] * 255, stageFontColor[1] * 255, stageFontColor[2] * 255));
	}
}

void SelectScene::Update()
{
#if _DEBUG

	ImGui::Begin("StageFont");
	ImGui::ColorEdit3("color", &stageFontColor[0], 0);
	ImGui::End();

	ImGui::Begin("select");
	ImGui::SliderInt("stage", &selectIndex, 0, stage::MAX_STAGE);
	if (ImGui::Button("start")) {
		if (stageData->LoadStageData(selectIndex)) {
			commonData->SetStage(selectIndex);
			
			commonData->SetStage(selectIndex);
			auto scf = stageData->GetStageConfigData();
			std::string bgt = std::to_string(scf.backGroundData) + "backGround";
			std::string bgd(std::to_string(scf.bgmData) + "bgmStage");
			//int handle = load->GetHandle(bgt);
			int bt = scf.backGroundData / 100;
			if (bt == 1) {
				load->AddLoadData(bgt, std::string(bgt + ".png"), "PlayScene", LoadDataType::TEXTURE);
			}
			else {
				load->AddLoadData(bgt, std::string(bgt + ".mv1"), "PlayScene", LoadDataType::MODEL);
			}
			load->AddLoadData(bgd, std::string(bgd + ".wav"), "PlayScene", LoadDataType::SOUND, 20);

			pro->SceneProductionChange("PlayScene");
		}
	}
	ImGui::End();

	
	if (CheckHitKey(KEY_INPUT_T)) {
		SceneManager::ChangeScene("TitleScene");
	}


#endif // _DEBUG

	SceneBase::Update();

	// selectのオブジェクトの拡大
	for (auto& s : select) {
		if (s == select[selectIndex]) {
			
			s.position.z = -STAGE_POP_POS;
		}
		else {
			
			s.position.z = 0.0f;
		}
	}

	switch (state)
	{
	case SelectScene::SELECTSCENE_STATE::READY:

		switch (clearedState)
		{
		case SelectScene::CLEARED_SELECT_STATE::NO_CLEAR:

			if (stateWaitTime++ > 30)
			{
				state = SELECTSCENE_STATE::SELECT;
				stateWaitTime = 0.0f;
			}
			break;
		case SelectScene::CLEARED_SELECT_STATE::CLEAR:

			if (stateWaitTime++ > 20)
			{
				selectIndex = min(selectIndex + 1, selectLevel);
				SetLerpData(selectIndex);
				commonData->SetStage(selectIndex);
				state = SELECTSCENE_STATE::SELECT;
			}
			break;
		default:
			assert(false);
			break;
		}
		break;
	case SelectScene::SELECTSCENE_STATE::SELECT:

		// 左右切り替え
		if (inputWaitTime <= 0.0f) {
			if (input->GetIsKeyOrButtonPutNow(KEY_INPUT_RIGHT, XINPUT_BUTTON_DPAD_RIGHT) ||
				input->GetStickInput().leftStick.x >= 1.0f || input->GetStickKnocking(0.9f).leftStick == S_UP_RIGHT || input->GetStickKnocking(0.9f).leftStick == S_DOWN_RIGHT) {
				inputWaitTime = INPUT_WAIT_TIME;

				if (selectIndex < selectLevel) 
				{
					SetLerpData(min(selectIndex + 1, selectLevel));

					if (selectIndex >= DEFAULT_SELECT_MAX_LEVEL && selectIndex < OVER_SELECT_MIN_LEVEL)
					{
						selectIndex = OVER_SELECT_MIN_LEVEL;
					}
					else
					{
						selectIndex++;
					}
					sound->PlaySe("セレクト");
				}
			}
			if (input->GetIsKeyOrButtonPutNow(KEY_INPUT_LEFT, XINPUT_BUTTON_DPAD_LEFT) ||
				input->GetStickInput().leftStick.x <= -1.0f || input->GetStickKnocking(0.9f).leftStick == S_UP_LEFT || input->GetStickKnocking(0.9f).leftStick == S_DOWN_LEFT) {
				inputWaitTime = INPUT_WAIT_TIME;

				if (selectIndex <= selectLevel)
				{
					int copyIndex = selectIndex;

					if (selectIndex != 0) {

						SetLerpData(selectIndex - 1);
					}

					if (selectIndex > DEFAULT_SELECT_MAX_LEVEL && selectIndex <= OVER_SELECT_MIN_LEVEL)
					{
						selectIndex = DEFAULT_SELECT_MAX_LEVEL;
					}
					else
					{
						selectIndex = max(selectIndex--, 0);
					}
					
					if (copyIndex != selectIndex)
						sound->PlaySe("セレクト");
				}
			}
#if _DEBUG
			if (input->GetIsKeyboardPushing(KEY_INPUT_RCONTROL) || input->GetIsKeyboardPushing(KEY_INPUT_LCONTROL) ||
				input->GetTriggerNow().leftStick >= 0.5f && input->GetTriggerNow().rightStick >= 0.5f)
			{
				selectLevel = OVER_SELECT_MAX_LEVEL;
			}
			else
			{
				selectLevel = DEFAULT_SELECT_MAX_LEVEL;
			}
#endif // _DEBUG
		}
		else {
			inputWaitTime -= 1.0f;
		}


		// 決定
		if (input->GetIsKeyOrButtonPutNow(KEY_INPUT_Z, XINPUT_BUTTON_A)) 
		{
			sound->PlaySe("決定");
			state = SELECTSCENE_STATE::PLAY_IN;
		}
		break;
	case SelectScene::SELECTSCENE_STATE::PLAY_IN:

		if (stageData->LoadStageData(selectIndex)) 
		{
			commonData->SetStage(selectIndex);
			auto scf = stageData->GetStageConfigData();
			std::string bgt = std::to_string(scf.backGroundData) + "backGround";
			std::string bgd(std::to_string(scf.bgmData) + "bgmStage");
			//int handle = load->GetHandle(bgt);
			int bt = scf.backGroundData / 100;
			if (bt == 1) {
				load->AddLoadData(bgt, std::string(bgt + ".png"), "PlayScene", LoadDataType::TEXTURE);
			}
			else {
				load->AddLoadData(bgt, std::string("sky/" + bgt + ".mv1"), "PlayScene", LoadDataType::MODEL);
			}
			load->AddLoadData(bgd, std::string(bgd + ".wav"), "PlayScene", LoadDataType::SOUND, 1);

			pro->SceneProductionChange("PlayScene");
			state = SELECTSCENE_STATE::END;
		}
		break;
	default:
		break;
	}
	
	// 更新関数内
	if (isLerp) {
		selectLerp += LERP_SPEED;
		if (selectLerp > 1.0f) {
			selectLerp = 1.0f;
			isLerp = false;
		}
		EaseUpdate();
	}
}

void SelectScene::EaseUpdate()
{
	VECTOR3 cameraPos;
	//cameraPos = SmoothStep<VECTOR3>(beforePos, lerpVelocity, selectLerp);
	cameraPos = EaseInOut<VECTOR3>(beforePos, lerpVelocity, selectLerp);
	camera->SetPosition(cameraPos);
	camera->SetTarget(VAdd(cameraPos, VGet(0.0f, 0.0f, 50.0f)));

	// backImage
	backImage.position = EaseInOut(backImage.startPos, backImage.endPos,selectLerp);
}

void SelectScene::SetLerpData(const int& changeIndex)
{
	isLerp = true;
	selectLerp = 0.0f; // 補間開始

	InitPosition(changeIndex, !isLerp);
}

void SelectScene::SetLerpData(VECTOR3 selectVelocity)
{
	isLerp = true;
	selectLerp = 0.0f; // 補間開始
	lerpVelocity= VGet(selectVelocity.x,selectVelocity.y, (selectVelocity.z) + CAMERA_Z_POS);
	beforePos = camera->GetPosition();

}


void SelectScene::InitPosition(const int& changeIndex,bool set)
{
	lerpVelocity = select[changeIndex].position;
	lerpVelocity.z = CAMERA_Z_POS;

	beforePos = camera->GetPosition();

	// backImage
	if (changeIndex != 0)
	{
		backImage.endPos.x = -changeIndex * BACKGROUND_IMAGE_MOVE_POS_LEVEL;
		backImage.endPos.x += BACKGROUND_IMAGE_POS_X_START;
	}
	else
	{
		backImage.endPos.x = BACKGROUND_IMAGE_POS_X_START;
	}
	backImage.endPos.y = backImage.position.y;
	backImage.startPos = backImage.position;

	// 初期化する場合
	if (set)
	{
		camera->SetPosition(lerpVelocity);
		camera->SetTarget(VAdd(lerpVelocity, VGet(0.0f, 0.0f, 50.0f)));
		backImage.position = backImage.endPos;
	}
}
