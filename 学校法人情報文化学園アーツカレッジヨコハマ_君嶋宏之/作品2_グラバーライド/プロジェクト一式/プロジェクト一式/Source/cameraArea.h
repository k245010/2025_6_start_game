#pragma once
#include "StageObject.h"

class CameraArea :public StageObject {
public:
	CameraArea(SceneBase*_scene);
	~CameraArea();
	void Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int collisionId,int _cameraAreaId,VECTOR3 _cameraPullAddPosition);
	int GetCameraId()const { return cameraAreaId; }
	VECTOR3 GetCameraPullAddPosition() { return cameraPullAddPosition; }

	//void Draw()override;
private:
	int cameraAreaId;
	VECTOR3 cameraPullAddPosition;
};