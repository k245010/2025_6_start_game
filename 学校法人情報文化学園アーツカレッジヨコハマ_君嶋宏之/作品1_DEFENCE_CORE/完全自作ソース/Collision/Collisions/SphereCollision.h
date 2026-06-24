#pragma once
#include "CollisionBase.h"

/// <summary>
/// 球の当たり判定
/// </summary>
class SphereCollision : public CollisionBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_trans"> トランスフォームポインタ </param>
	/// <param name="_tagMe"> 自身の当たり判定オブジェクトタグ </param>
	/// <param name="_ownerName"> 自身の当たり判定オブジェクト名 </param>
	/// <param name="func"> あたったときに呼ぶ関数 </param>
	/// <param name="_onHitDeleteMe"> あたったときに自身の当たり判定オブジェクトを非アクティブにするかどうか　非アクティブにする : 非アクティブにしない </param>
	SphereCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName = "", std::function<bool(const CollisionHitInfoData&)> func = nullptr, bool _onHitDeleteMe = false);
	~SphereCollision();

	void Draw() override;

	/// <summary>
	/// 自身と球の当たり判定をする
	/// </summary>
	/// <param name="col"> SphereCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToSphere(SphereCollision* col) override;

	/// <summary>
	/// 自身とOBBの当たり判定をする
	/// </summary>
	/// <param name="col"> OBBCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToOBB(OBBCollision* col) override;

	/// <summary>
	/// 自身とレイの当たり判定をする
	/// </summary>
	/// <param name="col"> RayCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToRay(RayCollision* col) override;

	/// <summary>
	/// 自身とモデルの当たり判定をする
	/// </summary>
	/// <param name="col"> ModelCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToModel(ModelCollision* col) override;

	/// <summary>
	/// 自身とOBBの当たり判定をしてはみ出しの値を返す
	/// </summary>
	/// <param name="col"> OBBCollisionクラスポインタ </param>
	/// <returns> はみ出し値 </returns>
	float CollisionToOBBLen(OBBCollision* col);

	/// <summary>
	/// 自身とOBBの当たり判定をしてはみ出しベクトルの長さの二乗を返す
	/// </summary>
	/// <param name="col"> OBBCollisionクラスポインタ </param>
	/// <returns> はみ出しベクトルの長さの二乗 </returns>
	float CollisionToOBBLenSq(OBBCollision* col);

	/// <summary>
	/// 半径を返す
	/// </summary>
	/// <returns> 半径 </returns>
	float GetRadius() { return GetScale().x * (GetVMax(GetSize()) / 2); }

private:

	/// <summary>
	/// 自身とOBBの当たり判定をしてはみ出しベクトルを返す
	/// </summary>
	/// <param name="col"> OBBCollisionクラスポインタ </param>
	/// <returns> はみ出しベクトル </returns>
	VECTOR3 CollisionToOBBVec(OBBCollision* col);
};