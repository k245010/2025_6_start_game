#pragma once
#include "noGravityEnemyBase.h"
#include "anime.h"

class WalkEnemy :public NoGravityEnemyBase
{
public:
	WalkEnemy(SceneBase* _scene);
	virtual ~WalkEnemy();
	void Update()override;
	void Draw()override;

private:
	enemyNomalAnime* anime;
	void AnimeInit();

	const int ANIMATION_WALK_TO_TURN_CHANGE_TIME = 20;//歩きアニメーションから見回しアニメーションに変わるフレーム数
	const int ANIMATION_TURN_TO_WALK_CHANGE_TIME = 20;//見回しアニメーションから歩きアニメーションに変わるフレーム数
};