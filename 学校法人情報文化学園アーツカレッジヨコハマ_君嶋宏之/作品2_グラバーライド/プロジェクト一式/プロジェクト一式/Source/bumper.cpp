#include "bumper.h"
#include "effectManager.h"

namespace {
	const float MAX_BOUND_SCALE = 5.5f;
}

Bumper::Bumper(SceneBase* _scene):StageObject(_scene)
{
	rate = 0.0f;
	//isBound = false;
	state = BumperState::STAY;
}

Bumper::~Bumper()
{
}

void Bumper::Update()
{
	switch (state)
	{
	case Bumper::BumperState::BOUND:
		if (rate < 1.0f) {
			rate += 0.08f;
			scale.x = EaseOutElastic(hitBeforeScale.x, MAX_BOUND_SCALE, rate);
			scale.y = EaseOutElastic(hitBeforeScale.y, MAX_BOUND_SCALE, rate);
		}
		else {
			state = BumperState::STAY;
			rate = 0.0f;
			scale = hitBeforeScale;
		}
		break;
	case Bumper::BumperState::STAY:
		if (rate < 1.0f) {
			rate += 0.02f;
			scale.x = SinCube(hitBeforeScale.x, MAX_BOUND_SCALE, rate);
			scale.y = SinCube(hitBeforeScale.y, MAX_BOUND_SCALE, rate);
		}
		else {
			scale = hitBeforeScale;
			rate = 0.0f;
		}
		break;
	default:
		break;
	}
}

void Bumper::CollisionEvent(CollisionBase& me, CollisionBase& target)
{
	if (target.GetTagMe() == COLLISION_OBJECT_KIND::PLAYER) {

		if (state==BumperState::STAY) {
			effectManager->CreateEffekseer(position, nullptr, "BUMPER_BOUND", 1.0f, 60, false);
			state = BumperState::BOUND;
			rate = 0.0f;
			scale = hitBeforeScale;
		}
	}
}

int Bumper::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int collisionId)
{
	if (StageObject::Init(_hModel, _position, _scale, _rotate, _type, collisionId) == -1) {
		return -1;
	}
	sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_BUMPER);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	hitBeforeScale = scale;
	return 0;
}
