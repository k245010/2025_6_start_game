#pragma once

enum StarType
{
	CLEAR_STAR = 0,
	TIME_STAR,
	COIN_STAR,
	//DAMAGE_STAR,

	STAR_NUM,
};

struct StarTarget
{
	float targetTime;
	int targetCoin;
	int targetDamageNum;
};
