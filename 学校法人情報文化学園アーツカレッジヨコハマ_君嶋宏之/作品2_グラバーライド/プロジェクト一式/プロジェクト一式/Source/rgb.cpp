#include "rgb.h"


Rgb::operator COLOR_F() const
{
	return GetColorF(static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b) / 255, 1.0f);
}

COLOR_F Rgb::ColorF(const float& _parm) const
{
	return GetColorF(static_cast<float>(r) / 255, static_cast<float>(g) / 255, static_cast<float>(b) / 255, _parm);
}



Rgb::operator int() const
{
	return GetColor(r, g, b);
}

void Rgb::SetColor(int _r, int _b, int _g)
{
	r = _r;
	g = _g;
	b = _b;
}



const COLOR_F  GetRgbToColorF(const int& _r, const int& _b, const int& _g, const float& _parm)
{
	return GetColorF(static_cast<float>(_r) / 255, static_cast<float>(_g) / 255, static_cast<float>(_b) / 255, _parm);
}
