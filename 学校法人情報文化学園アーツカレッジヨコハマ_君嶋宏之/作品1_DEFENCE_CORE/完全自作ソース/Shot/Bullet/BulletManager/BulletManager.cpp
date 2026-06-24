#include "BulletManager.h"
#include "../Bullet/Bullet.h"
#include "../../../Stage/StageManager/StageManager.h"
#include "../../../Common/Sound/SoundManager.h"
#include "../../../Common/Sound/SoundID.h"
#include "../../../../ImGui/imgui.h"
#include "../../../../Library/DebugNew.h"

bool BulletManager::isActive = false;

BulletManager::BulletManager()
{
	bullets.clear();
	hitBulletDatas.clear();

	BulletManager::isActive = true;
}

BulletManager::~BulletManager()
{
	BulletManager::isActive = false;

	// コンテナの削除
	DeleteAllBullet();
}

void BulletManager::Init(StageManager* _stageManager)
{
	assert(_stageManager != nullptr);
	stageManager = _stageManager;
}

void BulletManager::Update()
{
	// 当たった弾のデータをリセット
	hitBulletDatas.clear();

	/*for (auto itr = bullets.begin(); itr != bullets.end();)
	{
		BulletBase* b = *itr;

		if (b != nullptr)
		{
			b->DestroyMe();
			itr = bullets.erase(itr);
		}
		else
		{
			itr++;
		}
	}*/

	for (auto itr = bullets.begin(); itr != bullets.end();)
	{
		Bullet& bullet = **itr;

		if (*itr != nullptr)
		{
			// 当たっていたら
			if (bullet.IsHit())
			{
				// データを保存
				hitBulletDatas.emplace_back(bullet.GetBulletData());
			}

			if (!bullet.IsActive())
			{
				bullet.DestroyMe();
				itr = bullets.erase(itr);
				continue;
			}
		}
		itr++;
	}

#ifdef _DEBUG

	//ImGui::Begin("Bullet");
	//ImGui::Text("num %d", (int)bullets.size());
	//ImGui::End();
#endif // _DEBUG

}

void BulletManager::Draw()
{
}


//bool BulletManager::CreateBullet(const Transform& _trans, const VECTOR3& _vel, const BULLET_KIND& _kind, float _lifeTime, const COLLISION_OBJECT_KIND& _hitObj)
//{
//	BulletBase* b = nullptr;
//
//	switch (_kind)
//	{
//	case BULLET_KIND::NORMAL:
//
//		soundManager->PlaySE(Sound_ID::NORMAL_BULLET_SHOT_SE);
//		b = new NormalBullet(BulletData(_trans, _vel, _kind, _lifeTime, _hitObj));
//		break;
//	case BULLET_KIND::SLIME:
//
//		soundManager->PlaySE(Sound_ID::SLIME_BULLET_SHOT_SE);
//		b = new SlimeBullet(BulletData(_trans, _vel, _kind, _lifeTime, _hitObj));
//		break;
//	default:
//
//		assert(false);
//		break;
//	}
//
//	bullets.emplace_back(b);
//
//	return true;
//}

//void BulletManager::DeleteBulletOwner(const void* _pointer)
//{
//	for (auto itr = hitBulletDatas.begin(); itr != hitBulletDatas.end();)
//	{
//		// 発射親がnullptrだったら
//		if (itr->ownerPointer == nullptr)
//			assert(false);
//
//		// 発射親が一致したら
//		if (itr->ownerPointer == _pointer)
//			itr = hitBulletDatas.erase(itr);
//		else
//			itr++;
//	}
//
//	for (auto bulletItr = bullets.begin();bulletItr != bullets.end();)
//	{
//		BulletBase* b = *bulletItr;
//
//		// 発射親がnullptrだったら
//		if (b->GetBulletData().ownerPointer == nullptr)
//			assert(false);
//
//		// 発射親が一致したら
//		if (b->GetBulletData().ownerPointer == _pointer)
//			bulletItr = bullets.erase(bulletItr);
//		else
//			bulletItr++;
//	}
//}

//bool BulletManager::CreateBullet(const Transform& _trans, VECTOR3& _velocity, const VECTOR3& _friction, bool _isGravity, const float& _range, const float& _activeTime, const std::unordered_map<COLLISION_OBJECT_KIND, float>& _targetDamageList, const BULLET_KIND& _bulletKind)
//{
//	BulletBase* b = nullptr;
//	
//	switch (_bulletKind)
//	{
//	case BULLET_KIND::NORMAL:
//
//		soundManager->PlaySE(Sound_ID::NORMAL_BULLET_SHOT_SE);
//		b = new NormalBullet(BulletData(_trans, _velocity,_friction,_isGravity,_range, _activeTime, _targetDamageList,_bulletKind));
//		break;
//	case BULLET_KIND::SLIME:
//
//		soundManager->PlaySE(Sound_ID::SLIME_BULLET_SHOT_SE);
//		b = new SlimeBullet(BulletData(_trans, _velocity, _friction, _isGravity, _range, _activeTime, _targetDamageList, _bulletKind));
//		break;
//	default:
//
//		assert(false);
//		break;
//	}
//
//	bullets.emplace_back(b);
//
//	return true;
//}

bool BulletManager::CreateBullet(const BulletInfo::BulletData& _bulletData, const VECTOR3& _dir)
{
	Bullet* bulletInstance = new Bullet(BulletInfo::BulletData(_bulletData), _dir, stageManager);	// 弾の生成
	
	bullets.emplace_back(bulletInstance);
	
	return true;
}

void BulletManager::DeleteAllBullet()
{
	for (auto itr = bullets.begin(); itr != bullets.end();)
	{
		Bullet* bullet = *itr;

		if (bullet != nullptr)
		{
			bullet->DestroyMe();
			itr = bullets.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	hitBulletDatas.clear();
}

//std::list<BulletData> BulletManager::GetHitBulletDataContainer(const void* _owner)
//{
//	std::list<BulletData> re;
//
//	for (auto itr = hitBulletDatas.begin(); itr != hitBulletDatas.end();itr++)
//	{
//		// 発射親がnullptrだったら
//		if (itr->ownerPointer == nullptr)
//			assert(false);
//
//		// 発射親が一致したら
//		if (itr->ownerPointer == _owner)
//			re.emplace_back(*itr);
//	}
//	return re;
//}