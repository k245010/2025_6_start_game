#include "camera.h"
#include "player.h"
#include "gravityManager.h"
#include "debugScreen.h"
#include "commonData.h"
#include "../ImGui/imgui.h"
#include "rgb.h"

namespace {
	const float CAMERA_PLAYER_DISTANCE_HORIZON = 1000.0f;
	const VECTOR CAMERA_BASE_POSITION = VGet(CAMERA_PLAYER_DISTANCE_HORIZON, 0.0f, CAMERA_Z);
	const float CAMERA_Z_DEFALUT_ADD = 50.0f; //通常状態のときにターゲットにたされるポジション。
	/// <summary>
	/// フォグ関係
	/// INが室内用
	/// OUTが室外用
	/// </summary>
	const int FOG_COLOR_OUT = 255; //室外背景のフォグの色
	const int FOG_COLOR_IN = 10; //室内背景のフォグの色
	const float FOG_IN_START = 3000.0f;
	const float FOG_IN_FINISH = 18000.0f;
	const float FOG_OUT_START = 5000.0f;
	const float FOG_OUT_FINISH = 40000.0f;
	//--------------------------------------------------------------------------------//
	const float NEAR_CLIP = 10.0f;
	const float FAR_CLIP = 100000.0f;
	//--------------------------------------------------------------------------------//
	const VECTOR CAMERA_BASE_TARGET_ADD = VGet(0.0f, 0.0f, CAMERA_Z_DEFALUT_ADD);
	const int SHAKE_INFNITY = -1; //シェイクを一定のステートの間使いたいときに使う
	const int SIGN_REVERCE_MULT = -1; //掛け算で符号を反対にしたいときに使ってる。
	const float TARGET_PLAYER_DISTANCE_CHANGE_VECTOR = 500.0f; //プレイヤーと注視点の距離からこの定数の距離分離れればカメラの左右モードが解決。
	const float EASEING_COUNTER_MAX = 1.0f;
	const float EASEING_COUNTER_MIN = 0.0f;
	const float START_CAMERA_POSITION = 8000.0f; //始まりのターゲットの最小値
	const float START_TARGET_POSITION = 3000.0f;
	const float GOAL_TIME = 120.0f; //ゴールした時のカメラのいーじんぐの時間
	const float GOAL_Y_MOVE = 200.0f; //ゴールした時にカメラのポジションをあげるよう
	const float GOAL_BASE_UP_POSITION = 4000.0f; //リザルトの後の上にあげるポジションの最大値
	const float GOAL_Z_SUB = -5000.0f; //リザルトの後の引きの値最大に引く
	const float GOAL_Z_MAX = -1000.0f; //リザルトの後の引きの値最小に引く
	const float SHAKE_POWER_GRAVITY = 20.0f; //重力状態の時のカメラの揺れの強さ
	const float SHAKE_POWER_HIT = 120.0f; //敵に攻撃が当たった時のカメラの揺れの強さ
	const float HIT_CAMERA_Z_SUB = 4500.0f; //攻撃を始めたらズームをするのでそのパラメーター
	const float HIT_EASING_COUNTER_ADD = 0.08f; //敵に攻撃を当てた時のイージングの速さ
	const float STOP_COUNTER_MAX = 5.0f; //敵に当たってからズーム状態を解除する時間の速さ
	const float DAMAGE_CAMERA_BASE_POWER = 120.0f; //攻撃を食らった時のベースの力の強さ
	const float DAMAGE_CAMERA_TIME = 15.0f; //攻撃を食らった時のカメラ揺れの時間
	const float RESULT_CAMERA_MAX = -2000.0f; //リザルトの時のカメラの引く
	const float TAREGT_Z_RESULT = 10000.0f;
	const float SHAKE_HIT_TIME = 30.0f; //敵に攻撃を当てた時の
	const float NORMAL_EASING_COUNTER_ADD = 0.005f; //通常のカメラのイージング
	const float GRAVITY_EASING_COUNTER_ADD = 0.03f; //重力状態のカメラのイージング
	const float GRAVITY_EASING_MAX = 1500; //重力状態の先に見えるポジションのmax
	const float GRAVITYMODE_NORMALMODE_CHANGE_EASEING_COUNTER = 30.0f; //グラビティーモードから通常モードに戻るときのカメラのイージングまでかかる時間の量
	const float CAMERA_AREA_COLLSION_COUNTER_ADD = 0.015f; //カメラエリアに当たった時のイージングの量
	const float CAMERA_AREA_COLLSION_OUT_COUNTER_ADD = 0.02f; //カメラエリアに外れた時のイージングのカウンター量
}

