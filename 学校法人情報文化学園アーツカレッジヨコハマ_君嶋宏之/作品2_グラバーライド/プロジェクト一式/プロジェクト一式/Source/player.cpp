#include "player.h"
#include "playerUi.h"
#include "inputManager.h"
#include "sphereCollision.h"
#include "obbCollision.h"
#include "rayCollision.h"
#include "effectManager.h"
#include "camera.h"
#include "loadManager.h"
#include "stageObjectManager.h"
#include "soundManager.h"
#include "debugScreen.h"
#include "gravityArea.h"
#include "stageScore.h"
#include "../ImGui/imgui.h"
#include "cameraArea.h"
#include "shadow.h"
#include "gravityBrock.h"
#include "../Library/time.h"
#include "loadManager.h"

namespace {
	const float PLAYER_SIZE = 1.0f;
	const int MOVE_SPEED = 15;
	const float WALK_SPEED_RATE_MAX = 1.6f;
	const float WALK_SPEED_MIN = 1.0f;
	const float WALK_SPEED_RATE_ADD = 0.01f;
	const float GRAVITY_ADD_RATE_PLAY_AFTER = 0.5f;
	//JUMP関連の定数
	const float G = -0.3f;
	const float JUMP_SPEED = 21.0f;
	const int JUMP_NUM = 1;
	const float GROUND_POS = 0.0f;
	const float JUMP_MAX = 60.0f;
	const float JUMP_SPEED_ADD_RATE = 0.4f;
	//攻撃関連の定数
	const int ATTACK_TIME = 60;
	const float CENTER_POSITION_COM = 247.0f;
	const int PLAYER_DAMAGE = 1;
	const int PLAYER_INVINCIBLE = 120;
	const VECTOR MOVE_POSITION = VGet(0.0f, 0.0f, -MOVE_SPEED);
	const float BUMPER_SPEED_MAX = 200.0f;
	//const int PLAYER_HP_MAX = 3;
	//その他定数
	const float ONE_SECOND = 60.0f;
	const float ONE_SECOND_DELTA = 1.0f;
	const int GRAVITY_MODE_NORMAL_EFFECT = 15;
	const int GRAVITY_MODE_ATTACK_EFFECT = 10;
	const int GRAVITY_MODE_NORMAL_ALOOW_ANIM = 6;
	const int GRAVITY_MODE_ATTACK_ALOOW_ANIM = 8;
	const float EFFECT_NORMALSPEED = 1.0f;
	const float ENEMY_NO_HIT = -1.0f;
	const float LAST_TINE = 1.0f;
	//重力ゲージ関連定数
	const float GRAVITY_BASE_ADD = 0.001f;
	const float GRAVITY_GUAGE_SUB_MAX = 1.0f;
	const float GRAVITY_BASE_SUB_ADD = 0.1f;
	const float GRAVITY_BASE_SUB_ADD_MAX = 5.0f;
	const float GRAVITY_MOVE_ADD_RATE_SPEED = 0.07f;
	const float COMPULSION_GRAVITY_GUAGE_ADD = 5.0f;
	const float GRAVITY_USE_BASE_ADD = 0.05f;
	const float GRAVITY_USE_ATTACK = 0.02f;
	const int NO_RAND_EFFECT_AMONG = 30;
	const float GUAGE_MAX = 200.0f;
	const float GUAGE_HEAL_WAIT = 10.0f;
	const float GUAGE_WAIT_TIME = 60.0f;
	const float GUAGE_WAIT_TIME_ONE = 1.0f;
	const float GUAGE_ALL_HEAL_GROUND = 15.0f;
	const float GUAGE_ALL_HEAL_ATTACK_HIT = 30.0f;
	const float GUAGE_ALL_HEAL_ITEM = 20.0f;
	const float GUAGE_ALL_HEAL_ITEM_HEAL_NUM = 250.0f;
	//イージング関係定数
	const float EASING_MAX = 1.0f;
	const float EASING_MIN = 0.0f;
	//--------------------------------------
	const float ALPHA_BLEND_SUB = 0.2f;
	//UI関係
	const float SCREEN_EDGE_SUB = 20.0f;
	const float SCREEN_EDGE_SUB_2 = 15.0f;
	//スティック関連定数
	const float STICK_WALK = 0.9f;
	//慣性関係定数
	const float GROUNDJUMP_INERTIA_TIME = 60.0f;
	const float WALK_INERTIA_TIME = 30.0f;
	const float INERTIA_ADD = 2.0f;
	const float RAIL_INERTIA = 5.0f;
	const float RAIL_INTERIA_SUB_RATE = 0.95f;
	const float GRAVITY_INTERIA_DIV = 3.0f;
	const float GRAVITY_INTERIA_BASE_NUM = -3.0f;
	const float GRAVITY_INTERIA_RATE = 2.0f;
	const float ENEMY_HIT_INTERIA_MAX = 150.0f;
	const float INTERIA_DIV = 6.0f;
	//アニメーション制御系定数
	const float LAND_WALT_TIME = 15.0f;
	const float WALK_ANIM_SPEED = 0.02f;
	const float JUMPUP_JUMPDOWN_ANIM_CHANGE = 10.0f;
	//重力関係定数
	const float GRAVITY_MOVE_SPEED_MAX = 45.0f;
	const float ATTACK_SPEED_MAX = -75.0f;
	const float GRAVITY_CHANGE_ROTATE_SPEED = 0.02f;
	const float GRAVITY_CHANGE_ROTATE_BASE_NUM = 0.04f;
	const float GRAVITY_RATE_BASE = 1.5f;
	const float GRAVITY_RATE_ADD_ATTACK = 0.02f;
	//shake系定数
	const float GRAVITY_AMONG_SHAKE_POWER = 20.0f;
	const int SHAKE_INFINITY = -1;
	const  ShakeState NO_GRAVITY_SHAKE = ShakeGet(40.0f, 40.0f);
	const float ATTACK_SHAKE_POWER = 40.0f;
	const float ATTACK_SHAKE_TIME = 30.0f;
	const float ENEMY_HIT_FIRST_SHAKE = 20.0f;
	const float ENEMY_HIT_FIRST_TIME = 30.0f;
	const float ENEMY_HIT_ALOT_SHAKE = 20.0f;
	const float ENEMY_HIT_ALOT_TIME = 15.0f;
	const float BROKEN_BROCK_HIT_TIME = 1;
	const float BROKEN_BROCK_HIT_POWER = 10;
	const float ENEMY_DAMAGE_HIT_TIME = 30;
	const float ENEMY_DAMAGE_HIT_POWER = 10;
	const ShakeState SHAKE_SPICK = ShakeGet(10, 10);
	const ShakeState SHAKE_WORLD_CHANGE = ShakeGet(30.0f, 30.0f);
	//重力チェンジ関数
	const float GRAVITY_CHANGE_UP_POS = 20.0f;
	//コントローラー関係定数
	const int ATTACK_START_POWER = 100;
	const int ATTACK_START_FRAME = 20;
	const int ATTACK_HIT_CONTROL_POWER = 150;
	const int ATTACK_HIT_CONTROL_SECOND = 1;
	//敵ヒット系定数
	const float ENEMY_HIT_EASING_COUNTER_PLUS = 0.05f;
	//敵ダメージ系定数
	const VECTOR ENEMY_DAMAGE_MOVE_SPEED = VGet(0.0f, 60.0f, 60.0f);
	const float ENEMY_DAMAGE_MOVE_TIME = 20.0f;
	const VECTOR SPICK_DAMAGE_MOVE = VGet(0.0f, 30.0f, 0.0f);
	const float SPICK_MOVE_RATE = 50.0f;
	const float SPICK_ROTATION_CHANGE = 0.2f;
	const float SPICK_MOVE_ROTATION_CHANGE = 0.1f;
	const float ENEMY_DAMAGE_SPEED = 40.0f;
	//start系定数
	const float START_FIRST_ANGLE = 90.0f;
	const float START_ADD_ANGLE = 1.0f;
	const float START_FINISH_ANGLE = 180.0f;
	//バンパー系定数
	const float BUMPER_ATTACK_RATE = 2.0f;
	const float BUMPER_NORMAL_RATE = 3.0f;
	const float BUMPER_CENTER_DIST = 100.0f;
	const float BUMPER_UP_VEC = 130.0f;
	const VECTOR BUMPER_BASE_VEC = VGet(-80.0f, 50.0f, 0.0f);
	const float BUMPER_NO_COLLISION = 10.0f;
	const float BUMPER_NO_COLLISION_GRAVITY = 20.0f;
	//レール関係定数
	const float CHANGE_RAIL_TIME = 10.0f;
	const float RAIL_UP_POS = 250.0f;
	const float RAIL_MAX_SPEED = 60.0f;
	//ゴール関係定数
	const float GOAL_PRODUCT_TIME = 170.0f;
	const float GOAL_AFTER_RATE = 2.0f;
	const float GOAL_MAX = 80.0f;
}

Player::Player(SceneBase* _scene) :CharaBase(_scene)
{
	//hModel = loadManager->FindLoadDataObject<ModelData>()->GetHandle("PLAYER");

	position = VGet(-2800, 0, 200);//-2800//10000

	scale = VGet(PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE);
	//rotate = VGet(gravityRotation, DX_PI_F / 2, 0);

	jumpSpeed = 0.0f;
	jumpNum = JUMP_NUM;
	
	playerUi = _scene->CreateGameObject<PlayerUi>();
	_scene->SetDrawOrder(playerUi, 150);

	inputManager = commonFind->CommonFindObject<InputManager>();
	//camera = _scene->FindGameObject<Camera>();

	gravityAfterChangeRotateCounter = 0;
	gravityAfterRotate = 0.0f;

	rotateZAdd = 0.04f;

	attackState = NO_ATTACK;

	hp = PLAYER_HP_MAX;

	playerUi->SetHp(PLAYER_HP_MAX);
	gravityMode = UP_GRAVITY_START;
	playerUi->PlayerGravityStateInit(gravityMode);
	for (int i = 0; i < G_DIRECTION_NUM; i++)
	{
		canGravityDirection[i] = true;
	}
	gActive = true;

	size = VOne * 495;

	sound = SceneManager::CommonScene()->FindGameObject<SoundManager>();
	soundCounter = 0;
	walk = false;

	type = (int)ObjectType::PLAYER;

	//sound->PlaySdBgm("");

	sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::PLAYER);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GOAL);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_SPIKE);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_POINT);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_SWITCH);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_BUMPER);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::ITEM_GHEAL);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::ITEM_HEAL);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_RAIL);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY_AREA);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_BROKEN);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_COIN);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::CAMERA_AREA);
	playerLandSphereCollision = new SphereCollision(this);
	playerLandSphereCollision->SetTagMe(COLLISION_OBJECT_KIND::LAND);
	playerLandSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);
	playerLandSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
	playerLandSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_SPIKE);
	playerLandSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_BROKEN);
	playerLandSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR);
	//playerLandSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_BUMPER);

	attackSphereCollision->SetTagMe(COLLISION_OBJECT_KIND::PLAYER_ATTACK);
	attackSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);
	attackSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_BROKEN);

	playerHeadSphereCollision = new SphereCollision(this);
	playerHeadSphereCollision->SetTagMe(COLLISION_OBJECT_KIND::HEAD);
	playerHeadSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
	playerHeadSphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR);

	obbCollision->SetTagMe(COLLISION_OBJECT_KIND::NONE);
	obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::NONE);

	/*playerRayCollision = new RayCollision(this);
	playerRayCollision->SetRayDirecition(RAY_DIRECTION_KIND::DOWN);
	playerRayCollision->SetTagMe(COLLISION_OBJECT_KIND::PLAYER_RAY);
	playerRayCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);
	playerRayCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);*/

	centerPosition = VAdd(position, VTransform(VGet(0.0f, CENTER_POSITION_COM, 0.0f), GetRotateMatrix()));
	GuageAmontReset();

	goal = false;

	rotate = VGet(0.0f, 0.0f, 0.0f);

	moveRotate = rotate;

	//moveMax = MaxGet(-35.0f, -35.0f);
	moveMax = MaxGet(-25.0f, -50.0f);
	//jumpOnes = true;
	SpickMoveReset();
	attackHitTime = 0;
	attackAngle = 0;
	jumpDown = false;
	/*allowAngle = DX_PI_F / 2;
	gravityMode = RIGHT_GRAVITY_START;
	mode = GRAVITY_MOVE_MODE;
	GracityMoveModeChange();*/

	amongPointPosition = position;
	stageObjectManager = _scene->FindGameObject<StageObjectManager>();

	centerPositionAmong = CENTER_POSITION_COM;
	amongWorldGravity = gravityManager->GetWorldGravityState();

	effectNormalRotate = VGet(0.0f, gravityRotation, 0.0f);
	effectNormalSize = VGet(1.0f, 1.0f, 1.0f);
	effect2DSize = V2Get(1.0f, 1.0f);
	effect2DRotate = V2Get(0.0f, 0.0f);
	blendTime = 0.0f;
	walkAnimeSpeedCounter = 0.0f;
	//debugNoDamage = false;
	ResetAftimage();
	aftimageNum = 0;
	aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
	movePointer = nullptr;
	mode = NORMAL_MODE;
	playerUi->PlayerMode(mode);
	goalProductCounter = 0;
	landPositionAdd = 130.0f;

	stageScore = _scene->FindGameObject<StageScore>();
	walkSpeedAdd = 1.0f;
	groundRand = true;
	noLandEffectTime = 0;
	gravitySlideCounter = 0.0f;
	healG = false;
	enemyHitAddCounter = -1.0f;

	//shadow = _scene->CreateGameObject<Shadow>();
	//_scene->SetDrawOrder(shadow, 120);
	guageDownNumPlus = 0.0f;
	playStart = false;
	loseState = No_Lose;
	gravityNoChangeModeCounter = 0.0f;

	LoadManager* load = SceneManager::CommonScene()->FindGameObject<LoadManager>();

	for (int i = 0; i < RAIL_MOVE; i++) {
		switch (i)
		{
		case 0:
			controlAButtonImage[i] = LoadGraph("data\\texture\\A_Button1.png");
			controlBButtonImage[i] = LoadGraph("data\\texture\\B_Button1.png");
			break;
		case 1:
			controlAButtonImage[i] = LoadGraph("data\\texture\\A_Button2.png");
			controlBButtonImage[i] = LoadGraph("data\\texture\\B_Button2.png");
			break;
		case 2:
			controlAButtonImage[i] = LoadGraph("data\\texture\\A_Button2.png");
			controlBButtonImage[i] = LoadGraph("data\\texture\\B_Button1.png");
			break;
		default:
			break;
		}
		
	}
	rTriggerImage = LoadGraph("data\\texture\\RT.png");
	aButtonImage = LoadGraph("data\\texture\\アセット 3.png");
	bButtonImage = LoadGraph("data\\texture\\アセット 1.png");
	rtImage = LoadGraph("data\\texture\\アセット 5.png");

	uiDraw = true;
}

