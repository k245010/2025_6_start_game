#pragma once
#include "EnemyBase.h"

class Rusher : public EnemyBase
{
public:
	Rusher(const VECTOR3& _pos, const int& _hModel, EnemyManager* _enemyManager);
	~Rusher();

private:

	void Move() override;
};
