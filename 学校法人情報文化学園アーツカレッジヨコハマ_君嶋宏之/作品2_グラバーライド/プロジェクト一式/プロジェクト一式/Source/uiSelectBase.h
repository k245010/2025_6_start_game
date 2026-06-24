#pragma once
#include "uiBase.h"
#include "rgb.h"
#include "gameManager.h"
#include "config.h" 
#include "font.h"
#include <vector>

class GameManager;
class Font;
class UiSelectBase : public UiBase
{
public:

	UiSelectBase(SceneBase* _scene);
	~UiSelectBase();

	virtual void Update() override {};
	virtual void Draw() override {};

protected:

	GameManager* gameManager;

	
	VECTOR2F backPos;
	VECTOR2F backSize;
	Rgb backColor;

	VECTOR2F titlePos;
	VECTOR2F titleSize;
	VECTOR2F titleFontSize;
	Rgb titleColor;

	struct SelectData
	{
		VECTOR2F position;
		VECTOR2F size;
		VECTOR2F fontSize;
		Rgb color;
	};
	std::vector<SelectData> select;

	struct ButtonData
	{
		VECTOR2F position;
		VECTOR2F fontSize;
	};
	std::vector<ButtonData> button;

	float distance;
	VECTOR2F subDrawFontPos;

	Font* font;
};