Player::~Player()
{
	
	playerUi->DestroyMe();

	delete playerLandSphereCollision;
	delete playerHeadSphereCollision;
	delete playerRayCollision;
	//anime->AnimeRemove();
	/*delete sphereCollision;
	delete obbCollision;*/
	delete anime;
	if (hModel > 0) {
		//MV1DeleteModel(hModel);
	}

	for (int i = 0; i < RAIL_MOVE; i++) {
		DeleteGraph(controlAButtonImage[i]);
		DeleteGraph(controlBButtonImage[i]);
	}
	DeleteGraph(rTriggerImage);
	DeleteGraph(aButtonImage);
	DeleteGraph(bButtonImage);
	DeleteGraph(rtImage);
}

void Player::Update()
{
	if (gameState == GameManager::MENU) {
		return;
	}
	effectNormalRotate = rotate;
	//effectManager->CreateEffekseer(position, nullptr, "NO_LAND", 1.0f, 60, false, effectNormalSize, effectNormalRotate);
	anime->Update();
	previewAnimeInformation = anime->GetAnimesion();
	if (anime->GetNext() != PlayerAnimesionKind::None) {
		beforeAnimeInformation = anime->GetBeforeAnimesion();
		if (beforeAnimeInformation.time == 0.0f) {
			AnimePlayChange(anime->GetNext());
		}
	}
#ifdef _DEBUG
	/*ImGui::Begin("PLAYER_PLUS");
	ImGui::DragFloat3("plus", &sendPos.x, 0.0f, -2000.0f, 2000.0f);
	if (ImGui::Button("send")) {
		cameraMovePositionKeep = sendPos;
	}
	ImGui::End();*/
	//printfDx("%d", anime->GetNext());
#endif 
	blendTime = previewAnimeInformation.totalTime - previewAnimeInformation.time;
	if ((hitStop.time > 0 || shake.time > 0 || hitStop.time == SHAKE_INFINITY || shake.time == SHAKE_INFINITY) && !moveCan) {
		HitStopShake();
		if (mode != GRAVITY_MOVE_MODE && charaState != ATTACK_HIT) {
			CharaRotationSet();
		}
		velocity = VZero;
		PositionSetDraw();
		NoGround();
		if (attackState == ATTACK1) {
			if (hitStop.time <= 0.0f) {
				ShakeStart(ShakeGet(40, -1),true);
			}
		}
		//centerPosition = VAdd(position, VTransform(VGet(0.0f, CENTER_POSITION_COM, 0.0f), GetRotateMatrix()));
		return;
	}
	if ((hitStop.time > 0 || shake.time > 0 || hitStop.time == SHAKE_INFINITY || shake.time == SHAKE_INFINITY) && moveCan) {
		HitStopShake();
		velocity = VZero;
	}
	if (!compulsionGravity) {
		if (beforeCompulsionGravity) {
			if (mode == GRAVITY_CHANGE_MODE) {
				NormalModeStateChange();
				//InteriaStart(gravitySlide, 60);
				//AnimePlayChange(PlayerAnimesionKind::Jump_Down);
				gravityNoChangeModeCounter = 6.0f;
			}
		}
		beforeCompulsionGravity = false;
		
	}
	if (gravityNoChangeModeCounter > 0) {
		gravityNoChangeModeCounter--;
	}
	else {
		gravityNoChangeModeCounter = 0.0f;
	}
	switch (gameState)
	{
	case GameManager::PLAY_BEFORE:
		if (mode == GRAVITY_MOVE_MODE) {
			effectManager->CreateEffekseer(centerPosition, &centerPosition, "ENERGYWAVE", 1.0f, 10, false, effectNormalSize, effectNormalRotate);
			ZMove();
			
		}
		else {
			playStart = true;
		}
		/*//rotate.z += 0.2f;
		gravityAfterRotate = gravityRotation + rotate.x;
		//GravityMove(guadeDown + attackGuadeDown);
		GravityMoveRotateChange();
		guadeDown += 0.001f * (gravityAddRate + guageDownBase);
		gravityAddRate += 0.2f;
		guageDownBase += 0.2f;
		centerPosition = VAdd(position, VTransform(VGet(0.0f, CENTER_POSITION_COM, 0.0f), GetRotateMatrix()));*/
		break;
	case GameManager::PLAYING:
		CharaBase::Update();
		
		if (debugStop) {
			return;
		}
		switch (charaState)
		{
		case CharaBase::SPICK_DAMAGE:
			moveRotate = spickRotate;
			SpickDamageMove();
			break;
		case CharaBase::ENEMY_DAMAGE:
			//Move();
			Jump();
			if (mode != GRAVITY_MOVE_MODE) {
				CharaRotationSet();
			}
			DamageMove();
			break;
		case CharaBase::NORMAL:
			BumperHitMove();
			centerPosition.z = 0;
			if (changeRailTime > 0) {
				changeRailTime--;
			}
			switch (mode)
			{
			case Player::NORMAL_MODE:
				InteriaGravity();
				Move();
				Jump();
				CharaRotationSet();
				changeGravity = NO_CHANGE;
				GravityMode();
				break;
			case Player::GRAVITY_CHANGE_MODE:
				InteriaGravity();
				GravityMode();
				gravityAdd = BASE_GRAVITY_VELOCITY;
				AttackReady();
				break;
			case Player::GRAVITY_MOVE_MODE:
				startEffectCounter++;
				if (attackState == ATTACK1) {
					effectNum = GRAVITY_MODE_ATTACK_EFFECT;
				}
				else {
					effectNum = GRAVITY_MODE_NORMAL_EFFECT;
				}
				if (startEffectCounter % effectNum == 0) {
					EffectManager::AloowEffectData data = effectManager->GetEffectData(gravityManager, aloowKeepState);
					effectManager->StartAloow(data.position, aloowKeepState, data.angle,600.0f);
				}
				if (startEffectCounter % 30) {
					animeNum++;
					if (attackState == ATTACK1) {
						if (animeNum >= GRAVITY_MODE_ATTACK_ALOOW_ANIM) {
							animeNum = GRAVITY_MODE_ATTACK_ALOOW_ANIM;
						}
					}
					else {
						if (animeNum >= GRAVITY_MODE_NORMAL_ALOOW_ANIM) {
							animeNum = GRAVITY_MODE_NORMAL_ALOOW_ANIM;
						}
					}
					
				}
				
				effectManager->CreateEffekseer(centerPosition, &centerPosition, "ENERGYWAVE", EFFECT_NORMALSPEED, 10,false, effectNormalSize, effectNormalRotate);
				//effectManager->CreateEffekseer(headPosition, nullptr, "SMOKE", 1.0f, 30);
				
				gravityAfterRotate = gravityRotation + rotate.x;
	
				if (!compulsionGravity) {
					GravityMove(guadeDown + attackGuadeDown);
					guadeDown += GRAVITY_BASE_ADD * (guageDownBase);
					if (guadeDown >= GRAVITY_GUAGE_SUB_MAX) {
						guadeDown = GRAVITY_GUAGE_SUB_MAX;
					}
					guageDownBase += GRAVITY_BASE_SUB_ADD;
					if (guageDownBase >= GRAVITY_BASE_SUB_ADD_MAX) {
						guageDownBase = GRAVITY_BASE_SUB_ADD_MAX;
					}
				}
				GravityMoveRotateChange();
				AttackReady();
				GravityMode();
				//ControlVibrationStart(VibrationGet(8, 1));
				gravityAddRate += GRAVITY_MOVE_ADD_RATE_SPEED;
				if (enemyHitAddCounter > ENEMY_NO_HIT) {
					enemyHitAddCounter += enemyHitCounterPlus;
					moveMax.playerChangeGravityAddMax = EaseOut(gravityMaxSpeed, -50.0f, enemyHitAddCounter);
					if (enemyHitAddCounter >= EASING_MAX) {
						//enemyHitAddCounter = -1.0f;
						/*gravityMaxSpeed = -0.0f;
						enemyHitCounterPlus *= -1.0f;
						*/
						enemyHitAddCounter = ENEMY_NO_HIT;
						GravityMoveFinish();
						attackState = NO_ATTACK;
						charaState = NORMAL;
					}
					else {
						gravityAdd.y = moveMax.playerChangeGravityAddMax;
					}

				}
				break;
			case Player::RAIL_MOVE:
				changeRailTime--;
				if (!rail) {
					mode = NORMAL_MODE;
					gActive = true;
				}
				if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_SPACE, XINPUT_BUTTON_A)) {
					if (jumpNum > 0) {
						jumpNum--;
						jumpSpeed = JUMP_SPEED;
						//centerPosition += VGet(0.0f, 200.0f, 0.0f) * MGetRotZ(gravityManager->GetWorldGravity());
						jumpKeepPosition = centerPosition;
						jump = true;
						sound->PlaySe("ジャンプ");
						effectManager->CreateEffekseer(position, nullptr, "JUMP", EFFECT_NORMALSPEED, ONE_SECOND, false, effectNormalSize, effectNormalRotate);
						AnimePlayChange(PlayerAnimesionKind::Jump_Up);
						mode = NORMAL_MODE;
						gActive = true;
						changeRailTime = 20;
					}
				}
				rail = false;
				RailMove();
				break;
			default:
				break;
			}

			switch (attackState)
			{
			case Player::NO_ATTACK:
				break;
			case Player::ATTACK1:
				Attack();
				//ShakeStart(ShakeGet(50, -1), true);
				//attackPosition = VAdd(centerPosition, VGet(0.0f, -CENTER_POSITION_COM, 0));
				attackPosition = VAdd(centerPosition, VTransform(VGet(0.0f, -centerPositionAmong * scale.y, 0), GetRotateMatrix()));
				break;
			}
			//centerPosition = VAdd(position, VTransform(VGet(0.0f, CENTER_POSITION_COM, 0.0f), GetRotateMatrix()));
			GravityChange();
			DebugPlayer();
			break;
		case CharaBase::ATTACK_HIT:
			/*if (attackHitTime > 0) {
				AttackHitMove();
				PositionSetDraw();
				NoGround();
				if (attackHitTime % 4 == 0) {
					effectManager->CreateEffekseer(position, nullptr, "PLAYER_KICK_HIT", 1.0f, 40, false, effectNormalSize, effectNormalRotate);
				}
				return;
			}*/
			
			break;

		default:
			break;
		}
		if (charaState != SPICK_DAMAGE) {
			if (scale.y <= PLAYER_SIZE) {
				sizeCounter++;
				if (sizeCounter >= ONE_SECOND * 3) {
					sizeCounter = 0;
					scale = VGet(PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE);
				}
			}
			/*if (scale.y <= PLAYER_SIZE / 3.0f) {
				Dead();

			}*/
		}
		
		//centerPosition = VAdd(position, VTransform(VGet(0.0f, CENTER_POSITION_COM, 0.0f), GetRotateMatrix()));
		break;
	case GameManager::MENU:
		//gravityInfluence = abs(gravityAdd.y * objectGravityRate);
		//PositionMove(VGet(0.0f, gravityInfluence, 0.0f));
		//AnimePlayChange(PlayerAnimesionKind::Wait);
		break;
	case GameManager::LOSE:
#if false
		switch (loseState)
		{
		case Player::Lose:

			CharaBase::Update();
			CharaRotationSet();
			//rotate.z = DX_PI_F;
			gravityAddRate = 4.0f;
			objectGravityRate = 0.0f;
			//alpha -= 0.05f;
			//if (alpha <= 0.0f) {
				//alpha = 0.0f;
			//}
			break;
		case Player::Respown:
			gravityInfluence = abs(gravityAdd.y * objectGravityRate);
			PositionMove(VGet(0.0f, gravityInfluence, 0.0f));
			if (--loseTime > 0) {
				if (loseTime == 1) {
					ReSpown();
					alpha = 1.0f;
				}
			}
			break;
		}
#else
		switch (loseState)
		{
		case Player::Die:
			dieCounter -= Time::DeltaTime();
			if (dieCounter < EASING_MIN) {
				DieStateChange();
			}
			break;
		case Player::Lose:

			CharaBase::Update();
			CharaRotationSet();
			alpha -= ALPHA_BLEND_SUB;
			if (alpha <= EASING_MIN) {
				alpha = EASING_MIN;
			}
			break;
		case Player::Respown:
			//gravityInfluence = abs(gravityAdd.y * objectGravityRate);
			loseMoveTime += loseMoveAdd;
			if (loseMoveTime >= EASING_MAX) {
				loseMoveTime = EASING_MAX;
			}
			centerPosition = EaseInOut(loseCenter,loseFinalPosition,loseMoveTime);
			if (--loseTime > 0) {
				if (loseTime % 4 == 0) {
					effectManager->CreateEffekseer(centerPosition, nullptr, "PLAYER_RESPOWN", EFFECT_NORMALSPEED, loseTime, true);
				}
				if (loseTime == LAST_TINE) {
					ReSpown();
					alpha = 1.0f;
					effectManager->StopEffekseer("PLAYER_RESPOWN", false);
				}
			}
			break;
		}
#endif
		break;
	case GameManager::GOAL_FRONT_ANI:
		switch (goalState)
		{
		case Player::PlayingAfterMove:
			CharaRotationSet();
			goalProductCounter = ONE_SECOND_DELTA;
			goalTime -= Time::DeltaTime();
			if (goalTime <=EASING_MIN) {
				GoalProductStart();
				goalTime = 0.0f;
				sound->PlaySe("よし");
			}
			{
				GravityState wState = gravityManager->GetWorldGravityState();
				if (wState == DOWN_GRAVITY || wState == UP_GRAVITY) {
					centerPosition.x = EaseOut(goalPosition.x, goalNowPos.x, goalTime);
				}
				else {
					centerPosition.y = EaseOut(goalPosition.y, goalNowPos.y, goalTime);
				}
			}
			
		/*GravityMove(10.0f);
			if (mode != GRAVITY_MOVE_MODE) {
				gravityRotation = gravityManager->GetWorldGravity();
				CharaRotationSet();
				GoalProductStart();
			}*/
			/*goalProductCounter = 1;
			
			if (groundRand) {
				GoalProductStart();
			}*/
			break;
		case Player::Product:
			AnimePlayChange(PlayerAnimesionKind::Goal);
			goalProductCounter--;
			break;
		default:
			break;
		}
		break;
	case GameManager::PLAY_AFTER:
	case GameManager::SCENE_CHANGE:
		effectManager->CreateEffekseer(centerPosition, &centerPosition, "ENERGYWAVE", EFFECT_NORMALSPEED, 10, false, effectNormalSize, effectNormalRotate);
		gravityAddRate += GRAVITY_ADD_RATE_PLAY_AFTER;
		break;
	default:
		break;
	}
	GuageHeal();
	PositionSetDraw();
	NoGround();
	
	if (mode == GRAVITY_MOVE_MODE) {
		SetAftimageNum();
	}
	if (noDamageTime % 10 == 1) {
		sound->PlaySe("無敵時間");
	}
	compulsionGravity = false;
	noGravityArea = false;
	if (gravityLand) {
		moveMax = MaxGet(-25.0f, -50.0f);
		objectGravityRate = GRAVITY_RATE_BASE;
		gravityLand = false;
		GuageAmontReset();
	}
	//shadow->SetShadowPosition(rayP + position);
}

