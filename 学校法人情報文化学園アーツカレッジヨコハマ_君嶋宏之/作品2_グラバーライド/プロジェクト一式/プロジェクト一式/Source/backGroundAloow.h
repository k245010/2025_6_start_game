#pragma once
#include "../Library/gameObject.h"
#include "gravityManager.h"

class BackGroundAloow : public GameObject
{
public:
	BackGroundAloow(SceneBase* _scene);
	~BackGroundAloow();
	void Update()override;
	void Draw()override;

	void Start(VECTOR _pos, GravityState _state, float _angle,int _image, float _time);

	bool IsActive() { return active; }

private:

	int aloowImage;
	GravityState gravityState;

	float aloowMove;
	float aloowDirection;
	bool draw;
	VECTOR position;
	bool active;
	float time;
	float timeAdd;
	int animeCounter;
};
