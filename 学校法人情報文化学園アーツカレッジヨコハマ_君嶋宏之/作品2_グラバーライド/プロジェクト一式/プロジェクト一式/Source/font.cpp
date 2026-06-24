#include "font.h"
#include <string>
#include <assert.h>

Font::Font(SceneBase* _scene) : GameObject(_scene)
{
	for (int i = 0;i < (int)FONT_KIND::MAX;i++)
		fontHandle[i] = -1;

	AddFontResourceEx("data\\fonts\\UDDigiKyokashoN-B.ttc", FR_PRIVATE, NULL);
	AddFontResourceEx("data\\fonts\\UDDigiKyokashoN-R.ttc", FR_PRIVATE, NULL);
	AddFontResourceEx("data\\fonts\\ikamodoki1.ttf", FR_PRIVATE, NULL);
	AddFontResourceEx("data\\fonts\\HGRPP1.TTC", FR_PRIVATE, NULL);
	AddFontResourceEx("data\\fonts\\Corporate-Logo-Medium-ver3.otf", FR_PRIVATE, NULL);

	fontHandle[0] = CreateFontToHandle("UD デジタル 教科書体 N", 32, 0, -1);
	fontHandle[1] = CreateFontToHandle("イカモドキ", 32, 0, -1);
	fontHandle[2] = CreateFontToHandle("HG創英角ﾎﾟｯﾌﾟ体", 32, 0, -1);
	fontHandle[(int)FONT_KIND::CORPORATE] = CreateFontToHandle("Corporate-Logo-Medium-ver3", 32, 0, -1);

	for (int i = 0;i < (int)FONT_KIND::MAX;i++)
		assert(fontHandle[i] >= 0);
}

Font::~Font()
{
	for (int i = 0;i < (int)FONT_KIND::MAX;i++)
	{
		if (fontHandle[i] >= 0)
			DeleteFontToHandle(fontHandle[i]);
	}
}

void Font::Update()
{
}

void Font::Draw()
{
}

void Font::FontDraw(VECTOR2F _pos, VECTOR2F _size, float _angle, FONT_KIND _kind, Rgb _rgb, const char* _string, VECTOR2F _cenPos)
{
	DrawRotaFormatStringFToHandle(_pos.x, _pos.y, _size.x, _size.y, _cenPos.x, _cenPos.y, _angle, _rgb, GetFontHandle(_kind), 0xffffff, false, _string);
	//DrawFormatString(100, 100, 0xff0000, "あああ");
}

void Font::FontDraw(VECTOR2F _pos, VECTOR2F _size, float _angle, FONT_KIND _kind, Rgb _rgb, const char* _string)
{
	FontDraw(_pos, _size, _angle, _kind, _rgb, _string,V2Get(0.0f,0.0f));
}

//void Font::CreateFontString(VECTOR2F _pos, VECTOR2F _size, VECTOR2F _cenPos, float _angle, FONT_KIND _kind, const char* _string)
//{
//	
//}






int Font::GetFontHandle(FONT_KIND _kind)
{
	int handle = fontHandle[(int)_kind];
	assert(handle >= 0);
	return handle;
}
