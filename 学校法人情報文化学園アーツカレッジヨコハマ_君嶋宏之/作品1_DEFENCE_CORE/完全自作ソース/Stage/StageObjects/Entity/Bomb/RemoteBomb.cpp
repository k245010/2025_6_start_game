#include "RemoteBomb.h"

RemoteBomb::RemoteBomb(const Transform& _trans, const int& _hModel, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BombBase(_trans,_hModel, StageObjectData::STAGE_OBJECT_KIND::REMOTE_BOMB, _hp,_putPlaceKind)
{
}

RemoteBomb::~RemoteBomb()
{
}

void RemoteBomb::Update()
{
	BombBase::Update();

	if (CheckHitKey(KEY_INPUT_L))
		isExplosion = true;
}

void RemoteBomb::Draw()
{
	BombBase::Draw();
}