void Player::Draw()
{
	Object::Draw();
	CharaBase::Draw();


	
	VECTOR screenPos = ConvWorldPosToScreenPos(centerPosition);
	
	
	//screenPos.z = -4000.0f;
	/*if (gameState == GameManager::PLAYING) {
		
	}*/
	if (mode == GRAVITY_CHANGE_MODE) {
		playerUi->AloowDraw(screenPos,1);
	}
	if (mode == GRAVITY_MOVE_MODE) {
		if (gameState == GameManager::PLAYING) {
			playerUi->AloowDraw(screenPos,animeNum);
			for (int i = 0; i < aftimageNum; i++) {
				if (aftimagePosition[i].z == -1) {
					continue;
				}
				//drawBlendNum += 10;
				MV1SetMatrix(hModel, GetAftimageMatrix(aftimagePosition[i], aftimageRotate[i], scale));
				MV1SetDifColorScale(hModel, rgb.ColorF(EaseIn(aftimageAlpha, 0.0f, 1.0f - i / (float)aftimageNum)));
				MV1DrawModel(hModel);
			}
			//drawBlendNum = 90;
			//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, drawBlendNum);
		}
	}
	
	/*DrawFormatString(100.0f, 1000.0f, 0x00ff00, "%.1f", blendTime);
	VECTORDraw(V2Get(100, 600), centerPosition, nullptr);*/

	//if (attackState == ATTACK1) {
	//	VECTOR collPos = Transform(VGet(0, -100, 0));
	//	collPos = VAdd(collPos, position);
	//	//DrawCapsule3D(position, collPos, 200, 10, 0xffffff, 0xffffff, false);
	//	//DrawCapsule3D(position, attackPosition, 200, 10, 0xffffff, 0xffffff, false);
	//}

	VECTORDraw(V2Get(1000, 200), vec, nullptr);
	//DrawFormatString(500, 500, 0xff0000, "anime-> %.1f", MV1GetAttachAnim(hModel, anime->GetAnimesion().index));
	if (debugNoDamage) {
		//SetFontSize(30);
		//DrawFormatString(1200, 100, 0xff0000, "無敵！！！！");
		//SetFontSize(DEFAULT_FONT_SIZE);
	}

	int color = 0x000000;
	if (rayHit)
		color = 0x000000;
	
	//shadowPosition = VTransform(position,);
	//DrawSphere3D(VAdd(rayP, VGet(1.0f, 0.0f, 0.0f)), 100, 16, color, 0x000000, true);

	if (uiDraw) {
		if (gameState == GameManager::PLAYING) {
			int index = (int)mode;
			if (index >= 3) {
				index = 0;
			}
			VECTOR2I aButtonSize;
			GetGraphSize(controlAButtonImage[index], &aButtonSize.x, &aButtonSize.y);
			VECTOR2I bButtonSize;
			GetGraphSize(controlBButtonImage[index], &bButtonSize.x, &bButtonSize.y);
			VECTOR2I aButtonTrueSize = aButtonSize;
			VECTOR2I bButtonTrueSize = bButtonSize;
			VECTOR2I rSize;
			GetGraphSize(rTriggerImage, &rSize.x, &rSize.y);

			aButtonSize *= 0.5f;
			bButtonSize *= 0.5f;
			const int SIZE_X = 47;
			DrawRectRotaGraphF(SCREEN_WIDTH - SCREEN_EDGE_SUB - bButtonSize.x / 2, SCREEN_HEIGHT - SCREEN_EDGE_SUB - bButtonSize.y / 2, SIZE_X, 0, aButtonTrueSize.x * 2, aButtonTrueSize.y, 0.5f, 0.0f, controlBButtonImage[index], true, false);
			DrawRectRotaGraphF(SCREEN_WIDTH - SCREEN_EDGE_SUB - aButtonSize.x / 2 - bButtonSize.x, SCREEN_HEIGHT - SCREEN_EDGE_SUB - aButtonSize.y / 2, SIZE_X, 0, bButtonTrueSize.x * 2, bButtonTrueSize.y, 0.5f, 0.0f, controlAButtonImage[index], true, false);
			if (index == 2 || index == 1) {
				if (attackState == NO_ATTACK) {
					VECTOR2I rButtonTrueSize = rSize;
					rSize *= 0.5f;
					DrawRectRotaGraphF(SCREEN_WIDTH - SCREEN_EDGE_SUB - aButtonSize.x - bButtonSize.x - rSize.x / 2, SCREEN_HEIGHT - SCREEN_EDGE_SUB - rSize.y / 2, SIZE_X + 30, 0, rButtonTrueSize.x * 2, rButtonTrueSize.y, 0.5f, 0.0f, rTriggerImage, true, false);
					DrawRotaGraph(SCREEN_WIDTH - aButtonSize.x - bButtonSize.x - rSize.x, SCREEN_HEIGHT - SCREEN_EDGE_SUB_2 - bButtonSize.y / 2, 1.3f, 0.0f, rtImage, true);
				}

				
			}
			DrawRotaGraph(SCREEN_WIDTH - SCREEN_EDGE_SUB_2 - bButtonSize.x, SCREEN_HEIGHT - SCREEN_EDGE_SUB - bButtonSize.y / 2, 1.0f, 0.0f, bButtonImage, true);
			DrawRotaGraph(SCREEN_WIDTH - SCREEN_EDGE_SUB_2 - aButtonSize.x - bButtonSize.x, SCREEN_HEIGHT - SCREEN_EDGE_SUB - bButtonSize.y / 2, 1.0f, 0.0f, aButtonImage, true);

		}
	}
}

void Player::Move()
{
	if (inputManager->GetIsKeyboardPushing(KEY_INPUT_RIGHT) || inputManager->GetStickKnocking(STICK_WALK).leftStick == S_RIGHT ||inputManager->GetStickKnocking(STICK_WALK).leftStick == S_DOWN_RIGHT || inputManager->GetStickKnocking(STICK_WALK).leftStick == S_UP_RIGHT) {
		/*if (direction == LEFT) {
			/walkSpeedAdd = 1.0f;
			walkAnimeSpeedCounter = 0.0f;
		}*/
		DirectionChange(-DX_PI_F / 2, RIGHT);
		PositionMove(MOVE_POSITION * walkSpeedAdd);
		walk = true;
		slideCounter = slideCounterMax;
	}
	else if (inputManager->GetIsKeyboardPushing(KEY_INPUT_LEFT) || inputManager->GetStickKnocking(STICK_WALK).leftStick == S_LEFT || inputManager->GetStickKnocking(STICK_WALK).leftStick == S_DOWN_LEFT || inputManager->GetStickKnocking(STICK_WALK).leftStick == S_UP_LEFT) {
		/*if (direction == RIGHT) {
			walkSpeedAdd = 1.0f;
			walkAnimeSpeedCounter = 0.0f;
		}*/
		DirectionChange(DX_PI_F / 2, LEFT);
		PositionMove(MOVE_POSITION * walkSpeedAdd);
		walk = true;
		slideCounter = slideCounterMax;
	}
	else {
		if (walk) {
			slide = (MOVE_POSITION * walkSpeedAdd);
			slideCounter = EASING_MIN;
			if (groundJump) {
				stopJump = true;
				slideCounterMax = GROUNDJUMP_INERTIA_TIME;
			}
			else{
				stopJump = false;
				slideCounterMax = WALK_INERTIA_TIME;
			}
		}
		if (slideCounter < slideCounterMax) {
			slideCounter++;
			float r = slideCounter / slideCounterMax;
			VECTOR movePos;
			if (stopJump) {
				if (!jump) {
					slideCounter += INERTIA_ADD;
					r = slideCounter / slideCounterMax;
					movePos = EaseInExpo(slide, VZero, r);
				}
				else {
					movePos = EaseInExpo(slide, VZero, r);
				}
			}
			else {
				movePos = EaseOut(slide, VZero, r);
			}
			
			PositionMove(movePos);
		}
		WalkReset();
		if (previewAnimeInformation.animeKind == PlayerAnimesionKind::Land) {
			if (previewAnimeInformation.totalTime - previewAnimeInformation.time <= LAND_WALT_TIME) {
				AnimePlayChange(PlayerAnimesionKind::Wait);
			}
		}
		else if(jump){
			
		}
		else {
			if (loseState != Die) {
				AnimePlayChange(PlayerAnimesionKind::Wait);
			}
		}
	}
	if (walk && !jump && mode == NORMAL_MODE) {
		soundCounter++;
		walkSpeedAdd += WALK_SPEED_RATE_ADD;
		if (walkSpeedAdd >= WALK_SPEED_RATE_MAX) {
			walkSpeedAdd = WALK_SPEED_RATE_MAX;
		}
		if (previewAnimeInformation.animeKind == PlayerAnimesionKind::Land) {
			if (previewAnimeInformation.totalTime - previewAnimeInformation.time <= 10) {
				AnimePlayChange(PlayerAnimesionKind::Walk);
			}
		}
		else {
			if (walkAnimeSpeedCounter <= EASING_MAX) {
				walkAnimeSpeedCounter += WALK_ANIM_SPEED;
			}
			anime->AnimesionChangeSpeed(walkAnimeSpeedCounter);
			AnimePlayChange(PlayerAnimesionKind::Walk);
		}
		if (soundCounter % 10 == 0) {
			std::string name = "足音";
			int n = GetRand(2) + 1;
			name = name + std::to_string(n);
			sound->PlaySe(name);
			if (walkSpeedAdd >= WALK_SPEED_RATE_MAX) {
				effectManager->CreateEffekseer(position, nullptr, "SMOKE", EFFECT_NORMALSPEED, 10, false);
			}
		}
	}
	if (!groundRand) {
		//AnimePlayChange(PlayerAnimesionKind::Jump_Down); GRABOROX
		if (!jump) {
			jump = true;
			AnimePlayChange(PlayerAnimesionKind::Jump_Down);
		}
	}
	if (railMoveAdd.z < -RAIL_INERTIA) {
		PositionMove(MOVE_POSITION + railMoveAdd);
		railMoveAdd *= RAIL_INTERIA_SUB_RATE;
	}
	
}

void Player::Jump()
{
	if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_SPACE, XINPUT_BUTTON_A) && groundRand) {
		if (jumpNum > 0) {
			jumpNum--;
			jumpSpeed = JUMP_SPEED;
			//gravityAdd.y = JUMP_SPEED;
			jumpKeepPosition = centerPosition;
			//jump = true;
			sound->PlaySe("ジャンプ");
			effectManager->CreateEffekseer(position, nullptr, "LAND", EFFECT_NORMALSPEED, 15, false, effectNormalSize, effectNormalRotate);
			AnimePlayChange(PlayerAnimesionKind::Jump_Up);
			groundJump = true;
		}
	}
	if ((jump || groundJump) && bumperTime <= 0) {
		gravityInfluence = abs(gravityAdd.y * objectGravityRate);
		if (jumpSpeed >= -JUMP_MAX) {
			jumpSpeed += -JUMP_SPEED_ADD_RATE * gravityAddRate;
		}
		else {
			jumpSpeed = -JUMP_MAX;
		}
		
		if (jumpSpeed < 0) {
			jumpDown = true;
		}
		else {
			jumpDown = false;
		}
		//velocity = VGet(0.0f, 0.0f, 0.0f);
		VECTOR jumpVelo = VGet(0.0f, jumpSpeed + gravityInfluence, 0.0f);
		PositionMove(jumpVelo);
		if (previewAnimeInformation.totalTime - previewAnimeInformation.time <= JUMPUP_JUMPDOWN_ANIM_CHANGE) {
			if (previewAnimeInformation.animeKind == PlayerAnimesionKind::Jump_Down) {
				anime->AnimesionChangeSpeed(0.0f);
			}
			else if(previewAnimeInformation.animeKind == PlayerAnimesionKind::Zero_Gravity){

			}
			else {
				AnimePlayChange(PlayerAnimesionKind::Jump_Down);
			}
		}
	}
}

void Player::GravityChange()
{
	if (playerUi->GetGravityMoveGaugeNum() <= 0.0f && !compulsionGravity) {
		NormalModeStateChange();
		return;
	}
#if false
	//一番最初の方法
	if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && !noGravityArea) {
		if (mode == NORMAL_MODE) {
			GracityChangeModeChange();
			sound->PlaySe("重力起動");
			GracityMoveModeChange();
			//AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
		}
		if (mode == GRAVITY_CHANGE_MODE || mode == GRAVITY_MOVE_MODE) {
			if (mode == GRAVITY_MOVE_MODE) {
				if (attackState == ATTACK1) {
					AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
				}
				else {
					AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
				}
			}
			else {
				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
			}
			GracityMoveModeChange();
			effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
			sound->PlaySe("重力起動");
			moveMax.playerChangeGravityAddMax = -45.0f;
			if (attackState == ATTACK1) {
				attackState = NO_ATTACK;
				attackGuadeDown = 0.0f;
				ResetAftimage();
			}
			aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
			aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
			ShakeStart(ShakeGet(20, -1), true);
			debugNoDamage = false;
		}

	}
	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_V, XINPUT_BUTTON_Y) && !noGravityArea) {
		if (mode == NORMAL_MODE) {
			GracityChangeModeChange();
			sound->PlaySe("重力起動");
			AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
		}
	}
	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && noGravityArea) {
		HitStopStart(HitStopGet(40, 40));
	}
	else if (inputManager->GetIsKeyboardPut(KEY_INPUT_C) || inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B)) {
		/*if (mode == GRAVITY_CHANGE_MODE || mode == GRAVITY_MOVE_MODE) {
			GracityMoveModeChange();
			sound->PlaySe("重力起動");
			if (attackState == ATTACK1) {
				attackState = NO_ATTACK;
				attackGuadeDown = 0.0f;
			}
		}*/
	}
	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_Z, XINPUT_BUTTON_A) && !compulsionGravity) {
		if (mode == GRAVITY_CHANGE_MODE) {
			NormalModeStateChange();
			debugNoDamage = false;
			direction = keepDire;
			//camera->CameraStateChange(Camera::CameraState::GRAVITY_MOVE_CAMERA);
			//camera->ShakeReady(ShakeGet(50, -1));	
		}
		else if (mode == GRAVITY_MOVE_MODE) {
			NormalModeStateChange();
			StopShake();
			debugNoDamage = false;
			direction = keepDire;
		}
	}
