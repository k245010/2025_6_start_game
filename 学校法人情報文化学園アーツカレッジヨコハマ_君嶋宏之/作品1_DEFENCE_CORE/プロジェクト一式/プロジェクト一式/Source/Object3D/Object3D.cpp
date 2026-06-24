#include "Object3D.h"
#include <assert.h>

Object3D::Object3D() : Object3D(Transform(), 0) {};

Object3D::Object3D(const Transform& _trans) : Object3D(_trans, 0) {};

Object3D::Object3D(const Transform& _trans, const int& _hModel)
{
	transform	= _trans;
	hModel		= _hModel;
	assert(hModel >= 0);
}

Object3D::~Object3D()
{
	if (hModel > 0 && !dontDeleteMeHmodel)
	{
		MV1DeleteModel(hModel);
		hModel = -1;
	}
}

void Object3D::Update()
{
}

void Object3D::Draw()
{
	// 描画するとき
	if (onModelDraw)
	{
		// モデルハンドルに行列が設定できたら
		if (transform.SetMatrixModel(hModel) >= 0)
		{
			MV1DrawModel(hModel);
		}
	}
}


