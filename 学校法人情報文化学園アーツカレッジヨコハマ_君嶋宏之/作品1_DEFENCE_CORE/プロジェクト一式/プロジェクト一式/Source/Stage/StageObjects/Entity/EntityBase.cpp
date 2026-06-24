#include "EntityBase.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../EntityController/EntityController.h"
#include "../../../../Library/DebugNew.h"

EntityBase::EntityBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : StageObjectBase(_trans, _modelData, _kind, _hp, _putPlaceKind)
{
	//  ƒnƒ“ƒhƒ‹‚ðdelete‚µ‚È‚¢ EntityController‚ªíœ‚·‚é
	Object3D::SetDontDeleteHmodel();

	//sphereColl = new SphereCollision(&transform, [this](const CollisionHitInfoData& tr) {return HitBlock(tr);});
	sphereColl	= nullptr;
	obbColl		= new OBBCollision(&transform, COLLISION_OBJECT_KIND::NONE, "ENTITY_OBB", [this](const CollisionHitInfoData& tr) {return HitEntity(tr);});
	active		= true;
}

EntityBase::~EntityBase()
{
	if (obbColl != nullptr)
	{
		delete obbColl;
		obbColl = nullptr;
	}

	// Controller‚ªnew‚³‚ê‚Ä‚¢‚½‚ç
	if (EntityController::IsActiveInstance())
		EntityController::GetEntityController()->DeleteNavigationAreaBoxTransformPtr(&transform);
}

void EntityBase::Update()
{
	Object3D::Update();

	if (!active)
		EntityController::GetEntityController()->DeleteEntity(this);
}

void EntityBase::Draw()
{
	Object3D::Draw();

#if _DEBUG
	VECTOR3 halfLen = VDivI((VMult(transform.scale, transform.size)), 2);
	VECTOR3 startPos = transform.position - halfLen;
	VECTOR3 endPos = transform.position + halfLen;

	//DrawCube3D(startPos, endPos, 0xff00ff, 0xffffff, false);
#endif
}
