#pragma once
#include "stageObject.h"
#include "effectManager.h"
#include "soundManager.h"
class SoundManager;
class BreakableBlock :public StageObject
{
public:
	BreakableBlock(SceneBase* _scene);
	~BreakableBlock();
	void CollisionEvent(CollisionBase& me, CollisionBase& target)override;

private:
	SoundManager* soundManager;
};