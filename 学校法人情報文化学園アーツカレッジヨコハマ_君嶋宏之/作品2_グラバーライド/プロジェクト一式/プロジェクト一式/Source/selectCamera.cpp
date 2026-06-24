#include "selectCamera.h"

SelectCamera::SelectCamera(SceneBase* _scene):CameraBase(_scene)
{
	position = VECTOR3(1000.0f, 0.0f, CAMERA_Z_POS);
	target = VAdd(position,VECTOR3(0.0f,0.0f,50.0f));
	cameraUpVec = VECTOR3(0.0f, 1.0f, 0.0f);
	//debugMode = DEBUG_MODE;
}

SelectCamera::~SelectCamera()
{
}

void SelectCamera::Draw()
{
	CameraBase::Draw();
}

void SelectCamera::Update()
{
	CameraBase::Update();
	//target = VAdd(VGet(0.0f, 0.0f, 50.0f), position);
}

