#include "flyEnemy.h"
#include "sphereCollision.h"
#include "stageObjectManager.h"
#include "../Library/time.h"
#include "function.h"
#include "../ImGui/imgui.h"

FlyEnemy::FlyEnemy(SceneBase* _scene) :NoGravityEnemyBase(_scene)
{
	DirectionChange(-DX_PI_F / 2, RIGHT);
	moveChangeTimerMax = MOVE_CHANGE_TIMER_MAX_DEFAULT;
	moveChangeTimer = moveChangeTimerMax / 2;
	hitStopTimer = 0;
	hitStopTimerMax = DAMAGE_TIMER_MAX_DEFAULT;
	deathTimer = 0;
	deathTimerMax = DEATH_TIMER_MAX_DEFAULT;
	gActive = false;
	size = VOne * FLY_ENEMY_SIZE;
	centerPositionAmong = size.x / 2;
	speed = SPEED_DEFAULT;

	type = (int)ObjectType::FLY_ENEMY;
	moveType = NO_TYPE;
	virDir = UP;
	defaultRotation = VGet(0, 0, 0);
	drawRotation = VGet(0, 0, 0);
	totalDamageMoveVec = VGet(0, 0, 0);
	turnTimer = 0.0f;
	rotateRand = 0.0f;
	damageVecRand = VGet(0, 0, 0);
	damageMoveTargetPos = VGet(0, 0, 0);
	deathEffectFlg = false;

	anime = nullptr;
}

FlyEnemy::~FlyEnemy()
{
	delete enemyLandSphereCollision;
}

void FlyEnemy::Update()
{
	if (anime == nullptr && hModel > 0)
	{
		AnimeInit();
	}

	//StageObjectManagerで生成された最初だけ入る
	if (moveType == NO_TYPE)
	{
		moveType = (MoveType)inEnemyType;
		defaultRotation = rotate;
		drawRotation = defaultRotation;
	}
	if (!visionMove) {
		return;
	}
	EnemyBase::Update();

	VECTOR damageMoveSpeed = VGet(0, 0, 0);//ダメージを受けた時に動くベクトル
	switch (state)
	{
	case EnemyBase::ENEMY_WAIT:
		switch (moveType)
		{
		case HORIZONTAL:
			turnTimer += Time::DeltaTime() / HORIZONTAL_TURN_TIME_MAX;//回転タイマーの加算、1が最大
			if (turnTimer > 1)
			{
				turnTimer = 1.0f;
				state = ENEMY_MOVE;
				speed = SPEED_DEFAULT;
			}
			//向きを反転させる
			if (direction == RIGHT)
			{
				rotate.y = SmoothStep(0.0f, DX_PI_F, turnTimer) - DX_PI_F/2;
				drawRotation.z = SmoothStep(defaultRotation.z - FLY_ENEMY_HORIZONTAL_LEAN * DegToRad, defaultRotation.z + FLY_ENEMY_HORIZONTAL_LEAN * DegToRad, turnTimer);;
			}
			else
			{
				rotate.y = SmoothStep(DX_PI_F, 0.0f, turnTimer) - DX_PI_F / 2;
				drawRotation.z = SmoothStep(defaultRotation.z + FLY_ENEMY_HORIZONTAL_LEAN * DegToRad, defaultRotation.z - FLY_ENEMY_HORIZONTAL_LEAN * DegToRad, turnTimer);
			}

			if (turnTimer >= 1.0f)
			{
				turnTimer = 0.0f;
				if (direction == RIGHT)
				{
					direction = LEFT;
				}
				else
				{
					direction = RIGHT;
				}
			}
			break;
		case VERTICAL:
			rotate.y = 0;
			turnTimer += Time::DeltaTime() / VIRTICAL_TURN_TIME_MAX;//回転タイマーの加算、1が最大
			if (turnTimer > 1)
			{
				turnTimer = 1.0f;
				state = ENEMY_MOVE;
				speed = SPEED_DEFAULT;
			}
			if (virDir == UP)
			{
				//進行方向に機体を傾かせる
				drawRotation.x = SmoothStep(defaultRotation.x - FLY_ENEMY_VERTICAL_LEAN * DegToRad, defaultRotation.x + FLY_ENEMY_VERTICAL_LEAN * DegToRad, turnTimer);
			}
			else
			{
				//進行方向に機体を傾かせる
				drawRotation.x = SmoothStep(defaultRotation.x + FLY_ENEMY_VERTICAL_LEAN * DegToRad, defaultRotation.x - FLY_ENEMY_VERTICAL_LEAN * DegToRad, turnTimer);
			}
			if (turnTimer >= 1.0f)
			{
				turnTimer = 0.0f;
				if (virDir==UP)
				{
					virDir=DOWN;
				}
				else
				{
					virDir = UP;
				}
			}
			break;
		default:
			break;
		}
		break;
	case EnemyBase::ENEMY_MOVE:
		switch (moveType)
		{
		case NO_MOVE:
			speed = 0;
			//PositionMove(VGet(0, 0,0));
			break;
		case HORIZONTAL:
			//横方向への移動
			RoundTripMove(VGet(0, 0, -1 * speed));
			break;
		case VERTICAL:
			//縦方向の移動
			if (virDir == UP)
			{
				RoundTripMove(VGet(0, -1 * speed, 0));
			}
			else
			{
				RoundTripMove(VGet(0, speed, 0));
			}
			break;
		default:
			break;
		}

	case EnemyBase::ENEMY_ATTACK:
		break;
	case EnemyBase::ENEMY_HITSTOP:
		anime->AnimeFirstBack();
		EnemyHitStop();
		break;
	case EnemyBase::ENEMY_DAMAGE:
		anime->AnimeFirstBack();

		if (damageTimer <= 0)
		{
			//乱数で動きの終着点を変える
			damageVecRand.x = GetRand(65535) / 65535.0f * (FLY_ENEMY_DAMAGE_MOVE_RAND_MAX.x - FLY_ENEMY_DAMAGE_MOVE_RAND_MIN.x) + FLY_ENEMY_DAMAGE_MOVE_RAND_MIN.x;
			damageVecRand.y = GetRand(65535) / 65535.0f * (FLY_ENEMY_DAMAGE_MOVE_RAND_MAX.y - FLY_ENEMY_DAMAGE_MOVE_RAND_MIN.y) + FLY_ENEMY_DAMAGE_MOVE_RAND_MIN.y;
			damageVecRand.z = GetRand(65535) / 65535.0f * (FLY_ENEMY_DAMAGE_MOVE_RAND_MAX.z - FLY_ENEMY_DAMAGE_MOVE_RAND_MIN.z) + FLY_ENEMY_DAMAGE_MOVE_RAND_MIN.z;
			damageMoveTargetPos.x = FLY_ENEMY_DAMAGE_MOVE_MAX.x *damageVecRand.x;
			damageMoveTargetPos.y = FLY_ENEMY_DAMAGE_MOVE_MAX.y * damageVecRand.y;
			damageMoveTargetPos.z = FLY_ENEMY_DAMAGE_MOVE_MAX.z * damageVecRand.z;
			rotateRand = VSize(damageMoveTargetPos) / VSize(FLY_ENEMY_DAMAGE_MOVE_MAX);//動きの長さに応じて回転の量を変える
			//半分の確率で動きを反転させる
			if (GetRand(1))
			{
				rotateRand *= -1;
			}
			
		}

		//ダメージを受けた後の演出の処理
		damageTimer += Time::DeltaTime() / FLY_ENEMY_DAMAGE_TIME_MAX;

		rotate.y = EaseOut(beforeDamageRotation.y, beforeDamageRotation.y + DX_PI_F * FLY_ENEMY_DAMAGE_TURN_NUM * 2*rotateRand, damageTimer);//少しずつY回転させる

		damageMoveSpeed = EaseOut(VGet(0,0,0), damageMoveTargetPos, damageTimer);//定数で宣言した値に応じて徐々に移動させる
		damageMoveSpeed -= totalDamageMoveVec;//既に動いている分のベクトルは減らす
		totalDamageMoveVec += damageMoveSpeed;

		PositionMove(VGet(damageMoveSpeed.x,damageMoveSpeed.y,0));//X,Y軸の移動
		centerPosition.z += damageMoveSpeed.z;//Z座標はプレイヤーの向きに関係なく奥に移動させたいので直接positionに加算する

		if(!deathEffectFlg&&damageTimer>FLY_ENEMY_DEATH_EFFECT_CREATE_RATE)
		{
			deathEffectFlg = true;
			EnemyDeath(FLY_ENEMY_DEATH_EFFECT_OFFSET);
			ShakeStart(ShakeState{ HIT_STOP_POWER, HIT_STOP_TIME_MAX }, false);
			
		}
		if (damageTimer > 1.0f)
		{
			state = ENEMY_DEATH;
		}

		break;
	case EnemyBase::ENEMY_DEATH:
		anime->AnimeFirstBack();
		//scale.y -= 50.0f / deathTimerMax;

		if (++deathTimer > deathTimerMax)
		{
			active = false;
		}
		break;
	default:
		break;
	}

	anime->Update();
	moveRotate = rotate;
	PositionSetDraw();
}

