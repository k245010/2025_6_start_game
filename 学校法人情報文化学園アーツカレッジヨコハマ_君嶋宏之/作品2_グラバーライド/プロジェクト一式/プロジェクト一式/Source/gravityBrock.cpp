#include "gravityBrock.h"
#include "player.h"

GravityBrock::GravityBrock(SceneBase* _scene):StageObject(_scene)
{
}

GravityBrock::~GravityBrock()
{
}

int GravityBrock::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId)
{
	StageObject::Init(_hModel, _position, _scale, _rotate, _type, _collisionId);
		//canGravityDirection[_gDirectionType] = true;
		for (int i = 0; i < G_DIRECTION_NUM; i++)
		{
			canGravityDirection[i] = true;
		}
		size = VOne * 214.0f;
		gActive = true;
		obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::HEAD);
		sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
		//sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);
		sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_AIR_BARRIER);


		obbCollision->DeleteTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
		float volume = scale.x + scale.y + scale.z;
		float rate = (volume - 4 - 4 - 4) * -0.07f + 1.5f;
		objectGravityRate = rate;
		moveMax.playerChangeGravityAddMax = -rate * 40.0f;
		moveMax.normalGravityAddMax = -rate * 20.0f;
		gravityAddRate = 0.01f;
		player = GetScene()->FindGameObject<Player>();

		return 0;
}

void GravityBrock::Update()
{
	float s = scale.x;
	if (fabsf(VSize(lastPosition - position)) >= VSize(gravityVelocity) / 1.5f)
	{
		onMove = true;
	}
	else
	{
		onMove = false;
	}
	lastPosition = position;	
	if (player->GetPlayerMode() == Player::GRAVITY_MOVE_MODE) {
		counter++;
		if (counter >= 60) {
			gravityAddRate += 0.012f;
			if (gravityAddRate >= 1.0f) {
				gravityAddRate = 1.0f;
			}
		}
	}
	else if (player->GetPlayerMode() == Player::NORMAL_MODE) {
		gravityAddRate = 0.01f;
		counter = 0;
	}
}
