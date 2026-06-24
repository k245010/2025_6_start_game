#pragma once
#include <DxLib.h>
/// <summary>
/// 色を作成するクラス
/// </summary>
class Rgb
{
public:
	int r, g, b;
	Rgb():r(0),g(0),b(0){}
	Rgb(int s) :r(s), g(s), b(s) {};
	Rgb(int _r, int _g, int _b) :r(_r), g(_g), b(_b) {};
	Rgb(const Rgb& c) :r(c.r), g(c.g), b(c.b) {};
	/// <summary>
	/// COLOR_F型に変換してreturn
	/// </summary>
	operator COLOR_F() const;
	/// <summary>
	/// COLOR_F型に変換してreturn、alpha値変更可
	/// </summary>
	COLOR_F ColorF(const float& _parm) const;
	operator int() const;
	void SetColor(int _r, int _b, int _g);
	
	
private:
	
	
};
const COLOR_F GetRgbToColorF(const int& _r, const int& _b, const int& _g, const float& _parm);
static const Rgb RGB_WHITE = Rgb(255, 255, 255);
static const Rgb RGB_RED = Rgb(255, 0, 0);
static const Rgb RGB_GREEN = Rgb(0, 255, 0);
static const Rgb RGB_BLUE = Rgb(0, 0, 255);
static const Rgb RGB_YELLOW = Rgb(255, 255, 0);
static const Rgb RGB_MAGENTA = Rgb(255, 0, 255);
static const Rgb RGB_CYAN = Rgb(0, 255, 255);
static const Rgb RGB_BLACK = Rgb(0, 0, 0);
static const Rgb RGB_SAKURA = Rgb(253, 238, 239);