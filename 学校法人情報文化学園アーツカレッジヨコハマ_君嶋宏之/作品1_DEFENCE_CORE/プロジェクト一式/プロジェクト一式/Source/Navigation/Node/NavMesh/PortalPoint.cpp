#include "PortalPoint.h"

PortalPoint::PortalPoint(const int& _num, const VECTOR3& _centoerPos, const PortalData& _portal, const OwnerModelData& _ownerModel) : NavPointBase(_num, _centoerPos,_ownerModel)
{
	kind	= NAV_POINT_KIND::NAV_MESH;
	portal	= _portal;
}

PortalPoint::PortalPoint() : NavPointBase(-1)
{
}

PortalPoint::~PortalPoint()
{
	targetPortalPointLocalNumberList.clear();
}

void PortalPoint::Update()
{
	NavPointBase::Update();
}

void PortalPoint::Draw()
{
	NavPointBase::Draw();
}

void PortalPoint::PushInitTargetPortalPointNumber(const int& _num)
{
	// すでに接続相手が追加されていたら
	if (FindPushInitTargetNumber(_num) != targetPortalPointLocalNumberList.end())
	{
		assert(false);
	}

	// 接続相手のナンバーをpush
	targetPortalPointLocalNumberList.emplace(_num);
}

bool PortalPoint::DeleteInitTargetPortalPointLocalNumberContainer(const int& _num)
{
	std::unordered_set<int>::iterator itr = FindPushInitTargetNumber(_num);

	// 接続相手が追加されていたら
	if (itr != targetPortalPointLocalNumberList.end())
	{
		// 接続相手のナンバーを削除
		targetPortalPointLocalNumberList.erase(itr);
		return true;
	}
	return false;
}

void PortalPoint::PushInitTargetPortalPoint(PortalPoint* _target)
{
	if (FindPushInitTargetNumber(_target->globalNumber) != targetPortalPointLocalNumberList.end())
		return;	// すでに登録されていたら、return

	// 接続相手のポータルポイントをpush
	targetPortalPointList.emplace_back(_target);
}

std::unordered_set<int>::iterator PortalPoint::FindPushInitTargetNumber(const int& _num)
{
	// 接続相手コンテナにナンバーが見つかったらイテレータを返す
	return targetPortalPointLocalNumberList.find(_num);
}
