#include "walkEnemy.h"
#include "sphereCollision.h"
#include "stageObjectManager.h"
#include "function.h"

WalkEnemy::WalkEnemy(SceneBase* _scene) :NoGravityEnemyBase(_scene)
{
	DirectionChange(DX_PI_F / 2, RIGHT);
	deathTimer = 0;
	deathTimerMax = DEATH_TIMER_MAX_DEFAULT;
	gActive = false;
	speed = SPEED_DEFAULT;
	moveRotate = rotate;

	type = (int)ObjectType::WALK_ENEMY;
	moveChangeTimer = moveChangeTimerMax/2;
	rotate.y = 0;
	size = VOne * 300.0f;

	anime = nullptr;
	waitTimerMax = 120;
}

WalkEnemy::~WalkEnemy()
{
	delete enemyLandSphereCollision;
}

void WalkEnemy::Update()
{
	if (anime == nullptr && hModel > 0)
	{
		AnimeInit();
	}

	MultScaleInit(VGet(1.6f, 1.6f, 1.6f));
	moveRotate = rotate;
	if (!visionMove) {
		return;
	}
	EnemyBase::Update();
	switch (state)
	{
	case EnemyBase::ENEMY_WAIT:
		//Å‰‚Ì1F‚Ì‚Ý“ü‚é
		if (waitTimer == 0)
		{
			anime->AnimeChange(EnemyNomalAnimationKind::Turn, ANIMATION_WALK_TO_TURN_CHANGE_TIME);
		}

		if (direction == RIGHT)
		{
			rotate.y = SmoothStep(-DX_PI_F / 2, DX_PI_F / 2, (float)waitTimer / waitTimerMax);
		}
		else
		{
			rotate.y = SmoothStep(DX_PI_F / 2, -DX_PI_F / 2, (float)waitTimer / waitTimerMax);
		}

		if (++waitTimer > waitTimerMax)
		{
			waitTimer = 0;
			state = ENEMY_MOVE;
			speed = SPEED_DEFAULT;
			anime->AnimeChange(EnemyNomalAnimationKind::Walk, ANIMATION_TURN_TO_WALK_CHANGE_TIME);
			if (direction == RIGHT)
			{
				DirectionChange(DX_PI_F / 2, LEFT);
			}
			else
			{
				DirectionChange(-DX_PI_F / 2, RIGHT);
			}
		}
		break;
	case EnemyBase::ENEMY_MOVE:
		RoundTripMove(VGet(0, 0, -1 * speed));
		break;
	case EnemyBase::ENEMY_ATTACK:
		break;
	case ENEMY_HITSTOP:
		EnemyHitStop();
		break;
	case EnemyBase::ENEMY_DAMAGE:

		state = ENEMY_DEATH;

		break;
	case EnemyBase::ENEMY_DEATH:
		if (++deathTimer > deathTimerMax)
		{
			active = false;
		}
		break;
	default:
		break;
	}

	PositionSetDraw();

	anime->Update();
}

void WalkEnemy::Draw()
{
	Object::Draw();
}

void WalkEnemy::AnimeInit()
{
	anime = new enemyNomalAnime(loadManager);
	anime->AnimeInformation(hModel, true, EnemyNomalAnimationKind::Walk, 1.0f, "ENEMY_NOMAL_WALK");
	anime->AnimeInformation(hModel, true, EnemyNomalAnimationKind::Turn, 1.0f, "ENEMY_NOMAL_TURN");
	anime->AnimeSort();
	anime->AnimeStartHandle(EnemyNomalAnimationKind::Turn);
}
