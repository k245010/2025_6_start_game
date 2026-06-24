#include "backGroundAloow.h"
#include "function.h"
#include "config.h"

BackGroundAloow::BackGroundAloow(SceneBase* _scene) :GameObject(_scene)
{
	
}

BackGroundAloow::~BackGroundAloow()
{
	
}

void BackGroundAloow::Update()
{
	time += timeAdd;
	aloowMove = Lerp(0, 1920, time);
	position += VGet(0.0f, (float)-aloowMove, 0.0f) * MGetRotZ(aloowDirection);
	if (time >= 1.0f) {
		active = false;
	}
}

void BackGroundAloow::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
	animeCounter++;
	int anime = animeCounter / 4 % 3;
	//int anime = 0;
	DrawRectRotaGraphF(position.x, position.y, 0, SCREEN_HEIGHT * anime,SCREEN_WIDTH, SCREEN_HEIGHT,1.0, aloowDirection, aloowImage, true, false);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void BackGroundAloow::Start(VECTOR _pos, GravityState _state, float _angle,int _image,float _time)
{
	gravityState = _state;
	aloowDirection = _angle;
	active = true;
	aloowImage = _image;
	position = _pos;
	timeAdd = 1.0f / _time;
}
