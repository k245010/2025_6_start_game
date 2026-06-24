#include "switchObject.h"


SwitchObject::SwitchObject(SceneBase* scene):StageObject(scene)
{
	//objectManager = nullptr;

}

SwitchObject::~SwitchObject()
{
}

int SwitchObject::Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _switchId)
{
	StageObject::Init(_hModel, _position, _scale, _rotate, _type, _collisionId);
	switchId = _switchId;
	//objectManager = pointer;
	return 0;
}



