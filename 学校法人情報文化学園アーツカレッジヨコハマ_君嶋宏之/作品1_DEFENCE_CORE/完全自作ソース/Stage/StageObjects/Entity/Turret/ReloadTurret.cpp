#include "ReloadTurret.h"
#include "../../../../Gun/Gun/Gun.h"
#include "../../../../Gun/GunDataSerializer/GunDataSerializer.h"
#include "../../../EntityController/EntityController.h"

/*
namespace
{
	constexpr int MAX_SHOT_COUNT = 10;	// 最大発射回数
}
*/

ReloadTurret::ReloadTurret(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : TurretBase(_trans, _modelData, _kind, _hp, _putPlaceKind)
{
	std::string filePath;
	std::string recordName;
	switch (_kind)
	{
	case StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SINGLE:

		filePath	= GunDataSerializer::GetGunDataDirectoryPath() + "turret/gun_2.json";
		recordName	= "ReloadTurretSingle";
		break;
	case StageObjectData::STAGE_OBJECT_KIND::RELOAD_TURRET_SPREAD:

		filePath	= GunDataSerializer::GetGunDataDirectoryPath() + "turret/gun_3.json";
		recordName	= "ReloadTurretSpread";
		break;
	default:
		assert(false);
		break;
	}

	/*
	float bulletSpeed = 11000.0f;							// 弾の速度
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
	int ammo = MAX_SHOT_COUNT;							// 現在のマガジン内の弾数
	int holdBullet = 1000000;							// 弾の保持数
	int magazineSize = MAX_SHOT_COUNT;							// １マガジンのサイズ
	float fireRate = 0.5f;							// 連射速度　数値が大きいほど連射できる
	float reloadTime = 0.0f;							// リロード時間
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


	//GunDataSerializer::SaveGunData(filePath, GunInfo::GunData(bulletData, ammo, holdBullet, magazineSize, fireRate, reloadTime, spread, palette, fireEffectKInd, shotSoundIDList));
	*/

	gun = new Gun(transform, 0, filePath, recordName);

	// GunのInitを呼ぶ
	TurretBase::GunInit();
}

ReloadTurret::~ReloadTurret()
{
}

void ReloadTurret::Update()
{
	TurretBase::Update();
}

void ReloadTurret::Draw()
{
	TurretBase::Draw();
}

float ReloadTurret::Maintaining(const float& _add)
{
	if (TurretBase::Maintaining(_add) >= 1.0f)
	{
		// 弾をリロード
		gun->Reload();
	}
    return maintainNum;
}