Camera::Camera(SceneBase* _scene):CameraBase(_scene)
{
	target = VZero;
	cameraUpVec = BASE_CAMERA_UP_VEC;
	rotation = 0.0f;
	beforeRotation = rotation;
	
	cameraState = NORMAL_CAMERA;

	keyPush = false;
	keepState = NORMAL_CAMERA;

	fov = DX_PI_F / 4.0f;
	//nearClip=1.0f; 
	//farClip=1000.0f;
	targetPosition=position;
	targetTarget = target;
	lerpSpeed=0.1f;
	
	CommonData* commonData = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<CommonData>();
	StageData* stageData = SceneManager::CommonScene()->FindGameObject<CommonFind>()->CommonFindObject<StageData>();
	stageNum = commonData->GetData().stage;

	player = _scene->FindGameObject<Player>();
	gravityManager = _scene->FindGameObject<GravityManager>();
	playerPosition = player->GetPosition();
	position = VGet(playerPosition.x, playerPosition.y, CAMERA_Z);//引きモードの場合z-7000くらい
	PlayerPositionChange();
	target = VAdd(VGet(0.0f, 0.0f, CAMERA_Z_DEFALUT_ADD), position);
	position.z = CAMERA_Z;
	SetFogEnable(TRUE);					// フォグを有効にする
	if (stageData->GetStageConfigData().backGroundData / 100 == 1) {
		SetFogMode(DX_FOGMODE_LINEAR);
		SetFogColor(FOG_COLOR_IN, FOG_COLOR_IN, FOG_COLOR_IN);			// フォグの色
		SetFogStartEnd(FOG_IN_START, FOG_IN_FINISH);	// フォグの開始距離と終了距離*/
	}
	else {
		SetFogColor(FOG_COLOR_OUT, FOG_COLOR_OUT, FOG_COLOR_OUT);			// フォグの色
		SetFogStartEnd(FOG_OUT_START, FOG_OUT_FINISH);	// フォグの開始距離と終了距離
	}

	cameraMode.side = RIGHT_CAMERA;
	baseDist = VGet(1000.0f, 0.0f, 0.0f);

	//ScreenSideMaxPosSet(RIGHT_LEFT_POS, RIGHT_LEFT_POS + RIGHT_ADD);
	//ScreenVerticalMaxPosSet(200, 800);
	cameraDirectionChange = true;
	reverce = false;
	rockSide = false;
	nearClip = NEAR_CLIP;
	farClip = FAR_CLIP;
}

Camera::~Camera()
{

}

void Camera::Draw()
{
	CameraBase::Draw();
#ifdef _DEBUG
	float b = atan2f(target.y - playerPosition.y, target.x - playerPosition.x);
	b += DX_PI_F / 2;

	VECTORDraw(V2Get(100, 300), smoothStartPosition, nullptr);
	DrawFormatString(400, 600, 0xffffff, "%.3f", b);
	DrawLine3D(playerPosition, target, 0xff00000);

	//GravityState g = gravityManager->NearGravityNumState(gravityKeep, DX_PI_F);

	//float get = gravityManager->GetGravityAngle(g);
	//DrawFormatString(400, 800, 0xffffff, "%.3f", get);

	VECTOR playerScreenPositionRight;
	playerScreenPositionRight = ConvWorldPosToScreenPos(playerPosition);
	VECTOR playerScreenPositionLeft = ConvWorldPosToScreenPos(playerPosition);

	playerScreenPositionRight.x = 1200;
	playerScreenPositionLeft.x = 1450;
	DrawSphere3D(target, 50.0f, 2, 0xffffff, 0xffffff, false);
#endif // _DEBUG

	/*DrawLine(startPosition.leftMax, 0.0f, startPosition.leftMax, 1080, 0xffff00, 10);
	DrawLine(startPosition.rightMax, 0.0f, startPosition.rightMax, 1080, 0xffff00, 10);
	DrawLine(0, startPosition.upMax, 1920, startPosition.upMax, 0xffff00, 10);
	DrawLine(0, startPosition.downMax, 1920, startPosition.downMax, 0xffff00, 10);*/
}