#else
	//解除と発動同じボタンBボタンver
	//if (compulsionGravity) {
	//	if (mode == GRAVITY_CHANGE_MODE) {
	//		/*if (mode == GRAVITY_MOVE_MODE) {
	//			if (attackState == ATTACK1) {
	//				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//			}
	//			else {
	//				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//			}
	//		}*/
	//		AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//		GracityMoveModeChange();
	//		effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
	//		sound->PlaySe("重力起動");
	//		moveMax.playerChangeGravityAddMax = -45.0f;
	//		if (attackState == ATTACK1) {
	//			attackState = NO_ATTACK;
	//			attackGuadeDown = 0.0f;
	//			ResetAftimage();
	//		}
	//		aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
	//		aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
	//		ShakeStart(ShakeGet(20, -1), true);
	//		debugNoDamage = false;
	//		gravitySlideCounter = 60.0f;
	//	}
	//	if (mode == GRAVITY_MOVE_MODE) {
	//		if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && !noGravityArea) {
	//			AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//			GracityMoveModeChange();
	//			effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
	//			sound->PlaySe("重力起動");
	//			moveMax.playerChangeGravityAddMax = -45.0f;
	//			if (attackState == ATTACK1) {
	//				attackState = NO_ATTACK;
	//				attackGuadeDown = 0.0f;
	//				ResetAftimage();
	//			}
	//			aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
	//			aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
	//			ShakeStart(ShakeGet(20, -1), true);
	//			debugNoDamage = false;
	//			gravitySlideCounter = 60.0f;
	//		}
	//	}
	//}
	//else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && !noGravityArea) {
	//	if (mode == NORMAL_MODE) {
	//		GracityChangeModeChange();
	//		sound->PlaySe("重力起動");
	//		
	//		//GracityMoveModeChange();
	//		//AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
	//	}
	//	if (mode == GRAVITY_CHANGE_MODE) {
	//		/*if (mode == GRAVITY_MOVE_MODE) {
	//			if (attackState == ATTACK1) {
	//				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//			}
	//			else {
	//				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//			}
	//		}*/
	//		AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//		GracityMoveModeChange();
	//		effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
	//		sound->PlaySe("重力起動");
	//		moveMax.playerChangeGravityAddMax = -45.0f;
	//		if (attackState == ATTACK1) {
	//			attackState = NO_ATTACK;
	//			attackGuadeDown = 0.0f;
	//			ResetAftimage();
	//		}
	//		aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
	//		aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
	//		ShakeStart(ShakeGet(20, -1), true);
	//		debugNoDamage = false;
	//		gravitySlideCounter = 60.0f;
	//	}
	//	else if (mode == GRAVITY_MOVE_MODE) {
	//		GravityMoveFinish();
	//		//againGravityCounter = 10;
	//		//camera->CameraStateChange(Camera::CameraState::GRAVITY_MOVE_CAMERA);
	//		//camera->ShakeReady(ShakeGet(50, -1));	

	//	}

	//}
	//else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_V, XINPUT_BUTTON_Y) && !noGravityArea) {
	//	if (mode == NORMAL_MODE || mode == GRAVITY_MOVE_MODE) {
	//		if (mode == GRAVITY_MOVE_MODE) {
	//			GravityMoveFinish();
	//			StopShake();
	//		}
	//		GracityChangeModeChange();
	//		sound->PlaySe("重力起動");
	//		AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
	//	}
	//	else if (mode == GRAVITY_CHANGE_MODE) {
	//		NormalModeStateChange();
	//		debugNoDamage = false;
	//		direction = keepDire;
	//		AnimePlayChange(PlayerAnimesionKind::Jump_Down);
	//		//camera->CameraStateChange(Camera::CameraState::GRAVITY_MOVE_CAMERA);
	//		//camera->ShakeReady(ShakeGet(50, -1));	
	//	}
	//}
	//else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && noGravityArea) {
	//	HitStopStart(HitStopGet(40, 40));
	//}
	//else if (inputManager->GetIsKeyboardPut(KEY_INPUT_C) || inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B)) {
	//	/*if (mode == GRAVITY_CHANGE_MODE || mode == GRAVITY_MOVE_MODE) {
	//		GracityMoveModeChange();
	//		sound->PlaySe("重力起動");
	//		if (attackState == ATTACK1) {
	//			attackState = NO_ATTACK;
	//			attackGuadeDown = 0.0f;
	//		}
	//	}*/
	//}
	//else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_Z, XINPUT_BUTTON_A) && !compulsionGravity) {

	//	/*else if (mode == GRAVITY_MOVE_MODE) {
	//		NormalModeStateChange();
	//		StopShake();
	//		debugNoDamage = false;
	//		direction = keepDire;
	//	}*/
	//}
	//解除と発動同じボタンYボタンver/////////

	if (compulsionGravity) {

		playerUi->PlayerGaugeAdd(COMPULSION_GRAVITY_GUAGE_ADD);


		if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && !noGravityArea) {
			if (mode == GRAVITY_MOVE_MODE) {
				GravityMoveFinish();
				StopShake();
				GracityChangeModeChange();
				sound->PlaySe("重力起動");
				AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
				//guageDownNumPlus += 0.4f;
				jumpSpeed = 0.0f;
				VECTOR base = gravityVelocity / GRAVITY_INTERIA_DIV;
				base += VGet(0.0f, GRAVITY_INTERIA_BASE_NUM - guageDownNumPlus * GRAVITY_INTERIA_RATE, 0.0f) * MGetRotZ(gravityManager->GetWorldGravity());
				InteriaStart(base, 120.0f);
			}
			else if (mode == GRAVITY_CHANGE_MODE) {
				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
				GracityMoveModeChange();
				effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", EFFECT_NORMALSPEED, ONE_SECOND, false);
				sound->PlaySe("重力起動");
				moveMax.playerChangeGravityAddMax = -GRAVITY_MOVE_SPEED_MAX;
				if (attackState == ATTACK1) {
					attackState = NO_ATTACK;
					attackGuadeDown = 0.0f;
					ResetAftimage();
				}
				aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
				aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
				ShakeStart(ShakeGet(GRAVITY_AMONG_SHAKE_POWER, SHAKE_INFINITY), true);
				debugNoDamage = false;
				gravitySlideCounter = ONE_SECOND;
			}
		}
	}
	//else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_V, XINPUT_BUTTON_Y) && !noGravityArea) {
	//	if (mode == NORMAL_MODE) {
	//		GracityChangeModeChange();
	//		sound->PlaySe("重力起動");

	//		//GracityMoveModeChange();
	//		//AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
	//	}

	//	if (mode == GRAVITY_CHANGE_MODE) {
	//		/*if (mode == GRAVITY_MOVE_MODE) {
	//			if (attackState == ATTACK1) {
	//				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//			}
	//			else {
	//				AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//			}
	//		}*/
	//		AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//		GracityMoveModeChange();
	//		effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
	//		sound->PlaySe("重力起動");
	//		moveMax.playerChangeGravityAddMax = -45.0f;
	//		if (attackState == ATTACK1) {
	//			attackState = NO_ATTACK;
	//			attackGuadeDown = 0.0f;
	//			ResetAftimage();
	//		}
	//		aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
	//		aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
	//		ShakeStart(ShakeGet(20, -1), true);
	//		debugNoDamage = false;
	//		gravitySlideCounter = 60.0f;
	//	}
	//	else if (mode == GRAVITY_MOVE_MODE) {
	//		GravityMoveFinish();
	//		//againGravityCounter = 10;
	//		//camera->CameraStateChange(Camera::CameraState::GRAVITY_MOVE_CAMERA);
	//		//camera->ShakeReady(ShakeGet(50, -1));	

	//	}

	//}
	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && !noGravityArea) {
		if (mode == NORMAL_MODE) {
			GracityChangeModeChange();
			sound->PlaySe("重力起動");
			AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
			guageDownNumPlus = 0.0f;
			InteriaReset();
		}
		else if (mode == GRAVITY_MOVE_MODE) {
			GravityMoveFinish();
			StopShake();
			GracityChangeModeChange();
			sound->PlaySe("重力起動");
			AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
			guageDownNumPlus += 0.4f;
			if (guageDownNumPlus >= 2.0f) {
				guageDownNumPlus = 2.0f;
			}
			jumpSpeed = 0.0f;
			VECTOR base = gravityVelocity / GRAVITY_INTERIA_DIV;
			base += VGet(0.0f, GRAVITY_INTERIA_BASE_NUM - guageDownNumPlus * GRAVITY_INTERIA_RATE, 0.0f) * MGetRotZ(gravityManager->GetWorldGravity());
			InteriaStart(base, 120.0f);
		}
		else if (mode == GRAVITY_CHANGE_MODE) {
			AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
			GracityMoveModeChange();
			effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", EFFECT_NORMALSPEED, 60, false);
			sound->PlaySe("重力起動");
			moveMax.playerChangeGravityAddMax = -GRAVITY_MOVE_SPEED_MAX;
			if (attackState == ATTACK1) {
				attackState = NO_ATTACK;
				attackGuadeDown = 0.0f;
				ResetAftimage();
			}
			aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
			aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
			ShakeStart(ShakeGet(GRAVITY_AMONG_SHAKE_POWER, SHAKE_INFINITY), true);
			debugNoDamage = false;
			gravitySlideCounter = ONE_SECOND;
			//GracityMoveModeChange();
			//AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
			}
			//	//camera->CameraStateChange(Camera::CameraState::GRAVITY_MOVE_CAMERA);
			//		//camera->ShakeReady(ShakeGet(50, -1));
	}
	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && noGravityArea) {
		HitStopStart(NO_GRAVITY_SHAKE);
		sound->PlaySe("NO_GRAVITY");
	}
	else if (inputManager->GetIsKeyboardPut(KEY_INPUT_C) || inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B)) {
		/*if (mode == GRAVITY_CHANGE_MODE || mode == GRAVITY_MOVE_MODE) {
			GracityMoveModeChange();
			sound->PlaySe("重力起動");
			if (attackState == ATTACK1) {
				attackState = NO_ATTACK;
				attackGuadeDown = 0.0f;
			}
		}*/
	}
	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_Z, XINPUT_BUTTON_A) && !compulsionGravity) {
		if (mode == GRAVITY_MOVE_MODE) {
			InteriaReset();
			GravityMoveFinish();
			AnimePlayChange(PlayerAnimesionKind::Jump_Down);
		//againGravityCounter = 10;
		//camera->CameraStateChange(Camera::CameraState::GRAVITY_MOVE_CAMERA);
		//camera->ShakeReady(ShakeGet(50, -1));	

		}
		if (mode == GRAVITY_CHANGE_MODE) {
			NormalModeStateChange();
			InteriaStart(gravitySlide, 60);
			AnimePlayChange(PlayerAnimesionKind::Jump_Down);
			debugNoDamage = false;
		}
	}
//*********************************長押しの間重力発動*********************************************
	//if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_X, XINPUT_BUTTON_B) && !noGravityArea) {
	//	if (mode == NORMAL_MODE) {
	//		GracityChangeModeChange();
	//		//sound->PlaySe("重力起動");
	//		//GracityMoveModeChange();
	//		//AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
	//		mode = GRAVITY_MOVE_MODE;
	//	}
	//	if (mode == GRAVITY_MOVE_MODE) {
	//		if (aloowKeepState != gravityMode) {
	//			if (mode == GRAVITY_MOVE_MODE) {
	//				if (attackState == ATTACK1) {
	//					AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//				}
	//				else {
	//					AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//				}
	//				GracityMoveModeChange();
	//				effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
	//				sound->PlaySe("重力起動");
	//				moveMax.playerChangeGravityAddMax = -45.0f;
	//				/*if (attackState == ATTACK1) {
	//					attackState = NO_ATTACK;
	//					attackGuadeDown = 0.0f;
	//					ResetAftimage();
	//				}*/
	//				aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
	//				aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
	//				ShakeStart(ShakeGet(20, -1), true);
	//				debugNoDamage = false;
	//			}
	//		}
	//	}
	//	if (mode ==GRAVITY_CHANGE_MODE) {
	//		AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	//		GracityMoveModeChange();
	//		effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
	//		sound->PlaySe("重力起動");
	//		moveMax.playerChangeGravityAddMax = -45.0f;
	//		/*if (attackState == ATTACK1) {
	//			attackState = NO_ATTACK;
	//			attackGuadeDown = 0.0f;
	//			ResetAftimage();
	//		}*/
	//		aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
	//		aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
	//		ShakeStart(ShakeGet(20, -1), true);
	//		debugNoDamage = false;
	//	}
	//}
	//else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_V, XINPUT_BUTTON_Y) && !noGravityArea) {
	//	if (mode == NORMAL_MODE) {
	//		GracityChangeModeChange();
	//		sound->PlaySe("重力起動");
	//		AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
	//	}
	//}
	//else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && noGravityArea) {
	//	HitStopStart(HitStopGet(40, 40));
	//}
	//else if (inputManager->GetIsKeyboardPut(KEY_INPUT_C) || inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B)) {
	//	/*if (mode == GRAVITY_CHANGE_MODE || mode == GRAVITY_MOVE_MODE) {
	//		GracityMoveModeChange();
	//		sound->PlaySe("重力起動");
	//		if (attackState == ATTACK1) {
	//			attackState = NO_ATTACK;
	//			attackGuadeDown = 0.0f;
	//		}
	//	}*/
	//}
	//else {
	//	if (mode == GRAVITY_MOVE_MODE) {
	//		GravityMoveFinish();
	//		//		//againGravityCounter = 10;
	//	}
	//}
	 
//**************************単押しで重力発動で解除同じ**********************************************
//if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && !noGravityArea) {
//		if (mode == GRAVITY_CHANGE_MODE || mode == NORMAL_MODE) {
//			GracityChangeModeChange();
//			//sound->PlaySe("重力起動");
//			//GracityMoveModeChange();
//			//AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
//			mode = GRAVITY_MOVE_MODE;
//			if (aloowKeepState != gravityMode) {
//				if (mode == GRAVITY_MOVE_MODE) {
//					if (attackState == ATTACK1) {
//						AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
//					}
//					else {
//						AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
//					}
//					GracityMoveModeChange();
//					effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
//					sound->PlaySe("重力起動");
//					moveMax.playerChangeGravityAddMax = -45.0f;
//					/*if (attackState == ATTACK1) {
//						attackState = NO_ATTACK;
//						attackGuadeDown = 0.0f;
//						ResetAftimage();
//					}*/
//					aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
//					aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
//					ShakeStart(ShakeGet(20, -1), true);
//					debugNoDamage = false;
//				}
//			}
//
//		}
//		else if (mode == GRAVITY_MOVE_MODE) {
//			NormalModeStateChange();
//			StopShake();
//			debugNoDamage = false;
//			direction = keepDire;
//			
//		}
//	}
//	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_V, XINPUT_BUTTON_Y) && !noGravityArea) {
//		if (mode == NORMAL_MODE) {
//			GracityChangeModeChange();
//			sound->PlaySe("重力起動");
//			AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
//		}
//	}
//	else if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B) && noGravityArea) {
//		HitStopStart(HitStopGet(40, 40));
//	}
//	else if (inputManager->GetIsKeyboardPut(KEY_INPUT_C) || inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_X, XINPUT_BUTTON_B)) {
//		/*if (mode == GRAVITY_CHANGE_MODE || mode == GRAVITY_MOVE_MODE) {
//			GracityMoveModeChange();
//			sound->PlaySe("重力起動");
//			if (attackState == ATTACK1) {
//				attackState = NO_ATTACK;
//				attackGuadeDown = 0.0f;
//			}
//		}*/
//	}
//if (aloowKeepState != gravityMode) {
//	if (mode == GRAVITY_MOVE_MODE) {
//		AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
//		GracityMoveModeChange();
//		effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", 1.0f, 60, false);
//		sound->PlaySe("重力起動");
//		if (attackState == ATTACK1) {
//			/*attackState = NO_ATTACK;
//			attackGuadeDown = 0.0f;
//			ResetAftimage();*/
//		}
//		else {
//			moveMax.playerChangeGravityAddMax = -45.0f;
//			aftimageNum = GRAVITY_MOVE_AFTIMAGE_NUM;
//			aftimageAlpha = ALPHA_AFTIMAGE_GRAVITY_MOVE;
//		}
//		
//		ShakeStart(ShakeGet(20, -1), true);
//		debugNoDamage = false;
//	}
//}
#endif // false
	
}

