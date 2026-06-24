#include "backGround.h"
#include "camera.h"
#include <cassert>
namespace {
	const float BACKGTOUND_SCALE = 30.0f;
}
BackGround::BackGround(SceneBase* _scene):GameObject(_scene)
{
	camera = _scene->FindGameObject<Camera>();

	backHandle = LoadGraph("data\\texture\\backGround.png");
	
	//ScreenHandle = MakeScreen(1920, 1920);
	hModel = MV1LoadModel("data\\model\\sky\\domebase.mv1");
	position = VECTOR3(0.0f, -1000.0f, 0.0f);
	scale = VECTOR3(BACKGTOUND_SCALE, BACKGTOUND_SCALE, BACKGTOUND_SCALE-10.0f);
}

BackGround::~BackGround()
{
	DeleteGraph(backHandle);
	//DeleteGraph(ScreenHandle);
	MV1DeleteModel(hModel);
}

void BackGround::Draw()
{
#if true
	
	DrawRotaGraph(SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, 1.0f, camera->GetCameraRot(), backHandle, true);	
	//GetDrawScreenGraph(0,0, 1920, 1080, ScreenHandle, true);
	//GraphFilter(ScreenHandle, DX_GRAPH_FILTER_GAUSS, 8, 1400);
	//DrawRectGraph(0, 0, 0, 0, 1920, 1080, ScreenHandle, true, false);
#else
	MV1SetMatrix(hModel,MGetScale(scale) * MGetTranslate(position));
	MV1DrawModel(hModel);
#endif
}
void BackGround::Update()
{
}