void Camera::Update()
{
	playerPosition = player->GetCenterPosition();
	cameraAreaBefore = playerCameraAreaHit;
	playerCameraAreaHit = player->GetCameraHit();
	if (cameraAreaBefore != playerCameraAreaHit) {
		changeMode = true;
	}
	else {
		changeMode = false;
	}
	gameState = player->GetGameState();
	worldGravity = GravityManager::GetWorldGravity();

	/*if(CheckHitKey(KEY_INPUT_K)) {
		SetFogEnable(FALSE);
	}
	else {
		SetFogEnable(TRUE);
	}*/

#ifdef _DEBUG
	//ImGui::Begin("COLLSION_PLUS");
	///*ImGui::DragFloat3("plus", &collsionPlusPosition.x, 0.0f, 0.0f, 2000.0f);*/
	//if (ImGui::Button("coll")) {
	//	coll = !coll;
	//}
	//bool reverceKeep = reverce;
	//if (ImGui::Button("reverce")) {
	//	reverce = !reverce;
	//}
	//if (reverce) {
	//	if (reverce != reverceKeep) {
	//		collsionPlusPosition *= -1.0f;
	//	}

	//}
	//ImGui::End();
#endif // _DEBUG

	

	CameraBase::Update();
	if (debugMode == DEBUG_MODE) {
		return;
	}
	float rate;
	switch (gameState)
	{
	case GameManager::PLAY_BEFORE:
		PlayBeforeCamera();
		break;
	case GameManager::PLAYING:
		SetCameraRot();
		switch (cameraState)
		{
		case Camera::NORMAL_CAMERA:
			if (shakeState.power >= 0) {
				shakeState.power -= 5;
			}
			if (player->GetPlayerMode() == Player::GRAVITY_MOVE_MODE) {
				CameraStateChange(GRAVITY_MOVE_CAMERA);
				ShakeReady(ShakeGet(10, SHAKE_INFNITY));
				if (playerCameraAreaHit) {
					VECTOR plus = collsionPlusPosition * MGetRotZ(worldGravity);
					/*if (dire == CharaBase::LEFT) {
						plus.x *= -1.0f;
					}*/
					cameraAtan = atan2f((target.y) - (plus.y + playerPosition.y), (target.x) - (plus.x + playerPosition.x));
				}
				else {
					cameraAtan = atan2f((target.y) - playerPosition.y, (target.x) - playerPosition.x);
				}
			}
			CollsionMoveEaseIn();
			
			CameraNoMoveRange();
			PlayerPositionChange();
			CameraYMove();
			PositionSet();
			CameraTarget();
			break;
		case Camera::GRAVITY_MOVE_CAMERA:
			GravityModeCamera();
			CollsionMoveEaseIn();
			CameraNoMoveRange();
			//position.z = CAMERA_Z;
			/*if (collsionMoveStepCounter <= EASEING_COUNTER_MIN) {
				collsionMoveStepCounter = EASEING_COUNTER_MIN;
			}
			else {
				collsionMoveStepCounter -= 0.02f;
			}*/
			//position.z = SmoothStep(CAMERA_Z, baseMoveMaxPosition.z, collsionMoveStepCounter);
			CameraTarget();
			//target = playerPosition;
			if (shakeState.power <= 10) {
				shakeState.power++;
			}
			if (player->GetPlayerAttackState() == Player::ATTACK1) {
				AttackCameraStart();
			}
			else if (player->GetPlayerMode() == Player::NORMAL_MODE || player->GetPlayerMode() == Player::RAIL_MOVE) {
				NormalCameraChange();
				if (shakeState.time == SHAKE_INFNITY) {
					shakeState.time = 0;
					shakeState.power = 0;
				}
					
			}
			if (player->GetPlayerMode() == Player::GRAVITY_CHANGE_MODE) {
				shakeState.time = 0;
			}
			else {
				if (shakeState.time == 0 && player->GetPlayerMode() != Player::NORMAL_MODE) {
					ShakeReady(ShakeGet(SHAKE_POWER_GRAVITY / 2.0f, -1));
				}
			}
			break;
		case Camera::HIT_CAMERA:
		{
			AttackCamera();
			break;
		}
		default:
			break;
		}
		SetCameraRot();
		break;
	case GameManager::LOSE:
		switch (player->GetLoseState())
		{
		case Player::Die:
			break;
		case Player::Lose:
			break;
		default:
			position = playerPosition;
			target = VAdd(CAMERA_BASE_TARGET_ADD, position);
			position.z = CAMERA_Z;
			SetCameraRot();
			break;
		}
		break;
	case GameManager::GOAL_FRONT_ANI:
		GoalAnimCamera();
		break;
	case GameManager::PLAY_AFTER:
		GoalAfterCamera();
		break;
	case GameManager::SCENE_CHANGE:
		break;
	default:
		break;
	}
	if (player->GetDamageCamera()) {
		int hp = min(PLAYER_HP_MAX, player->GetHp());
		ShakeReady(ShakeGet(DAMAGE_CAMERA_BASE_POWER + (PLAYER_HP_MAX - hp) * SHAKE_POWER_GRAVITY, DAMAGE_CAMERA_TIME));
	}
	player->DamageCamera();
	ShakeCamera();
	player->TargetCameraRay(target);
	player->CameraHitRe();
	player->EnemyHitReset();
}

void Camera::SetCameraRot()
{
	//float playerGravityRot = player->GetGravityRot();
	float playerGravityRot;
	if (gameState == GameManager::GOAL_FRONT_ANI) {
		playerGravityRot = player->GetGravityRot();
	}
	else {
		playerGravityRot = GravityManager::GetWorldGravity();
	}
	//左重力(-DX_PI_F/2)→上重力時(DX_PI_F)のカメラの回転の修正
	if (playerGravityRot > 0&&rotation<0)
	{
		rotation += DX_PI_F * 2;
	}
	//上重力(DX_PI_F)→左重力時(-DX_PI_F/2)のカメラの回転の修正
	if (playerGravityRot < 0 && rotation >= DX_PI_F)
	{
		rotation -= DX_PI_F * 2;
	}

	rotation += (playerGravityRot - rotation) / CAMERA_ROT_TIME;

	MATRIX rotZ = MGetRotZ(rotation);
	cameraUpVec = VTransform(BASE_CAMERA_UP_VEC, rotZ);
}

int Camera::CameraStateChange(CameraState _state)
{
	if (cameraState == _state) {
		return 1;
	}
	cameraState = _state;
	smoothCounter = EASEING_COUNTER_MIN;
	return static_cast<int>(cameraState);
}

