#pragma once
#include "../../../Library/myDxLib.h"
#include "../../../nlohmann/json.hpp"

namespace ColorFunction
{
	/// <summary>
	///								正規化されたいるfloat値のカラーからbyteカラーへ変換する
	/// </summary>
	/// <param name="_byteRed">		byteカラー 赤		</param>
	/// <param name="_byteGreen">	byteカラー 緑		</param>
	/// <param name="_byteBlue">	byteカラー 青		</param>
	/// <param name="_floatRed">	float値のカラー 赤	</param>
	/// <param name="_floatGreen">	float値のカラー	緑	</param>
	/// <param name="_floatBlue">	float値のカラー	青	</param>
	inline void NormalizeColorToByteColor
	(
		unsigned char& _byteRed,
		unsigned char& _byteGreen,
		unsigned char& _byteBlue,
		const float& _floatRed,
		const float& _floatGreen,
		const float& _floatBlue
	)
	{
		// 正規化されたいるカラーをbyteカラーの255に変換 //
		_byteRed	= unsigned char(_floatRed * 255);
		_byteGreen	= unsigned char(_floatGreen * 255);
		_byteBlue	= unsigned char(_floatBlue * 255);
	}

	/// <summary>
	///								byteカラーから正規化されたいるfloat値のカラーへ変換する
	/// </summary>
	/// <param name="_floatRed">	float値のカラー 赤	</param>
	/// <param name="_floatGreen">	float値のカラー	緑	</param>
	/// <param name="_floatBlue">	float値のカラー	青	</param>
	/// <param name="_byteRed">		byteカラー 赤		</param>
	/// <param name="_byteGreen">	byteカラー 緑		</param>
	/// <param name="_byteBlue">	byteカラー 青		</param>
	inline void ByteColorToNormalizeColor
	(
		float& _floatRed,
		float& _floatGreen,
		float& _floatBlue,
		const unsigned char _byteRed,
		const unsigned char _byteGreen,
		const unsigned char _byteBlue
	)
	{
		// byteカラーの255を正規化されたいるカラーへ変換 //
		_floatRed = float(_byteRed / 255.0f);
		_floatGreen = float(_byteGreen / 255.0f);
		_floatBlue = float(_byteBlue / 255.0f);
	}
}

//struct COLOR_F3
//{
//	float r, g, b;
//
//	COLOR_F3(const float& _red, const float& _green, const float& _blue) : r(_red), g(_green), b(_blue) {}
//	COLOR_F3() : r(1.0f), g(1.0f), b(1.0f) {}
//};

struct COLOR_F4 : public COLOR_F
{
	COLOR_F4(const float& _red, const float& _green, const float& _blue,const float& _alpha = 1.0f)
	{
		r = _red;
		g = _green;
		b = _blue;
		a = _alpha;
	}

	COLOR_F4()
	{
		r = 1.0f;
		g = 1.0f;
		b = 1.0f;
		a = 1.0f;
	}
};

class Color
{
public:

	// byteカラー 0~255 //
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;

	//COLOR_F3 floatColor3;	// 正規化されたいるfloat値のカラー 0.0f~1.0f	
	COLOR_F4 floatColor4;	// 正規化されたいるfloat値のカラー 0.0f~1.0f	アルファを含む

	/// <summary>
	///							byteカラーで色を初期化する
	/// </summary>
	/// <param name="_red">		byteカラー 赤	</param>
	/// <param name="_green">	byteカラー 緑	</param>
	/// <param name="_blue">	byteカラー 青	</param>
	Color(const unsigned char _red, const unsigned char _green, const unsigned char _blue);

	/// <summary>
	///							float値で色を初期化する
	/// </summary>
	/// <param name="_red">		float値のカラー 赤			</param>
	/// <param name="_green">	float値のカラー	緑			</param>
	/// <param name="_blue">	float値のカラー	青			</param>
	/// <param name="_alpha">	float値のカラー アルファ	</param>
	Color(const float& _red, const float& _green, const float& _blue,const float& _alpha = 1.0f) : floatColor4(_red,_green,_blue,_alpha) 
	{
		// byteRGBの設定
		ColorFunction::NormalizeColorToByteColor(r, g, b, _red, _green, _blue);
	}

	~Color();

	/// <summary>
	///				COLOR_F4の値を返す
	/// </summary>
	/// <returns>	COLOR_F4の値	</returns>
	const COLOR_F4& GetColorF4() const { return floatColor4; }

private:
	 
};

namespace Colors
{
	static const Color BLACK	= { 0.0f, 0.0f, 0.0f };
	static const Color RED		= { 1.0f, 0.0f, 0.0f };
	static const Color GREEN	= { 0.0f, 1.0f, 0.0f };
	static const Color BLUE		= { 1.0f, 0.0f, 1.0f };
	static const Color WHITE	= { 1.0f, 1.0f, 1.0f };
}

/// <summary>
///								Colorをjsonへ変換する
/// </summary>
/// <param name="_json">		json参照					</param>
/// <param name="_color">		Color						</param>
inline void to_json(nlohmann::json& _json, const Color& _color)
{
	_json = nlohmann::json
	{
		{"Red",					_color.r				},
		{"Green",				_color.g				},
		{"Blue",				_color.b				},
		{"FloatColor4Red",		_color.floatColor4.r	},
		{"FloatColor4Green",	_color.floatColor4.g	},
		{"FloatColor4Blue",		_color.floatColor4.b	},
		{"FloatColor4Alpha",	_color.floatColor4.a	},
	};
}

/// <summary>
///							jsonからColorへ変換する
/// </summary>
/// <param name="_json">	json						</param>
/// <param name="_color">	Color参照					</param>
inline void from_json(const nlohmann::json& _json, Color& _color)
{
	_json.at("Red").get_to(_color.r);
	_json.at("Green").get_to(_color.g);
	_json.at("Blue").get_to(_color.b);
	_json.at("FloatColor4Red").get_to(_color.floatColor4.r);
	_json.at("FloatColor4Green").get_to(_color.floatColor4.g);
	_json.at("FloatColor4Blue").get_to(_color.floatColor4.b);
	_json.at("FloatColor4Alpha").get_to(_color.floatColor4.a);
}
