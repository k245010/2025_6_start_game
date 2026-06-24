#include "GroundBlock.h"
#include "../../../Collision/Collisions/SphereCollision.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../../Collision/Collisions/ModelCollision.h"
#include "../../../../ImGui/imgui.h"

GroundBlock::GroundBlock(const Transform& _trans, const int& _hModel, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BlockBase(_trans, _hModel, StageObjectData::STAGE_OBJECT_KIND::GROUND_BLOCK,_hp, _putPlaceKind)
{
	obbColl->SetTagMe(COLLISION_OBJECT_KIND::GROUND_BLOCK);
	//obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ERASER);

	// トンネルモデル(ステージ5)に合わせたサイズ
	transform.size = VECTOR3(400, 114, 300);

	modelCollision = new ModelCollision(&transform, COLLISION_OBJECT_KIND::GROUND_BLOCK, hModel, "GROUND_BLOCK_OBB", [this](const CollisionHitInfoData& tr) {return HitBlock(tr);});
	modelCollision->SetTargetTag(COLLISION_OBJECT_KIND::BULLET);
	modelCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);

	// シャドウマップを使用する
	SetUseShadowMapFlag(true);

	materialEmissiveColor = GetColorF(0.3f, 0.3f, 0.3f, 0.5f);
	
	// 3Dモデルに含まれる0番目のマテリアルの発光カラーを設定する
	MV1SetMaterialEmiColor(hModel, 0, materialEmissiveColor);
}

GroundBlock::~GroundBlock()
{
	if (obbColl != nullptr)
	{
		delete obbColl;
		obbColl = nullptr;
	}

	if (modelCollision != nullptr)
	{
		delete modelCollision;
		modelCollision = nullptr;
	}
}

void GroundBlock::Update()
{
	BlockBase::Update();
}

void GroundBlock::Draw()
{
	BlockBase::Draw();
}

bool GroundBlock::HitBlock(const CollisionHitInfoData& _targetData)
{
	return false;
}
