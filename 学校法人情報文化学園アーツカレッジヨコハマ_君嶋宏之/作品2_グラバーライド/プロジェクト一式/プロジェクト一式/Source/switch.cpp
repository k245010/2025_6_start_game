#include "switch.h"
#include "irondoor.h"
#include "../Library/time.h"
#include "../ImGui/imgui.h"

namespace {
	const float RAID_TIME=1.0f;
	const float DENT_SIZE = 56.0f;
	//次にスイッチが切り替えられるまでの時間
	const int WAIT_TIME = 30;
}

Switch::Switch(SceneBase* _scene):SwitchObject(_scene)
{
	
	stageObjectManager = nullptr;

}

Switch::~Switch()
{
}

void Switch::CollisionEvent(CollisionBase& me, CollisionBase& target)
{
	if (target.GetTagMe() == COLLISION_OBJECT_KIND::PLAYER)
	{

		switchAnimState = SwitchAnimState::DENT;
		if (anime->GetAnimesion().index == -1) {
			anime->AnimeStartHandle(SwitchAnimState::DENT);
		}
		
	}
	//else {
	//	if (size.y < DENT_SIZE) {
	//		size.y += Time::DeltaTime();
	//	}
	//}
}

void Switch::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _switchId, StageObjectManager* p)
{
	SwitchObject::Init(_hModel, _position, _scale, _rotate, _type, _collisionId,_switchId);
	sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::BLOCK_SWITCH);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::LAND);
	stageObjectManager = p;
	//isSwitch = false;
	rideTime = RAID_TIME;
	initialSizeY = size.y;
	//anim = new Anim();

	isAnimation = false;
	switchAnimState = SwitchAnimState::NONE;

	hModel= MV1DuplicateModel(_hModel);
	anime = new Anime<SwitchAnimState>(loadManager);
	anime->AnimeInformation(hModel, FALSE, SwitchAnimState::DENT, 1.0f, "SWITCH");

}

void Switch::Update()
{
	switch (switchAnimState)
	{
	case SwitchAnimState::NONE:
	
		break;
	case SwitchAnimState::DENT:
		anime->Update();
		if (size.y > DENT_SIZE) {
			size.y -= Time::DeltaTime() * 100.0f;
		}
		else {
			stageObjectManager->SetSwitchObjectState(switchId, 1);
			anime->AnimesionChangeSpeed();
			switchAnimState = SwitchAnimState::UP;
		}
		break;
	case SwitchAnimState::UP:
		anime->Update();
		size.y += Time::DeltaTime() * 100.0f;
		if (size.y > initialSizeY) {
			switchAnimState = SwitchAnimState::NONE;
		}
		break;
	default:
		break;
	}




#if _DEBUG
	ImGui::Begin("switch");
	ImGui::SliderFloat3("size", &size.x, 0.0f, 300.0f);
	ImGui::End();
#endif // _DEBUG

	
}

