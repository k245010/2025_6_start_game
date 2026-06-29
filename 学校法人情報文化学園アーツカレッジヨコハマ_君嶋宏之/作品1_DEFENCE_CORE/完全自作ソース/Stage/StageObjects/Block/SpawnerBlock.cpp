#include "SpawnerBlock.h"
#include "../../BlockController/BlockController.h"

SpawnerBlock::SpawnerBlock(const Transform& _trans, const ModelData& _modelData, const int& _hp, const PUT_PLACE_KIND& _putPlaceKind) : BlockBase(_trans, _modelData, StageObjectData::STAGE_OBJECT_KIND::SPAWNER_BLOCK, _hp, _putPlaceKind)
{
}

SpawnerBlock::~SpawnerBlock()
{
}

void SpawnerBlock::Update()
{
	BlockBase::Update();
}

void SpawnerBlock::Draw()
{
	BlockBase::Draw();
}
