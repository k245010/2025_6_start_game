#include "switchMoveObject.h"

SwitchMoveObject::SwitchMoveObject(SceneBase* scene):SwitchObject(scene)
{
	switchMoveObjectState = SwitchMoveObjectState::STAY;
}

SwitchMoveObject::~SwitchMoveObject()
{
}

void SwitchMoveObject::Update()
{
	SwitchObject::Update();
	switch (switchMoveObjectState)
	{
	case SwitchMoveObjectState::STAY:
		break;
	case  SwitchMoveObjectState::MOVE:
		SwitchObjectMoveProcess();
		break;
	case SwitchMoveObjectState::BACK:
		SwitchObjectBackProcess();
		break;
	default:
		break;
	}
}

void SwitchMoveObject::SwitchObjectMoveProcess()
{
}

void SwitchMoveObject::SwitchObjectBackProcess()
{
}
