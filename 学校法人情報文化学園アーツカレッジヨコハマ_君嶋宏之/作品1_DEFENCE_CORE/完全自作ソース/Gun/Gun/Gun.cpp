#include "Gun.h"
#include "../../Physics/Physics.h"
#include "../../Shot/ShotManager/ShotManager.h"
#include "../../Stage/StageManager/StageManager.h"
#include "../../Common/Sound/SoundManager.h"

//Gun::Gun(Transform& _trans, const int& _hModel, std::string_view _path, const void* _parentPointer) : Object3D(_trans, _hModel)
//{
//	
//}

//Gun::Gun(Transform& _trans, const int& _hModel, GunInfo::GunData&& _gunData) : Object3D(_trans,_hModel), gunData(std::move(_gunData))
//{
//	transformPtr = &_trans;
//}

Gun::Gun(Transform& _trans, const int& _hModel, std::string_view _path, std::string_view _recordName)
{
	transformPtr		= &_trans;
	gunData				= GunDataSerializer::LoadGunData(_path);
	loadGunDataFilePath = _path;
	gunIDName			= _recordName;

	// 自身をマネージャーに登録
	GunManager::GetGunManagerInstance()->PushGunInstance(gunIDName, this);
}

void Gun::Init(ShotManager* _shotManager, StageManager* _stageManager)
{
	shotManager = _shotManager;
	assert(shotManager != nullptr);
	stageManager = _stageManager;
	assert(stageManager != nullptr);

	// マガジンに入っている弾数の初期化
	ammo		= gunData.ammo;
	// 保持する弾数の初期化
	holdBullet	= gunData.holdBullet;

	// 初期の弾が0以下の時
	if (ammo <= 0)
		canShot = false;
}

Gun::~Gun()
{
	if (physics != nullptr)
	{
		physics->DestroyMe();
		physics = nullptr;
	}

	// GunDataのコンテナ解放
	gunData.ReleaseContainer();

	// 自身をマネージャーにから解放
	GunManager::GetGunManagerInstance()->PopGunInstance(this);
}

void Gun::Update()
{
	Object3D::Update();

	// 弾の発射ができないとき
	if (!canShot)
	{
		// 弾を発射してから経過した時間が連射率を超えたら
		if ((shotAfterTime += Time::GameDeltaTime()) >= gunData.fireRate)
		{
			// マガジン内に弾が存在したら
			if (ammo > 0)
				canShot = true;	// 弾を発射できる
		}
	}
	
	// リロード中の時
	if (isReloading)
	{
		// リロードUpdateを実行
		ReloadUpdate();
	}
	else
	{
		// リロード経過時間をリセット
		reloadTime = 0.0f;
	}
}

void Gun::GunDraw(const Transform& _trans)
{
	// 描画用にトランスフォームを設定
	transform = _trans;

	Object3D::Draw();
}

bool Gun::Shot(const VECTOR3& _offset, const VECTOR3& _dir)
{
	assert(shotManager != nullptr && "ShotManagerクラスポインタがnullです　Init関数を呼んで下さい");

	// ShotManagerがアクティブでなかったら
	if (!shotManager->IsActiveInstance())
		return false;	// 発射処理を行わない

	// リロードを中断
	isReloading = false;

	if (!canShot)
		return false;	// 発射できなとき、発射処理を行わない

	BulletInfo::BulletData& bulletData = gunData.bulletData;

	Transform trans		= *transformPtr;	// 現在の銃のトランスフォーム
	trans.position		+= _offset;			// オフセットで座標を調整

	bulletData.transform = trans;			// 弾のトランスフォーム初期値から変わってる可能性があるので更新

	//_ 発射する弾が複数の時 (拡散の時) _//
	
	if (gunData.palette > 1)
	{
		// 複数弾の弾の発射に成功したら
		if (shotManager->SpreadShot(bulletData, _dir,trans.GetRotMatrix(), gunData.spread, gunData.palette))
		{
			// 発射時の処理
			OnShot(_offset);
			return true;
		}
		return false;
	}
	
	//_ 発射する弾が単発の時 _//

	// 単発の弾の発射に成功したら
	if (shotManager->Shot(bulletData,_dir))
	{
		// 発射時の処理
		OnShot(_offset);
		return true;
	}
	return false;
}

void Gun::OnShot(const VECTOR3& _offset)
{
	// 発射してからの経過時間をリセット
	shotAfterTime = 0.0f;

	// 現在の段数を減らす
	ammo--;

	// 弾数が0以下の時
	if (ammo <= 0)
		ammo = 0;

	// 連続発射できないようにフラグをfalse
	canShot = false;	

	// 発射音の再生		再生するサウンドが複数あった場合はランダムに再生される
	SoundManager::PlaySERandom(gunData.shotSoundIDList);

	// 発射エフェクトが設定されていたら
	if (gunData.fireEffectKind != EffekseerObjectManager::EF_NONE)
	{
		// 発射座標を更新
		shotPosition = transform.position + _offset;

		// 発射エフェクトの再生
		EffekseerObjectManager::SetEffect(shotPosition, (int)gunData.fireEffectKind, 1.0f, &shotPosition);
	}
}

bool Gun::Reload()
{
	// リロードできる弾数を保持しているかつマガジン数に対して、装填数が減っていたら
	if (holdBullet >= 0 && ammo < gunData.magazineSize)
	{
		// リロードを開始
		isReloading = true;
		return true;
	}
	return false;
}

void Gun::ReloadUpdate()
{
	// リロード時間が、GunDataのリロード時間以上だったら
	if ((reloadTime += Time::GameDeltaTime()) >= gunData.reloadTime)
	{
		int lackAmmo = gunData.magazineSize - ammo;	// マガジンサイズに対して不足している弾数を算出

		// 不足弾数に対して保持している弾数のほうが多かったら
		if (holdBullet >= lackAmmo)
		{
			// 保持している弾数を減らす
			holdBullet -= lackAmmo;
		}
		// 不足弾数に対して保持している弾数のほうが少なかったら
		else
		{
			// 不足弾数を現在の保持している弾数にする
			lackAmmo	= holdBullet;
			// 保持している弾数を空にする
			holdBullet	= 0;
		}

		// 現在の弾を増やす
		ammo		+= lackAmmo;
		
		// リロード時間をリセット
		reloadTime	= 0.0f;
		// リロードを中断
		isReloading = false;

		assert(lackAmmo >= 0);
	}
}