void Camera::PlayerPositionChange()
{
	/*if (playerPosition.x >= MAP_LEFT_WIDTH_POS[stageNum] && playerPosition.x <= MAP_RIGHT_WIDTH_POS[stageNum]) {
		position.x = playerPosition.x;
	}
	else if(playerPosition.x  < MAP_LEFT_WIDTH_POS[stageNum]){
		position.x = MAP_LEFT_WIDTH_POS[stageNum];
	}
	else {
		position.x = MAP_RIGHT_WIDTH_POS[stageNum];
	}
	if (playerPosition.y >= MAP_DOWN_WIDTH_POS[stageNum] && playerPosition.y <= MAP_UP_WIDTH_POS[stageNum]) {
		position.y = playerPosition.y;
	}
	else if(playerPosition.y < MAP_DOWN_WIDTH_POS[stageNum]){
		position.y = MAP_DOWN_WIDTH_POS[stageNum];
	}
	else {
		position.y = MAP_UP_WIDTH_POS[stageNum];
	}*/

	GravityState worldState = gravityManager->GetWorldGravityState();


	/*
	VECTOR rotate = player->GetMoveRotate();
	VECTOR velo = player->GetVelocity();
	VECTOR gvelo = player->GetGravityVelocity();
	VECTOR v = VScale(velo,1.0f);
	movePlayerPosition = VSub(movePlayerPosition,gvelo);*/

	smoothCounter += NORMAL_EASING_COUNTER_ADD;
	if (smoothCounter >= EASEING_COUNTER_MAX) {
		smoothCounter = EASEING_COUNTER_MAX;
	}
	if (cameraMode.side == RIGHT_CAMERA) {
		if (smoothCounter > EASEING_COUNTER_MIN && cameraDirectionChange) {
			smoothCounter = EASEING_COUNTER_MIN;
			cameraDirectionChange = false;
			if (worldState == UP_GRAVITY || worldState == DOWN_GRAVITY) {
				moveCameraAddPosition.x = -(playerPosition.x - target.x);
			}
			else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
				moveCameraAddPosition.x = -(playerPosition.y- target.y);
			}
			if (moveCameraAddPosition.x > EASEING_COUNTER_MIN) {
				moveCameraAddPosition.x *= SIGN_REVERCE_MULT;
			}
		}
		moveCameraAddPosition = VGet(SmoothStep(moveCameraAddPosition.x, baseMoveMaxPosition.x, smoothCounter), SmoothStep(moveCameraAddPosition.y, baseMoveMaxPosition.y, smoothCounter), 0);
	}
	else {
		if (smoothCounter > EASEING_COUNTER_MIN && cameraDirectionChange) {
			smoothCounter = EASEING_COUNTER_MIN;
			cameraDirectionChange = false;
			if (worldState == UP_GRAVITY || worldState == DOWN_GRAVITY) {
				moveCameraAddPosition.x = fabs(playerPosition.x - target.x);
			}
			else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
				moveCameraAddPosition.x = fabs(playerPosition.y - target.y);
			}
		}
		moveCameraAddPosition = VGet(SmoothStep(moveCameraAddPosition.x, -baseMoveMaxPosition.x, smoothCounter), SmoothStep(moveCameraAddPosition.y, baseMoveMaxPosition.y, smoothCounter), 0);
	}



}

void Camera::PlayingBeforeCamera()
{
	beforeCounter++;
	position = playerPosition;
	position.z = EaseOut(-1000.0f, -6000.0f, beforeCounter / 90.0f);

}

void Camera::ShakeFinish()
{
	//CameraStateChange(NORMAL_CAMERA);
}

void Camera::DebugOpsionSet()
{
	debugPosition = V2Get(1200, 800);
}

void Camera::GravityModeCamera()
{

	float ro = GravityManager::GetWorldGravity();
	GravityState gState = gravityManager->GetGravityState();
	VECTOR movePlayerPosition = playerPosition;
	
	/*if (changeMode) {
		smoothCounter = 0.0f;
	}*/
	smoothCounter += GRAVITY_EASING_COUNTER_ADD;
	if (smoothCounter >= EASEING_COUNTER_MAX) {
		smoothCounter = EASEING_COUNTER_MAX;
	}
	//Aloow();
	//position = allowPos;
	GravityState playerState = player->GetStartGravityMode();
	if (playerChangeGravityKeep != playerState) {
		smoothCounter = EASEING_COUNTER_MIN;
		if (cameraAtan == EASEING_COUNTER_MIN || cameraAtan == DX_PI_F) {
			smoothStartPosition = moveCameraAddPosition * SIGN_REVERCE_MULT;
			GravityState reverceGravity = gravityManager->GetPlayerReverceChangeGravity(playerChangeGravityKeep);
			gravityKeep = gravityManager->GetGravityAngle(reverceGravity);
			if (smoothStartPosition.x != 0.0f) {
				smoothStartPosition.y = abs(moveCameraAddPosition.x * 1.0f);
				if (dire == CharaBase::RIGHT) {
					GravityState reverceGravitys;
					if (playerState == RIGHT_GRAVITY_START) {
						if (moveCameraAddPosition.x >= EASEING_COUNTER_MIN) {
							smoothStartPosition.y = moveCameraAddPosition.x * SIGN_REVERCE_MULT;
							reverceGravitys = gravityManager->GetPlayerReverceChangeGravity(RIGHT_GRAVITY_START);
						}
						else {
							reverceGravitys = gravityManager->GetPlayerReverceChangeGravity(LEFT_GRAVITY_START);
						}
					}
					else {
						reverceGravitys = gravityManager->GetPlayerReverceChangeGravity(RIGHT_GRAVITY_START);
					}
					gravityKeep = gravityManager->GetGravityAngle(reverceGravitys);
				}
				else {
					GravityState reverceGravitys;
					if (playerState == LEFT_GRAVITY_START) {
						if (moveCameraAddPosition.x <= EASEING_COUNTER_MIN) {
							reverceGravitys = gravityManager->GetPlayerReverceChangeGravity(LEFT_GRAVITY_START);
						}
						else {
							reverceGravitys = gravityManager->GetPlayerReverceChangeGravity(RIGHT_GRAVITY_START);
						}
					}
					else {
						reverceGravitys = gravityManager->GetPlayerReverceChangeGravity(LEFT_GRAVITY_START);
					}
					gravityKeep = gravityManager->GetGravityAngle(reverceGravitys);
				}
			}
			smoothStartPosition.x = EASEING_COUNTER_MIN;
		}
		else {
			gravityKeep = cameraAtan + (DX_PI_F / 2 + DX_PI_F);
			cameraAtan = 0.0f;
			gravityNo = true;
			
			
			//GravityState g = gravityManager->NearGravityNumState(gravityKeep, DX_PI_F / 2);
			//if ((g == LEFT_GRAVITY && dire == CharaBase::LEFT) || (g == RIGHT_GRAVITY && dire == CharaBase::RIGHT)) {
				//gravityKeep = gravityManager->GetGravityAngle(gravityManager->GetReverceGravity(g));
			//}
			//else {
				//gravityKeep = gravityManager->GetGravityAngle(gravityManager->GetReverceGravity(g));
			//}
			
			VECTOR cameraTarget = VGet(target.x, target.y, playerPosition.z);
			float amount = VSize(cameraTarget - playerPosition);
			smoothStartPosition.y = amount;
			//smoothStartPosition = moveCameraAddPosition * -1.0f;
			//smoothStartPosition.y = abs(moveCameraAddPosition.x);
			//smoothStartPosition.y = abs(VSize(cameraTarget - playerPosition));
		}
		
	}
	else {
		float rot = player->GetGravityRot();
		MATRIX matrix;
		
		/*if (playerCameraAreaHit) {
			moveCameraAddPosition.x = 0.0f;
			moveCameraAddPosition.y = 0.0f;
		}*/
		/*else {*/
			if (smoothStartPosition.x != EASEING_COUNTER_MIN) {
				moveCameraAddPosition = VGet(0.0f, SmoothStep(smoothStartPosition.y, -GRAVITY_EASING_MAX, smoothCounter), 0.0f);
			}
			else {
				moveCameraAddPosition = VGet(0.0f, SmoothStep(smoothStartPosition.y, -GRAVITY_EASING_MAX, smoothCounter), 0.0f);
			}
			if (moveCameraAddPosition.y <= EASEING_COUNTER_MIN) {
				matrix = MGetRotZ(rot);
			}
			else {
				matrix = MGetRotZ(gravityKeep);
			}
		/*}*/
		position = VAdd(movePlayerPosition, VTransform(moveCameraAddPosition, matrix));
		VECTOR collsionPos = collsionPlusPosition;
		if (playerCameraAreaHit) {
			if (cameraMode.side == LEFT_CAMERA) {
				collsionPos.x *= -1.0f;
			}
			VECTOR movePos = SmoothStep(VGet(0.0f, 0.0f, 0.0f), collsionPos, collsionMoveStepCounter);
			position += movePos * MGetRotZ(worldGravity);
			keepVector = VTransform(movePos, MGetRotZ(worldGravity));
		}
		else {
			if (dire == CharaBase::LEFT) {
				collsionPos.x *= -1.0f;
			}
			VECTOR movePos = SmoothStep(VGet(0.0f, 0.0f, 0.0f), collsionPos, collsionMoveStepCounter);
			position += movePos * MGetRotZ(worldGravity);
			keepVector = VTransform(movePos, MGetRotZ(worldGravity));
		}
		position.z = 0.0f;
		keepVector += VTransform(moveCameraAddPosition, matrix);
		
	}
	playerChangeGravityKeep = playerState;

	/*////MATRIX matrixY = MGetRotY(player->GetMoveRotate().y);
	////MATRIX matrix = MMult(matrixY, matrixX);
	VECTOR base = VGet(0.0f, 0.0f, 0.0f);
	VECTOR pos = VAdd(base, VTransform(moveCameraAddPosition, matrix));
	position = VAdd(movePlayerPosition, pos);*/

	//ShakeCamera();
}

