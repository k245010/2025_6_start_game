#include "BombBase.h"
#include "../../../../Collision/Collisions/SphereCollision.h"
#include "../../../../Common/Effekseer/EffekseerObjectManager.h"
#include "../../../../Collision/Collisions/OBBCollision.h"
		  
BombBase::BombBase(const Transform& _trans, const int& _hModel, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : EntityBase(_trans, _hModel, _kind, _hp, _putPlaceKind)
{
	// 自身の当たり判定のTagを設定
	obbColl->SetTagMe(COLLISION_OBJECT_KIND::BOMB);
	obbColl->SetTargetTag(COLLISION_OBJECT_KIND::ERASER);
}

BombBase::~BombBase()
{
}

void BombBase::Update()
{
	EntityBase::Update();

	// 爆発フラグがオンだったら
	if (isExplosion)
	{
		// 爆発処理
		Explosion();
		active = false;
	}
}

void BombBase::Draw()
{
	EntityBase::Draw();

	// 爆発フラグがオンだったら
	if (isExplosion)
	{
		// 爆発範囲の描画
		DrawSphere3D(explosionCollisionTransform.position, explosionCollisionTransform.GetLenX() / 2, 8, 0xff0000, 0xffffff,false);
	}
}

bool BombBase::HitEntity(const CollisionHitInfoData& _targetData)
{
	active = false;
	return true;
}

void BombBase::Explosion()
{
	explosionCollisionTransform.position	= transform.position;
	explosionCollisionTransform.scale		= VOne * 1000.0f;

	sphereColl = new SphereCollision(&explosionCollisionTransform, COLLISION_OBJECT_KIND::BOMB_EXPLOSION, "", nullptr, true);
	sphereColl->SetTargetTag(COLLISION_OBJECT_KIND::ENEMY, 100);

	// 爆発エフェクトの再生
	EffekseerObjectManager::SetEffect(transform.position, EffekseerObjectManager::EFFEKSEER_KIND::EF_EXPLOSION);
}