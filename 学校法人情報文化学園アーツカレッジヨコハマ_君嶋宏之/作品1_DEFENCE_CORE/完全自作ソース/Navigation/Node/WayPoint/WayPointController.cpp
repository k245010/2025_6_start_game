#include "WayPointController.h"
#include "../../../Collision/CollisionFunction.h"
#include "../../NavigationManager/NavigationManager.h"
#include "VoxelWayPoint.h"
#include "../../../../ImGui/imgui.h"
#include <assert.h>
#include "../../../../Library/DebugNew.h"

WayPointController::WayPointController()
{
	wayPointList.clear();
}

WayPointController::~WayPointController()
{
	for (auto itr = wayPointList.begin();itr != wayPointList.end();)
	{
		WayPoint* wayP = itr->second;

		if (wayP != nullptr)
		{
			wayP->DestroyMe();
			wayP = nullptr;
		}
		// 新しいイテレータを代入
		itr = wayPointList.erase(itr);
	}
}

void WayPointController::Update()
{
	
}

void WayPointController::Draw()
{

}

bool WayPointController::DeleteWayPoint(const int& _num)
{
	std::unordered_map<int, WayPoint*>::iterator itr = wayPointList.find(_num);

	// キーが見つからなかったら
	if (itr == wayPointList.end())
	{
		// 引数のキーのmap要素を見つけられませんでした。
		assert(false);
		return false;
	}

	WayPoint* wayPoint = wayPointList[_num];

	if (wayPoint != nullptr)
	{
		wayPoint->DestroyMe();
	}

	itr = wayPointList.erase(itr);
	return true;
}

NavPointBase* WayPointController::PushWayPoint(const int& _num, const std::unordered_map<NavPointBase*, int>& _target, const VECTOR3& _pos, const NAV_POINT_KIND& _kind)
{
	if (HasUnorderedMapContainerKey(wayPointList,_num))
		return nullptr;	// キーが見つかったので、return

	//_ WayPointの生成 _//

	WayPoint* wayPoint = nullptr;

	switch (_kind)
	{
	case NAV_POINT_KIND::WAY_POINT:

		wayPoint = new WayPoint(_num, _pos, OwnerModelData());
		break;
	case NAV_POINT_KIND::VOXEL_WAY_POINT:

		wayPoint = new VoxelWayPoint(_num, _pos, OwnerModelData());
		break;
	default:
		assert(false);
		break;
	}

	// グローバルナンバーの設定
	wayPoint->SetGlobalNumber(_num);

	//_ 接続相手の設定 _//

	for (auto& target : _target)
	{
		// 接続相手を設定
		wayPoint->PushTarget(target.first, (TARGET_CONNECT_KIND)target.second);
	}

	// 使えるWayPointとして追加
	wayPointList.insert(std::make_pair(_num, wayPoint));

	return wayPoint;
}
