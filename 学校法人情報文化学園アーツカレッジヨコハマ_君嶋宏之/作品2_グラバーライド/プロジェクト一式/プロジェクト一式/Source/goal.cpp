#include "goal.h"
#include "effectManager.h"

namespace {
	//ƒS[ƒ‹‚ÌƒŠƒ“ƒO‚Ì”
	const int GOAL_LING_NUM = 3;
	//ƒŠƒ“ƒO‚ÆƒŠƒ“ƒO‚ÌŠÔŠu
	const float GOAL_LING_INTERVAL = 300.0f;
}

Goal::Goal(SceneBase* _scene) :StageObject(_scene)
{
	size = VECTOR3(100.0f,100.0f,300.0f);
	effectManager = _scene->FindGameObject<EffectManager>();
	
}

Goal::~Goal()
{
}

void Goal::Draw()
{

	Object::Draw();
	for (auto itr = goalRingPos.begin(); itr != goalRingPos.end();++itr) {

		MV1SetMatrix(hModel, GetAftimageMatrix(*itr, rotate, scale));
		
		MV1DrawModel(hModel);
	}



}

void Goal::Update()
{
	StageObject::Update();

}

int Goal::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId)
{
	StageObject::Init(_hModel, _position, _scale, _rotate, _type, _collisionId);
	obbCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_GOAL);
	obbCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	for (int i = 0; i < GOAL_LING_NUM; i++) {
		goalRingInterval.emplace_back(GOAL_LING_INTERVAL * (i + 1));
		goalRingPos.emplace_back(VECTOR3(position.x, position.y, position.z + goalRingInterval[i]));
		effectManager->CreateEffekseer(goalRingPos[i], nullptr, "GOAL", 0.6f, 100, true,VECTOR3(2.0f,2.0f,2.0f),VZero);
	}
	return 1;
}
