#pragma once
#include "../Library/gameObject.h"
#include "camera.h"

class BackGround2D :public GameObject {
public:
	BackGround2D(SceneBase* _scene);
	~BackGround2D();
	void Draw()override;
	void Init(int _handle);
private:
	int handle;
	Camera* camera;
};