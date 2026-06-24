#pragma once
#include "stageObject.h"

#include "collisionBase.h"
class SwitchObject : public StageObject {
public:
	SwitchObject(SceneBase* scene);
	~SwitchObject();
	int Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _switchId);
	int GetSwitchId()const { return switchId; }
	
	//void SetSwitchActive(bool active) { switchActive = active; }
protected:
	int switchId;
	//StageObjectManager* objectManager;
	//bool switchActive;
};
