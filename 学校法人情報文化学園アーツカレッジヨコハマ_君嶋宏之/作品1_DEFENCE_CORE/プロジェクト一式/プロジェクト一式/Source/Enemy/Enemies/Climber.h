#pragma once
#include "EnemyBase.h"

class Climber : public EnemyBase
{
public:

	Climber(const VECTOR3& _pos, const int& _hModel, EnemyManager* _enemyManager);
	~Climber();

	void Update()	override;
	void Draw()		override;

private:

	void Move() override;

	bool isClim				= false;	// •Ç‚ð“o‚Á‚Ä‚¢‚é‚©‚Ç‚¤‚©
	VECTOR3 climingRotate	= VZero;	// •Ç‚ð“o‚Á‚Ä‚¢‚éŽž‚Ìƒ‚ƒfƒ‹‰ñ“]’l
};
