#pragma once
#include "../../../Object3D/Object3D.h"
#include "../BulletInfo.h"

struct CollisionHitInfoData;
class SphereCollision;
class RayCollision;
class Physics;
class StageManager;
/// <summary>
/// 弾の基底クラス
/// </summary>
class Bullet : public Object3D
{
public:

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="_data">			弾のデータ						</param>
	/// <param name="_dir">				弾の方向						</param>
	/// <param name="_stageManager">	StageManagerクラスポインタ		</param>
	/// <param name="_hModel">			モデルハンドル					</param>
	Bullet(BulletInfo::BulletData&& _data, const VECTOR3& _dir, StageManager* _stageManager,const int& _hModel);

	/// <summary>
	///									コンストラクタ
	/// </summary>
	/// <param name="_data">			弾のデータ					</param>
	/// <param name="_dir">				弾の方向					</param>
	/// <param name="_stageManager">	StageManagerクラスポインタ	</param>
	Bullet(BulletInfo::BulletData&& _data,const VECTOR3& _dir, StageManager* _stageManager);
	~Bullet();

	void Update() override;
	void Draw() override;

	/// <summary>
	/// アクティブかどうかを返す
	/// </summary>
	/// <returns> アクティブ：アクティブでない </returns>
	bool IsActive() const { return active; }

	/// <summary>
	/// オブジェクトに当たったかどうか
	/// </summary>
	/// <returns> 当たった：当たってない </returns>
	bool IsHit() const { return isHit; }

	/// <summary>
	/// 弾のデータを返す
	/// </summary>
	/// <returns> 弾のデータ </returns>
	const BulletInfo::BulletData& GetBulletData() { return data; }

private:

	/// <summary>
	/// 当たり判定で当たったときの処理を行う
	/// </summary>
	/// <param name="_num"> ナンバー </param>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitBullet(const CollisionHitInfoData& _targetData);

	SphereCollision* sphereColl			= nullptr;		// 球の当たり判定クラスポインタ　壁衝突後の処理用
	RayCollision* rayColl				= nullptr;		// レイの当たり判定クラスポインタ　現在フレームの弾の座標と次フレームの弾の座標でレイを作って、衝突点を求める
	Physics* physics					= nullptr;		// 弾の移動処理に使用
	StageManager* stageManager			= nullptr;

	BulletInfo::BulletData data;			// 弾のデータ
	bool active;							// アクティブ状態
	bool isHit;								// 弾がオブジェクトに当たったかどうか

	VECTOR3 velocity;				// 弾のヴェロシティ
	VECTOR3 initVelocity;			// 初期の弾のヴェロシティ
	float maxDistance		= 0.0f;	// 弾のヴェロシティの最大速度
	VECTOR3 lastPosition	= VZero;
	bool inLastPosition		= false;
};