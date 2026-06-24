#pragma once
#include "../../../Library/gameObject.h"
#include "CollisionBase.h"
//#include "../../CharaBase.h"
#include <list>
#include <vector>

/// <summary>
/// OBBの当たり判定
/// </summary>
class OBBCollision : public CollisionBase
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
	OBBCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName = "", std::function<bool(const CollisionHitInfoData&)> func = nullptr, bool _onHitDeleteMe = false);
	~OBBCollision();

	void Update() override;
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
	/// BOXの一辺の半分の値を返す
	/// </summary>
	/// <returns> BOXの一辺の半分の値 </returns>
	VECTOR GetHalfLen() { return VDivF(VMult(GetScale(), GetSize()), 2); }

	/// <summary>
	/// GetLen関数の割る値の引数つき.
	/// 基本は一辺の半分の値がほしいので、2で割るが当たり判定の大きさを変更したいときに使う
	/// </summary>
	/// <param name="div">どのくらい割るか.通常２</param>
	/// <returns>一辺の値</returns>
	VECTOR GetDivLen(const float div) { return VDivF(VMult(GetScale(), GetSize()), div); }

	/// <summary>
	/// 指定された要素の方向ベクトルを返す
	/// </summary>
	/// <param name="member"> どの要素か指定い x 0, y 1, z 2 </param>
	/// <returns> 方向ベクトル </returns>
	const VECTOR3 GetVDirection(int member) const;

private:
	
};