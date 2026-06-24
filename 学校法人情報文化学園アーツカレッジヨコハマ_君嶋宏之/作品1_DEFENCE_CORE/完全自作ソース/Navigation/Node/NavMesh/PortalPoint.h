#pragma once
#include "../NavPointBase.h"
#include "../../../Common/MeshInfo/MeshInfo.h"
#include <unordered_set>

struct OwnerModelData;
/// <summary>
/// ポータルのデータ
/// </summary>
struct PortalData
{
	int numberA, numberB;	// 二つの頂点ナンバーa,b
	VECTOR3 averageNorm;	// 二つの頂点で構成されるポリゴンの法線の平均値

	float minNormY;			// 最小値の法線のY軸値	壁に使うポータルか判断するため

	PortalData()				: numberA(-1), numberB(-1), averageNorm(VZero), minNormY(0.0f) {}
	PortalData(int _a, int _b)	: numberA(_a), numberB(_b), averageNorm(VZero), minNormY(0.0f) {}
};

/// <summary>
/// ナビゲーションを行うポータル（ポータルの中心座標をノードとして扱う）クラス
/// </summary>
class PortalPoint : public NavPointBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 識別用ナンバーid </param>
	/// <param name="_centoerPos"> ポータルの中心座標 </param>
	/// <param name="_portal"> ポータル情報 </param>
	PortalPoint(const int& _num,const VECTOR3& _centoerPos, const PortalData& _portal,const OwnerModelData& _ownerModel);
	PortalPoint();
	~PortalPoint();

	void Update()	override;
	void Draw()		override;
	
	/// <summary>
	/// 座標を設定する
	/// </summary>
	/// <param name="set"> 設定したい座標 </param>
	void SetPosition(const VECTOR3& set) { transform.position = set; }

	/// <summary>
	/// ポータルのデータを返す
	/// </summary>
	/// <returns> ポータルのデータ </returns>
	const PortalData& GetPortalData() const { return portal; }

	/// <summary>
	/// ポータルデータの法線平均値を設定する
	/// </summary>
	/// <param name="_ave"> 法線平均値 </param>
	void SetPortalDataAverageNorm(const VECTOR3& _ave) { portal.averageNorm = _ave; }

	/// <summary>
	/// ポータルデータの法線のY軸の最小値を設定する
	/// </summary>
	/// <param name="_min"> 法線のY軸の最小値 </param>
	void SetPortalDataMaxNorm(const float _min) { portal.minNormY = _min; }

	/// <summary>
	///	接続相手のポータルナンバーを追加する　ポータルの接続相手の初期化に使う
	/// </summary>
	/// <param name="_num"> 追加するポータルのナンバー </param>
	void PushInitTargetPortalPointNumber(const int& _num);

	/// <summary>
	/// 初期化時の接続相手のポータルナンバーを保存したコンテナを返す
	/// </summary>
	/// <returns> 接続相手のポータルナンバーを保存したコンテナ </returns>
	const std::unordered_set<int>& GetInitTargetPortalPointLocalNumberContainer() const { return targetPortalPointLocalNumberList; }

	/// <summary>
	/// 初期化時の接続相手のポータルナンバーを削除する
	/// </summary>
	/// <param name="_num"> 削除したいナンバー </param>
	/// <returns> 成功：失敗 </returns>
	bool DeleteInitTargetPortalPointLocalNumberContainer(const int& _num);

	/// <summary>
	///	接続相手のポータルを追加する　ポータルの接続相手の初期化に使う
	/// </summary>
	/// <param name="_num"> 追加するポータル </param>
	void PushInitTargetPortalPoint(PortalPoint* _target);

	/// <summary>
	/// 初期化時の接続相手のポータルを保存したコンテナを返す
	/// </summary>
	/// <returns> 接続相手のポータルを保存したコンテナ </returns>
	const std::list<PortalPoint*>& GetInitTargetPortalPointContainer() const { return targetPortalPointList; }

private:
	
	/// <summary>
	/// 接続相手のローカルナンバーを記録したコンテナ内に引数のナンバーがあるか返す
	/// </summary>
	/// <param name="_num"> 調べたいナンバー</param>
	/// <returns> ある:イテレータ / ない:イテレータend </returns>
	std::unordered_set<int>::iterator FindPushInitTargetNumber(const int& _num);

	PortalData portal;											// ポータル（ポリゴン同士の共有エッジ）
	std::unordered_set<int> targetPortalPointLocalNumberList;	// 初期化時の接続相手のポータルコンテナ
	std::list<PortalPoint*> targetPortalPointList;				// 初期化時の接続相手のポータルコンテナ

	//const void* parentModelPointer;					// このポータルポイントを構築するナビメッシュのモデルのクラスポインタ
};