#pragma once
#include "WayPoint.h"

class VoxelWayPoint : public WayPoint
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid </param>
	/// <param name="_pos"> 座標 </param>
	VoxelWayPoint(const int& _num, const VECTOR3& _pos = VZero, const OwnerModelData& _ownerModel = OwnerModelData());

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid </param>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_state"></param>
	VoxelWayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_STATE& _state);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid </param>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_role">  </param>
	VoxelWayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_ROLE& _role);
	~VoxelWayPoint();

private:

};
