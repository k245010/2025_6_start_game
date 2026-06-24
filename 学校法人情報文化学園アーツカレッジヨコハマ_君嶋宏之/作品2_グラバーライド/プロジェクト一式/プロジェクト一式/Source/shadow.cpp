#include "shadow.h"
#include "loadManager.h"

Shadow::Shadow(SceneBase* _scene) : Object(_scene)
{
	//scale = VGet(1.0f, 0.1f, 1.0f);

	hModel = loadManager->GetHandle("36");
	//hModel = -1;
	scale = VGet(180.0f, 0.2f, 100.0f);
}

Shadow::~Shadow()
{

}

void Shadow::Update()
{
	rotate.z = gravityManager->GetWorldGravity();
	//rotate.z += 0.01f;
	//if (rotate.z >= DX_PI_F) {
		//rotate.z = -DX_PI_F;
	//}
}

void Shadow::Draw()
{
	Object::Draw();
}
