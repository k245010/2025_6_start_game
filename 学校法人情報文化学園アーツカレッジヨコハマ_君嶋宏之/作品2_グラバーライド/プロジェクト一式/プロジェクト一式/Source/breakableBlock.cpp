#include "breakableBlock.h"
#include "collisionBase.h"
BreakableBlock::BreakableBlock(SceneBase* _scene):StageObject(_scene)
{
	soundManager = commonFind->CommonFindObject<SoundManager>();
}

BreakableBlock::~BreakableBlock()
{
}

void BreakableBlock::CollisionEvent(CollisionBase& me, CollisionBase& target)
{
if (target.GetTagMe() == COLLISION_OBJECT_KIND::PLAYER_ATTACK) 
{
	//if (dynamic_cast<Player*>(target.GetObjectMe())->GetPlayerAttackState() == Player::PlayerAttackState::ATTACK1) {
	active = false;
	//}
	
	soundManager->PlaySe("ƒuƒƒbƒN”j‰ó");
	//effectManager->CreateEffekseer(position, nullptr, "BREAK_BROCK", 1.0f, 60.0f, false);
}
}
