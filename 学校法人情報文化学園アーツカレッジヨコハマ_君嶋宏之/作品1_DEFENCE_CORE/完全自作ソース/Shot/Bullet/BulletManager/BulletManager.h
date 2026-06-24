#pragma once
#include "../../../../Library/GameObject.h"
#include "../BulletInfo.h"
#include <list>

class Bullet;
struct CollisionHitDamageData;
class StageManager;
/// <summary>
/// 弾を総括するクラス
/// </summary>
class BulletManager : public GameObject
{
public:

	BulletManager();
	~BulletManager();

	void Init(StageManager* _stageManager);

	void Update() override;
	void Draw() override;

	/// <summary>
	/// 弾の生成
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_vel"> ヴェロシティ </param>
	/// <param name="_kind"> 弾の種類 </param>
	/// <returns> 成功：失敗 </returns>
	//bool CreateBullet(const VECTOR3& _pos, const VECTOR3& _vel, const BULLET_KIND& _kind);

	/// <summary>
	/// 弾の生成
	/// </summary>
	/// <param name="_tra">			トランスフォーム		</param>
	/// <param name="_vel">			速度					</param>
	/// <param name="_kind">		弾の種類				</param>
	/// <param name="_lifeTime">	生存時間				</param>
	/// <param name="_hitObj">		ダメージを与える相手	</param>
	/// <returns>					true:成功 / false:失敗 </returns>
	//bool CreateBullet(const Transform& _tra, const VECTOR3& _vel, const BULLET_KIND& _kind, float _lifeTime, const COLLISION_OBJECT_KIND& _hitObj);

	/// <summary>
	/// 弾の生成
	/// </summary>
	/// <param name="_tra">			トランスフォーム		</param>
	/// <param name="_vel">			速度					</param>
	/// <param name="_kind">		弾の種類				</param>
	/// <param name="_lifeTime">	生存時間				</param>
	/// <param name="_hitObj">		ダメージを与える相手	</param>
	/// <returns>					true:成功 / false:失敗 </returns>
	//bool CreateBullet(const Transform& _tra, const VECTOR3& _vel, const BULLET_KIND& _kind, float _lifeTime, const COLLISION_OBJECT_KIND& _hitObj);

	/// <summary>
	/// 通常単発射撃
	/// </summary>
	/// <param name="_trans">				トランスフォーム														</param>
	/// <param name="_vel">					ヴェロシティ															</param>
	/// <param name="_friction">			抵抗値																	</param>
	/// <param name="_isGravity">			弾が重力の影響を受けるかどうか											</param>
	/// <param name="_range">				射程距離																</param>
	/// <param name="_activeTime">			弾の生存時間															</param>
	/// <param name="_targetDamageList">	ダメージを与える相手の当たり判定オブジェクトの種類とダメージ量のコンテナ</param>
	/// <param name="_bulletKInd">			弾の種類																</param>
	/// <returns>							成功：失敗																</returns>
	//bool CreateBullet(const Transform& _trans, VECTOR3& _velocity, const VECTOR3& _friction, bool _isGravity, const float& _range, const float& _activeTime, const std::unordered_map<COLLISION_OBJECT_KIND, float>& _targetDamageList, const BULLET_KIND& _bulletKind);

	/// <summary>
	///								弾の発射
	/// </summary>
	/// <param name="_bulletData">	弾データ		</param>
	/// <param name="_dir">			発射方向		</param>
	/// <returns>					成功：失敗		</returns>
	bool CreateBullet(const BulletInfo::BulletData& _bulletData, const VECTOR3& _dir);

	/// <summary>
	///							一致するBullteDataのオーナークラスポインタをコンテナから削除
	/// </summary>
	/// <param name="_pointer">	オーナーポインタ						</param>
	//void DeleteBulletOwner(const void* _pointer);

	/// <summary>
	/// 全てのbulletDataを管理するコンテナを削除
	/// </summary>
	void DeleteAllBullet();

	/// <summary>
	/// 当たった弾のデータコンテナを返す　毎フレーム確認する必要がある	ToDo:コンテナを返すのはよくないので、改修予定
	/// </summary>
	/// <param name="_owner"> 発射した親ポインタ </param>
	/// <returns> 当たった弾のデータ </returns>
	//std::list<BulletData> GetHitBulletDataContainer(const void* _owner);

	/// <summary>
	///				BulletManagerがアクティブかどうか
	/// </summary>
	/// <returns>	true:アクティブ / false:非アクティブ		</returns>
	const static bool IsActive() { return isActive; }
	
private:

	StageManager* stageManager = nullptr;

	static bool isActive;					// 自身のアクティブフラグ

	std::list<Bullet*> bullets;				// 弾のデータリスト
	std::list<BulletInfo::BulletData> hitBulletDatas;	// 当たった弾リスト
};