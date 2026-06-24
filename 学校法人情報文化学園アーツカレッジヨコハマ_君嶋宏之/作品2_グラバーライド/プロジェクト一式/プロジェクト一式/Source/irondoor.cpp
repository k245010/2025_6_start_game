#include "irondoor.h"

namespace {
	//ドアの動く速さ
	const float IRON_DOOR_SPEED = 5.0f;

		//次にスイッチが切り替えられるまでの時間
	const int WAIT_TIME = 30;
}

IronDoor::IronDoor(SceneBase* _scene) :SwitchMoveObject(_scene)
{
	
	size = VGet(106.0f, 206.0f, 52.0f);
	for (int i = 0; i < (int)SphereCollisionType::MAX; i++) {
		sphereCollisionV.emplace_back(new SphereCollision(this));
		collisionPositionV.emplace_back(VZero);
	}
	sphereCollisionV[0]->SetTagMe(COLLISION_OBJECT_KIND::HEAD);
	sphereCollisionV[0]->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);
	sphereCollisionV[1]->SetTagMe(COLLISION_OBJECT_KIND::LAND);
	sphereCollisionV[1]->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);

	waitTime = WAIT_TIME;
	timeStart = false;
}	

IronDoor::~IronDoor()
{
	for (int i = 0; i < (int)SphereCollisionType::MAX; i++)
	{
		if (sphereCollisionV[i] != nullptr)
			delete sphereCollisionV[i];
	}
}

int IronDoor::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _switchId)
{
	SwitchObject::Init(_hModel, _position, _scale, _rotate, _type, _collisionId,_switchId);
	sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK);			// 今後消したほうが処理が軽くなる可能性がある　その場合、全ての鉄ドアの上下の接触地点にバリアブロックをおく必要がある
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_AIR_BARRIER);
	obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR);
	obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::LAND);
	obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::HEAD);
	return 0;
}

void IronDoor::Update()
{
	SwitchMoveObject::Update();
	collisionPositionV[(int)SphereCollisionType::TOP] =  position+VECTOR3(0.0f,(scale.y * size.y) / 2.0f / 2.0f,0.0f) * GetRotateMatrix();
	collisionPositionV[(int)SphereCollisionType::BOTTOM] = position + VECTOR3(0.0f, (scale.y * size.y) / -2.0f / 2.0f, 0.0f) * GetRotateMatrix();
	//velocity = VZero;
	if (switchMoveObjectState == SwitchMoveObjectState::STAY) {
		if (timeStart) {
			waitTime--;
			if (waitTime < 0) {
				switchMoveObjectState = SwitchMoveObjectState::BACK;
				waitTime = WAIT_TIME;
				timeStart = false;
			}
		}
	}

	//bool da = sphereCollision[0]->IsTargetTag(COLLISION_OBJECT_KIND::BLOCK);
}

void IronDoor::CollisionEvent(CollisionBase& me, CollisionBase& target)
{

	switch (me.GetTagMe())
	{
	case COLLISION_OBJECT_KIND::HEAD:
		position-= VGet(0,1,0)*MGetRotZ(rotate.z);
		switchMoveObjectState = SwitchMoveObjectState::STAY;
		timeStart = true;
		break;
	case COLLISION_OBJECT_KIND::LAND:
		
		//timeStart = false;
		break;
	default:
		break;
	}
	/*if (target.GetTagMe() == COLLISION_OBJECT_KIND::BLOCK) {
		switchMoveObjectState = SwitchMoveObjectState::STAY;
	}*/


	//やりたいこと...①ブロックに当たるまで動かす
					//②timerが起動timeが0になったらbackが起動
					//③けつがブロックに当たったら止まる
			
	//if (頭が当たったら) {
	//	waitTime = WAIT_TIME;
	//	timeStart = true;
	//}
	//if (けつがあたったら) {
	//	timeStart = false;
	//}


}

void IronDoor::SwitchObjectMoveProcess()
{
	velocity.y += IRON_DOOR_SPEED;
	position += VTransform(velocity, MGetRotZ(rotate.z));
}

void IronDoor::SwitchObjectBackProcess()
{
	velocity.y += IRON_DOOR_SPEED;
	position -= VTransform(velocity, MGetRotZ(rotate.z));
}



