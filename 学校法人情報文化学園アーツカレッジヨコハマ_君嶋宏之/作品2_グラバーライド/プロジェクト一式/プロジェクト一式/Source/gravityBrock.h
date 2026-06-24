#pragma once
#include "stageObject.h"

class Player;

class GravityBrock :public StageObject {
public:
	GravityBrock(SceneBase* _scene);
	~GravityBrock();
	int Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId)override;

	void Update() override;
	bool IsMove() { return onMove; }
private:

	bool onMove;
	VECTOR lastPosition;
	Player* player;
	int counter;
};