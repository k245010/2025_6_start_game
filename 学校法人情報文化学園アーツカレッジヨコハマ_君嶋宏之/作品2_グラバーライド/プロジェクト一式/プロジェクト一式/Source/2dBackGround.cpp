#include "2dBackGround.h"
#include "config.h"


BackGround2D::BackGround2D(SceneBase* _scene):GameObject(_scene)
{
	camera = _scene->FindGameObject<Camera>();
}

BackGround2D::~BackGround2D()
{
	//DeleteGraph(handle);
}

void BackGround2D::Draw()
{
	DrawRotaGraph(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, 1.0f, camera->GetCameraRot(), handle, true);
}

void BackGround2D::Init(int _handle)
{
	handle = _handle;
}
