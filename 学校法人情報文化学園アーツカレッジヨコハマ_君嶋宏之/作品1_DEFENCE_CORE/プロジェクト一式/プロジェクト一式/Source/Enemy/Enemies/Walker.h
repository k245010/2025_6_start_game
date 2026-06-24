#pragma once
#include "EnemyBase.h"

class Walker : public EnemyBase
{
public:
	
	Walker(const VECTOR3& _pos,const int& _hModel, EnemyManager* _enemyManager);
	~Walker();

private:

	void Move() override;

	VECTOR3 debugRay[2];
};
