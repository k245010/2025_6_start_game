#include "Fade.h"
#include "../UI/UIDrawManager/UIDrawManager.h"
#include "../../Screen.h"

Fade::Fade()
{
	current = 0.0f;
	time	= 0;
	target	= 0.0f;
	color	= GetColor(0, 0, 0);

	DontDestroyOnSceneChange(true);

	// FadeDrawŠÖ”‚Ì“o˜^
	fadeDrawFuncitionIDNumber = UIDrawManager::GetUIDrawManagerInstance()->PushUIDrawFunction([this] { FadeDraw(); });
	UIDrawManager::GetUIDrawManagerInstance()->SetDrawOrder(fadeDrawFuncitionIDNumber, -99999);
}

Fade::~Fade()
{
	// FadeDrawŠÖ”‚Ì‰ð•ú
	UIDrawManager::GetUIDrawManagerInstance()->PopUIDrawFunction(fadeDrawFuncitionIDNumber);
}

void Fade::Update()
{
	if (time > 0)
	{
		current += (target - current) / time;
		time--;
	}
}

void Fade::FadeDraw()
{
	if (current > 0.0f)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)current);
		DrawBox(0, 0, Screen::WIDTH, Screen::HEIGHT, color, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

void Fade::ChangeColor(unsigned char r, unsigned char g, unsigned char b)
{
	color = GetColor(r, g, b);
}

void Fade::Start(unsigned char _level, unsigned int _time)
{
	target	= _level;
	time	= _time;

	if (time == 0)
		current = _level;
}

bool Fade::IsEnd()
{
	return (time == 0);
}

unsigned char Fade::GetLevel()
{
	return (unsigned char)current;
}