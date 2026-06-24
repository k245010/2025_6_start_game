#include "gravityArea.h"

GravityArea::GravityArea(SceneBase* _scene):StageObject(_scene)
{
	//effectManager->CreateEffekseer(, nullptr, e, 1.0f, 5000, true, VGet(scale.x / 15.0f, scale.y / 10.0f, scale.z / 10.0f), rotation);
}

GravityArea::~GravityArea()
{
	//effectManager->StopEffekseer()
}

void GravityArea::Init(int Model, VECTOR pos, VECTOR scale, VECTOR rotation,int type, int _collisionId, int _gravityAreaType)
{
	StageObject::Init(Model, pos, scale, rotation,type, _collisionId);
	sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_GRAVITY_AREA);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	graviyAreaType = _gravityAreaType;
	std::string e = "GRAVITY_AREA" +std::to_string(_gravityAreaType);
	effectManager->CreateEffekseer(pos, nullptr, e, 1.0f, 5000, true, VGet(scale.x / 15.0f, scale.y / 10.0f, scale.z / 10.0f), rotation);
}

void GravityArea::Draw()
{
	StageObject::Draw();
	//VECTOR3 size = scale*58.0f;

	//DrawCube3D(VGet(position.x - size.x, position.y + size.y, position.z -size.z), VGet(position.x + size.x, position.y - size.y, position.z + size.z), RGB_RED, RGB_WHITE, false);
}
