#include "GameController.h"
#include "../Common/Input/Input.h"
#include "../Navigation/NavigationManager/NavigationManager.h"
#include "../Stage/StageManager/StageManager.h"
#include "../Stage/StageInfo.h"
#include "../Wave/WaveController/WaveController.h"
#include "../Enemy/EnemyManager/EnemyManager.h"
#include "../Camera/Camera.h"
#include "../Screen.h"
#include "../HoldCoin/HoldCoin.h"
#include "../Stage/Trap/TrapInfo.h"
#include "../Common/Sound/SoundManager.h"
#include "../Common/Sound/SoundID.h"
#include "../Common/UI/UIDrawManager/UIDrawManager.h"
#include "../Common/Function/Function.h"
#include "../Common/Fade/Fade.h"
#include <algorithm>
#include "../../ImGui/imgui.h"

#define TEST_STAGE_OBJECT_PUT_POINT	(1)		// 設置ポイントに応じた罠設置をする
#define USE_PLAYER					(1)		// プレイヤーをゲームで使う
#ifdef _DEBUG
#define DRAW_MOUSEPOINTER			(1)		// マウスポインタの描画
#endif // _DEBUG
#define CAN_PUT_TRAP_IN_DEFENCE		(0)		// 防衛時に罠を置けるかどうか

#define USE_CONTROLL_OLD			(0)		// 古い操作方法を使うかどうか


namespace
{
	constexpr float SLOW_GAME_DEITATIME_RATE				= 0.5f;	// ゲームデルタタイムを遅くする倍率設定値

	const VECTOR3 TEST_OVER_LOOK_POS						= VECTOR3(5000.0f, 3500.0f, -6000.0f);	// 俯瞰視点座標
	const VECTOR3 TEST_OVER_LOOK_TARGET_POS					= VECTOR3(1000.0f, -1950.0f, -800.0f);	// 俯瞰視点ターゲット座標
	constexpr float DEFAULT_CAMERA_TARGET_TO_POS_DISTANCE	= 2000.0f;								// 初期のカメラのターゲット(注視点)からカメラの座標までの距離 
	constexpr float MIN_CAMERA_TARGET_TO_POS_DISTANCE		= 10.0f;								// カメラのターゲット(注視点)からカメラの座標までの最小距離 
	constexpr float MAX_CAMERA_TARGET_TO_POS_DISTANCE		= 3000.0f;								// カメラのターゲット(注視点)からカメラの座標までの最大距離 
#if USE_CONTROLL_OLD
	constexpr float CAMERA_TARGET_TO_POS_MOVE_SPEED			= 10.0f;								// カメラのターゲット(注視点)からカメラの座標まで移動する速度 
#else
	constexpr float CAMERA_TARGET_TO_POS_MOVE_SPEED			= 50.0f;								// カメラのターゲット(注視点)からカメラの座標まで移動する速度 
#endif
	constexpr float TRAP_DELETE_TURN_COST_RATE				= 1.0f;	// 罠の取り壊しをした時に戻ってくるコスト率 0.0f～1.0fで0%～100%を表している　
}

