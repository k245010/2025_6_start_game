#pragma once
#pragma once
#include "Object.h"
class Camera;
class BackGround3D :public Object {
public:
	BackGround3D(SceneBase* _scene);
	~BackGround3D();
	void Draw()override;
	void Init(int _handle, Camera* _camera) { hModel = _handle; camera = _camera; };
	void Update()override;
private:
	Camera* camera;
	//int handle;
};