#include "RemoteBomb.h"

RemoteBomb::RemoteBomb(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BombBase(_trans, _modelData, StageObjectData::STAGE_OBJECT_KIND::REMOTE_BOMB, _hp,_putPlaceKind)
{
}

RemoteBomb::~RemoteBomb()
{
}

void RemoteBomb::Update()
{
	BombBase::Update();

	/*if (CheckHitKey(KEY_INPUT_L))
		canExplosion = true;*/
}

void RemoteBomb::Draw()
{
	BombBase::Draw();
}
