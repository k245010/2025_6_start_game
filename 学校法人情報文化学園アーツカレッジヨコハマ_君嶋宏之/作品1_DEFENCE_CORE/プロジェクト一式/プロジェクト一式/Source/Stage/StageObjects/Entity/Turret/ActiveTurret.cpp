#include "ActiveTurret.h"
#include "../../../../Gun/Gun/Gun.h"
#include "../../../../Gun/GunDataSerializer/GunDataSerializer.h"

namespace
{
    constexpr float MAX_ACTIVE_TIME = 5.0f; // 最大動作時間
}

ActiveTurret::ActiveTurret(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : TurretBase(_trans, _modelData,StageObjectData::STAGE_OBJECT_KIND::ACTIVE_TURRET, _hp, _putPlaceKind)
{
    canShot = false;

	std::string filePath = GunDataSerializer::GetGunDataDirectoryPath() + "turret/gun_1.json";
	/*
	float bulletSpeed = 1500.0f;							// 弾の速度
	VECTOR3 bulletVelocityFriction = VZero;							// 弾のヴェロシティの抵抗値
	bool isBulletGravity = false;							// 弾が重力の影響を受けるかどうか
	float range = 1000.0f;							// 射程距離
	float bulletSize = 10.0f;							// 弾のサイズ 直径
	float bulletLifeTime = 8.0f;								// 弾の生存時間
	std::unordered_map<COLLISION_OBJECT_KIND, float> bulletTargetDamageList =	// 弾を当て相手と与えるダメージ量を設定
	{
		{ COLLISION_OBJECT_KIND::GROUND_BLOCK,	0.0f },
		{ COLLISION_OBJECT_KIND::CORE_BLOCK,	0.0f },
		{ COLLISION_OBJECT_KIND::WALL_BLOCK,	0.0f },
		{ COLLISION_OBJECT_KIND::ENEMY,			10.0f},
	};
	std::set<StageObjectData::STAGE_OBJECT_KIND> bulletHitPosSummonStageObjectKindList = 	// 着弾点に召喚するステージオブジェクトの種類コンテナ
	{
		{StageObjectData::STAGE_OBJECT_KIND::NONE},
	};
	std::set<Sound_ID::SOUND_ID> bulletHitPosSoundIDList =									// 着弾時のサウンド
	{
		std::set<Sound_ID::SOUND_ID>{},
	};
	EffekseerObjectManager::EFFEKSEER_KIND bulletHitPosEffectKind = EffekseerObjectManager::EFFEKSEER_KIND::EF_NONE;	// 着弾のエフェクト
	BulletInfo::BULLET_KIND bulletKind = BulletInfo::BULLET_KIND::NORMAL;					// 発射する弾の種類　
	int ammo = 10;							// 現在のマガジン内の弾数
	int holdBullet = 100;							// 弾の保持数
	int magazineSize = 10;							// １マガジンのサイズ
	float fireRate = 0.5f;							// 連射速度　数値が大きいほど連射できる
	float reloadTime = 3.0f;							// リロード時間
	float spread = 0.0f;							// 拡散量　ショットガンなどの場合、数値が大きいほど拡散する
	int palette = 1;							// パレット数　ショットガンなどの場合複数同時発射なので、この数を増やす
	EffekseerObjectManager::EFFEKSEER_KIND fireEffectKInd = EffekseerObjectManager::EFFEKSEER_KIND::EF_NONE;	// 発射エフェクト種類
	std::set<Sound_ID::SOUND_ID> shotSoundIDList =													// 発射音のサウンドIDコンテナ
	{
		{ Sound_ID::SOUND_ID::NORMAL_BULLET_SHOT_SE }
	};

	BulletInfo::BulletData bulletData =
		BulletInfo::BulletData
		(
			transform,
			bulletSpeed,
			bulletVelocityFriction,
			isBulletGravity,
			bulletSize,
			range,
			bulletLifeTime,
			bulletTargetDamageList,
			bulletHitPosSummonStageObjectKindList,
			bulletHitPosSoundIDList,
			bulletHitPosEffectKind,
			bulletKind
		);



	GunDataSerializer::SaveGunData(filePath, GunInfo::GunData(bulletData, ammo, holdBullet, magazineSize, fireRate, reloadTime, spread, palette, fireEffectKInd, shotSoundIDList));
	*/

	gun = new Gun(transform, 0, filePath, this);


	// GunのInitを呼ぶ
	TurretBase::GunInit();
}

ActiveTurret::~ActiveTurret()
{
}

void ActiveTurret::Update()
{
    // 最大動作時間を超えたら
    if ((activeTime += Time::GameDeltaTime()) >= MAX_ACTIVE_TIME)
    {
        // 発射をしないようにする
        canShot = false;
    }
    TurretBase::Update();
}

void ActiveTurret::Draw()
{
    TurretBase::Draw();

    //DrawRota2Graph3D()
}

float ActiveTurret::Maintaining(const float& _add)
{
    if (TurretBase::Maintaining(_add) >= 1.0f)
    {
        // 時間をリセット
        activeTime = 0.0f;    
    }
    return maintainNum;
}