void Player::GravityMode()
{
	if (!compulsionGravity) {
		if (mode == GRAVITY_CHANGE_MODE) {
			playerUi->PlayerGaugeUse(GRAVITY_USE_BASE_ADD + guageDownNumPlus);
			if (playerUi->GetGravityMoveGaugeNum() <= 0.0f) {
				GracityMoveModeChange();
				NormalModeStateChange();
				InteriaStart(gravitySlide, 60);
				debugNoDamage = false;
			}
		}
	}
	if (GetJoypadNum() <= 0) {
		if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_UP, XINPUT_BUTTON_DPAD_UP)) {
			if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_LEFT, XINPUT_BUTTON_DPAD_LEFT)) {
				//allowAngle = -DX_PI_F / 4;
				gravityMode = UP_LEFT_GRAVITY_START;
				keepDire = LEFT;
			}
			else if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_RIGHT, XINPUT_BUTTON_DPAD_RIGHT)) {
				//allowAngle = DX_PI_F / 4;
				gravityMode = UP_RIGHT_GRAVITY_START;
				keepDire = RIGHT;
			}
			else {
				//allowAngle = 0.0f;
				gravityMode = UP_GRAVITY_START;
			}
		}
		else if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_DOWN, XINPUT_BUTTON_DPAD_DOWN)) {
			if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_LEFT, XINPUT_BUTTON_DPAD_LEFT)) {
				//allowAngle = DX_PI_F - -DX_PI_F / 4;
				gravityMode = DOWN_LEFT_GRAVITY_START;
				keepDire = LEFT;
			}
			else if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_RIGHT, XINPUT_BUTTON_DPAD_RIGHT)) {
				//allowAngle = DX_PI_F - DX_PI_F / 4;
				gravityMode = DOWN_RIGHT_GRAVITY_START;
				keepDire = RIGHT;
			}
			else {
				//allowAngle = DX_PI_F;
				gravityMode = DOWN_GRAVITY_START;
			}
		}
		else if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_RIGHT, XINPUT_BUTTON_DPAD_RIGHT)) {
			//allowAngle = DX_PI_F / 2;
			gravityMode = RIGHT_GRAVITY_START;
			keepDire = RIGHT;
		}
		else if (inputManager->GetIsKeyOrButtonPushingNow(KEY_INPUT_LEFT, XINPUT_BUTTON_DPAD_LEFT)) {
			//allowAngle = -DX_PI_F / 2;
			gravityMode = LEFT_GRAVITY_START;
			keepDire = LEFT;
		}
		playerUi->SetDrawArrowAngle();
	}
	else {
		if (inputManager->GetStickKnocking(0.9f).leftStick == S_UP) {
			//allowAngle = 0.0f;
			gravityMode = UP_GRAVITY_START;
		}
		else if (inputManager->GetStickKnocking(0.9f).leftStick == S_UP_RIGHT) {
			//allowAngle = DX_PI_F / 4;
			gravityMode = UP_RIGHT_GRAVITY_START;
			keepDire = RIGHT;
		}
		else if (inputManager->GetStickKnocking(0.9f).leftStick == S_UP_LEFT) {
			//allowAngle = -DX_PI_F / 4;
			gravityMode = UP_LEFT_GRAVITY_START;
			keepDire = LEFT;
		}
		else if (inputManager->GetStickKnocking(0.9f).leftStick == S_LEFT) {
			//allowAngle = -DX_PI_F / 2;
			gravityMode = LEFT_GRAVITY_START;
			keepDire = LEFT;
		}
		else if (inputManager->GetStickKnocking(0.9f).leftStick == S_RIGHT) {
			//allowAngle = DX_PI_F / 2;
			gravityMode = RIGHT_GRAVITY_START;
			keepDire = RIGHT;
		}
		else if (inputManager->GetStickKnocking(0.9f).leftStick == S_DOWN) {
			//allowAngle = DX_PI_F;
			gravityMode = DOWN_GRAVITY_START;
		}
		else if (inputManager->GetStickKnocking(0.9f).leftStick == S_DOWN_RIGHT) {
			//allowAngle = DX_PI_F - DX_PI_F / 4;
			gravityMode = DOWN_RIGHT_GRAVITY_START;
			keepDire = RIGHT;
		}
		else if (inputManager->GetStickKnocking(0.9f).leftStick == S_DOWN_LEFT) {
			//allowAngle = DX_PI_F - -DX_PI_F / 4;
			gravityMode = DOWN_LEFT_GRAVITY_START;
			keepDire = LEFT;
		}
		playerUi->SetDrawArrowAngle();
	}
}

void Player::GravityMove(float _guageNum)
{
	playerUi->PlayerGaugeUse(_guageNum);
	if (playerUi->GetGravityMoveGaugeNum() <= 0.0f) {
		GravityMoveFinish();
	}
}

void Player::GravityMoveRotateChange()
{
	if (bumperTime > 0) {
		return;
	}
	if (gravityAfterRotate >= 0) {
		if (gravityAfterRotate > rotate.z) {
			rotate.z += rotateZAdd;
			rotateZAdd += GRAVITY_CHANGE_ROTATE_SPEED;
		}
		else {
			rotate.z = gravityAfterRotate;
		}
	}
	else {
		if (gravityAfterRotate < rotate.z) {
			rotate.z -= rotateZAdd;
			rotateZAdd +=  GRAVITY_CHANGE_ROTATE_SPEED ;
		}
		else {
			rotate.z = gravityAfterRotate;
		}
	}
}

void Player::GravityMove(GravityState _state)
{
	if (gameState == GameManager::MENU) {
		return;
	}
	if (gameState == GameManager::PLAY_BEFORE || gameState == GameManager::PLAY_AFTER || gameState == GameManager::SCENE_CHANGE) {
		MATRIX matrix = GetRotateMatrix(zMoveGravityRot);
		gravityVelocity = VTransform(gravityAdd, matrix);
		gravityVelocity = VScale(gravityVelocity, objectGravityRate);

		centerPosition = VAdd(centerPosition, gravityVelocity);
	}
	else {
		if (_state != NO_GRAVITY)
		{
			MATRIX mRotZ = MGetRotZ(gravityRotation);
			gravityVelocity = VTransform(gravityAdd, mRotZ);
			gravityVelocity = VScale(gravityVelocity, objectGravityRate);
			//velocity += gravityVelocity;
			centerPosition = VAdd(centerPosition, gravityVelocity);
		}
	}
}

void Player::NormalModeStateChange()
{
	if (mode == NORMAL_MODE) {
		return;
	}
	if (attackState == ATTACK1) {
		attackGuadeDown = 0.0f;
		attackState = NO_ATTACK;
	}
	//effectManager->StopEffekseer("WIND", true);
	mode = NORMAL_MODE;
	rotate.z = 0.0f;
	rotateZAdd = GRAVITY_CHANGE_ROTATE_BASE_NUM;
	gravityAdd = BASE_GRAVITY_VELOCITY;
	effectManager->StopEffekseer("NO_GRAVITY_EFFECT", false);
	//camera->CameraStateChange(Camera::CameraState::NOEMAL_CAMERA);
	//gravityMode = NO_GRAVITY;
	gravityManager->SetGravityChangeMode(GravityManager::WORLD);
	
	if (direction == RIGHT) {
		DirectionChange(-DX_PI_F / 2, RIGHT);
	}
	else {
		DirectionChange(DX_PI_F / 2, LEFT);
	}
	//GuageAmontReset();
	rateKeep = gravityAddRate;
	gravityAddRate = GRAVITY_RATE_BASE;
	attackState = NO_ATTACK;
	attackHitTime = 0;
	rgb = RGB_WHITE;
	sound->StopSdSe("風");
	ResetAftimage();
	aloowKeepState = NO_GRAVITY;
	if (shake.time == SHAKE_INFINITY) {
		StopShake();
	}
	enemyHitAddCounter = ENEMY_NO_HIT;
}

void Player::GracityChangeModeChange()
{
	if (!healG) {
		GuageHealReset();
	}
	mode = GRAVITY_CHANGE_MODE;
	effectManager->CreateEffekseer(centerPosition, &centerPosition, "CHARGE_START_EFFECT", EFFECT_NORMALSPEED, 60, false);
	effectManager->CreateEffekseer(centerPosition, &centerPosition, "NO_GRAVITY_EFFECT", 0.5f, 60, true);
	rotate.y = 0.0f;
	rotate.x = 0.0f;
	rotate.z = gravityRotation;
	PositionMove(VGet(0.0f, GRAVITY_CHANGE_UP_POS, 0.0f));
	gravityManager->SetGravityChangeMode(GravityManager::PLAYER_CHANGE_MODE);
	debugNoDamage = true;
	startEffectCounter = 0;
	keepDire = direction;
	groundJump = false;
	jump = true;
	WalkReset();
	slideCounter = slideCounterMax;
}

void Player::GracityMoveModeChange()
{
	mode = GRAVITY_MOVE_MODE;
	effectManager->StopEffekseer("NO_GRAVITY_EFFECT", false);
	//effectManager->StopEffekseer("WIND", true);
	gravityAfterRotate = gravityRotation;
	gravityManager->SetGravityChangeMode(GravityManager::ALL_CHANGE, gravityMode);
	jumpSpeed = 0.0f;
	beforeGravityMode = startGravityMode;
	startGravityMode = gravityMode;
	//attackGuadeDown = 0.01f;
	//gravityAddRate = 0.1f;
	rgb.SetColor(255, 255, 0);
	sound->PlaySe("風");
	aloowKeepState = gravityMode;
	direction = keepDire;
	gravityAddRate = rateKeep;
	groundJump = false;
	jump = true;
	WalkReset();
	slideCounter = slideCounterMax;
	debugNoDamage = false;
	animeNum = 1;
	//NormalModeStateChange();
	//effectManager->CreateEffekseer(V2Get(860.0f, 540.0f), nullptr, "WIND", 1.0f, 60, true);
	//PlayEffect4();
}

void Player::GravityMoveFinish()
{
	NormalModeStateChange();
	debugNoDamage = false;
	direction = keepDire;
	StopShake();
	InteriaStart(gravityVelocity / GRAVITY_INTERIA_DIV, 60.0f);
}

void Player::AttackReady()
{
	//if (attackState == NO_ATTACK) {
	if (inputManager->GetIsKeyOrButtonPutNow(KEY_INPUT_C, XINPUT_BUTTON_RIGHT_SHOULDER) || inputManager->GetTriggerNow().rightStick >= 0.5f) {
		if (attackState == NO_ATTACK) {
			GracityMoveModeChange();
			HitStopStart(HitStopGet(ATTACK_SHAKE_POWER, ATTACK_SHAKE_TIME));
			moveMax.playerChangeGravityAddMax = ATTACK_SPEED_MAX;
			attackGuadeDown = 1.0f;
			ResetAftimage();
			inputManager->ControlVibrationStartFrame(ATTACK_START_POWER, ATTACK_START_FRAME);
			AnimePlayChange(PlayerAnimesionKind::Attack);
			aftimageNum = ATTACK_AFTIMAGE_NUM;
			aftimageAlpha = ALPHA_AFTIMAGE_ATTACK;
			attackState = ATTACK1;
			attackTime = ATTACK_TIME;
			sound->PlaySe("キック開始");
			gravityAdd.y = moveMax.playerChangeGravityAddMax;
		}
		//sound->PlaySe("重力起動");
	}
	//}
}

void Player::Attack()
{
	if (mode == NORMAL_MODE) {
		if (--attackTime >= 0) {
			//attackGuadeDown += 0.02f;
		}
		else {
			attackState = NO_ATTACK;
		}
	}
	else {
		attackGuadeDown += GRAVITY_USE_ATTACK;
		gravityAddRate += GRAVITY_RATE_ADD_ATTACK;
	}
}
//
//void Player::AttackHit()
//{
//	if (attackHitTime > 0 || attackState == NO_ATTACK) {
//		return;
//	}
//	//moveRotate = VZero;
//	direction = keepDire;
//	float angle = gravityManager->GetGravityAngle(gravityManager->GetReverceGravity(aloowKeepState));
//	float worldAngle = gravityManager->GetWorldGravity();
//	GravityState st = gravityManager->GetWorldGravityState();
//	/*if ( st == LEFT_GRAVITY) {
//		worldAngle *= -1;
//	}*/
//	if (angle > 0.0f) {
//		moveRotate.x = angle - DX_PI_F / 2.0f + worldAngle;
//	}
//	else {
//		moveRotate.x = angle + DX_PI_F / 2.0f + worldAngle;
//	}
//	if (direction == LEFT) {
//		if (angle != 0.0f || angle == DX_PI_F) {
//			moveRotate.x *= -1.0f;
//		}
//	}
//	GravityState aloowKeep = aloowKeepState;
//	NormalModeStateChange();
//	switch (aloowKeep)
//	{
//	case DOWN_RIGHT_GRAVITY_START:
//	case DOWN_RIGHT_GRAVITY:
//		//break;
//	case RIGHT_GRAVITY_START:
//	case RIGHT_GRAVITY:
//		//break;
//	case UP_GRAVITY:
//	case UP_GRAVITY_START:
//	case DOWN_GRAVITY:
//	case DOWN_GRAVITY_START:
//	case UP_RIGHT_GRAVITY_START:
//	case UP_RIGHT_GRAVITY:
//		moveRotate.y = -DX_PI_F / 2;
//		break;
//	case UP_LEFT_GRAVITY_START:
//	case UP_LEFT_GRAVITY:
//		//break;
//	
//	case LEFT_GRAVITY_START:
//	case LEFT_GRAVITY:
//		//break;
//	case DOWN_LEFT_GRAVITY_START:
//	case DOWN_LEFT_GRAVITY:
//		moveRotate.y = DX_PI_F / 2;
//		break;
//	}
//	gActive = false;
//	rotate = moveRotate;
//	sound->PlaySe("キック当たる");
//	attackHitTime = 30;
//	StopShake();
//	HitStopStart(HitStopGet(60, 15));
//	attackAngle = DX_PI_F * 2;
//	attackDistanceZ = 50.0f;
//	attackDistanceY = 25.0f;
//	playerUi->GravityGaugeMax();
//	charaState = CharaBase::ATTACK_HIT;
//	attackState = HIT;
//	effectManager->CreateEffekseer(position, nullptr, "ENEMY_HIT", EFFECT_NORMALSPEED, ONE_SECOND, false);
//	
//}
//
//void Player::AttackHitMove()
//{
//	moveRotate;
//	attackAngle -= 0.1f;
//	//gravityInfluence = abs(gravityAdd.y * objectGravityRate);
//	velocity = VZero;
//	VECTOR velo = VZero;
//	velo.z = -sinf(attackAngle) * attackDistanceZ;
//	velo.y = cosf(attackAngle) * attackDistanceY;
//	PositionMove(VGet(velo.x, velo.y, velo.z));
//	attackHitTime--;
//	//attackDistanceZ += 0.1f;
//	if (attackHitTime == 0) {
//		AttackHitMoveReset();
//	}
//}
//
//void Player::AttackHitMoveReset()
//{
//	attackHitTime = 0;
//	attackAngle = DX_PI_F * 2;
//	attackDistanceZ = 50.0f;
//	attackDistanceY = 25.0f;
//	charaState = CharaBase::NORMAL;
//	attackState = NO_ATTACK;
//	gActive = true;
//	//NormalModeStateChange();
//}