void FlyEnemy::Draw()
{
	rotate = VGet(drawRotation.x, rotate.y, drawRotation.z);//回転の適用
	Object::Draw();
	rotate = VGet(defaultRotation.x,rotate.y,defaultRotation.z);//回転を元に戻す
}

void FlyEnemy::CollisionEvent(CollisionBase& me, CollisionBase& _target)
{
	EnemyBase::CollisionEvent(me, _target);
	switch (_target.GetTagMe())
	{
	case COLLISION_OBJECT_KIND::BLOCK:
		DirectionReverse();
		break;
	default:
		break;
	}
}

void FlyEnemy::AnimeInit()
{
	anime = new enemyFlyAnime(loadManager);
	anime->AnimeInformation(hModel, true, EnemyFlyAnimationKind::Walk, 10.0f, "ENEMY_FLY_WALK");
	anime->AnimeSort();
	anime->AnimeStartHandle(EnemyFlyAnimationKind::Walk);
}

void FlyEnemy::DirectionReverse()
{
	switch (moveType)
	{
	case HORIZONTAL:
		EnemyBase::DirectionReverse();
		//無理やり壁にめりこまないよう押し出す
		switch (moveType)
		{
		case HORIZONTAL:
			PositionMove(VGet(0, 0, 5));
			break;
		case VERTICAL:
			PositionMove(VGet(0, 5, 0));
			break;
		default:
			break;
		}
		break;
	case VERTICAL:
		if (virDir == UP)
		{
			virDir = DOWN;
		}
		else
		{
			virDir = UP;
		}
		break;
	}
}
