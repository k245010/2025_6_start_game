#pragma once
#include "../../Object3D/Object3D.h"
//#include "../CollisionObjectKind.h"
//#include "../StageObjectData.h"
//#include "../Effekseer/EffekseerObjectManager.h"
//#include "../Sound/SoundID.h"
#include "../GunInfo.h"
#include "../GunDataSerializer/GunDataSerializer.h"
#include "../GunManager/GunManager.h"
#include "../../../nlohmann/json.hpp"
#include "../../Common/Function/Function.h"
#include <unordered_map>
#include <set>

class Physics;
class ShotManager;
class StageManager;
class GunManager;
class Gun : public Object3D
{
public:

	/// <summary>
	///								コンストラクタ	引数のファイルパスからGunDataを読み込んで、GunDataの内容でGunを初期化するコンストラクタ
	/// </summary>
	/// <param name="_trans">		トランスフォーム参照					</param>
	/// <param name="_hModel">		モデルハンドル							</param>
	/// <param name="_path">		ファイルパス							</param>
	/// <param name="_parent">		親クラスのポインタ						</param>
	template<class ParentClassPointer>
	inline Gun(Transform& _trans, const int& _hModel, std::string_view _path, const ParentClassPointer* _parentClassPointer) : Object3D(_trans, _hModel)
	{
		transformPtr		= &_trans;
		gunData				= GunDataSerializer::LoadGunData(_path);
		loadGunDataFilePath = _path;
		gunIDName			= GetClassNameString(_parentClassPointer);

		// 自身をマネージャーに登録
		GunManager::GetGunManagerInstance()->PushGunInstance(gunIDName, this);
	}

	/// <summary>
	///								コンストラクタ	引数のファイルパスからGunDataを読み込んで、GunDataの内容でGunを初期化するコンストラクタ
	/// </summary>
	/// <param name="_trans">		トランスフォーム参照					</param>
	/// <param name="_hModel">		モデルハンドル							</param>
	/// <param name="_path">		ファイルパス							</param>
	/// <param name="_recordName">	登録名									</param>
	Gun(Transform& _trans, const int& _hModel, std::string_view _path, std::string_view _recordName);

	~Gun();

	/// <summary>
	///								GunDataからメンバ変数を初期化
	/// </summary>
	/// <param name="_shotManager">	ShotManagerクラスポインタ					</param>
	/// <param name="_stageManager">StageManagerクラスポインタ					</param>
	void Init(ShotManager* _shotManager,StageManager* _stageManager);

	void Update()	override;

	/// <summary>
	///							銃の描画を行う
	/// </summary>
	/// <param name="_trans">	描画するためのトランスフォーム		</param>
	void GunDraw(const Transform& _trans);

	/// <summary>
	///								発射処理を行う
	/// </summary>
	/// <param name="_offset">		発射開始座標のオフセット座標 0,0,0の場合Gunクラスのトランスフォームに従う	</param>
	/// <param name="_velocity">	発射方向					</param>
	/// <returns>					true:成功 / false:失敗　	</returns>
	bool Shot(const VECTOR3& _offset, const VECTOR3& _dir);

	/// <summary>
	///				マガジンに弾をこめる
	/// </summary>
	/// <returns>	true:弾をこめられた / false:弾をこめられなかった	</returns>
	bool Reload();

	/// <summary>
	///				リロード中か返す
	/// </summary>
	/// <returns>	true:リロード中 / false:リロードしていない		</returns>
	bool IsReloading() const { return isReloading; }

	/// <summary>
	///				現在のリロードを初めてからの経過時間を返す
	/// </summary>
	/// <returns>	リロード経過時間	</returns>
	float GetReloadTime() const { return reloadTime; }

	/// <summary>
	///							GunDataを設定する
	/// </summary>
	/// <param name="_gunData">	GunData			</param>
	void SetGunData(const GunInfo::GunData& _gunData) { gunData = _gunData; }

