#pragma once
#include "../NavPointBase.h"
#include <vector>

struct OwnerModelData;
/// <summary>
/// ナビゲーションを行うWayPointクラス
/// </summary>
class WayPoint : public NavPointBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid </param>
	/// <param name="_pos"> 座標 </param>
	WayPoint(const int& _num, const VECTOR3& _pos = VZero, const OwnerModelData& _ownerModel = OwnerModelData());

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid </param>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_state"></param>
	WayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel,const NODE_STATE& _state);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid </param>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_role">  </param>
	WayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel,const NODE_ROLE& _role);
	virtual ~WayPoint();

	void Update() override;
	void Draw() override;

	/// <summary>
	/// 座標を設定する
	/// </summary>
	/// <param name="_pos"> 設定したい座標 </param>
	void SetPosition(const VECTOR3& _pos) { transform.position = _pos; }

	/// <summary>
	/// 相手を記録したコンテナイテレータを返す
	/// </summary>
	/// <returns> 相手を記録したコンテナイテレータ </returns>
	//std::vector<int>::iterator GetInitTargetWayPointNumberItr() { return targetWayPointNumberList.begin(); }

	/// <summary>
	///	初期化時の接続相手のWayPointナンバーを追加する　WayPointの接続相手の初期化に使う
	/// </summary>
	/// <param name="_num"> 追加するWayPointのナンバー </param>
	bool PushInitTargetWayPointNumber(const int _num);

	/// <summary>
	/// 初期化時の接続相手のWayPointナンバーを保存したコンテナを返す
	/// </summary>
	/// <returns> 接続相手のWayPointナンバーを保存したコンテナ </returns>
	const std::vector<int>& GetInitTargetWayPointNumberContainer() const { return targetWayPointNumberList; }

	/// <summary>
	/// 初期化時の接続相手のWayPointナンバーを削除
	/// </summary>
	/// <param name="_num"> 削除したいWayPointナンバー </param>
	/// <returns> 成功  erase時の次のiterator：失敗 iterator end() </returns>
	std::vector<int>::iterator DeleteInitTargetWayPointNumber(const int _num);

	/// <summary>
	/// 相手を記録したコンテナから一時的に削除されたナンバーを保存しておくコンテナにナンバーを戻す
	/// </summary>
	/// <param name="pushNum"> 戻したい相手ナンバー </param>
	/// <returns> 戻せた：戻せなかった </returns>
	//bool PushRemoveTargetNumberListForTargetList(const int& pushNum);

	/// <summary>
	/// 相手を記録するコンテナの総サイズを返す
	/// </summary>
	//const size_t& GetGlobalTargetNumberSize() const { return targetNumber.size() + removeTargetNumberList.size(); }

protected:
	
	/// <summary>
	/// 相手を記録したコンテナ内に引数のナンバーがあるか返す
	/// </summary>
	/// <param name="_num"> 調べたいナンバー</param>
	/// <returns> ある:ない </returns>
	bool IsPushInitTargetNumber(const int& _num);

	/// <summary>
	/// 相手を記録したコンテナから一時的に削除されたナンバーに引数のナンバーがないかどうか探す
	/// </summary>
	/// <param name="_num"> 調べたいナンバー</param>
	/// <returns> ない:ある </returns>
	//bool IsntRemoveTargetSetNum(const int& _num);

	/// <summary>
	/// 相手を記録したコンテナから一時的に削除されたナンバーを保存しておくコンテナ内の引数のナンバーイテレータを返す
	/// </summary>
	/// <param name="_num"> ほしいナンバー </param>
	/// <returns> ナンバーイテレータ </returns>
	//const std::vector<int>::iterator GetRemoveTargetNumberIndex(const int& _num);

	std::vector<int> targetWayPointNumberList;				// 初期化時の相手のナンバー
	//std::vector<int> removeTargetNumberList;	// 相手の使用できなくなったナンバーを保存しておく

};