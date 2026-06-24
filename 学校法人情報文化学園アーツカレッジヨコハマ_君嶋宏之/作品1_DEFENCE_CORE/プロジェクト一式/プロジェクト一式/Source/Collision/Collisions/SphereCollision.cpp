#include "sphereCollision.h"
#include "obbCollision.h"
#include "RayCollision.h"
#include "ModelCollision.h"


SphereCollision::SphereCollision(Transform* _trans, const COLLISION_OBJECT_KIND& _tagMe, const std::string& _ownerName, std::function<bool(const CollisionHitInfoData&)> func, bool _onHitDeleteMe) : CollisionBase(_trans, _tagMe,_ownerName, func, _onHitDeleteMe)
{
	kind						= COLLISION_KIND::SPHERE;
	collisionHitInfoData.kind	= COLLISION_KIND::SPHERE;

}

SphereCollision::~SphereCollision()
{
}

void SphereCollision::Draw()
{
	// 当たり判定の描画
	DrawSphere3D(collTransform.trans->position, GetRadius(), 16, 0xffff00, 0xff0000, false);
}

bool SphereCollision::CollisionToSphere(SphereCollision* col)
{
	const float HIT_SIZE	= GetRadius() + col->GetRadius();
	VECTOR3 targetVec		= GetPosition() - col->GetPosition();	// 自身から相手へのベクトル

	// ２点の距離がお互いの半径の合計よりも小さかったら
	if (VSquareSize(targetVec) < HIT_SIZE * HIT_SIZE)
	{
		VECTOR3 hitPos = col->GetPosition() + (targetVec.Normalize() * GetRadius());

		// お互いに当たった場所のデータを代入 //
		collisionHitInfoData.hitPointData		= CollisionHitPointData(std::vector<VECTOR3>{hitPos});
		col->collisionHitInfoData.hitPointData	= CollisionHitPointData(std::vector<VECTOR3>{hitPos});
		return true;
	}
	return false;
}

bool SphereCollision::CollisionToOBB(OBBCollision* col)
{
	float toOBBLenSq = CollisionToOBBLenSq(col);	// 自身とOBBとの距離

	return (toOBBLenSq < GetRadius() * GetRadius());
}

bool SphereCollision::CollisionToRay(RayCollision* col)
{
	return RayToSphere(col->GetPosition(), col->GetRayVec(), GetPosition(), GetRadius(), VSize(col->GetRayVec()));
}

bool SphereCollision::CollisionToModel(ModelCollision* col)
{
	return col->CollisionToSphere(this);
}

float SphereCollision::CollisionToOBBLen(OBBCollision* col)
{
	return CollisionToOBBVec(col).Size();
}

float SphereCollision::CollisionToOBBLenSq(OBBCollision* col)
{
	return CollisionToOBBVec(col).SquareSize();
}

VECTOR3 SphereCollision::CollisionToOBBVec(OBBCollision* col)
{
	constexpr int COLL_VMEM_MAX = 3;

	VECTOR3 halfLen				= col->GetHalfLen();// １辺の半分の長さ
	VECTOR3 spherePosition		= GetPosition();	// 球の座標
	float collRadius			= GetRadius();		// 球の半径
	VECTOR3 overVec				= VZero;			// はみ出しベクトル

	const VECTOR DISTANCE		= spherePosition - col->GetPosition();	// お互いのベクトル

	// VECTORの座標のメンバ一つ一つを見る
	for (int mem = 0;mem < COLL_VMEM_MAX;mem++)
	{
		// 辺の半分の長さが０以下だったら計算しない
		if (GetVMem(halfLen, mem) <= 0)
			continue;

		// 回転値からOBBの各面の法線方向を取得する
		VECTOR3 obbDir	= col->GetVDirection(mem);

		// 内積を使って四角形に対して現在座標の位置を割り出す -1以上 or 1以上のとき、mem軸の辺に対して、はみ出している
		float lenRate	= VDot(DISTANCE, obbDir) / GetVMem(halfLen, mem);	// 辺に対する位置の割合

		// 絶対値にする
		lenRate = fabsf(lenRate);

		// 四角形の辺からはみ出ている
		if (lenRate > 1)
		{
			// どれだけはみ出したかの算出
			overVec += ((obbDir * (lenRate - 1)) * GetVMem(halfLen, mem));
		}
	}
	return overVec;
}