int Player::GroundLanding(bool _can)
{
	//position.y = GROUND_POS;
	jumpNum = JUMP_NUM;
	jumpSpeed = 0.0f;

	//playerUi->PlayerGaugeAdd(8.0f);
	GaugeHealInit(GUAGE_ALL_HEAL_GROUND, GUAGE_WAIT_TIME);
	if (jump || groundJump) {
		sound->PlaySe("着地");
		if (attackState == ATTACK1) {
			effectManager->CreateEffekseer(position, nullptr, "PLAYER_KICK_HIT", EFFECT_NORMALSPEED, ONE_SECOND, false, VGet(2.0f, 2.0f, 2.0f), effectNormalRotate);
		}
		effectManager->CreateEffekseer(position, nullptr, "JUMP", EFFECT_NORMALSPEED, 15, false, effectNormalSize, effectNormalRotate);
		jump = false;
		AnimePlayChange(PlayerAnimesionKind::Land);
		groundJump = false;
	}
	if (attackState == ATTACK1) {
		effectManager->CreateEffekseer(position, nullptr, "PLAYER_KICK_HIT", EFFECT_NORMALSPEED, ONE_SECOND, false, VGet(2.0f, 2.0f, 2.0f), effectNormalRotate);
	}
	if (_can) {
		Object::GroundLanding();
		GuageAmontReset();
	}
	else {

	}
	groundRand = true;
	
	//jumpOnes = true;
	//SpickMoveReset();
	return 1;
}

void Player::GuageAmontReset()
{
	guadeDown = 0.1f;
	attackGuadeDown = 0.0f;
	gravityAddRate = GRAVITY_RATE_BASE;
	guageDownBase = 0.0f;
	objectGravityRate = GRAVITY_RATE_BASE;
	rateKeep = 0.1f;
}

void Player::DebugPlayer()
{
#ifdef _DEBUG

	if (inputManager->GetIsKeyboardPushing(KEY_INPUT_LSHIFT)) {
		if (inputManager->GetIsKeyboardPut(KEY_INPUT_1)) {
			gActive = !gActive;
		}
		if (inputManager->GetIsKeyboardPut(KEY_INPUT_2)) {
			BumperHitInit(VGet(0.0f,0.0f,0.0f));
		}
		if (inputManager->GetIsKeyboardPut(KEY_INPUT_3)) {
			railNo = true;
		}
		else {
			railNo = false;
		}
		if (inputManager->GetIsKeyboardPushing(KEY_INPUT_4)) {
			CompulsionGravityInit();
		}
		else {
			compulsionGravity = false;
		}
	}
	else {
		railNo = false;
		if (CheckHitKey(KEY_INPUT_1)) {
			centerPosition = VGet(0, 0, 0);
			//centerPosition = VGet(0, 0, 0);
			NormalModeStateChange();
		}
		else if(inputManager->GetIsKeyboardPut(KEY_INPUT_2)) {
			debugNoDamage = !debugNoDamage;
		}
		else if (CheckHitKey(KEY_INPUT_3)) {
			GoalStart();
		}
		else if (CheckHitKey(KEY_INPUT_4)) {
			Damage(PLAYER_DAMAGE, PLAYER_INVINCIBLE);
			playerUi->SetHp(hp);
			HitStopStart(HitStopGet(10, 30));
			NormalModeStateChange();
			DamageMoveReady(VGet(0.0f, 0.0f, 50.0f), 60);
		}
		else if (CheckHitKey(KEY_INPUT_5)) {
			hp++;
			playerUi->SetHp(hp);
		}
		else if (inputManager->GetIsKeyboardPut(KEY_INPUT_6)) {
			AmongPointReset();
		}
		else if (inputManager->GetIsKeyboardPut(KEY_INPUT_7)) {
			if (SetAmongPosition(stageObjectManager->GetCurrentCpPosition(),0.0f)) {
				stageObjectManager->NextCheckPoint();
				sound->PlaySe("中間ポイント");
			}
		}
		else if (CheckHitKey(KEY_INPUT_8)) {
			//AttackHit();
		}
		else if (CheckHitKey(KEY_INPUT_9)) {
			centerPosition = VGet(200, 2000, 0);
		}
	}
	

#endif

}

void Player::CollisionEvent(CollisionBase& me, CollisionBase& target)
{
	rayHit = false;
	Object::CollisionEvent(me, target);

	int d = 0;
	if (gameState == GameManager::PLAY_AFTER) {
		return;
	}
	switch (me.GetTagMe())
	{
	case COLLISION_OBJECT_KIND::LAND:
		/*me.SetGroundLanding();
 		//これ*/
		if (mode == GRAVITY_CHANGE_MODE) {
			 return;
		}

		if (target.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_SPIKE)
			int a = 0;

		if (target.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK  || target.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_BROKEN || target.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR) {
			if (!beforeCompulsionGravity) {
				GroundLanding(true);
				PlayerWorldChange(target,false);
				VECTOR lastPos = position * MGetRotZ(gravityManager->GetWorldGravity());
				lastGroundPosition = lastPos;
			}
			noLandEffectTime = 0;
			
		}
		else if (target.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK_GRAVITY) {
			if (!beforeCompulsionGravity) {

				bool bo = !dynamic_cast<GravityBrock*>(target.GetObjectMe())->IsMove();
					
				GroundLanding(bo);
				PlayerWorldChange(target,bo);
				VECTOR lastPos = position * MGetRotZ(gravityManager->GetWorldGravity());
				lastGroundPosition = lastPos;


				VECTOR testPos = gravityVelocity;
				if (!bo) {
					int a = 0;
					gravityVelocity = target.GetObjectMe()->GetGravityVelocity();
					moveMax = MaxGet(-35.0f, -50.0f);
					objectGravityRate = target.GetObjectMe()->GetObjectRate() + 1.0f;
					gravityLand = true;
				}
				else {
					int a = 0;
					moveMax = MaxGet(-25.0f, -50.0f);
					objectGravityRate = GRAVITY_RATE_BASE;
				}
			}
		}
		else {
			if (mode != GRAVITY_MOVE_MODE) {
				GroundLanding(true);
			}
			else {
				/*if (noLandEffectTime % NO_RAND_EFFECT_AMONG == 0) {
					effectManager->CreateEffekseer(position, nullptr, "NO_LAND", EFFECT_NORMALSPEED, ONE_SECOND, false, effectNormalSize, effectNormalRotate);
				}
				noLandEffectTime++;*/
			}
		}
		
		break;
	case COLLISION_OBJECT_KIND::PLAYER_ATTACK:
		if (target.GetTagMe() == COLLISION_OBJECT_KIND::ENEMY && bumperTime <= 0) {
			StopShake();
			if (enemyHitAddCounter <= ENEMY_NO_HIT) {
				HitStopStart(HitStopGet(ENEMY_HIT_FIRST_SHAKE, ENEMY_HIT_FIRST_TIME));
			}
			else {
				HitStopStart(HitStopGet(ENEMY_HIT_ALOT_SHAKE, ENEMY_HIT_ALOT_TIME));
			}
			sound->PlaySe("キック当たる");
			/*if (moveMax.playerChangeGravityAddMax <= -90.0f) {
				moveMax.playerChangeGravityAddMax -= 10.0f;
			}
			else {
				moveMax.playerChangeGravityAddMax = -90.0f;
			}*/
			//attackState = HIT;
			//charaState = ATTACK_HIT;
			gravityMaxSpeed = -ENEMY_HIT_INTERIA_MAX;
			enemyHitAddCounter = 0.0f;
			enemyHitCounterPlus = ENEMY_HIT_EASING_COUNTER_PLUS;
			GaugeHealInit(GUAGE_ALL_HEAL_ATTACK_HIT, GUAGE_WAIT_TIME_ONE, 100.0f);
			attackGuadeDown = 0.0f;
			enemyHit = true;
			//gravityAdd.y = moveMax.playerChangeGravityAddMax;
			sound->PlaySe("高掛け声");
		}
		else if(target.GetTagMe() != COLLISION_OBJECT_KIND::ENEMY){
			HitStopStart(HitStopGet(BROKEN_BROCK_HIT_POWER, BROKEN_BROCK_HIT_TIME));
		}
		//if (target.GetTagMe() != COLLISION_OBJECT_KIND::BLOCK_BROKEN) {
			effectManager->CreateEffekseer(position, nullptr, "PLAYER_KICK_HIT", EFFECT_NORMALSPEED, 40, false, VGet(3.0f, 3.0f, 3.0f), effectNormalRotate);
		//}
		//sound->PlaySe("着地");
		inputManager->ControlVibrationStartTime(ATTACK_HIT_CONTROL_POWER, ATTACK_HIT_CONTROL_SECOND);
		break;
	case COLLISION_OBJECT_KIND::PLAYER_RAY:

		//VECTOR ray = dynamic_cast<RayCollision&>(me).GetRayDirection();
		////ray = VNorm(ray);
		//if (ray.y <= -VNorm(ray).y)
		//{
		//	rayP = ray;
		//}
		//rayHit = true;
		break;
	default:
		break;
	}
	int a = 0;
	switch (target.GetTagMe())
	{
	case COLLISION_OBJECT_KIND::ENEMY_ATTACK:	//今のところはENEMYはATTACKの処理がないので、ENEMYの体に当たったらダメージ
	case COLLISION_OBJECT_KIND::ENEMY:
		if (attackState == NO_ATTACK) {
			if (Damage(PLAYER_DAMAGE, PLAYER_INVINCIBLE)) {
				//NormalModeStateChange();
				playerUi->SetHp(hp);
				sound->PlaySe("プレイヤーダメージ");
				stageScore->AddDamageNum();
				if (hp > 0) {
					if (mode == GRAVITY_MOVE_MODE) {
						NormalModeStateChange();
						DamageMoveReady(ENEMY_DAMAGE_MOVE_SPEED, ENEMY_DAMAGE_MOVE_TIME);
					}
					else {
						DamageMoveReady(ENEMY_DAMAGE_MOVE_SPEED, ENEMY_DAMAGE_MOVE_TIME);
					}
					HitStopStart(HitStopGet(ENEMY_DAMAGE_HIT_POWER, ENEMY_DAMAGE_HIT_TIME));
					effectManager->CreateEffekseer(centerPosition, &centerPosition, "ENEMY_DESTROY_HIT_EFFECT", EFFECT_NORMALSPEED, 60, false, effectNormalSize, effectNormalRotate);
					AnimePlayChange(PlayerAnimesionKind::Damage);
				}
				else {
					noDamageTime = 0;
					DieProduct();
					NormalModeStateChange();
					AnimePlayChange(PlayerAnimesionKind::Die);
				}
				damageCamera = true;
				std::string voice = "やられ";
				voice = voice + std::to_string(hp);
				sound->PlaySe(voice);
			}
		}
		break;
	case COLLISION_OBJECT_KIND::BLOCK_GOAL:
		if (!goal) {
			GoalStart();
			goalPosition = target.GetObjectMe()->GetPosition();
			gravityManager->ChangeWorldGravity(gravityManager->AngleToGravityStateGet(target.GetObjectMe()->GetRotation().z,false));
			goalTime = 1.0f;
			rgb = RGB_WHITE;
		}
		break;
	case COLLISION_OBJECT_KIND::BLOCK_SPIKE:


		//break;



		if (attackState != HIT) {
			if (Damage(PLAYER_DAMAGE, PLAYER_INVINCIBLE)) {
				NormalModeStateChange();
				playerUi->SetHp(hp);
				attackState = NO_ATTACK;
				jump = false;
				sound->PlaySe("プレイヤーダメージ");
				stageScore->AddDamageNum();
				if (hp > 0) {
					effectManager->CreateEffekseer(centerPosition, &centerPosition, "ENEMY_DESTROY_HIT_EFFECT", EFFECT_NORMALSPEED, 60, false, effectNormalSize, effectNormalRotate);
					charaState = CharaBase::SPICK_DAMAGE;
					velocityKeep = Transform(velocityKeep, velocity);
					PositionMove(SPICK_DAMAGE_MOVE);
					HitStopStart(SHAKE_SPICK);
					spickRotate = target.GetObjectMe()->GetRotation();
					moveRotate = spickRotate;
				}
				else {
					noDamageTime = 0;
					DieProduct();
					AnimePlayChange(PlayerAnimesionKind::Die);
				}
				damageCamera = true;
				std::string voice = "やられ";
				voice = voice + std::to_string(hp);
				sound->PlaySe(voice);
			}
		}
		break;
	case COLLISION_OBJECT_KIND::BLOCK:
		//SpickMoveReset();
		//AttackHitMoveReset();
		break;
	case COLLISION_OBJECT_KIND::BLOCK_POINT:
		if (SetAmongPosition(stageObjectManager->GetCurrentCpPosition(),target.GetObjectMe()->GetRotation().z)) {
			stageObjectManager->NextCheckPoint();
			sound->PlaySe("中間ポイント");
		}
		break;
	case COLLISION_OBJECT_KIND::BLOCK_BUMPER:
		BumperHitInit(target.GetObjectMe()->GetPosition() * MGetRotZ(-gravityManager->GetWorldGravity()));
		
		break;
	case COLLISION_OBJECT_KIND::ITEM_GHEAL:
		HealGravityGuage();
		break;
	case COLLISION_OBJECT_KIND::ITEM_HEAL:
		HealHp();
		break;
	case COLLISION_OBJECT_KIND::BLOCK_RAIL:
		if (gameState == GameManager::PLAYING && !railNo) {
			RailMoveReady(target);
			rail = true;
		}
		break;
	case COLLISION_OBJECT_KIND::BLOCK_GRAVITY_AREA:
		if (dynamic_cast<GravityArea*>(target.GetObjectMe())->GetGravityAreaType() == 0) {
			CompulsionGravityInit();
			beforeCompulsionGravity = true;
		}
		else {
			bumperNoCollsionCounter = 0;
			if (mode == GRAVITY_MOVE_MODE) {
				NormalModeStateChange();
				HitStopStart(NO_GRAVITY_SHAKE);
				sound->PlaySe("NO_GRAVITY");
				debugNoDamage = false;
				AnimePlayChange(PlayerAnimesionKind::Jump_Down);
			}
			else if (mode == GRAVITY_CHANGE_MODE) {
				NormalModeStateChange();
				HitStopStart(NO_GRAVITY_SHAKE);
				sound->PlaySe("NO_GRAVITY");
				debugNoDamage = false;
				AnimePlayChange(PlayerAnimesionKind::Jump_Down);
			}
			noGravityArea = true;

		}
		break;
	case COLLISION_OBJECT_KIND::BLOCK_BROKEN:
		a = 0;
		break;
	case COLLISION_OBJECT_KIND::CAMERA_AREA:
		cameraHit = true;
		cameraMovePositionKeep = dynamic_cast<CameraArea*>(target.GetObjectMe())->GetCameraPullAddPosition();
		break;
	default:
		break;
	}
	
}

