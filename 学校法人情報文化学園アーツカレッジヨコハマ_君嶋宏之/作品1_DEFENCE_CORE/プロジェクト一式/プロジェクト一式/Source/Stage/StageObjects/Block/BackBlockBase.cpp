#include "BackBlockBase.h"
#include "../../../Collision/Collisions/ModelCollision.h"

BackBlockBase::BackBlockBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, bool _isMeshCollision, const PUT_PLACE_KIND& _putPlaceKind) : BlockBase(_trans,_modelData,_kind,_hp,_putPlaceKind)
{
	if (!_isMeshCollision)
		return;	// ƒƒbƒVƒ…‚Ì“–‚½‚è”»’è‚ð‚µ‚È‚¢‚Æ‚«‚ÍAreturn

	modelCollision = new ModelCollision(&transform, COLLISION_OBJECT_KIND::BACK_BLOCK, hModel, StageObjectData::GetToString(_kind));
	modelCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
}

BackBlockBase::~BackBlockBase()
{
	if (modelCollision != nullptr)
	{
		delete modelCollision;
		modelCollision = nullptr;
	}
}
