#include "ShotManager.h"
#include "../../Collision/CollisionObjectKind.h"
#include "../../Collision/Collisions/CollisionBase.h"
#include "../Bullet/BulletManager/BulletManager.h"
#include "../../../ImGui/imgui.h"

namespace
{
	constexpr float BULLET_LIFE_TIME = 1.5f; // 弾の寿命 秒
	//constexpr float BULLET_SPEED = 300.0f; // 弾のスピード
}

bool ShotManager::isActiveInstance = true;

ShotManager::ShotManager()
{
	bulletManager = nullptr;
	//shotDatas.clear();

	ShotManager::isActiveInstance = true;
}

void ShotManager::Init()
{
	bulletManager = FindGameObject<BulletManager>();
}

ShotManager::~ShotManager()
{
	ShotManager::isActiveInstance = false;

	for (auto itr = shotInterval.begin(); itr != shotInterval.end();)
	{
		if (itr->first != nullptr)
		{
			itr = shotInterval.erase(itr);
		}
		else
		{
			itr++;
		}
	}

	//_ bulletManagerの弾のコンテナも全て削除_ //

	if (bulletManager->IsActive())
		bulletManager->DeleteAllBullet();

	//for (auto itr = shotDatas.begin(); itr != shotDatas.end();)
	//{
	//	if(itr->first != nullptr)
	//	{
	//		/*for (auto dataItr = itr->second.begin(); dataItr != itr->second.end(); dataItr++)
	//		{
	//			dataItr
	//		}*/
	//		itr->second.clear();
	//		itr = shotDatas.erase(itr);
	//	}
	//	else
	//	{
	//		itr++;
	//	}
	//}
}

void ShotManager::Update()
{
	//for(auto itr = shotInterval.begin(); itr != shotInterval.end(); itr++)
	//{
	//	itr->second.interval -= Time::GameDeltaTime();

	//	if (itr->second.interval < 0.0f)
	//	{
	//		auto find = shotDatas.find(itr->first);

	//		// キーをみつけられたら
	//		if(find != shotDatas.end())
	//		{
	//			// 要素の数だけ弾を生成
	//			for (auto dataItr = find->second.begin();dataItr != find->second.end();)
	//			{
	//				bulletManager->CreateBullet(*dataItr);
	//				dataItr = find->second.erase(dataItr);
	//			}
	//			find = shotDatas.erase(find);
	//		}
	//		// インターバルを設定
	//		itr->second.interval = itr->second.intervalSave;
	//	}
	//}
}
//
//bool ShotManager::Shot(const Transform& _trans, VECTOR3& _velocity, const VECTOR3& _friction, bool _isGravity, const float& _range, const float& _activeTime, const std::unordered_map<COLLISION_OBJECT_KIND, float>& _targetDamageList, const BULLET_KIND& _bulletKind)
//{
//	return  bulletManager->CreateBullet(_trans, _velocity, _friction, _isGravity, _range, _activeTime, _targetDamageList, _bulletKind);
//}

bool ShotManager::Shot(const BulletInfo::BulletData& _bulletData, const VECTOR3& _dir)
{
	return  bulletManager->CreateBullet(_bulletData, _dir);	// 弾の生成
}

bool ShotManager::SpreadShot(const BulletInfo::BulletData& _bulletData, const VECTOR3& _dir, const MATRIX _rotMatrix, const float& _spread, const int& _palette)
{
	assert(_palette > 0 && "パレット数が0以下です");

	bool isShot		= false;
	VECTOR3 baseDir = _dir;	// 基本発射方向

	float angle		= (360 * DegToRad) / _palette;	// 拡散を2D円形として、見たときの弾から弾への角度(ラジアン)

	float frontAxisLevel	= cosf(_spread);		// 前方向の進むZ軸のレベル　0 ~ 1
	float sideAxisLevel		= sinf(_spread);		// 横方向の進むXY軸のレベル 0 ~ 1

	// 全方向の進むレベルに合わせる
	baseDir.z = (VECTOR3(0.0f, 0.0f, frontAxisLevel) * _rotMatrix).z;

	for (int i = 0;i < _palette;i++)
	{
		float x = cosf(angle * i);
		float y = sinf(angle * i);

		// 横方向の進むレベルに合わせる //
		x *= sideAxisLevel;
		y *= sideAxisLevel;

		// 最終発射方向
		VECTOR3 shotDir = baseDir + VECTOR3(x, y, 0.0f) * _rotMatrix;

		// 弾の生成
		isShot = bulletManager->CreateBullet(_bulletData, shotDir.Normalize());
	}
	return isShot;
}

