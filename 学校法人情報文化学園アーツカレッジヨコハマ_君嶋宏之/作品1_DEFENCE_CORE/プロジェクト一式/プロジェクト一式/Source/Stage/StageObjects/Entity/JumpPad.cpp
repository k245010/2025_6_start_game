#include "JumpPad.h"
#include "../../../Collision/Collisions/OBBCollision.h"

JumpPad::JumpPad(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : EntityBase(_trans, _modelData,StageObjectData::STAGE_OBJECT_KIND::JUMP_PAD,_hp,_putPlaceKind)
{
	obbColl->SetTagMe(COLLISION_OBJECT_KIND::JUMP_PAD);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ERASER);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY);
}

JumpPad::~JumpPad()
{
}

void JumpPad::Update()
{
	EntityBase::Update();
}

void JumpPad::Draw()
{
	EntityBase::Draw();
}

bool JumpPad::HitEntity(const CollisionHitInfoData& _targetData)
{
	if (_targetData.tag == COLLISION_OBJECT_KIND::ERASER)
	{
		active = false;
		return !obbColl->GetCollisionHitInfoData().onDeleteMe;
	}
	return false;
}