bool Player::Goal() const
{
	return goal;
}

bool Player::NoGround()
{
	groundRand = false;
	return true;
}

void Player::PlayerWorldChange(CollisionBase& target, bool _move)
{
	//if (gameState == GameManager::PLAYING) {
		if (mode != NORMAL_MODE) {
			GravityState stateG = NO_GRAVITY;
			if (target.GetCollisionKind() == COLLISION_KIND::OBB) {
				vec = dynamic_cast<OBBCollision&>(target).GetNearModelDirection(landPosition);

				if (vec.x > 0.0f) {
					stateG = LEFT_GRAVITY;
				}
				else if (vec.x < 0.0f) {
					stateG = RIGHT_GRAVITY;
				}
				else if (vec.y > 0.0f) {
					stateG = DOWN_GRAVITY;
				}
				else if (vec.y < 0.0f) {
					stateG = UP_GRAVITY;
				}
			}
			if (stateG != gravityManager->GetWorldGravityState()) {
				changeGravity = CHANGE;
			}
			else {
				changeGravity = SAME_GRAVITY;
			}
			if (attackState != NO_ATTACK) {
				enemyHitAddCounter = ENEMY_NO_HIT;
				//NormalModeStateChange();
				attackState = NO_ATTACK;
				charaState = NORMAL;
			}
			bumperTime = 0;
			bumperNoCollsionCounter = 0;
			//effectManager->StopEffekseer("WIND", true);
			gravityManager->ChangeWorldGravity((GravityState)stateG);
			if (attackState == ATTACK1) {
				attackGuadeDown = 0.0f;
				attackState = NO_ATTACK;
			}
			//effectManager->StopEffekseer("WIND", true);
			mode = NORMAL_MODE;
			rotate.z = 0.0f;
			rotateZAdd = GRAVITY_CHANGE_ROTATE_BASE_NUM;
			if (_move) {
				gravityAdd = BASE_GRAVITY_VELOCITY;
				gravityAddRate = GRAVITY_RATE_BASE;
			}
			else {
				
			}
			
			effectManager->StopEffekseer("NO_GRAVITY_EFFECT", false);
			//camera->CameraStateChange(Camera::CameraState::NOEMAL_CAMERA);
			//gravityMode = NO_GRAVITY;
			gravityManager->SetGravityChangeMode(GravityManager::WORLD);

			if (direction == RIGHT) {
				DirectionChange(-DX_PI_F / 2, RIGHT);
			}
			else {
				DirectionChange(DX_PI_F / 2, LEFT);
			}
			//GuageAmontReset();
			rateKeep = gravityAddRate;

			// 上のif文内に移動
			//gravityAddRate = GRAVITY_RATE_BASE;
			
			
			
			attackState = NO_ATTACK;
			attackHitTime = 0;
			rgb = RGB_WHITE;
			sound->StopSdSe("風");
			ResetAftimage();
			aloowKeepState = NO_GRAVITY;
			if (shake.time == SHAKE_INFINITY) {
				StopShake();
			}
			enemyHitAddCounter = ENEMY_NO_HIT;
			StopShake();
			ShakeStart(SHAKE_WORLD_CHANGE,false);
			sound->PlaySe("重力着地");
			AnimePlayChange(PlayerAnimesionKind::Land);
			effectManager->CreateEffekseer(position, &position, "JUMP", EFFECT_NORMALSPEED, ONE_SECOND, false, effectNormalSize, effectNormalRotate);
			
		}
	//}
}

void Player::SpickDamageMove()
{
	VECTOR spick = VZero;
	spickMoveAngle += SPICK_MOVE_ROTATION_CHANGE;
	rotate.x += SPICK_ROTATION_CHANGE;
	
	//if (jumpDown) {
		PositionMove(VGet(0.0f, sinf(spickMoveAngle) * SPICK_MOVE_RATE, 0.0f));
	//}
	//else {
		//PositionMove(VGet(0.0f, sinf(spickMoveAngle) * -50, 0.0f));
	//}
	if (spickMoveAngle >= DX_PI_F) {
		SpickMoveReset();
	}
}

void Player::SpickMoveReset()
{
	spickMoveAngle = 0;
	charaState = CharaBase::NORMAL;
	jumpDown = true;
}

void Player::AmongPointReset()
{
	NormalModeStateChange();
	centerPosition = amongPointPosition;
	gravityManager->ChangeWorldGravity(amongWorldGravity);
}

void Player::DamageMoveReady(VECTOR _velo, int _time)
{
	damageVelocity = _velo;
	damageMoveTime = _time;
	charaState = CharaBase::ENEMY_DAMAGE;
	damageVelocityKeep = damageVelocity;
}

void Player::DamageMove()
{
	if (damageMoveTime > 0) {
		float rate = 1.0f / (float)damageMoveTime;
		damageVelocity = EaseOut(damageVelocityKeep, VGet(0.0f, 0.0f, ENEMY_DAMAGE_SPEED),rate);
		//damageVelocity = VScale(damageVelocity, 0.95f);
		PositionMove(damageVelocity);
		damageMoveTime--;
	}
	else {
		DamageMoveReset();
	}
}

void Player::DamageMoveReset()
{
	InteriaStart(damageVelocity * GetRotateMatrix(), 60);
	damageVelocity = VZero;
	damageMoveTime = 0;
	charaState = CharaBase::NORMAL;
	if (groundRand) {
		AnimePlayChange(PlayerAnimesionKind::Wait);
	}
	else {
		AnimePlayChange(PlayerAnimesionKind::Jump_Down);
	}
	
}

void Player::PlayerInit(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId)
{
	//hModel = MV1DuplicateModel(_hModel);
	hModel = _hModel;
	//position = _position;
	centerPosition = _position + VGet(0.0f, centerPositionAmong, 0.0f);
	PositionSetDraw();
	scale = VGet(PLAYER_SIZE,PLAYER_SIZE,PLAYER_SIZE);
	rotate = _rotate;
	type = _type;
	collisionId = _collisionId;
	startPosition = _position;
	SetAmongPosition(_position,0.0f);
	moveRotate = rotate;
	
	AnimeInit();
//#ifdef _DEBUG
	//gameState = GameManager::PLAYING;
	//playStart = true;
//#else
	gameState = GameManager::PLAY_BEFORE;
//#endif
	if (gameState == GameManager::PLAY_BEFORE) {
		direction = RIGHT;
		//PlayEffect4();
		NoPlayingZModeInit();
		debugNoDamage = true;
		if (direction == RIGHT) {
			centerPosition.z -= 3000.0f;
			centerPosition.x -= 200.0f;
			centerPosition.y += 500.0f;
			movePointer = &Player::RightNoPlayingMove;
			degZMove = DEG_Z_MOVE;
		}
		else {
			centerPosition.z -= 3000.0f;
			centerPosition.x += 200.0f;
			centerPosition.y += 500.0f;
			movePointer = &Player::LeftNoPlayingMove;
			degZMove = -DEG_Z_MOVE;
		}
		debugNoDamage = true;
	}
}

void Player::LoseMoveStart(GameManager::LoseSceneSelect _kind, const int _time)
{
	int time = _time - 30;

	VECTOR sub = VZero;
	float norm = 0.0f;

	switch (_kind)
	{
	case GameManager::FirstStart:
		sub = VSub(startPosition, position);
		norm = atan2f(startPosition.y - position.y,startPosition.x - startPosition.x);
		//centerPosition = startPosition;
		//centerPosition = startPosition + VGet(0.0f, centerPositionAmong, 0.0f);
		loseFinalPosition = startPosition;
		loseFinalPosition.z = 0.0f;
		loseCenter = centerPosition;
		break;
	case GameManager::PointStart:
		sub = VSub(amongPointPosition, position);
		norm = atan2f(amongPointPosition.y - position.y, amongPointPosition.x - position.x);
		//centerPosition = amongPointPosition;
		//centerPosition = amongPointPosition + VGet(0.0f, centerPositionAmong, 0.0f);
		loseFinalPosition = amongPointPosition;
		loseFinalPosition.z = 0.0f;
		loseCenter = centerPosition;
		break;
	}
	loseMoveAdd = 1.0f / time;
	loseMoveTime = 0.0f;
	loseTime = _time;
	loseAddPosition.x = sub.x / loseMoveTime;
	loseAddPosition.y = sub.y / loseMoveTime;
	moveRotate = VZero;
	gravityManager->ChangeWorldGravity(amongWorldGravity);
	scale = VGet(PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE);
	gameState = GameManager::LOSE;
	loseState = Respown;
	loseAngle = norm;
	GaugeHealInit(loseTime, GUAGE_WAIT_TIME_ONE);
}

void Player::ReSpown()
{
	NormalModeStateChange();
	hp = PLAYER_HP_MAX;
	playerUi->ResetUi();
	scale = VGet(PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE);
	GuageAmontReset();
	gActive = true;
	loseState = No_Lose;

}

void Player::Dead()
{
	hp = 0;
	playerUi->SetHp(hp);
	DieStateChange();
	gActive = false;
	effectManager->CreateEffekseer(centerPosition, &centerPosition, "PLAYER_DIE_EFFECT", 0.2f, 240, false, effectNormalSize, effectNormalRotate);
	
}

void Player::DieStateChange()
{
	NormalModeStateChange();
	effectManager->CreateEffekseer(centerPosition, &centerPosition, "PLAYER_DIE_EFFECT", EFFECT_NORMALSPEED, 120, false, effectNormalSize, effectNormalRotate);
	HitStopStart(HitStopGet(10.0f, 60));
	loseState = Lose;
	gravityManager->SetGravityChangeMode(GravityManager::WORLD);
	gActive = false;
	bumperAdd = VZero;
	bumperTime = 0;
	hitStop.time = 0.0f;
	bumperNoCollsionCounter = 0;
}

void Player::AnimeInit()
{
	anime = new playerAnime(loadManager);
	anime->AnimeInformation(hModel, true, PlayerAnimesionKind::Jump_Down,1.0f, "PLAYER_DOWN");
	anime->AnimeInformation(hModel, false, PlayerAnimesionKind::Jump_Up,1.0f, "PLAYER_JUMP");
	anime->AnimeInformation(hModel, false, PlayerAnimesionKind::Land,1.0f, "PLAYER_LAND");
	anime->AnimeInformation(hModel, true, PlayerAnimesionKind::Zero_Gravity,1.0f, "PLAYER_GRAVITYMOVE");
	//anime->AnimeInformation(hModel, true, PlayerAnimesionKind::Attack);
	//anime->AnimeInformation(hModel, true, PlayerAnimesionKind::Gravity_Change);
	anime->AnimeInformation(hModel, true, PlayerAnimesionKind::Walk,0.2f,"PLAYER_WALK");
	anime->AnimeInformation(hModel, true, PlayerAnimesionKind::Wait, 1.0f, "PLAYER_STAY");
	anime->AnimeInformation(hModel, false, PlayerAnimesionKind::Damage, 0.05f, "PLAYER_DAMAGE");
	anime->AnimeInformation(hModel, true, PlayerAnimesionKind::Gravity_Move, 1.0f, "PLAYER_ZEROGRAVITY");
	anime->AnimeInformation(hModel, false, PlayerAnimesionKind::Die, 1.0f, "PLAYER_DIE");
	anime->AnimeInformation(hModel, false, PlayerAnimesionKind::Attack, 1.0f, "PLAYER_ATTACK");
	anime->AnimeInformation(hModel, false, PlayerAnimesionKind::Goal, 1.0f, "PLAYER_GOAL");
	anime->AnimeInformation(hModel, false, PlayerAnimesionKind::Rail, 1.0f, "PLAYER_RAIL");
	anime->AnimeSort();
	anime->AnimeStartHandle(PlayerAnimesionKind::Wait);
}

void Player::AnimePlayChange(PlayerAnimesionKind _kind)
{
	if (blendTime >= 10.0f) {
		if (anime->AnimeChange(_kind, 10.0f) != PlayerAnimesionKind::None) {
   			//printfDx("10        ");
		}
		
	}
	else {
		if (anime->AnimeChange(_kind, blendTime) != PlayerAnimesionKind::None) {
			//printfDx("%.1f       ", blendTime);
		}
	}
	
}

void Player::SetAftimageNum()
{
	aftimagePosition[0] = position;
	aftimageRotate[0] = rotate;


	for (int i = aftimageNum - 1; i > 0; i--)
	{
		aftimagePosition[i] = aftimagePosition[i - 1];
		aftimageRotate[i] = aftimageRotate[i - 1];
	}
}

void Player::ResetAftimage()
{
	for (int i = 0; i < ATTACK_AFTIMAGE_NUM; i++) {
		aftimagePosition[i] = VGet(-1, -1, -1);
		aftimageRotate[i] = VGet(-1, -1, -1);
	}
}

void Player::RightNoPlayingMove()
{
	if (zMoveGravityRot.y >= START_FIRST_ANGLE * DegToRad) {
		zMoveGravityRot.y = START_FIRST_ANGLE * DegToRad;
		zMoveGravityRot.x += START_ADD_ANGLE * DegToRad;
		rotate.x -= START_ADD_ANGLE * DegToRad;
		if (rotate.x <= -START_FINISH_ANGLE * DegToRad) {
			rotate.x = -START_FINISH_ANGLE * DegToRad;
		}
	}
}

void Player::LeftNoPlayingMove()
{
	if (zMoveGravityRot.y <= -START_FIRST_ANGLE * DegToRad) {
		zMoveGravityRot.y = -START_FIRST_ANGLE * DegToRad;
		zMoveGravityRot.x += START_ADD_ANGLE * DegToRad;
		rotate.x -= START_ADD_ANGLE * DegToRad;
		if (rotate.x <= -START_FINISH_ANGLE * DegToRad) {
			rotate.x = -START_FINISH_ANGLE * DegToRad;
		}
	}
}

void Player::ZMove()
{
	if (centerPosition.z >= 0) {
		centerPosition.z = 0.0f;
		objectGravityRate -= 0.01f;
		zMoveGravityRot.y += degZMove * DegToRad;
		rotate.y = zMoveGravityRot.y;
	}
	(this->*movePointer)();
}

void Player::NoPlayingZModeInit()
{
	rotate = VGet(-DX_PI_F / 2, 0.0f, DX_PI_F);
	zMoveGravityRot = VECTOR3(-DX_PI_F / 2, 0.0f, 0.0f);
	rgb.SetColor(255, 255, 0);
	mode = GRAVITY_MOVE_MODE;
	sound->PlaySe("風");
}