void Camera::CameraYMove()
{
	/*if (cameraMode.vertical == UP_CAMERA) {
		if (smoothCounter > 0.0f && cameraDirectionChange) {
			smoothCounter = 0.0f;
			cameraDirectionChange = false;
			moveCameraAddPosition.y = fabs(target.x - playerPosition.x);
		}
		moveCameraAddPosition = VGet(moveCameraAddPosition.x, SmoothStep(moveCameraAddPosition.y, 1000.0f, smoothCounter), 0);
	}
	else if(cameraMode.vertical == DOWN_CAMERA) {
		if(smoothCounter > 0.0f && cameraDirectionChange) {
			smoothCounter = 0.0f;
			cameraDirectionChange = false;
			moveCameraAddPosition.y = fabs(target.x - playerPosition.x);
		}
		moveCameraAddPosition = VGet(moveCameraAddPosition.x, SmoothStep(moveCameraAddPosition.y, -1000.0f, smoothCounter), 0);
	}
	else {
		moveCameraAddPosition = VGet(moveCameraAddPosition.x, SmoothStep(0.0f, moveCameraAddPosition.y, smoothCounter), 0);
	}*/
}

void Camera::PositionSet()
{
	GravityState worldState = gravityManager->GetWorldGravityState();

	VECTOR movePlayerPosition = playerPosition;
	VECTOR v =VZero;
	//MATRIX matrixY = MGetRotY(player->GetMoveRotate().y);
	//MATRIX matrix = MMult(matrixY, matrixX);
	
	VECTOR base = VZero;
	VECTOR pos = VAdd(base, VTransform(moveCameraAddPosition, GetCameraMatrix()));
	if (rockSide) {
		if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
			movePlayerPosition.x = rockPosition.x;
		}
		else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
			movePlayerPosition.y = rockPosition.y;
		}
	}
	//if(rockVertical){
		//if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
			//movePlayerPosition.y = rockPosition.y;
		//}
		//else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
			//movePlayerPosition.x = rockPosition.x;
		//}
		//movePlayerPosition = rockPosition;
	//}
	position = pos + movePlayerPosition;
	/*VECTOR setPos = VAdd(base, VTransform(moveCameraSetPosition, matrix));
	if (setPos.x != 0.0f) {
		position.x = setPos.x;
	}
	else if(setPos.y != 0.0f){
		position.y = setPos.y;
	}*/
}

