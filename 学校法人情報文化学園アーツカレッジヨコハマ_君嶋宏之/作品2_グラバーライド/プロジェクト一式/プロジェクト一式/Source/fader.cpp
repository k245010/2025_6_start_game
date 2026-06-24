#include "fader.h"
#include "config.h"

Fader::Fader(SceneBase* _scene) : GameObject(_scene), 
	current(0), time(0), target(0), color(GetColor(0, 0, 0))
{
}

void Fader::Update()
{
	if (time > 0) {
		current += (target - current) / time;
		time--;
	}
}

void Fader::Draw()
{
	if (current > 0.0f) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(current));
		DrawBox(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
}

void Fader::ChangeColor(unsigned char r, unsigned char g, unsigned char b)
{
	color = GetColor(r, g, b);
}



void Fader::Start(unsigned char level, unsigned int _time)
{
	target = level;
	time = _time;
	if (time == 0)
		current = level;
}

bool Fader::IsEnd() const 
{
	if (time == 0) {
		return true;
	}
	else {
		return false;
	}
}

unsigned char Fader::GetLevel()const
{
	return static_cast<unsigned char>(current);
}

