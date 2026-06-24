#include "cameraArea.h"

CameraArea::CameraArea(SceneBase* _scene):StageObject(_scene)
{
	cameraAreaId = -1;
	cameraPullAddPosition = VZero;
}

CameraArea::~CameraArea()
{
}

void CameraArea::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int collisionId, int _cameraAreaId, VECTOR3 _cameraPullAddPosition)
{
	StageObject::Init(_hModel,_position, _scale, _rotate, _type,collisionId);
	sphereCollision->SetTagMe(COLLISION_OBJECT_KIND::CAMERA_AREA);
	sphereCollision->SetTargetTag(COLLISION_OBJECT_KIND::PLAYER);
	cameraAreaId = _cameraAreaId;
	cameraPullAddPosition = _cameraPullAddPosition;
}

