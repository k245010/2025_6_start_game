#pragma once
#include "EnemyBase.h"

class Crusher : public EnemyBase
{
public:

	Crusher(const VECTOR3& _pos, const int& _hModel, EnemyManager* _enemyManager);
	~Crusher();

private:

	void Move() override;
};