//void ShotManager::SetShotInterval(const void* _pointer, const float _interval)
//{
//	std::unordered_map<const void*, IntervalData>::iterator itr = shotInterval.find(_pointer);
//
//	if (itr == shotInterval.end())
//	{
//		shotInterval.insert(std::make_pair(_pointer, IntervalData{ _interval ,0.0f }));
//	}
//	else
//	{
//		shotInterval[_pointer].intervalSave = _interval;
//	}
//}

//void ShotManager::DeleteShotOwner(const void* _pointer)
//{
//	if (!ShotManager::isActiveInstance)
//		return;
//
//	auto intervalItr = shotInterval.find(_pointer);
//
//	// キーが見つかったら
//	if (intervalItr != shotInterval.end())
//		intervalItr = shotInterval.erase(intervalItr);
//
//	bulletManager->DeleteBulletOwner(_pointer);
//
//	//auto dataItr = shotDatas.find(_pointer);
//
//	//// キーが見つかったら
//	//if (dataItr != shotDatas.end())
//	//{
//	//	dataItr->second.clear();
//	//	dataItr = shotDatas.erase(dataItr);
//	//}
//}
//
//bool ShotManager::Shot(const VECTOR3& _pos, const VECTOR3& _vel, const BULLET_KIND& _bulletKind, const int& _target/*const void* _pointer*/)
//{
//	//if (IsShotIntervalSetting(_pointer))
//	//{
//	//	Transform tr = Transform();
//	//	tr.position = _pos;
//
//	//	// ToDo ライフタイム設定は今後別で設定
//	//	float bulletLifeTime = BULLET_LIFE_TIME;
//	//	if (_bulletKind == BULLET_KIND::SLIME)
//	//		bulletLifeTime = 5.0f;
//
//	//	bulletManager->CreateBullet(_pointer, tr, _vel, _bulletKind, bulletLifeTime, (COLLISION_OBJECT_KIND)_target);
//
//	//	//std::unordered_map<const void*, std::list<BulletData>>::iterator itr = shotDatas.find(_pointer);
//
//	//	// キーが見つからなかったら
//	//	//if (itr == shotDatas.end())
//	//	//	shotDatas.emplace(_pointer, std::list<BulletData>{BulletData(_pointer,tr, _vel, _bulletKind, BULLET_LIFE_TIME, (COLLISION_OBJECT_KIND)_target)});
//	//	// データ更新
//	//	//else
//	//		//shotDatas[_pointer].emplace_back(BulletData(_pointer,tr, _vel, _bulletKind, BULLET_LIFE_TIME, (COLLISION_OBJECT_KIND)_target));
//	//}
//	//else
//	//{
//	//	assert(false); // ShotIntervalが設定されていない
//	//	return false;
//	//}
//	//return true;
//
//	Transform tr = Transform();
//	tr.position = _pos;
//
//	// ToDo ライフタイム設定は今後別で設定
//	float bulletLifeTime = BULLET_LIFE_TIME;
//	if (_bulletKind == BULLET_KIND::SLIME)
//		bulletLifeTime = 5.0f;
//
//	return bulletManager->CreateBullet(tr, _vel, _bulletKind, bulletLifeTime, (COLLISION_OBJECT_KIND)_target);
//}

//bool ShotManager::BurstShot(const VECTOR3& _pos, const VECTOR3& _vel, const BULLET_KIND& _kind, const int _shotNum, const int _interval)
//{
//	return false;
//}
//