void Camera::NormalCameraChange()
{
	
	switch (cameraState)
	{
	case Camera::NORMAL_CAMERA:
		break;
	case Camera::GRAVITY_MOVE_CAMERA:
		changeCounter = GRAVITYMODE_NORMALMODE_CHANGE_EASEING_COUNTER;
		playerChangeGravityKeep = NO_GRAVITY;
		if (player->GetChangeGravity() == Player::NO_CHANGE) {
			float world = 0.0f;
			if (moveCameraAddPosition.y <= EASEING_COUNTER_MIN) {
				GravityState playerState = player->GetStartGravityMode();
				//GravityState reverceGravity = gravityManager->GetPlayerReverceChangeGravity(playerState);x
				world = gravityManager->GetGravityAngle(playerState);
			}
			else {
				world = gravityKeep;
			}
			MATRIX m = MGetRotZ(world);
			moveCameraAddPosition = VTransform(moveCameraAddPosition, m);
			if (playerCameraAreaHit) {
				VECTOR collsionPos = collsionPlusPosition;
				if (cameraMode.side == LEFT_CAMERA) {
					collsionPos.x *= -1.0f;
				}
				moveCameraAddPosition += collsionPos;
			}
		}
		else if (player->GetChangeGravity() == Player::CHANGE) {
			//if (moveCameraAddPosition.y <= 0.0f) {
				//moveCameraAddPosition *= -1;
			//}
			float world;
			GravityState worldGravityState = gravityManager->GetWorldGravityState();
			GravityState playerState = player->GetStartGravityMode();
			GravityState beforeWState = gravityManager->GetBeforeWorldGravityState();
			playerState = gravityManager->MultGravityState(playerState, beforeWState);
			//GravityState reverceGravity = gravityManager->GetPlayerReverceChangeGravity(playerState);x
			world = gravityManager->GetGravityAngle(playerState);
			MATRIX m = MGetRotZ(world);
			moveCameraAddPosition = VTransform(moveCameraAddPosition, m);
			if (moveCameraAddPosition.y > EASEING_COUNTER_MIN) {
				if (playerState != LEFT_GRAVITY_START) {
					moveCameraAddPosition *= SIGN_REVERCE_MULT;
				}

			}
			if (playerState == RIGHT_GRAVITY_START || playerState == LEFT_GRAVITY_START) {
				moveCameraAddPosition.y = moveCameraAddPosition.x;
				moveCameraAddPosition.x = EASEING_COUNTER_MIN;
			}
			if (worldGravityState == RIGHT_GRAVITY || worldGravity == LEFT_GRAVITY) {
				moveCameraAddPosition.x *= SIGN_REVERCE_MULT;
			}
			if (beforeWState == RIGHT_GRAVITY || beforeWState == LEFT_GRAVITY) {
				if (playerState == UP_GRAVITY_START || playerState || DOWN_GRAVITY_START) {
					moveCameraAddPosition.x = moveCameraAddPosition.y;
					moveCameraAddPosition.y = EASEING_COUNTER_MIN;
				}
			}
			player->CameraSe();
		}
		else {
			if (moveCameraAddPosition.y >= EASEING_COUNTER_MIN) {
				if (smoothStartPosition.x > EASEING_COUNTER_MIN) {
					moveCameraAddPosition.x = -moveCameraAddPosition.y;
				}
				else if (smoothStartPosition.x < EASEING_COUNTER_MIN) {
					moveCameraAddPosition.x = moveCameraAddPosition.y;
				}

				moveCameraAddPosition.y = EASEING_COUNTER_MIN;
			}
		}
		break;
	case Camera::HIT_CAMERA:
		moveCameraAddPosition = VZero;
		rockSide = false;
		break;
	default:
		break;
	}
	CameraStateChange(NORMAL_CAMERA);
}

void Camera::ShakeCamera()
{
	if (shakeState.time > 0 || shakeState.time == SHAKE_INFNITY) {
		ShakeTime();
	}
}

bool Camera::CameraPlayerCollision(float _pos1, float _pos2, float _pos3)
{
	if (_pos3 > _pos1 && _pos3 < _pos2) {
		return true;
	}
	return false;
}

