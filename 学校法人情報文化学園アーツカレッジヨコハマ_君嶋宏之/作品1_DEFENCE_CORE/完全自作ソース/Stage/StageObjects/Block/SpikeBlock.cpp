#include "SpikeBlock.h"
#include "../../../Collision/Collisions/OBBCollision.h"
#include "../../../Common/Effekseer/EffekseerObjectManager.h"

SpikeBlock::SpikeBlock(const Transform& _trans, const int& _hModel, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BlockBase(_trans, _hModel, StageObjectData::STAGE_OBJECT_KIND::SPIKE_BLOCK,_hp, _putPlaceKind)
{
	obbColl->SetTagMe(COLLISION_OBJECT_KIND::SPIKE_BLOCK);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY, 5.0f);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER, 5.0f);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ERASER);

	// ToDo:モデルの修正をして、ここでスケール変換をやめる
	// モデルのスケールがY軸に高いので、半分にする
	transform.scale.y /= 2.0f;
	transform.position.y -= (transform.size.y * transform.scale.y) / 2;

	// 底面座標
	VECTOR3 bottomPos = transform.position - VNormY * (transform.GetLenY() / 2);

	// 罠の設置のエフェクト再生
	EffekseerObjectManager::SetEffect(bottomPos, EffekseerObjectManager::EF_TRAP_PUT, 1.5f);
}

SpikeBlock::~SpikeBlock()
{

}

void SpikeBlock::Update()
{
	BlockBase::Update();
}

void SpikeBlock::Draw()
{
	BlockBase::Draw();
}

bool SpikeBlock::HitBlock(const CollisionHitInfoData& _targetData)
{
	if (_targetData.tag == COLLISION_OBJECT_KIND::ERASER)
	{
		active = false;
		return !obbColl->GetCollisionHitInfoData().onDeleteMe;
	}

	return obbColl->GetCollisionHitInfoData().onDeleteMe;
}