void Player::BumperHitInit(VECTOR _bumperPos)
{
	if (bumperNoCollsionCounter > 0) {
		return;
	}
#if false
	if (mode == GRAVITY_MOVE_MODE) {
		bumperAdd = gravityVelocity * -3.0f;
	}
	else {
		bumperAdd = velocity * -3.0f;
	}
#else 
	
	if (attackState == ATTACK1) {
		bumperAdd = (gravityVelocity + velocity) * -BUMPER_ATTACK_RATE;
		bumperTime = ONE_SECOND;
		if (bumperAdd.x >= BUMPER_SPEED_MAX) {
			bumperAdd.x = BUMPER_SPEED_MAX;
		}
		else if (bumperAdd.x <= -BUMPER_SPEED_MAX) {
			bumperAdd.x = -BUMPER_SPEED_MAX;
		}
		if (bumperAdd.y >= BUMPER_SPEED_MAX) {
			bumperAdd.y = BUMPER_SPEED_MAX;
		}
		else if (bumperAdd.y <= -BUMPER_SPEED_MAX) {
			bumperAdd.y = -BUMPER_SPEED_MAX;
		}
	}
	else {
		VECTOR _landPos = position * MGetRotZ(-gravityManager->GetWorldGravity());
		VECTOR _headPos = headPosition * MGetRotZ(-gravityManager->GetWorldGravity());
		//_centerPos.y = _centerPos.z;
	
		if (mode != GRAVITY_MOVE_MODE) {
			bumperTime = ONE_SECOND;
			VECTOR basePos;
			if (_bumperPos.y + BUMPER_CENTER_DIST <= _landPos.y) {
				basePos = VGet(0.0f, BUMPER_UP_VEC, 0.0f);
			}
			else if (_bumperPos.y - BUMPER_CENTER_DIST >= _headPos.y) {
				basePos = VGet(0.0f, -BUMPER_UP_VEC, 0.0f);
			}
			else {
				basePos = BUMPER_BASE_VEC;
				if (direction == LEFT) {
					basePos.x *= -1.0f;
				}
			}
			
			VECTOR bumperPlus = basePos * MGetRotZ(gravityManager->GetWorldGravity());

			bumperAdd = bumperPlus;
			bumperNoCollsionCounter = BUMPER_NO_COLLISION;
		}
		else {
			bumperTime = ONE_SECOND / 2;
			bumperAdd = (gravityVelocity + velocity) * -BUMPER_NORMAL_RATE;
			if (bumperAdd.x >= BUMPER_SPEED_MAX) {
				bumperAdd.x = BUMPER_SPEED_MAX;
			}
			else if (bumperAdd.x <= -BUMPER_SPEED_MAX) {
				bumperAdd.x = -BUMPER_SPEED_MAX;
			}
			if (bumperAdd.y >= BUMPER_SPEED_MAX) {
				bumperAdd.y = BUMPER_SPEED_MAX;
			}
			else if (bumperAdd.y <= -BUMPER_SPEED_MAX) {
				bumperAdd.y = -BUMPER_SPEED_MAX;
			}
			
		}
	}
	if (mode == GRAVITY_MOVE_MODE) {
		gravityAfterRotate = gravityRotation;
		GravityState g;
		g = gravityManager->GetReverceGravity(gravityMode);
		rotateZAdd = 0.04f;
		rotate.z = gravityManager->GetGravityAngle(gravityManager->MultGravityState(g));
		bumperNoCollsionCounter = BUMPER_NO_COLLISION_GRAVITY;
	}
	
#endif
	
	/*if (fabs(bumperAdd.x) >= 200.0f) {
		if (bumperAdd.x > 200.0f) {
			bumperAdd.x = 200.0f;
		}
		else {
			bumperAdd.x = -200.0f;
		}
	}
	if (fabs(bumperAdd.y) >= 200.0f) {
		if (bumperAdd.y > 200.0f) {
			bumperAdd.y = 200.0f;
		}
		else {
			bumperAdd.y = -200.0f;
		}
	}
	if (fabs(bumperAdd.z) >= 0.0f) {
		bumperAdd.z = 0.0f;
	}*/
	
	bumperStepAdd = 1.0f / bumperTime;
	bumperStep = 0.0f;
	sound->PlaySe("跳ね返り");
	//gravityMode = gravityManager->GetReverceGravity(gravityMode);
	//GetAloowAngle();
	//GracityMoveModeChange();
}

void Player::BumperHitMove()
{
	if (bumperTime > 0) {
		bumperTime--;
		bumperNoCollsionCounter--;
		bumperStep += bumperStepAdd;
		centerPosition += EaseInOut(bumperAdd,VZero,bumperStep);
		velocity += EaseInOut(bumperAdd, VZero, bumperStep);
		/*if (!jump) {
			gravityInfluence = abs(gravityAdd.y * objectGravityRate);
			PositionMove(VGet(0.0f, gravityInfluence, 0.0f));
		}*/
	}
}


int Player::GameStateChange(GameManager::GAME_STATE _state)
{
	Object::GameStateChange(_state);
	if (gameState == GameManager::PLAYING) {
		debugNoDamage = false;
	}
	playerUi->SetGameManagerState(_state);
	return static_cast<int>(gameState);
}

void Player::HealHp()
{
	if (hp >= PLAYER_HP_MAX) {
		effectManager->CreateEffekseer(centerPosition, &centerPosition, "PLAYER_HEAL_NO", EFFECT_NORMALSPEED, ONE_SECOND, false);
		return;
	}
	hp++;
	playerUi->SetHp(hp);
	effectManager->CreateEffekseer(centerPosition, &centerPosition, "PLAYER_HEAL_HP", EFFECT_NORMALSPEED, ONE_SECOND, false);
}

void Player::HealGravityGuage()
{
	if (playerUi->GetGravityMoveGaugeNum() >= 200) {
		effectManager->CreateEffekseer(centerPosition, &centerPosition, "PLAYER_HEAL_NO", EFFECT_NORMALSPEED, ONE_SECOND, false);
		return;
	}
	GaugeHealInit(GUAGE_ALL_HEAL_ITEM, GUAGE_WAIT_TIME_ONE,250.0f);
	effectManager->CreateEffekseer(centerPosition, &centerPosition, "PLAYER_HEAL_G", EFFECT_NORMALSPEED, ONE_SECOND, false);
	healG = true;
}
/*
void Player::PlayEffect4()
{
	//effectManager->CreateEffekseer(V2Get(0.0f, 0.0f), nullptr, "WIND", 1.0f, 60, true);
	//effectManager->CreateEffekseer(V2Get(1920.0f, 0.0f), nullptr, "WIND", 1.0f, 60, true);
	//effectManager->CreateEffekseer(V2Get(960.0f, 0.0f), nullptr, "WIND", 1.0f, 60, true);
	//effectManager->CreateEffekseer(V2Get(960.0f, 1080.0f), nullptr, "WIND", 1.0f, 60, true);
	//effectManager->CreateEffekseer(V2Get(0.0f, 540.0f), nullptr, "WIND", 1.0f, 60, true);
	//effectManager->CreateEffekseer(V2Get(1920.0f, 540.0f), nullptr, "WIND", 1.0f, 60, true);
	//effectManager->CreateEffekseer(V2Get(0.0f, 1080.0f), nullptr, "WIND", 1.0f, 60, true);
	//effectManager->CreateEffekseer(V2Get(1920.0f, 1080.0f), nullptr, "WIND", 1.0f, 60, true);
}
*/
void Player::RailMoveReady(CollisionBase& target)
{
	VECTOR rRo = target.GetObjectMe()->GetRotation();
	VECTOR rPos = target.GetObjectMe()->GetPosition();
	
	if (mode == RAIL_MOVE) {
		if ((railRotateKeep.x == rRo.x && railRotateKeep.y == rRo.y && railRotateKeep.z == rRo.z)) {
			return;
		}
		
	}
	if (changeRailTime > 0) {
		return;
	}
	/*
	//normのYが元ある値より大きかったら上に行く
	VECTOR norm = VGet(0.0f,0.0f,0.0f);
	if (rRo.z != 0.0f) {
		norm = railPos - rPos;
		norm = norm * MGetRotZ(gravityManager->GetWorldGravity());
		char str[256];
		sprintf_s<256>(str, "X = %.f1", norm.x);
		OutputDebugString(str);
		char str2[256];
		sprintf_s<256>(str2, "Y = %.f1\n", norm.y);
		OutputDebugString(str2);
	}
	
	
	
	//まいなすにすると右斜め上から右斜め下へ変更できる。左も同様
	if (gravityManager->GetWorldGravityState() == UP_GRAVITY) {
		if (norm.y < 0) {
			rotate.x = rRo.z + gravityManager->GetWorldGravity();
		}
		else {
			rotate.x = -rRo.z + gravityManager->GetWorldGravity();
		}
		
		//rotate.y *= -1;
	}
	else if (gravityManager->GetWorldGravityState() == LEFT_GRAVITY) {
		if (norm.y < 0) {
			rotate.x = rRo.z;
		}
		else {
			rotate.x = -rRo.z;
		}
	}
	else {
		if (norm.y > 0) {
			rotate.x = -rRo.z;
		}
		else {
			rotate.x = rRo.z;
		}
	}
	railPos = rPos;
	VECTOR p = VECTOR3(0, 250, 0) * GetRotateMatrix();
	if (mode != RAIL_MOVE) {
		if (direction == RIGHT) {
			railMoveAdd = VECTOR3(0, 0, -1);

		}
		else {
			railMoveAdd = VECTOR3(0, 0, -1);
		}
		centerPosition = target.GetPosition() + p;
		railCounter = 0.0f;
	}*/



	if (mode != RAIL_MOVE) {
		NormalModeStateChange();
		jumpNum = JUMP_NUM;
	}
	railMoveAdd.z = 0.0f;
	
	if (rRo.y == 0.0f) {
		DirectionChange(-DX_PI_F / 2, RIGHT);
	}
	else {
		DirectionChange(DX_PI_F / 2, LEFT);
	}
	
	rotate.x = rRo.z;

	AnimePlayChange(PlayerAnimesionKind::Rail);
	moveRotate = rotate;
	StopShake();
	mode = RAIL_MOVE;

	railRotateKeep = rRo;
	changeRailTime = CHANGE_RAIL_TIME;
	gActive = false;

	VECTOR p = VECTOR3(0, RAIL_UP_POS, 0) * target.GetObjectMe()->GetRotateMatrix();
	centerPosition = target.GetPosition() + p;
	
}

void Player::RailMove()
{
	PositionMove(railMoveAdd);
	railCounter += 0.005f;
	if (railCounter >= 1.0f) {
		railCounter = 1.0f;
	}
	//railMoveAdd.z -= 1.0f;
	railMoveAdd.z = EaseIn(railMoveAdd.z, -RAIL_MAX_SPEED, railCounter);
}

void Player::CompulsionGravityInit()
{
	if (!compulsionGravity && gravityNoChangeModeCounter <= 0.0f) {
		compulsionGravity = true;
		if (mode == NORMAL_MODE) {
			mode = GRAVITY_CHANGE_MODE;
			GracityChangeModeChange();
			AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
			//sound->PlaySe("重力起動");
			//AnimePlayChange(PlayerAnimesionKind::Zero_Gravity);
		}
	}
	
}

void Player::GoalStart()
{
	goal = true;
	sound->PlaySe("ファンファーレ");
	//NormalModeStateChange();
	//if (mode == GRAVITY_MOVE_MODE) {
		goalState = PlayingAfterMove;
		goalNowPos = centerPosition;
		NormalModeStateChange();
	//}
	//else {
		//GoalProductStart();
	//}
	effectManager->CreateEffekseer(centerPosition + VGet(1500.0f,0.0f,0.0f), nullptr, "CLEAR", 1.0f, 80, false);
	effectManager->CreateEffekseer(centerPosition + VGet(-1500.0f,0.0f,0.0f), nullptr, "CLEAR", 1.0f, 80, false);
	effectManager->CreateEffekseer(centerPosition + VGet(000.0f,1500.0f,0.0f), nullptr, "CLEAR", 1.0f, 80, false);
	
	gameState = GameManager::GOAL_FRONT_ANI;
	gActive = true;

}

void Player::GoalProductStart()
{
	goalState = Product;
	rotate = VZero;
	rotate.z = gravityRotation;
	AnimePlayChange(PlayerAnimesionKind::Goal);
	goalProductCounter = GOAL_PRODUCT_TIME;
	playerUi->InGoal();
	gActive = true;
}

void Player::GuageHeal()
{
	if (noGravityArea) {
		return;
	}
	if (guageWaitCounter > 0) {
		guageWaitCounter--;
		if (guageWaitCounter == 0.0f) {
			sound->PlaySe("回復");
		}
		return;
	}
	if (guageHealCounter > 0) {
		guageHealCounter--;
		playerUi->PlayerGaugeAdd(guageHealNum);
		if (guageHealCounter == 0) {
			healG = false;
		}
	}
}

void Player::GaugeHealInit(float _time, float _waitTime)
{
	if (guageHealCounter > 0 || playerUi->GetGravityMoveGaugeNum() == GUAGE_MAX) {
		return;
	}
	float num = GUAGE_MAX -  playerUi->GetGravityMoveGaugeNum();
	guageWaitCounter = _waitTime;
	guageHealNum = num / _time;
	guageHealCounter = _time + GUAGE_HEAL_WAIT;
}

void Player::GaugeHealInit(float _time, float _waitTime, float _num)
{
	if (guageHealCounter > 0) {
		return;
	}
	guageWaitCounter = _waitTime;
	guageHealNum = _num / _time;
	guageHealCounter = _time + GUAGE_HEAL_WAIT;
}

void Player::GuageHealReset()
{
	guageHealCounter = 0.0f;
	guageWaitCounter = 0.0f;
}

void Player::PlayAfterStart()
{
	NoPlayingZModeInit();
	objectGravityRate = GOAL_AFTER_RATE;
	moveMax.playerChangeGravityAddMax = GOAL_MAX;
	AnimePlayChange(PlayerAnimesionKind::Gravity_Move);
	gActive = true;
}

void Player::HpZero()
{
	hp = 0;
	playerUi->SetHp(hp);
	gActive = false;
}

void Player::WalkReset()
{
	soundCounter = 0;
	walk = false;
	walkSpeedAdd = WALK_SPEED_MIN;
	walkAnimeSpeedCounter = 0.0f;
}

void Player::InteriaStart(VECTOR _amont, float _max)
{
	gravitySlide = _amont;
	gravitySlideCounter = 0.0f;
	gravitySlideCoutnerMax = _max;
	sliderKeep = _amont;
}

void Player::InteriaReset()
{
	gravitySlideCoutnerMax = 0.0f;
	gravitySlide = VZero;
}

void Player::InteriaGravity()
{
	if (gravitySlideCounter < gravitySlideCoutnerMax) {
		gravitySlideCounter++;
		float r = gravitySlideCounter / gravitySlideCoutnerMax;
		VECTOR movePos;
		movePos = EaseInOut(gravitySlide, gravitySlide / INTERIA_DIV, r);
		centerPosition += movePos;
		velocity += movePos;
		//PositionMove(movePos);
	}
	else if (guageDownNumPlus != 0.0f && mode == GRAVITY_CHANGE_MODE) {
		InteriaReset();
		VECTOR movePos;
		movePos = sliderKeep / INTERIA_DIV;
		centerPosition += movePos;
		velocity += movePos;
	}
}

void Player::CameraSe()
{
	sound->PlaySe("カメラ回転");
}

void Player::DieProduct()
{
	NormalModeStateChange();
	loseState = Die;
	AnimePlayChange(PlayerAnimesionKind::Die);
	dieCounter = ONE_SECOND_DELTA;
}