GameController::GameController()
{
	gameUIImage[GAM_DEFEAT_CORE]			= LoadGraph("data/texture/gameDefeatCoreImage.png");
	gameUIImage[GAM_VICTORY_CORE]			= LoadGraph("data/texture/gameVictoryCoreImage.png");
	gameUIImage[GAM_MOUSE]					= LoadGraph("data/texture/mouseImage.png");
	gameUIImage[GAM_DEFEAT_LETTER]			= LoadGraph("data/texture/letter/defeat.png");
	gameUIImage[GAM_VICTORY_LETTER]			= LoadGraph("data/texture/letter/victory.png");
	gameUIImage[GAM_PUT_TRAP_LETTER]		= LoadGraph("data/texture/letter/trap.png");
	gameUIImage[GAM_WALL_LETTER]			= LoadGraph("data/texture/letter/wall.png");
	gameUIImage[GAM_SPIKE_LETTER]			= LoadGraph("data/texture/letter/spike.png");
	gameUIImage[GAM_TURRET_LETTER]			= LoadGraph("data/texture/letter/turret.png");
	gameUIImage[GAM_JUMP_PAD_LETTER]		= LoadGraph("data/texture/letter/jump_pad.png");
	gameUIImage[GAM_CHANGE_CAMERA_LETTER]	= LoadGraph("data/texture/letter/change_camera.png");
	gameUIImage[GAM_WAVE_START_LETTER]		= LoadGraph("data/texture/letter/wave_start.png");
	gameUIImage[GAM_COST_NUM]				= LoadGraph("data/texture/letter/num_white.png");
	gameUIImage[GAM_E_KEY]					= LoadGraph("data/texture/e_key.png");
	gameUIImage[GAM_A_KEY]					= LoadGraph("data/texture/a_key.png");
	gameUIImage[GAM_D_KEY]					= LoadGraph("data/texture/d_key.png");
	gameUIImage[GAM_ENTER_KEY]				= LoadGraph("data/texture/enter_key.png");
	gameUIImage[GAM_1_KEY]					= LoadGraph("data/texture/1_key.png");
	gameUIImage[GAM_3_KEY]					= LoadGraph("data/texture/3_key.png");
	gameUIImage[GAM_4_KEY]					= LoadGraph("data/texture/4_key.png");

	for (int i = 0;i < GAM_MAX;i++)
		assert(gameUIImage[i] >= 0);

	gameState				= GAME_STATE::SETUP;
#if USE_PLAYER
	playState				= PLAY_STATE::FIRST_PERSON;
#else
	playState				= PLAY_STATE::OBJECT_PUT;
#endif

	copyPlayState			= playState;
	camTargetToPosDistance	= DEFAULT_CAMERA_TARGET_TO_POS_DISTANCE;

	const auto& trapCon		= TrapInfo::GetTrapStageObjectKindContainer();
	for (auto trap = trapCon.cbegin();trap != trapCon.cend();trap++)
	{
		putTrapDataList.insert(std::make_pair(*trap, PutTrapData(*trap, TrapInfo::GetPutCost((StageObjectData::STAGE_OBJECT_KIND)*trap))));
	}

	holdCoin			= new HoldCoin();

	assert(coinUIFunctionIDNumber < 0);
	assert(uiFunctionIDNumber < 0);


	// HoldCoinクラスのCoinDraw関数を登録
	coinUIFunctionIDNumber = UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this] { holdCoin->CoinDraw(); });
	// 描画順を後に回す
	UIDrawManager::GetUIDrawManagerInstance()->SetDrawOrder(coinUIFunctionIDNumber, -1);

	// UIDraw関数を登録
	uiFunctionIDNumber = UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this] { UIDraw(); });
	// 描画順を後に回す
	UIDrawManager::GetUIDrawManagerInstance()->SetDrawOrder(uiFunctionIDNumber, -1);
	
	inputPad			= -1;
}

void GameController::Init()
{
	inputManager		= FindGameObject<Input>();
	assert(inputManager != nullptr);

	camera				= FindGameObject<Camera>();
	navigationManager	= FindGameObject<NavigationManager>();
	enemyManager		= FindGameObject<EnemyManager>();
	waveController		= FindGameObject<WaveController>();
	stageManager		= FindGameObject<StageManager>();
	fade				= FindGameObject<Fade>();

	// フェードイン
	fade->FadeIn(45);
	
	SoundManager::PlayBGM(Sound_ID::SETUP_BGM, true, true);

	// 一人称始点の時に罠の影響範囲を描画する
	stageManager->SetDrawTrapImpactRadiusFlag(playState == PLAY_STATE::FIRST_PERSON);

	// 一人称始点の時に罠のトラップ情報の描画をする
	stageManager->SetTurretInfoUIDrawFlag(playState == PLAY_STATE::FIRST_PERSON);



#if DRAW_MOUSEPOINTER
#else
	SetMouseDispFlag(false);
#endif
}

GameController::~GameController()
{
	for (int i = 0;i < GAM_MAX;i++)
	{
		DeleteGraph(gameUIImage[i]);
	}

	// HoldCoinクラスのCoinDraw関数を解放
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(coinUIFunctionIDNumber);
	// UIDrawを解放
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(uiFunctionIDNumber);

	coinUIFunctionIDNumber	= -1;
	uiFunctionIDNumber		= -1;

	Time::ResetGameDeltaTime();
	SetMouseDispFlag(true);
}

