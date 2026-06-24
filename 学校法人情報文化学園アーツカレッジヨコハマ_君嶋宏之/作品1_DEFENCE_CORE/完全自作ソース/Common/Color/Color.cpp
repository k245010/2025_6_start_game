#include "Color.h"

Color::Color(const unsigned char _red, const unsigned char _green, const unsigned char _blue)
{
	// float’l‚ÌƒJƒ‰[İ’è
	ColorFunction::ByteColorToNormalizeColor(floatColor4.r, floatColor4.g, floatColor4.b, _red, _green, _blue);
	floatColor4.a = 1.0f;

	r = _red;
	g = _green;
	b = _blue;
}

Color::~Color()
{
}
