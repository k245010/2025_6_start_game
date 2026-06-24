#pragma once
#pragma once
#include "switchObject.h"

class SwitchMoveObject : public SwitchObject {
public:
	SwitchMoveObject(SceneBase* scene);
	~SwitchMoveObject();
	void Update()override;
	//int Init(int _hModel, VECTOR _position, VECTOR _scale, VECTOR _rotate, int _type, int _collisionId, int _switchId);
	enum class SwitchMoveObjectState {
		STAY,
		MOVE,
		BACK,
	};
	inline void IsMove() { if (switchMoveObjectState != SwitchMoveObjectState::MOVE)  switchMoveObjectState = SwitchMoveObjectState::MOVE; }
	//void SetSwitchActive(bool active) { switchActive = active; }
	virtual void SwitchObjectMoveProcess();
	virtual void SwitchObjectBackProcess();
	void SetSwitchMoveObjectState(int type) { switchMoveObjectState = (SwitchMoveObjectState)type; }
protected:
	SwitchMoveObjectState switchMoveObjectState;
	//StageObjectManager* objectManager;
	//bool switchActive;
};
