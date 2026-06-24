#include "NavPointBase.h"
#include "../../Collision/CollisionFunction.h"
#include "../CostInfo.h"
#ifdef _DEBUG

#include "../NavigationManager/NavigationManager.h"
#endif // _DEBUG

#include "../../../ImGui/imgui.h"
#include <string.h>
#include <assert.h>

namespace
{
	static const float DRAW_RADIUS = 5.0f;
}

NavPointBase::NavPointBase(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel)
{
	if (_num < 0)
		assert(false);

	localNumber			= _num;
	globalNumber		= -1;
	transform.position	= _pos;

	blockState		= NODE_BLOCK_STATE::FREE;

	cost.g			= 0.0f;
	cost.h			= 0.0f;
	cost.parent		= nullptr;

	costRate		= NodeCostRateInfo::GetNormalAreaCostRate();

	ownerModelData	= _ownerModel;

	isDrawNum		= false;
	isActive		= true;

	role			= NODE_ROLE::NORMAL;
	state			= NODE_STATE::NONE;
	useKind			= NODE_USE_KIND::NORMAL;
	kind			= NAV_POINT_KIND::NONE;

	norm			= VZero;

	targetList.clear();
}

NavPointBase::NavPointBase(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_STATE& _state) : NavPointBase(_num, _pos, _ownerModel)
{
	transform.position = _pos;
	state = _state;
}

NavPointBase::NavPointBase(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_ROLE& _role) : NavPointBase(_num, _pos, _ownerModel)
{
	transform.position = _pos;
	role = _role;
}

NavPointBase::~NavPointBase()
{
	targetList.clear();

#ifdef _DEBUG

	NavigationManager::debugDeleteNavPointNumberCheckList.erase(globalNumber);
#endif // _DEBUG
}

void NavPointBase::Update()
{

}

void NavPointBase::Draw()
{
	
}

bool NavPointBase::PushTarget(NavPointBase* _target, TARGET_CONNECT_KIND _kind)
{
	// すでに保存されていたら
	if (targetList.find(_target) != targetList.end())
		return false;

	// 接続相手の情報をpush
	targetList.emplace(_target, TargetData(_kind));

	//// 法線に値が設定されていたら、または接続相手が外積で法線を求める量に達していなかったら
	//if (!VAllSame(norm, VZero) || targetList.size() < 2)
	//	return true;

	////__ 接続相手から外積を使い、自身の法線情報を設定 __//

	//VECTOR3 originPos = VZero;	// 原点

	//auto itr = targetList.begin();
	//NavPointBase& n = *(*itr);
	//VECTOR3 dirAPos = n.GetPosition(); // 方向ベクトルA
	//
	//itr++;
	//n = *(*itr);
	//VECTOR3 dirBPos = n.GetPosition(); // 方向ベクトルB

	return true;
}

bool NavPointBase::DeleteTarget(const int& _targetNum)
{
	for (auto& target : targetList)
	{
		// 引数の接続相手がボクセルのコンテナ内に存在した場合、コンテナから削除する
		target.second.DeleteVoxelOwnerKeyAndEmptyConfirm(_targetNum);
	}

	auto itr = IsPushTargetNumber(_targetNum);

	// 指定のNavPointBaseが保存されていなかったら
	if (itr == targetList.end())
		return false;

	// 接続相手の削除
	itr = targetList.erase(itr);
	return true;
}

bool NavPointBase::DeleteTarget(NavPointBase* _target)
{
	for (auto& target : targetList)
	{
		// 引数の接続相手がボクセルのコンテナ内に存在した場合、コンテナから削除する
		target.second.DeleteVoxelOwnerKeyAndEmptyConfirm(_target->GetGlobalNumber());
	}

	auto itr = targetList.find(_target);

	// 指定のNavPointBaseが保存されていなかったら
	if (itr == targetList.end())
		return false;

	// 接続相手の削除
	itr = targetList.erase(itr);
	return true;
}

const TargetData* NavPointBase::GetTargetData(const NavPointBase* _nav)
{
	for (const auto& target : targetList)
	{
		if (_nav == target.first)
			return &target.second;	// 接続相手と引数が一致したら、接続相手のデータを return
	}
	return nullptr;
}

std::list<int> NavPointBase::GetTargetNumberContainer()
{
	std::list<int> numList;

	for (const auto& target : targetList)
	{
		// 接続相手のナンバーをpush
		numList.emplace_back(target.first->GetGlobalNumber());
	}
	return numList;
}

std::unordered_map<NavPointBase*, TargetData>::iterator NavPointBase::IsPushTargetNumber(const int& _targetNum)
{
	for (auto itr = targetList.begin(); itr != targetList.end(); itr++)
	{
		const NavPointBase& n = *(itr->first);

		if (n.GetGlobalNumber() == _targetNum)
			return itr;	// 接続相手のグローバルナンバーと引数が、一致したら接続相手をイテレーターで返す
	}
	return targetList.end();
}
