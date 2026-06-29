#pragma once
#include "CollisionBase.h"

/// <summary>
/// レイの当たり判定
/// </summary>
class RayCollision : public CollisionBase
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
	RayCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName = "", std::function<bool(const CollisionHitInfoData&)> func = nullptr, bool _onHitDeleteMe = false);
	~RayCollision();

	void Draw() override;

	/// <summary>
	/// 自身と球の当たり判定をする
	/// </summary>
	/// <param name="col"> SphereCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToSphere(SphereCollision* col) override;

	/// <summary>
	/// レイと球の当たり判定...OBBなのに球なのは簡単だから
	/// </summary>
	/// <param name="col"></param>
	/// <returns></returns>
	bool CollisionToOBB(OBBCollision* col) override;

	/// <summary>
	/// レイとレイの当たり判定は未実装
	/// </summary>
	/// <param name="col"></param>
	/// <returns></returns>
	bool CollisionToRay(RayCollision* col) override { return false; }

	/// <summary>
	/// 自身とモデルの当たり判定をする
	/// </summary>
	/// <param name="col"> ModelCollisionクラスポインタ </param>
	/// <returns> 当たっている：当たっていない </returns>
	bool CollisionToModel(ModelCollision* col) override;

	/// <summary>
	/// レイの方向ベクトルを返す
	/// </summary>
	/// <returns> レイの方向ベクトル </returns>
	const VECTOR3& GetRayDirection() const { return rayVec; }

	/// <summary>
	/// レイの始点を返す
	/// </summary>
	/// <returns> レイの原点 </returns>
	const VECTOR3& GetCenter() const { return centerPos; }

	/// <summary>
	/// レイの終点を返す
	/// </summary>
	/// <returns> レイの終点 </returns>
	const VECTOR3& GetTarget() const { return targetPos; }

	/// <summary>
	/// レイの方向ベクトルを設定
	/// </summary>
	/// <param name="_pos1"> 始点 </param>
	/// <param name="_pos2"> 終点 </param>
	void SetRayDirection(const VECTOR3& _pos1, const VECTOR3& _pos2)
	{ 
		centerPos	= _pos1;
		targetPos	= _pos2;
		rayVec		= _pos2 - _pos1; 
	}

private:

	VECTOR3 rayVec;		// レイベクトル
	VECTOR3 centerPos;	// レイの原点
	VECTOR3 targetPos;	// 届く点
};