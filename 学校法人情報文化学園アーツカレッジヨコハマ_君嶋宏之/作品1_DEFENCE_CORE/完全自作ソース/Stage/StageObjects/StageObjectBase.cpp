#include "StageObjectBase.h"

StageObjectBase::StageObjectBase(const Transform& _trans, const ModelData& _modelData, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : Object3D(_trans, _modelData.modelHandle)
{
	kind			= _kind;
	hp				= (float)_hp;
	putPlaceKind	= _putPlaceKind;
	active			= true;
	modelData		= _modelData;

	// モデルの当たり判定のセットアップ
	SetupCollInfo(hModel,_trans.GetMatrix());
}

void StageObjectBase::SetupCollInfo(const int _hModel, const MATRIX& _m)
{
	// 当たり判定用に、モデルにトランスフォームを反映
	MV1SetMatrix(_hModel, _m);
	// 当たり判定の更新
	MV1RefreshCollInfo(_hModel);
}

