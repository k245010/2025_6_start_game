#pragma once
#include "stageObject.h"

class Point :public StageObject {
public:
	Point(SceneBase* _scene);
	~Point();
	int GetCheckPointId()const { return checkPointId; }
	void Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type,int collisionId, int checkId);

	void CollisionEvent(CollisionBase& me, CollisionBase& target) override;

	void Update()override;
	void Draw()override;
private:
	int checkPointId;
	float rate;
	float wordUpPos;
	bool isWord;
};