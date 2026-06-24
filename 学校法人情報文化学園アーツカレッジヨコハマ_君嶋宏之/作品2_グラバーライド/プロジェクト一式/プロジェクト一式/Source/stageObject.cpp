#include "stageObject.h"
#include "stageObjectManager.h"


StageObject::StageObject(SceneBase* scene):Object(scene)
{
	size = VOne * 107.6f;
	type = -1;

	moveMax = MaxGet(-25.0f, -25.0f);
}

StageObject::~StageObject()
{

}

int StageObject::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId)
{
	Object::Init(_hModel, _position, _scale, _rotate,_type,_collisionId);
	
	if (IsException((ObjectType)type))
	{
		return -1;
	}
	
	switch (type)
	{
	case (int)ObjectType::NOMAL_A:
	case (int)ObjectType::NOMAL_B:
	case (int)ObjectType::GRAVITY_A:

		obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::LAND);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::HEAD);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER_RAY);

		break;
	case (int)ObjectType::AIR_BARRIER:
		obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_AIR_BARRIER);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_GRAVITY);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::BLOCK_IRON_DOOR);
		break;

	case (int)ObjectType::SPIKE:
		obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_SPIKE);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::LAND);
		break;
	case (int)ObjectType::HEAL_ITEM:
		sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::ITEM_HEAL);
		sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
		break;
	case (int)ObjectType::GHEAL_ITEM:
		sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::ITEM_GHEAL);
		sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
		break;
	case (int)ObjectType::RAIL:
		sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_RAIL);
		sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
		break;
	case (int)ObjectType::BREAKABLE:
		obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_BROKEN);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER_ATTACK);
		obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::LAND);
		break;

	case (int)ObjectType::COIN:
		sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_COIN);
		sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
		break;

	}
	
	return 1;
}

bool StageObject::IsException(const ObjectType& _type)
{
	if (_type != ObjectType::GOAL && _type != ObjectType::POINT && position.z >= 300.0f) 
	{
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
		return true;
	}

	switch (_type)
	{
	case ObjectType::FENCE:
	case ObjectType::SIGNBOARD:
	case ObjectType::WALLHANGING:
	case ObjectType::BORAD:
	case ObjectType::BORAD2:
	case ObjectType::DESK:
	case ObjectType::PC:
	case ObjectType::PLUMB:
	case ObjectType::PLUMB2:
	case ObjectType::CAMERA:
	case ObjectType::BOX:
	case ObjectType::MACHINE:

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
		return true;
		break;
	}
	return false;
}

