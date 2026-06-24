#pragma once
#include "stageObject.h"

class Bumper:public StageObject {
public:
	Bumper(SceneBase* _scene);
	~Bumper();
	void Update()override;
	void CollisionEvent(CollisionBase& me, CollisionBase& target)override;
	int Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int collisionId);
	enum class BumperState {
		BOUND,
		STAY,
	};

private:
	float rate;
	VECTOR3 hitBeforeScale;
	BumperState state;
};