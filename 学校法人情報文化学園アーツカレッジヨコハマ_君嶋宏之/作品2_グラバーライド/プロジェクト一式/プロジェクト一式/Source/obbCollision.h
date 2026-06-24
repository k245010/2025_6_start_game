#pragma once
#include "../Library/gameObject.h"
#include "collisionBase.h"
#include "charaBase.h"
#include <list>
#include <vector>

class OBBCollision : public CollisionBase
{
public:

	OBBCollision(Object* obj);
	~OBBCollision();

	void Update() override;
	void Draw() override;

	bool CollisionToSphere(SphereCollision* col) override;
	bool CollisionToOBB(OBBCollision* col) override;
	bool CollisionToRay(RayCollision* col) override;

	float CollisionToOBBLen(OBBCollision* col);

	VECTOR GetLen() { return VDivF(VMult(object->GetScale(), object->GetSize()), 2); }

	/// <summary>
	/// GetLenの割る値の引数つき.
	/// 基本は一辺の半分の値がほしいので、2で割るが当たり判定の大きさを変更したいときに使う
	/// </summary>
	/// <param name="div">どのくらい割るか.通常２</param>
	/// <returns>一辺の値</returns>
	VECTOR GetLen(const float div) { return VDivF(VMult(object->GetScale(), object->GetSize()), div); }

	VECTOR GetVDirection(int member) const;

	/// <summary>
	/// 一番近いブロックの法線を返す
	/// </summary>
	/// <param name="vec"> 自分の座標 </param>
	/// <returns> 一番近い法線 </returns>
	VECTOR GetNearModelDirection(const VECTOR& vec);
	
	/// <summary>
	/// モデルから法線情報を初期化できたかどうか
	/// </summary>
	/// <returns> 完了:未初期化 </returns>
	bool GetModelDirectionDone() { return modelDirectionDone; }

	/// <summary>
	/// モデルから法線情報を変数に初期化
	/// </summary>
	/// <param name="hModel"> モデルハンドル </param>
	void ModelDirectionInit(const int hModel);

private:
	
	void ModelDirectionInfo();


	void AddModelDirection(const VECTOR& vec);
	void DeleteModelDirection();

	std::vector<VECTOR> modelDirection;
	MV1_REF_POLYGONLIST polygonList;	// modelのポリゴン情報

	bool modelDirectionDone;

	int frameIndex;

	bool initDirection;					// 法線情報を初期化したかのフラグ
};