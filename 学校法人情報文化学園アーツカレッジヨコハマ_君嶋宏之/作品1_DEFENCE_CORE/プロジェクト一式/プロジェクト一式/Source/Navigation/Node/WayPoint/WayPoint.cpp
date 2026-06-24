#include "WayPoint.h"
#include "../../../Transform/Transform.h"
#include "../../../../Library/DebugNew.h"

WayPoint::WayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel) : WayPoint(_num, _pos, _ownerModel, NODE_STATE::NONE)
{
}

WayPoint::WayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_STATE& _state) : NavPointBase(_num, _pos, _ownerModel, _state)
{
	kind = NAV_POINT_KIND::WAY_POINT;
}

WayPoint::WayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_ROLE& _role) : NavPointBase(_num, _pos, _ownerModel, _role)
{
	kind = NAV_POINT_KIND::WAY_POINT;
}

WayPoint::~WayPoint()
{
}

void WayPoint::Update()
{
	NavPointBase::Update();
}

void WayPoint::Draw()
{
	NavPointBase::Draw();
}

bool WayPoint::PushInitTargetWayPointNumber(const int _num)
{
	if (IsPushInitTargetNumber(_num))
		return false;	// すでに追加されていたら return

	// 接続相手のナンバーをpush
	targetWayPointNumberList.emplace_back(_num);
	return true;
}

std::vector<int>::iterator WayPoint::DeleteInitTargetWayPointNumber(const int _num)
{
	// 追加されていなかったら
	if (!IsPushInitTargetNumber(_num))
	{
		assert(false);
		return targetWayPointNumberList.end();
	}

	// 接続相手のナンバーを削除
	return targetWayPointNumberList.erase(std::remove(targetWayPointNumberList.begin(), targetWayPointNumberList.end(), _num));
}

bool WayPoint::IsPushInitTargetNumber(const int& _num)
{
	// キーが見つかったら、true / 見つからなかったら、false
	return (std::find(targetWayPointNumberList.begin(), targetWayPointNumberList.end(),_num) != targetWayPointNumberList.end());
}
