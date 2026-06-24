#pragma once
#include "itemObject.h"

class StageScore;

class Coin : public ItemObject
{
public:
	Coin(SceneBase* _scene);
	~Coin();
	//void Update() override;
	//void CollisionEvent(CollisionBase& me, CollisionBase& target)override;
	void ItemGetReaction()override;
	void ItemIdle()override;
private:
	StageScore* stageScore;
};