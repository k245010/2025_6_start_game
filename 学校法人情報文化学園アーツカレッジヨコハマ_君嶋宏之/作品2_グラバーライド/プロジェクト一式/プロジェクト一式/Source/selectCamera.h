#pragma once
#include "cameraBase.h"

static const float CAMERA_Z_POS = -2000.0f;

class SelectCamera :public CameraBase {
public:
	SelectCamera(SceneBase* _scene);
	~SelectCamera();
	void Draw()override;
	void Update()override;
	void SetPosition(VECTOR3 pos) { position = pos; }
	void SetTarget(VECTOR3 pos) { target = pos; }
	VECTOR3 GetPosition(){ return position; }
	VECTOR3 GetTarget() { return target; }
private:
};