void GameController::Update()
{
	//_ マウスポインタ描画の切り替え _//

	bool copyUseImGuiMouse	= useImGuiMouse;					// onImGuiWindowのフラグ変更・不変更前の状態を保持
	useImGuiMouse			= ImGui::GetIO().WantCaptureMouse;	// ImGuiでマウス入力を使っているかのフラグを代入
	
	// フラグが変更されていたら
	if (useImGuiMouse != copyUseImGuiMouse)
	{
		// マウスポインタをフラグ通りに、描画するまたは描画しない
		SetMouseDispFlag(useImGuiMouse);
	}

	//__ GameStateの切り替え __//

	// Setter用のswitch文今後StateManager追加時に変更
	if (copyPlayState != playState)
	{
		copyPlayState	= playState;
		changedState	= true;
	}
	else
	{
		changedState	= false;
	}

	switch (gameState)
	{
	case GameController::GAME_STATE::START:
		break;
	case GameController::GAME_STATE::SETUP:
	case GameController::GAME_STATE::DEFENCE:

		// コアが破壊されたら
		if (stageManager->IsCoreBroken())
		{
			gameState = GAME_STATE::DEFEAT;
			// ゲームの時間経過をスローにする
			Time::SetGameDeltaTimeRate(SLOW_GAME_DEITATIME_RATE);

			SoundManager::StopBGM(Sound_ID::SETUP_BGM);
			SoundManager::StopBGM(Sound_ID::DEFENCE_BGM);
			SoundManager::PlaySE(Sound_ID::DEFEAT_JINGLE);
			//soundManager->PlayBGM(Sound_ID::DEFEAT_BGM, true, true);
		}

		// ウェーブに完全勝利したら
		if (waveController->IsWaveAllWin())
		{
			gameState = GAME_STATE::VICTORY;
			// ゲームの時間経過をスローにする
			Time::SetGameDeltaTimeRate(SLOW_GAME_DEITATIME_RATE);

			SoundManager::StopBGM(Sound_ID::SETUP_BGM);
			SoundManager::StopBGM(Sound_ID::DEFENCE_BGM);
			SoundManager::PlaySE(Sound_ID::VICTORY_JINGLE);
			SoundManager::PlayBGM(Sound_ID::VICTORY_BGM, true, true);
		}

		switch (playState)
		{
		case PLAY_STATE::OBJECT_PUT:

			
#if USE_PLAYER
			if (inputManager->GetKeyConfigPut("MODE_CHANGE"))
			{
				playState = PLAY_STATE::FIRST_PERSON;
				// 罠の影響範囲を描画する
				stageManager->SetDrawTrapImpactRadiusFlag(true);
				// 罠のトラップ情報の描画をする
				stageManager->SetTurretInfoUIDrawFlag(true);

				pushChangeCameraKey = true;

				break;	// 以下の処理を飛ばす
			}
#endif
			// 設置の処理
			SettingUpdate();
			break;
		case PLAY_STATE::FIRST_PERSON:

			if (inputManager->GetKeyConfigPut("MODE_CHANGE"))
			{
				playState = PLAY_STATE::OBJECT_PUT;
				// 罠の影響範囲を描画しない
				stageManager->SetDrawTrapImpactRadiusFlag(false);
				// 罠のトラップ情報の描画をしない
				stageManager->SetTurretInfoUIDrawFlag(false);

				// カメラの操作処理　俯瞰位置にカメラを設定 //
				CameraControllUpdate();
				camera->SettingCamera();

				pushChangeCameraKey = true;
				break;
			}
			break;
		default:
			assert(false);
			break;
		}
		// ウェーブの処理
		WaveUpdate();
		break;
	case GameController::GAME_STATE::DEFEAT:
	case GameController::GAME_STATE::VICTORY:

		if (inputManager->GetKeyConfigPut("NEXT"))
		{
			// フェードアウトの開始
			fade->FadeOut(60 * 1);

			SoundManager::StopAllSound();
			SoundManager::PlaySE(Sound_ID::SELECT_PUSH_SE);
			/*soundManager->StopBGM(Sound_ID::VICTORY_BGM);
			soundManager->StopBGM(Sound_ID::DEFEAT_BGM);*/

			isSceneChange = true;
		}

		if (isSceneChange)
		{
			// フェードアウトが終了したら
			if (fade->IsEnd())
			{
				SceneManager::ChangeScene("TITLE");
			}
		}
		break;
	default:
		assert(false);
		break;
	}

	//* 展示会などで作品紹介がしやすいように、デバッグの機能をexeでも実行するようにする

	if (CheckHitKey(KEY_INPUT_T))
	{
		SoundManager::StopBGM(Sound_ID::DEFEAT_BGM);
		SoundManager::StopBGM(Sound_ID::SETUP_BGM);
		SoundManager::StopAllSound();

		SceneManager::ChangeScene("TITLE");
	}

	//****************************************************

#ifdef _DEBUG

	if (inputManager->GetKeyConfigPush("_D_ADD_COIN"))
	{
		holdCoin->AddCoin(10);
	}

	int copyGameState = (int)gameState;

	ImGui::Begin("GAME");
	// GAME_STATEの変更
	ImGui::SliderInt("gameState", &copyGameState, 0, (int)GAME_STATE::MAX - 1);
	gameState = (GAME_STATE)copyGameState;

	ImGui::End();

#endif // _DEBUG
}

void GameController::AddHoldCoin()
{
	holdCoin->AddCoin(5);
}

void GameController::SubHoldCoin(const int& _sub)
{
	holdCoin->SubCoin(_sub);
}

int GameController::GetHoldCoin()
{
	return holdCoin->GetHoldCoin();
}

void GameController::SetInsufficientCoin()
{
	holdCoin->SetInsufficientCoin();
}