void Camera::CameraNoMoveRange()
{
	VECTOR playerScreenPosition;
	VECTOR playerRockPos = playerPosition;
	GravityState worldState = gravityManager->GetWorldGravityState();
	VECTOR v = VZero;
	float plus = 0.0f;
	//if (coll) {
		//baseDist = baseMoveMaxPosition;
	//}
	//else {
		baseDist = baseMoveMaxPosition;
	//}
	playerScreenPosition = ConvWorldPosToScreenPos(playerPosition);
	playerTargetDist.x = fabs(target.x - playerPosition.x);
	playerTargetDist.y = fabs(target.y - playerPosition.y);
	float targetP = 0.0f;;
	if (worldState == UP_GRAVITY || worldState == DOWN_GRAVITY) {
		targetP = fabs(playerPosition.x - target.x);
	}
	else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
		targetP = fabs(playerPosition.y - target.y);
	}
	if (changeCounter > 0) {
		changeCounter--;
		rockSide = false;
		return;
	}
	if (cameraMode.side == RIGHT_CAMERA) {
		if (baseDist.x + TARGET_PLAYER_DISTANCE_CHANGE_VECTOR <= targetP) {
			if (moveCameraAddPosition.x >= baseMoveMaxPosition.x) {
				cameraMode.side = LEFT_CAMERA;
				//baseDist = VGet(1000.0f, 0.0f, 0.0f);
				//rockSide = false;
				cameraDirectionChange = true;
				changeCounter = GRAVITYMODE_NORMALMODE_CHANGE_EASEING_COUNTER;
				rockSide = false;
			}
			else if (!rockSide) {
				rockSide = true;
				if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
					rockPosition.x = playerPosition.x;
				}
				else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
					rockPosition.y = playerPosition.y;
				}
			}
			
		}
		else if (baseDist.x <= targetP) {
			if (!rockSide) {
				rockSide = true;
				if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
					rockPosition.x = playerPosition.x;
				}
				else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
					rockPosition.y = playerPosition.y;
				}
			}
			/*float add = playerScreenPosition.x - startPosition.rightMax;
			ScreenSideMaxPosSet(startPosition.leftMax, startPosition.rightMax,add);*/
		}
		else {
			/*if (!rockSide) {
				if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
					rockPosition.x = playerPosition.x;
				}
				else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
					rockPosition.y = playerPosition.y;
				}
			}*/
			rockSide = false;
		}
	}
	else {
		if (baseDist.x + TARGET_PLAYER_DISTANCE_CHANGE_VECTOR <= targetP) {
			if (moveCameraAddPosition.x <= -baseMoveMaxPosition.x) {
				cameraMode.side = RIGHT_CAMERA;
				//baseDist = VGet(1000.0f, 0.0f, 0.0f);
				//rockSide = false;
				cameraDirectionChange = true;
				changeCounter = GRAVITYMODE_NORMALMODE_CHANGE_EASEING_COUNTER;
				rockSide = false;
			}
			else if (!rockSide) {
				rockSide = true;
				if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
					rockPosition.x = playerPosition.x;
				}
				else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
					rockPosition.y = playerPosition.y;
				}
			}
		}
		else if (baseDist.x <= targetP) {
			if (!rockSide) {
				rockSide = true;
				if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
					rockPosition.x = playerPosition.x;
				}
				else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
					rockPosition.y = playerPosition.y;
				}
			}
			/*float add = playerScreenPosition.x - startPosition.leftMax ;
			ScreenSideMaxPosSet(startPosition.leftMax, startPosition.rightMax, add);*/
		}
		else {
			/*if (!rockSide) {
				if (worldState == DOWN_GRAVITY || worldState == UP_GRAVITY) {
					rockPosition.x = playerPosition.x;
				}
				else if (worldState == RIGHT_GRAVITY || worldState == LEFT_GRAVITY) {
					rockPosition.y = playerPosition.y;
				}
			}*/
			rockSide = false;
		}
	}
	
}

MATRIX Camera::GetCameraMatrix()
{
	MATRIX matrix = MGetRotZ(worldGravity);
	return matrix;
}

MATRIX Camera::GetCameraPlayerMatrix()
{
	MATRIX matrix = player->GetRotateMatrix();
	return matrix;
}

void Camera::CollsionMoveEaseIn()
{
	if (playerCameraAreaHit) {
		//baseMoveMaxPosition = CAMERA_BASE_POSITION;
		if (!firstColl) {
			firstColl = true;
			collsionMoveStepCounter = EASEING_COUNTER_MIN;
			collBeforePositionKeep = moveCameraAddPosition;
			collBeforePositionKeep.z = baseMoveMaxPosition.z;
		}
		VECTOR beforePos = collsionPlusPosition;
		collsionPlusPosition = player->GetCameraCollPos();
		if (beforePos.x != collsionPlusPosition.x || beforePos.y != collsionPlusPosition.y || beforePos.z != collsionPlusPosition.z) {
			collsionMoveStepCounter = EASEING_COUNTER_MIN;
			collBeforePositionKeep = moveCameraAddPosition;
			collBeforePositionKeep.z = baseMoveMaxPosition.z;
		}
		if (collsionMoveStepCounter >= EASEING_COUNTER_MAX) {
			collsionMoveStepCounter = EASEING_COUNTER_MAX;
		}
		else {
			collsionMoveStepCounter += CAMERA_AREA_COLLSION_COUNTER_ADD;
		}
		VECTOR moveKeep = collBeforePositionKeep;
		//moveKeep.z = CAMERA_Z;
		if (cameraMode.side == RIGHT_CAMERA) {
			baseMoveMaxPosition = SmoothStep(moveKeep, CAMERA_BASE_POSITION + collsionPlusPosition, collsionMoveStepCounter);
		}
		else {
			VECTOR revercePos = CAMERA_BASE_POSITION;
			revercePos.x = CAMERA_BASE_POSITION.x * SIGN_REVERCE_MULT;
			VECTOR reversPlus = collsionPlusPosition;
			reversPlus.x = collsionPlusPosition.x * SIGN_REVERCE_MULT;
			moveKeep.x = fabs(moveKeep.x);
			baseMoveMaxPosition = SmoothStep(moveKeep, CAMERA_BASE_POSITION + collsionPlusPosition, collsionMoveStepCounter);
		}

		//baseMoveMaxPosition += collsionPlusPosition;
	}
	else {
		if (collsionMoveStepCounter <= EASEING_COUNTER_MIN) {
			collsionMoveStepCounter = EASEING_COUNTER_MIN;
		}
		else {
			collsionMoveStepCounter -= CAMERA_AREA_COLLSION_OUT_COUNTER_ADD;
		}
		baseMoveMaxPosition = SmoothStep(CAMERA_BASE_POSITION, CAMERA_BASE_POSITION + collsionPlusPosition, collsionMoveStepCounter);
		firstColl = false;
		//baseMoveMaxPosition.z = 0.0f;
		//baseMoveMaxPosition = VGet(1000.0f, 0.0f, 0.0f);
		//collsionPlusPosition = VGet(0.0f, 0.0f, 0.0f);
	}
}

