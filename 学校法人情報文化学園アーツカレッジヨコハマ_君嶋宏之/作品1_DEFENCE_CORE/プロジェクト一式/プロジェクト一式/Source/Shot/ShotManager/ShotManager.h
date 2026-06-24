#pragma once
#include "../../../Library/GameObject.h"
#include "../Bullet/BulletInfo.h"
#include <list>
#include <vector>
#include <unordered_map>

class BulletManager;
struct CollisionHitDamageData;
/// <summary>
/// 弾を撃つクラス　Shot関数を呼ぶ前に、SetShotIntervalを呼び発射間隔を設定すること
/// </summary>
class ShotManager : public GameObject
{
public:
	ShotManager();
	~ShotManager();

	void Init();
	void Update() override;

	/// <summary>
	///				インスタンスがnewされているかどうか
	/// </summary>
	/// <returns>	true:newされている / false:newされていない　 </returns>
	static bool IsActiveInstance() { return isActiveInstance; }

	/// <summary>
	/// 発射間隔データ
	/// </summary>
	struct IntervalData
	{
		float intervalSave;	// 発射までの間隔 秒
		float interval;		// 発射までの間隔 秒 カウント用
	};

	/// <summary>
	/// 発射間隔を設定　Shot関数を呼ぶ前に呼ぶ
	/// </summary>
	/// <param name="_pointer"> 識別用ポインタ(thisを入れる) </param>
	/// <param name="_interval"> 発射までの間隔 秒 </param>
	//void SetShotInterval(const void* _pointer, const float _interval);

	/// <summary>
	/// インターバル設定されているかどうか
	/// </summary>
	/// <param name="_pointer">  識別用ポインタ(thisを入れる) </param>
	/// <returns> されている：されていない </returns>
	/*bool IsShotIntervalSetting(const void* _pointer)
	{
		std::unordered_map<const void*, IntervalData>::iterator itr = shotInterval.find(_pointer);

		if (itr == shotInterval.end())
		{
			return false;
		}
		return true;
	}*/

	/// <summary>
	/// Shotを使うオーナークラスポインタをコンテナから削除
	/// </summary>
	//void DeleteShotOwner(const void* _pointer);

	/// <summary>
	/// 通常単発射撃
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_vel"> ヴェロシティ </param>
	/// <param name="_bulletKind"> 弾の種類 </param>
	/// <param name="_target"> 弾を当てる相手の種類 </param>
	/// <param name="_pointer"> 親識別用ポインタ </param>
	/// <returns> 成功：失敗 </returns>
	//bool Shot(const VECTOR3& _pos,const VECTOR3& _vel, const BULLET_KIND& _bulletKind,const int& _target,const void* _pointer);

	/// <summary>
	///										単発射撃
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
	//bool Shot(const Transform& _trans, VECTOR3& _velocity, const VECTOR3& _friction, bool _isGravity, const float& _range, const float& _activeTime, const std::unordered_map<COLLISION_OBJECT_KIND, float>& _targetDamageList, const BULLET_KIND& _bulletKind);

	/// <summary>
	///								弾の発射
	/// </summary>
	/// <param name="_bulletData">	弾データ					</param>
	/// <param name="_dir">			発射方向					</param>
	/// <returns>					true:成功 / false:失敗		</returns>
	bool Shot(const BulletInfo::BulletData& _bulletData, const VECTOR3& _dir);

	/// <summary>
	///								拡散弾の発射
	/// </summary>
	/// <param name="_bulletData">	弾データ					</param>
	/// <param name="_matrix">		発射方向					</param>
	/// <param name="_matrix">		銃の回転行列				</param>
	/// <param name="_spread">		拡散量						</param>
	/// <param name="_palette">		パレット数					</param>
	/// <returns>					true:成功 / false:失敗		</returns>
	bool SpreadShot(const BulletInfo::BulletData& _bulletData, const VECTOR3& _dir,const MATRIX _rotMatrix, const float& _spread,const int& _palette);

	/// <summary>
	/// バースト射撃
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_vel"> ヴェロシティ </param>
	/// <param name="_bulletKind"> 弾の種類 </param>
	/// <param name="_shotNum"> 発射数 </param>
	/// <param name="_interval"> バースト間隔 </param>
	/// <returns> 成功：失敗 </returns>
	//bool BurstShot(const VECTOR3& _pos,const VECTOR3& _vel, const BULLET_KIND& _kind,const int _shotNum,const int _interval);

	/// <summary>
	/// 拡散射撃
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_vel"> ヴェロシティ </param>
	/// <param name="_bulletKind"> 弾の種類 </param>
	/// <param name="_shotNum"> 発射数 </param>
	/// <param name="_centerForSideAngle"> センターから横の最大拡散角度 </param>
	/// <returns> 成功：失敗 </returns>
	//bool SpreadShot(const VECTOR3& _pos,const VECTOR3& _vel, const BULLET_KIND& _kind,const int _shotNum,const float _centerForSideAngle);

private:

	static bool isActiveInstance;								// インスタンスがnewされているかのフラグ

	BulletManager* bulletManager;								// BulletManagerのポインタ

	//std::unordered_map<const void*, std::list<BulletData>> shotDatas;	// 識別用ポインタと弾データリスト
	std::unordered_map<const void*, IntervalData> shotInterval;			// 識別用ポインタと発射間隔データ
};