void GameController::SettingUpdate()
{
	inputPad = GetJoypadInputState(DX_INPUT_KEY_PAD1);

	// カメラの操作処理
	CameraControllUpdate();

	// マウスの画面座標を設定
	GetMousePoint(&screenPosition.x, &screenPosition.y);

#if CAN_PUT_TRAP_IN_DEFENCE

#else

	// ゲーム状態が準備でなかったら
	if (gameState != GAME_STATE::SETUP)
		return;	// 以下の処理を行わない
#endif
	switch (settingState)
	{
	case SETTING_STATE::STAGE_OBJ_SET:

		StageObjectSetting(settingWayState);
		break;
	default:
		assert(false);
		break;
	}
}

void GameController::CameraControllUpdate()
{

#if USE_CONTROLL_OLD

	//// カメラを近づける
	//if(inputManager->GetKeyConfigPush("MOVE_FRONT"))
	//	camTargetToPosDistance -= CAMERA_TARGET_TO_POS_MOVE_SPEED;

	//// カメラを遠ざける
	//if (inputManager->GetKeyConfigPush("MOVE_BACK"))
	//	camTargetToPosDistance += CAMERA_TARGET_TO_POS_MOVE_SPEED;

	// 右回転
	if (inputManager->GetKeyConfigPut("MOVE_RIGHT"))
		setCamMoveRotYMatrixAngle -= 90;

	// 左回転
	if (inputManager->GetKeyConfigPut("MOVE_LEFT"))
		setCamMoveRotYMatrixAngle += 90;

	// 一周したら丸める
	if (setCamMoveRotYMatrixAngle >= 360)
		setCamMoveRotYMatrixAngle -= 360;
#else

	// 右回転
	if (inputManager->GetKeyConfigPut("MOVE_RIGHT"))
		setCamMoveRotYMatrixAngle -= 90;

	// 左回転
	if (inputManager->GetKeyConfigPut("MOVE_LEFT"))
		setCamMoveRotYMatrixAngle += 90;

	// 一周したら丸める
	if (setCamMoveRotYMatrixAngle >= 360)
		setCamMoveRotYMatrixAngle -= 360;

	int wheelMove = inputManager->GetMouseInput()->GetMouseWheelFrame();	// 動いたマウスホイール値
	const int MAX_PUT_TRAP_KIND_LIST = (int)putTrapDataList.size() - 1;		// 罠の種類の最大数

	// ホイールが動いていたら
	if (wheelMove != 0)
	{
		// 1 or -1に値を収める
		wheelMove = std::clamp(wheelMove, -1, 1);
		// ホイール値のプラスマイナスを逆にして、ユーザーが直感的に操作しやすいようにする
		wheelMove *= -1;

		if (wheelMove < 0)
		{
			// カメラをズームイン
			camTargetToPosDistance -= CAMERA_TARGET_TO_POS_MOVE_SPEED;
		}
		else
		{
			// カメラをズームアウト
			camTargetToPosDistance += CAMERA_TARGET_TO_POS_MOVE_SPEED;
		}
	}
#endif

	camTargetToPosDistance = std::clamp(camTargetToPosDistance, MIN_CAMERA_TARGET_TO_POS_DISTANCE, MAX_CAMERA_TARGET_TO_POS_DISTANCE);

	VECTOR3 camAddVec = VECTOR3(0.0f, 1.2f, -0.85f);
	camAddVec *= camTargetToPosDistance;

	camMoveRotYMatrix = MGetRotY(setCamMoveRotYMatrixAngle * DegToRad);

	// 現在選択されているステージの設置ポイントの中心座標を得る
	//VECTOR3 putPointPos = stageManager->GetStageObjectPutPointSelectPosition();

	VECTOR3 targetPos = VECTOR3(0, 700, 0);

	// カメラ座標を設定
	VECTOR3 camPos = targetPos + camAddVec * camMoveRotYMatrix;
	camera->SetPosition(camPos);

	// カメラターゲットを設定
	VECTOR3 cemTarV = targetPos - camPos;
	camera->SetTarget(camPos + cemTarV);
}

void GameController::WaveUpdate()
{
	// ウェーブが終了したら
	if (waveController->IsWaveEnd())
	{
		if (firstFrameWaveEnd)
		{
			// コインを加算
			holdCoin->AddCoin(100);

			//soundManager->PlaySE(Sound_ID::WAVE_END_JINGLE);
		}

		if (gameState == GAME_STATE::DEFENCE)
		{
			// ゲーム状態を準備にする
			gameState = GAME_STATE::SETUP;
			SoundManager::StopBGM(Sound_ID::DEFENCE_BGM);
			// BGMを再生
			SoundManager::PlayBGM(Sound_ID::SETUP_BGM, true, true);
		}

		if (inputManager->GetKeyConfigPut("WAVE_START"))
		{
			// ウェーブをスタート
			waveController->WaveStart();
			SoundManager::PlaySE(Sound_ID::WAVE_START_JINGLE);

			if (gameState == GAME_STATE::SETUP)
			{
				// ゲーム状態を防衛にする
				gameState = GAME_STATE::DEFENCE;
				// BGMを止める
				SoundManager::StopBGM(Sound_ID::SETUP_BGM);
				// BGMを再生
				SoundManager::PlayBGM(Sound_ID::DEFENCE_BGM,true,true);
			}
		}
		firstFrameWaveEnd = false;
	}
	else
	{
		firstFrameWaveEnd = true;
	}
}

