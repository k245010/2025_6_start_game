#pragma once
#include "switchObject.h"
#include "stageObjectManager.h"
#include "switchMoveObject.h"
#include "anime.h"
enum class SwitchAnimState {
	NONE = -1,
	DENT,
	UP,
};

class StageObjectManager;
class Switch:public SwitchObject
{
public:
	Switch(SceneBase* _scene);
	~Switch();
	void CollisionEvent(CollisionBase& me, CollisionBase& target)override;
	void Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _switchId, StageObjectManager* p);
	void Update()override;
private:
	float rideTime;
	StageObjectManager* stageObjectManager;
	Anime<SwitchAnimState>*anime;
	SwitchAnimState switchAnimState;
	float initialSizeY;
	bool isAnimation;
};