	/// <summary>
	///				GunDataを返す
	/// </summary>
	/// <returns>	GunData			</returns>
	const GunInfo::GunData& GetGunData() const { return gunData; }

	/// <summary>
	///				マガジン内の弾数を返す
	/// </summary>
	/// <returns>	マガジン内の弾数			</returns>
	int GetAmmo() const { return ammo; }

	/// <summary>
	/// マガジン内の弾数を設定
	/// </summary>
	void SetAmmo(const int& _ammo) 
	{
		assert(_ammo <= gunData.ammo && "設定するマガジン内の弾数が、GunDataのマガジン内弾数の最大値を超えています。");
		assert(_ammo >= 0 && "設定するマガジン内の弾数がマイナスです。");

		ammo = _ammo; 
	}

	/// <summary>
	///								保持する弾数を設定	
	/// </summary>
	/// <param name="_holdBullet">	保持する弾数	</param>
	void SetHoldBullet(const int& _holdBullet) 
	{
		assert(_holdBullet <= gunData.holdBullet && "設定する保持する弾数が、GunDataの保持弾数の最大値を超えています。");
		assert(_holdBullet >= 0 && "設定する保持する弾数がマイナスです。");

		holdBullet = _holdBullet;
	}

	/// <summary>
	///				保持している弾数を返す
	/// </summary>
	/// <returns>	保持している弾数	</returns>
	int GetHoldBullet() const { return holdBullet; }

	/// <summary>
	/// 自身のGunDataが読み込まれたれたファイルパスを返す
	/// </summary>
	const std::string& GetLoadGunDataFilePath() const { return loadGunDataFilePath; }

	/// <summary>
	///								GunクラスのID名を設定する
	/// </summary>
	/// <param name="_gunIDName">	GunクラスのID名	</param>
	void SetGunIDName(std::string _gunIDName) { gunIDName = _gunIDName; }

	/// <summary>
	///				GunクラスのID名を返す
	/// </summary>
	/// <returns>	GunクラスのID名		</returns>
	const std::string& GetGunIDName() const { return gunIDName; }

	/// <summary>
	///				親クラスのステージオブジェクトの種類を返す
	/// </summary>
	/// <returns>	親クラスのステージオブジェクトの種類	</returns>
	///const StageObjectData::STAGE_OBJECT_KIND& GetParentStageObjectKind() const { return parentStageObjectKind; }

private:

	/// <summary>
	/// リロードUpdate
	/// </summary>
	void ReloadUpdate();

	/// <summary>
	///							弾を発射した時の処理
	/// </summary>
	/// <param name="_offset">	発射開始座標のオフセット座標 0,0,0の場合Gunクラスのトランスフォームに従う		</param>
	void OnShot(const VECTOR3& _offset);

	std::string gunIDName;	// GunのIDの名前

	Physics* physics			= nullptr;
	ShotManager* shotManager	= nullptr;
	StageManager* stageManager	= nullptr;

	Transform* transformPtr;			// トランスフォームポインタ
	//StageObjectData::STAGE_OBJECT_KIND parentStageObjectKind;	// 親クラスのステージオブジェクトの種類

	GunInfo::GunData gunData;			// 銃のデータ
	std::string loadGunDataFilePath;	// 銃のデータが読み込まれたファイルパスの文字列
	VECTOR3 shotPosition		= VZero;// 弾の発射座標

	int ammo					= 0;	// 現在マガジンに入っている弾数
	int holdBullet				= 0;	// 保持している弾数
	float reloadTime			= 0.0f;	// 現在のリロード時間
	float shotAfterTime			= 0.0f;	// 弾を発射してからの経過時間
	bool canShot				= true;	// 現在弾を発射できるかどうか
		
	bool isReloading			= false;// リロード中かどうか

	/*VECTOR3 velocity = VZero;
	VECTOR3 friction = VZero;*/
};
