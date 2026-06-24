#pragma once
#include "WayPoint.h"
//#include "../../../Transform.h"
#include "../../../../Library/GameObject.h"
#include <list>
#include <vector>
#include <unordered_map>


class NavigationManager;
class WayPointCollisionController;
class WayPoint;
/// <summary>
/// WayPointを総括するクラス
/// </summary>
class WayPointController : public GameObject
{
public:

	WayPointController();
	~WayPointController();

	void Update() override;
	void Draw() override;

	/// <summary>
	///						WayPointの削除と他ナビポイントとの接続をやめる	// 接続相手の削除はWayPointのデストラクタでやっているので不要
	/// </summary>
	/// <param name="_num"> WayPointナンバー					</param>
	/// <returns>			true:成功 / false:失敗				</returns>
	//bool DeleteWayPointAndTargetDisconnect(const int& _num);

	/// <summary>
	///						WayPointの削除
	/// </summary>
	/// <param name="_num"> WayPointナンバー					</param>
	/// <returns>			true:成功 / false:失敗				</returns>
	bool DeleteWayPoint(const int& _num);

	/// <summary>
	///							WayPointを生成する
	/// </summary>
	/// <param name="_num">		生成するWayPointのナンバー													</param>
	/// <param name="_target">	接続相手のコンテナ　key:ナビポイントポインタ / value:TARGET_CONECT_KIND値	</param>
	/// <param name="_kind">	生成するウェイポイントの種類												</param>
	/// <returns>				true:NavPointBaseポインタ / false:nullptr									</returns>
	NavPointBase* PushWayPoint(const int& _num, const std::unordered_map<NavPointBase*,int>& _target,const VECTOR3& _pos,const NAV_POINT_KIND& _kind);

private:

	

	std::unordered_map<int, WayPoint*> wayPointList;							// 現在使うことができるWayPointを保存
};