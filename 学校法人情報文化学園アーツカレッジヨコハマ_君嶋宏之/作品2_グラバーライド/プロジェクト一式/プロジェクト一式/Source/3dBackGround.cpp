#include "3dBackGround.h"
#include "camera.h"
namespace {
	const float BACKGTOUND_SCALE = 30.0f;
}
BackGround3D::BackGround3D(SceneBase* _scene):Object(_scene)
{
	camera = nullptr;
	position = VECTOR3(0.0f, 0.0f, 0.0f);
	scale = VECTOR3(BACKGTOUND_SCALE, BACKGTOUND_SCALE, BACKGTOUND_SCALE - 10);
}

BackGround3D::~BackGround3D()
{
}

void BackGround3D::Draw()
{
	if (hModel > 0) {
		MV1SetMatrix(hModel, GetMatrix());
		MV1SetDifColorScale(hModel, rgb.ColorF(alpha));
		MV1DrawModel(hModel);
	}
}

void BackGround3D::Update()
{
	if (camera != nullptr) {
		position = camera->GetTargetPosition();
	}
}


