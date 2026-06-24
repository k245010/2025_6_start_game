#include "VoxelWayPoint.h"

VoxelWayPoint::VoxelWayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel) : VoxelWayPoint(_num, _pos, _ownerModel, NODE_STATE::NONE)
{
}

VoxelWayPoint::VoxelWayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_STATE& _state) : WayPoint(_num, _pos, _ownerModel, _state)
{
	kind = NAV_POINT_KIND::VOXEL_WAY_POINT;
}

VoxelWayPoint::VoxelWayPoint(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_ROLE& _role) : WayPoint(_num, _pos, _ownerModel, _role)
{
	kind = NAV_POINT_KIND::VOXEL_WAY_POINT;
}

VoxelWayPoint::~VoxelWayPoint()
{
}