void Camera::AttackCameraStart()
{
	shakeState.power = SHAKE_POWER_GRAVITY;
	shakeState.time = 1;
	cameraState = HIT_CAMERA;
	nowPosition = position;
	nowPosition.z = CAMERA_Z;
	hitCounter = 0.0f;
	plus = 0.0f;
	stopCounter = 0.0f;
	change = false;
	sub = false;
}

void Camera::AttackCamera()
{
	Player::PlayerAttackState a = player->GetPlayerAttackState();
	if (player->GetEnemyHit()) {
		ShakeReady(ShakeGet(SHAKE_POWER_HIT, SHAKE_HIT_TIME));
	}
	if (a == Player::NO_ATTACK) {
		nowPosition = playerPosition;
		if (!change) {
			hitCounter += HIT_EASING_COUNTER_ADD;
			if (hitCounter >= EASEING_COUNTER_MAX) {
				hitCounter = EASEING_COUNTER_MAX;
			}
			if (++stopCounter >= STOP_COUNTER_MAX) {
				stopCounter = 0.0f;
				change = true;
				if (shakeState.time == SHAKE_INFNITY) {
					shakeState.time = 0;
					shakeState.power = 0;
				}
				posKeep = position;
				//hitCounter = 1.0f;
			}
			position = EaseOut(nowPosition, playerPosition + VECTOR3(0, 0, -HIT_CAMERA_Z_SUB), hitCounter);
		}
		else {
			hitCounter -= 0.02f;
			if (hitCounter <= EASEING_COUNTER_MIN) {
				NormalCameraChange();
				hitCounter = EASEING_COUNTER_MIN;
			}
			position = SmoothStep(playerPosition, posKeep, hitCounter);
			position.z = EaseOut(CAMERA_Z, posKeep.z, hitCounter);
		}

	}
	else {
		if (!sub) {
			hitCounter += HIT_EASING_COUNTER_ADD;
			if (hitCounter >= EASEING_COUNTER_MAX) {
				hitCounter = EASEING_COUNTER_MAX;
				//sub = true;

			}
			stopCounter = EASEING_COUNTER_MIN;
			change = false;
			//plus -= player->GetGravityVelocity().y;
			//VECTOR v = VECTOR3(0, plus, 1000.0f) * MGetRotZ(player->GetGravityRot());
			position = EaseOut(nowPosition, playerPosition + VECTOR3(0, 0, -HIT_CAMERA_Z_SUB), hitCounter);
		}
	}
	VECTOR g = player->GetGravityVelocity();
	position += g;
	target = VAdd(CAMERA_BASE_TARGET_ADD, position);
}

void Camera::PlayBeforeCamera()
{
	ShakeReady(ShakeGet(SHAKE_POWER_GRAVITY, SHAKE_INFNITY));
	position = playerPosition + VECTOR3(0, 0, 0);
	target = VAdd(CAMERA_BASE_TARGET_ADD, position);
	beforeCounter++;
	rate = beforeCounter / 230.0f;
	if (rate > EASEING_COUNTER_MAX) {
		rate = EASEING_COUNTER_MAX;
	}
	position.z = EaseInExpo(-START_CAMERA_POSITION, CAMERA_Z, rate);
	target = EaseOut(playerPosition - VGet(-START_TARGET_POSITION, -START_TARGET_POSITION, 100.0f), playerPosition, rate);
	if (player->GetPlayerMode() == Player::NORMAL_MODE) {
		if (shakeState.time == SHAKE_INFNITY) {
			shakeState.time = 0;
			shakeState.power = 0;
		}
	}
	if (shakeState.power >= 0) {
		shakeState.power -= 0.05f;
	}
}

void Camera::GoalAnimCamera()
{
	SetCameraRot();
	if (player->GetGoalState() == Player::Product) {
		position = playerPosition;
		position.y += GOAL_Y_MOVE;
		goalTime++;
		if (goalTime >= GOAL_TIME) {
			goalTime = GOAL_TIME;
		}
		float time;
		time = goalTime / GOAL_TIME;
		position.z = EaseInExpo(CAMERA_Z, RESULT_CAMERA_MAX, time);
		target = VAdd(VGet(0.0f, 0.0f, TAREGT_Z_RESULT), position);
		if (shakeState.time == SHAKE_INFNITY) {
			shakeState.time = 0;
			shakeState.power = 0;
		}
	}
}

void Camera::GoalAfterCamera()
{
	goalTime--;
	if (goalTime <= EASEING_COUNTER_MIN) {
		goalTime = EASEING_COUNTER_MIN;
	}
	float time;
	time = goalTime / GOAL_TIME;
	position.z = EaseIn(CAMERA_Z + GOAL_Z_SUB, GOAL_Z_MAX, time);
	{
		VECTOR upPos = VGet(0.0f, GOAL_BASE_UP_POSITION, 0.0f) * MGetRotZ(worldGravity);
		GravityState worldGravityState = gravityManager->GetWorldGravityState();
		if (worldGravityState == RIGHT_GRAVITY || worldGravityState == LEFT_GRAVITY) {
			position.x = EaseIn(playerPosition.x + upPos.x, playerPosition.x, time);
		}
		else {
			position.y = EaseIn(playerPosition.y + upPos.y, playerPosition.y, time);
		}
	}
	//target.y = position.y;
	ShakeReady(ShakeGet(SHAKE_POWER_GRAVITY, SHAKE_INFNITY));
	shakeTarget = false;
}

void Camera::CameraTarget()
{
	target = VAdd(CAMERA_BASE_TARGET_ADD, position);
	position.z = baseMoveMaxPosition.z;

}
