#pragma once
#include "../Library/gameObject.h"
#include "config.h"
class Camera;
class BackGround :public  GameObject {
public:
	BackGround(SceneBase* _scene);
	~BackGround();
	void Draw()override;
	void Update()override;
	
private:
	int backHandle;
	int ScreenHandle;
	Camera* camera;
	int hModel;
	VECTOR position;
	VECTOR scale;
};
