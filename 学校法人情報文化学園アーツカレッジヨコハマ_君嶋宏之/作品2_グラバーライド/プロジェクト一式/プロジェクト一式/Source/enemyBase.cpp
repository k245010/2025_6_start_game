#include "enemyBase.h"
#include "sphereCollision.h"
#include "obbCollision.h"

#include "effectManager.h"

EnemyBase::EnemyBase(SceneBase* _scene) : CharaBase(_scene)
{
	counter = 60;

	position = VGet(0, -200, 0);
	size = VOne * 436.0f;

	sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::ENEMY);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER_ATTACK);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);

	obbCollision->SetTagMe(COLLISION_OBJECT_KIND::NONE);
	obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::NONE);


	speed = 5.0f;

	moveTime = 240;

	state = ENEMY_WAIT;

	gActive = true;

	for (int i = 0; i < G_DIRECTION_NUM; i++)
	{
		canGravityDirection[i] = true;
	}

	centerPositionAmong = size.x/2;
	inEnemyType = 0;

	moveDecel = 0;
	waitTimerMax = WAIT_TIMER_MAX_DEFAULT;
	waitTimer = waitTimerMax;
	hitStopTimerMax = HIT_STOP_TIME_MAX;

	doMultScaleInit = false;

	beforeDamageRotation = VGet(0, 0, 0);
}

EnemyBase::~EnemyBase()
{
	/*delete sphereCollision;
	delete obbCollision;*/
	//MV1DeleteModel(hModel);
}

void EnemyBase::Update()
{
	if (hitStop.time > 0 || shake.time > 0) {
		HitStopShake();
		velocity = VGet(0, 0, 0);
		return;
	}
	CharaBase::Update();
	if (debugStop) {
		return;
	}
}

void EnemyBase::Draw()
{
	Object::Draw();
}

void EnemyBase::EnemyInit(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId)
{
	EnemyInit(_hModel, _position, _scale, _rotate, _type, _collisionId,0);
}

void EnemyBase::EnemyInit(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _eType)
{
	if (_position.z >= 300.0f) {
		if (sphereCollision != nullptr)
		{
			delete sphereCollision;
			sphereCollision = nullptr;
		}
		if (obbCollision != nullptr)
		{
			delete obbCollision;
			obbCollision = nullptr;
		}
		if (enemyLandSphereCollision != nullptr) {
			delete enemyLandSphereCollision;
			enemyLandSphereCollision = nullptr;
		}
		if (attackSphereCollision != nullptr) {
			delete attackSphereCollision;
			attackSphereCollision = nullptr;
		}
	}
	hModel = MV1DuplicateModel(_hModel);
	centerPosition = _position;
	PositionSetDraw();
	scale = _scale;
	rotate = _rotate;
	type = _type;
	collisionId = _collisionId;
	inEnemyType = _eType;
}

void EnemyBase::StateInDamage()
{
	noDamageTime = hitStopTimerMax;
	HitStopStart(HitStopGet(30, hitStopTimerMax));
	state = ENEMY_DAMAGE;
}

void EnemyBase::CollisionEvent(CollisionBase& me, CollisionBase& _target)
{
	switch (me.GetTagMe())
	{
	case COLLISION_OBJECT_KIND::LAND:
		GroundLanding();
		break;
	default:
		break;
	}
	switch (_target.GetTagMe())
	{
	case COLLISION_OBJECT_KIND::PLAYER_ATTACK:
		state = ENEMY_HITSTOP;
		beforeDamagePosition = position;
		beforeDamageRotation = rotate;
		me.DeleteTagMe();
		break;
	default:
		break;
	}
}

void EnemyBase::DirectionReverse()
{
	if (direction == RIGHT)
	{
		DirectionChange(DX_PI_F / 2, LEFT);
	}
	else
	{
		DirectionChange(-DX_PI_F / 2, RIGHT);
	}
}

void EnemyBase::EnemyHitStop()
{
	HitStopShake();
	if (hitStopTimer <= 0)
	{
		ShakeStart(ShakeState{ HIT_STOP_POWER, HIT_STOP_TIME_MAX},false);
		EnemyDeath();
	}
	if (++hitStopTimer > hitStopTimerMax)
	{
		state = ENEMY_DAMAGE;
	}
}

void EnemyBase::EnemyDeath(VECTOR _offset)
{
	effectManager->CreateEffekseer(position + _offset, nullptr, "ENEMY_DEATH", 1, 180, false, VGet(2.0f, 2.0f, 2.0f), VZero);
}

void EnemyBase::RoundTripMove(VECTOR _moveVec)
{
	if (++moveChangeTimer < MOVE_CHANGE_TIMER_MAX_DEFAULT)
	{
		if (moveChangeTimer > MOVE_CHANGE_TIMER_MAX_DEFAULT * MOVE_CHANGE_DECELERATION_START_RATIO)
		{
			moveDecel *= MOVE_DECELERATION_BEFORE_SPEED_RATIO;
			moveDecel += MOVE_DECELERATION_RATIO;
			speed -= moveDecel;
			if (speed < 0)
			{
				speed = 0;
			}

		}
		PositionMove(_moveVec);
	}
	else
	{
		state = ENEMY_WAIT;
		moveChangeTimer = 0;
	}
}

void EnemyBase::MultScaleInit(VECTOR _scale)
{
	if(!doMultScaleInit)
	{
		doMultScaleInit = true;
		scale.x = _scale.x;
		scale.y*= _scale.y;
		scale.z = _scale.z;
	}
}
