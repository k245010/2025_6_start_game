#pragma once
#include "stageObject.h"

class Goal : public StageObject
{
public:
	Goal(SceneBase*_scene);
	~Goal();
	void Draw() override;
	void Update() override;
	int Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId);
private:
	EffectManager* effectManager;
	std::vector<float> goalRingInterval;
	std::vector<VECTOR3> goalRingPos;
};