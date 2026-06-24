#include "BlockBase.h"
#include "../../BlockController/BlockController.h"
#include "../../StageInfo.h"
#include "../../../Collision/Collisions/SphereCollision.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../../Collision/CollisionFunction.h"
#include "../../../../Library/DebugNew.h"

BlockBase::BlockBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : StageObjectBase(_trans, _modelData, _kind, _hp,_putPlaceKind)
{
	//  ƒnƒ“ƒhƒ‹‚ðdelete‚µ‚È‚¢ BlockController‚ªíœ‚·‚é
	Object3D::SetDontDeleteHmodel();
	transform.size	= VOne * (StageInfo::PUT_GRID_SIZE / transform.scale.x);

	sphereColl		= nullptr;
	obbColl			= new OBBCollision(&transform, COLLISION_OBJECT_KIND::NONE, "BLOCK_OBB", [this](const CollisionHitInfoData& tr) {return HitBlock(tr);});
}

BlockBase::~BlockBase()
{
	if (sphereColl != nullptr)
	{
		delete sphereColl;
		sphereColl = nullptr;
	}
	if (obbColl != nullptr)
	{ 
		delete obbColl;
		obbColl = nullptr;
	}
	
	// Controller‚ªnew‚³‚ê‚Ä‚¢‚½‚ç
	if (BlockController::IsActiveInstance())
		BlockController::GetBlockController()->DeleteNavigationAreaBoxTransformPtr(&transform);
}

void BlockBase::Update()
{
	Object3D::Update();

	if (!active)
	{
		BlockController::GetBlockController()->DeleteBlock(this);
	}
}

void BlockBase::Draw()
{
	Object3D::Draw();

	/*
	VECTOR3 halfLen = VDivI((VMult(transform.scale, transform.size)), 2);
	VECTOR3 startPos = transform.position - halfLen;
	VECTOR3 endPos = transform.position + halfLen;

	DrawCube3D(startPos, endPos, 0xff00ff, 0xffffff, false);
	*/
}