void GameController::Draw()
{
	// ToDo: ステートパターンでの実装
	switch (playState)
	{
	case PLAY_STATE::OBJECT_PUT:

		switch (settingState)
		{
		case SETTING_STATE::STAGE_OBJ_SET:

#if CAN_PUT_TRAP_IN_DEFENCE

#else
			// ゲーム状態が準備でなかったら
			if (gameState != GAME_STATE::SETUP)
				break;	// 罠の配置描画を行わない
#endif
			// 設置のモデル描画
			SettingModelDraw();

			break;
		default:
			assert(false);
			break;
		}
		break;
	}
}

void GameController::UIDraw()
{
	//_ CHANGE CAMERAの描画 _//

	if (!pushChangeCameraKey)
	{
		// 下敷き //
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		DrawBox(Screen::WIDTH - 450, Screen::HEIGHT - 140, Screen::WIDTH, Screen::HEIGHT - 70, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// CHANGE CAMERA の文字
		DrawRotaGraph(Screen::WIDTH - 230, Screen::HEIGHT - 105, 0.7f, 0.0f, gameUIImage[GAM_CHANGE_CAMERA_LETTER], true);
		// Eキー
		DrawRectRotaGraph(Screen::WIDTH - 500, Screen::HEIGHT - 105, 0, 0, 32, 32, 2.0f, 0.0f, gameUIImage[GAM_E_KEY], true);
	}

	switch (playState)
	{
	case PLAY_STATE::OBJECT_PUT:

		switch (settingState)
		{
		case SETTING_STATE::STAGE_OBJ_SET:

#if CAN_PUT_TRAP_IN_DEFENCE

#else
			// ゲーム状態が準備でなかったら
			if (gameState != GAME_STATE::SETUP)
				break;	// 罠の配置描画を行わない
#endif
			// 設置のUI描画
			SettingUIDraw();
			
			break;
		default:
			assert(false);
			break;
		}

		// 下の処理も続行

	case PLAY_STATE::FIRST_PERSON:

		// 保持しているコイン数の描画　FIRST_PERSONとOBJECT_PUTの際に描画する
		UIDrawManager::GetUIDrawManagerInstance()->SetDrawFlag(coinUIFunctionIDNumber, true);

		if (gameState == GAME_STATE::SETUP)
		{
			// ウェーブスタートの情報UIの描画
			WaveStartInfoDraw();
		}
		break;
	default:

		// 保持しているコイン数の描画をオフ
		UIDrawManager::GetUIDrawManagerInstance()->SetDrawFlag(coinUIFunctionIDNumber, false);
		break;
	}

	// ゲームリザルトの時
	if (gameState == GAME_STATE::DEFEAT || gameState == GAME_STATE::VICTORY)
	{
		// 描画順を一番最後に設定する
		UIDrawManager::GetUIDrawManagerInstance()->SetDrawOrder(uiFunctionIDNumber, -99);

		//_ 背景画面全体を暗く _//

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
		DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		VECTOR2I imageSize;
		float coreScale = 0.8f;
		int coreImage	= gameUIImage[GAM_DEFEAT_CORE];
		int letterImage = gameUIImage[GAM_DEFEAT_LETTER];

		if (gameState == GAME_STATE::VICTORY)
		{
			coreImage = gameUIImage[GAM_VICTORY_CORE];
			letterImage = gameUIImage[GAM_VICTORY_LETTER];
			coreScale = 1.2f;
		}

		//_ コア画像の描画 _//

		GetGraphSize(coreImage, &imageSize.x, &imageSize.y);
		DrawRectRotaGraph(Screen::WIDTH_CENTER, Screen::HEIGHT_CENTER, 0, 0, imageSize.x, imageSize.y, coreScale, 0.0f, coreImage, true);

		//_ VICTORY / DEFEAT 文字の描画 _//

		GetGraphSize(letterImage, &imageSize.x, &imageSize.y);
		DrawRectRotaGraph(Screen::WIDTH_CENTER, Screen::HEIGHT_CENTER, 0, 0, imageSize.x, imageSize.y, 1.0f, 0.0f, letterImage, true);

		SetFontSize(50);

		VECTOR2I pressLetterPos = VECTOR2I(Screen::WIDTH_CENTER - 400, Screen::HEIGHT - 80);

		DrawFormatString(pressLetterPos.x + 4, pressLetterPos.y + 4, 0x555555, "Press [ENTER] to Return to Title");
		DrawFormatString(pressLetterPos.x, pressLetterPos.y, 0xffffff, "Press [ENTER] to Return to Title");
		SetFontSize(DEFAULT_FONT_SIZE);
	}
}

void GameController::SettingUIDraw()
{
	//_ 罠のUI表示 _//

	// 下敷き //
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
	DrawBox(0, Screen::HEIGHT - 70, 500, Screen::HEIGHT, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	float letterScale = 0.6f;	// 文字の大きさ

	// TRAP の文字 
	DrawRotaGraph(80, Screen::HEIGHT - 35, letterScale, 0.0f, gameUIImage[GAM_PUT_TRAP_LETTER], true);

	// 選択されている罠の文字
	DrawRotaGraph(270, Screen::HEIGHT - 35, letterScale, 0.0f, GetPutTrapImageHandle(putTrapKind), true);

	// 選択されている罠のコスト数字
	DrawRotaNum(450.0f, (float)Screen::HEIGHT - 35.0f, putTrapDataList[putTrapKind].cost, 794 / 20, gameUIImage[GAM_COST_NUM], 794 / 10, 159 / 2, 0, 0.7f, 0.0f, 2);

	//_ 1 - 4のキー描画 _//

	// 1キー
	DrawRectRotaGraph(550, Screen::HEIGHT - 40, 0, 0, 32, 32, 2.0f, 0.0f, gameUIImage[GAM_1_KEY], true);
	// - の文字
	DrawBox(590, Screen::HEIGHT - 40, 610, Screen::HEIGHT - 35, 0xffffff, true);
	// 4キー
	DrawRectRotaGraph(650, Screen::HEIGHT - 40, 0, 0, 32, 32, 2.0f, 0.0f, gameUIImage[GAM_4_KEY], true);

	//_ マウスポインタの描画 _//

	// ImGuiがマウス入力を使っていないとき
	if (!ImGui::GetIO().WantCaptureMouse)
	{
		VECTOR2I mousePos;
		GetMousePoint(&mousePos.x, &mousePos.y);
		// マウスポインタのトンカチUIを描画
		DrawRotaGraph(mousePos.x, mousePos.y, 2.0f, 0.0f, gameUIImage[GAM_MOUSE], true);
	}

	//_ カメラ回転のキー描画 _//

	float toScreenFrameLen			= 58.0f;	// 画面からの距離
	VECTOR2 cameraRotAKeyDrawPos	= VECTOR2(toScreenFrameLen, Screen::HEIGHT_CENTER);					// カメラの左回転のキー描画座標
	VECTOR2 cameraRotDKeyDrawPos	= VECTOR2(Screen::WIDTH - toScreenFrameLen, Screen::HEIGHT_CENTER);	// カメラの右回転のキー描画座標
	float cameraRotKeyDrawScale		= 1.75f;	// カメラの回転のキー描画スケール

	// カメラ左回転のAキーの描画
	DrawRectRotaGraphF(cameraRotAKeyDrawPos.x, cameraRotAKeyDrawPos.y, 0, 0, 32, 32, cameraRotKeyDrawScale, 0.0f, gameUIImage[GAM_A_KEY], true);
	// カメラ右回転のDキーの描画
	DrawRectRotaGraphF(cameraRotDKeyDrawPos.x, cameraRotDKeyDrawPos.y, 0, 0, 32, 32, cameraRotKeyDrawScale, 0.0f, gameUIImage[GAM_D_KEY], true);

	float arrowLen				= 22.0f;				// 矢印の長さ
	float arrowThickness		= 3.5f;					// 矢印の線の太さ
	VECTOR2 toKeyDistanceVec	= VECTOR2(52.0f, -2.0f);// キー画像との距離

	VECTOR2 arrowAkeyDrawPos = cameraRotAKeyDrawPos - toKeyDistanceVec;									// Aキーに対する矢印の描画座標
	VECTOR2 arrowDkeyDrawPos = cameraRotDKeyDrawPos + VECTOR2(toKeyDistanceVec.x, -toKeyDistanceVec.y);	// Dキーに対する矢印の描画座標
	VECTOR2 arrowAkeyLineVec = VECTOR2(arrowLen, arrowLen);												// Aキーに対する矢印のベクトル
	VECTOR2 arrowDkeyLineVec = VECTOR2(-arrowLen, arrowLen);											// Dキーに対する矢印のベクトル

	//_ 左側の矢印の描画 _//

	// ／ の線描画
	DrawLineAA(arrowAkeyDrawPos.x, arrowAkeyDrawPos.y, arrowAkeyDrawPos.x + arrowAkeyLineVec.x, arrowAkeyDrawPos.y - arrowAkeyLineVec.y, 0xffffff, arrowThickness);
	// ＼ の線描画 
	DrawLineAA(arrowAkeyDrawPos.x, arrowAkeyDrawPos.y, arrowAkeyDrawPos.x + arrowAkeyLineVec.x, arrowAkeyDrawPos.y + arrowAkeyLineVec.y, 0xffffff, arrowThickness);

	//_ 右側の矢印の描画 _//

	// ／ の線描画
	DrawLineAA(arrowDkeyDrawPos.x, arrowDkeyDrawPos.y, arrowDkeyDrawPos.x + arrowDkeyLineVec.x, arrowDkeyDrawPos.y - arrowDkeyLineVec.y, 0xffffff, arrowThickness);
	// ＼ の線描画 
	DrawLineAA(arrowDkeyDrawPos.x, arrowDkeyDrawPos.y, arrowDkeyDrawPos.x + arrowDkeyLineVec.x, arrowDkeyDrawPos.y + arrowDkeyLineVec.y, 0xffffff, arrowThickness);
}

void GameController::SettingModelDraw()
{
	// 半透明の罠モデルの描画
	stageManager->DrawTrapPutPreview(screenPosition, (StageObjectData::STAGE_OBJECT_KIND)putTrapDataList[putTrapKind].kind, 0.5f);
}

void GameController::WaveStartInfoDraw()
{
	VECTOR2 drawWaveStartUIPos = VECTOR2((float)Screen::WIDTH - 55.0f, 120.0f);	// 描画座標

	// ウェーブスタートキー(Enterキー)の描画
	DrawRotaGraphF(drawWaveStartUIPos.x, drawWaveStartUIPos.y, 2.0f, 0.0f, gameUIImage[GAM_ENTER_KEY], true);

	// ウェーブスタート 文字の描画
	DrawRotaGraphF(drawWaveStartUIPos.x - 190.0f, drawWaveStartUIPos.y - 80.0f, 1.0f, 0.0f, gameUIImage[GAM_WAVE_START_LETTER], true);
}

int GameController::GetPutTrapImageHandle(const int& _kind)
{
	switch ((StageObjectData::STAGE_OBJECT_KIND)_kind)
	{
	case StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK:
		
		return gameUIImage[GAM_WALL_LETTER];
		break;
	case StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK:
		
		return gameUIImage[GAM_SPIKE_LETTER];
		break;
	case StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE:
	case StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD:

		return gameUIImage[GAM_TURRET_LETTER];
		break;
	case StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD:
		
		return gameUIImage[GAM_JUMP_PAD_LETTER];
		break;
	default:
		assert(false);
		break;
	}
	return -1;
}

//void GameController::NavigationReload()
//{
//	// ゲーム開始時はリロードしない
//	if (gameState == GameController::GAME_STATE::START)
//		return;
//	// WayPointの当たり判定をリロード
//	navigationManager->NavPointCollisionReload();
//}

VECTOR3 GameController::GetScreenForWorldPosI(const VECTOR2I& _screenPos, const SETTING_WAY_STATE& _mode)
{
	return GetScreenForWorldPosF(VECTOR2((float)_screenPos.x, (float)_screenPos.y), _mode);
}

VECTOR3 GameController::GetScreenForWorldPosF(const VECTOR2& _screenPos, const SETTING_WAY_STATE& _mode)
{
	VECTOR3 worldPos	= ConvScreenPosToWorldPos(VECTOR3(_screenPos.x, _screenPos.y, 0.9860f));	// スクリーン座標からワールド座標に変換した座標
	VECTOR3I posI		= VGetI((int)worldPos.x, (int)worldPos.y, (int)worldPos.z);

	switch (_mode)
	{
	case SETTING_WAY_STATE::GRID:

		posI.x		+= (int)StageInfo::PUT_GRID_SIZE / 2;
		posI.z		-= (int)StageInfo::PUT_GRID_SIZE / 2;
		posI		= (posI / (int)StageInfo::PUT_GRID_SIZE) * (int)StageInfo::PUT_GRID_SIZE;
		worldPos	= VECTOR3((float)posI.x, (float)posI.y, (float)posI.z);
		break;
	case SETTING_WAY_STATE::FREE:
		break;
	default:
		assert(false);
		break;
	}
	return worldPos;
}

bool GameController::StageObjectSetting(const SETTING_WAY_STATE& _mode)
{
	bool isObjectSettingControll = false;	// オブジェクトの設置または、削除操作をしたかどうか

#if TEST_STAGE_OBJECT_PUT_POINT
	
	putTransform.position	= stageManager->GetStageObjectPutPointSelectPosition();
#else
	putTransform.position	= GetScreenForWorldPosI(screenPos, _mode);

	// ブロックモデルの座標に影響される
	putTransform.position.y = 0.0f;
#endif
	
	if (inputManager->GetKeyConfigPut("PUT_TRAP"))
	{
		// ステージオブジェクトの配置
#if 0 // Blockモデルが白のモデルの時
		putTransform.size = VOne * 100.0f;
#else // Blockモデルが黒のモデルの時

		switch ((StageObjectData::STAGE_OBJECT_KIND)putTrapKind)
		{
		case StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK:

			putTransform.scale	= VOne * 250.0f;
			putTransform.size	= VOne * 1.0f;
			break;
		default:

			putTransform.scale	= VOne * 2.5f;
			putTransform.size	= VOne * 100.0f;
			break;
		}
		putTransform.position.y += (putTransform.scale.y * putTransform.size.y) / 2;
#endif
		PutStageObject();
		isObjectSettingControll = true;
	}

	if (inputManager->GetKeyConfigPut("DELETE_TRAP"))
	{
		// ステージオブジェクトの削除
		DeleteStageObject();
		isObjectSettingControll = true;
	}

#if USE_CONTROLL_OLD
	//__ 設置する罠の変更 __//

	int wheelMove						= inputManager->GetMouseInput()->GetMouseWheelFrame();	// 動いたマウスホイール値
	const int MAX_PUT_TRAP_KIND_LIST	= (int)putTrapDataList.size() - 1;						// 罠の種類の最大数

	// ホイールが動いていたら
	if (wheelMove != 0)
	{
		// 1 or -1に値を収める
		wheelMove		= std::clamp(wheelMove, -1, 1);
		// ホイール値のプラスマイナスを逆にして、ユーザーが操作しやすいようにする
		wheelMove		*= -1;
		
		if (wheelMove < 0)
		{
			// 罠の種類をプラスして変更
			putTrapKind = putTrapKind + wheelMove < 0 ? MAX_PUT_TRAP_KIND_LIST : putTrapKind - 1;
		}
		else
		{
			// 罠の種類をマイナスして変更
			putTrapKind = putTrapKind + wheelMove > MAX_PUT_TRAP_KIND_LIST ? 0 : putTrapKind + 1;
		}
	}
#endif

	// WALLを選択 //
	if (inputManager->GetKeyConfigPut("SELECT_1"))
		putTrapKind = (int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK;	
	// SPIKEを選択 //
	if (inputManager->GetKeyConfigPut("SELECT_2"))
		putTrapKind = (int)StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK;
	// RELOAD_TURRET_SINGLEを選択 //
	if (inputManager->GetKeyConfigPut("SELECT_3"))
		putTrapKind = (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE;
	// RELOAD_TURRET_SPREADを選択 //
	if (inputManager->GetKeyConfigPut("SELECT_4"))
		putTrapKind = (int)StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD;
	// JUMP_PADを選択 //
	if (inputManager->GetKeyConfigPut("SELECT_5"))
		putTrapKind = (int)StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD;

	/*if (inputManager->GetKeyConfigPut("SELECT_4"))
		putTrapKind = 4;
	if (inputManager->GetKeyConfigPut("SELECT_5"))
		putTrapKind = 4;
	if (inputManager->GetKeyConfigPut("SELECT_6"))
		putTrapKind = 4;
	if (inputManager->GetKeyConfigPut("SELECT_7"))
		putTrapKind = 4;*/

	return isObjectSettingControll;
}

void GameController::PutStageObject()
{
	int cost = putTrapDataList[putTrapKind].cost;

	// 設置コストに対しコイン数が足りなかったら
	if (holdCoin->GetHoldCoin() < cost)
	{
		// コイン不足状態を設定
		holdCoin->SetInsufficientCoin();
		// 設置ができないので、Donotサウンド再生
		SoundManager::PlaySE(Sound_ID::DO_NOT_SE);
		return;
	}

	// 罠の設置
	if (stageManager->CreatePutPointStageObject(camera->GetPosition(), (StageObjectData::STAGE_OBJECT_KIND)putTrapDataList[putTrapKind].kind, screenPosition))
	{
		// 罠の設置に成功したので、コインを減らす
		holdCoin->SubCoin(cost);
	}
}

void GameController::DeleteStageObject()
{
	StageObjectData::STAGE_OBJECT_KIND deleteStageObjKind = (StageObjectData::STAGE_OBJECT_KIND)stageManager->DeletePutPointStageObject(camera->GetPosition(), screenPosition);

	if (!TrapInfo::IsTrapStageObjectKind(deleteStageObjKind))
		return;	// 罠オブジェクトでなかったら return
	
	// 罠の設置コストを代入
	int trapCost			= TrapInfo::GetPutCost(deleteStageObjKind);
	// 罠の取り壊し時のコスト還元率を掛ける
	float turnCost			= trapCost * TRAP_DELETE_TURN_COST_RATE;

	holdCoin->AddCoin((int)turnCost);
}




