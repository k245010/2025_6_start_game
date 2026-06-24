#pragma once
#include "enemyBase.h"

class NoGravityEnemyBase :public EnemyBase
{
public:
	NoGravityEnemyBase(SceneBase* _scene);
	virtual ~NoGravityEnemyBase();

	MATRIX GetRotateMatrix()override;
	MATRIX MoveRotateMatrix()override;
};