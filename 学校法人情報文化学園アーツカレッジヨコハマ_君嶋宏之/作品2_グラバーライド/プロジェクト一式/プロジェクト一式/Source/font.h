#pragma once
#include "../Library/gameObject.h"
#include "rgb.h"

enum class FONT_KIND
{
	NORMAL,
	IKA,
	POP,
	CORPORATE,

	MAX,
};

class Font : public GameObject
{
public:

	Font(SceneBase* _scene);
	~Font();

	void Update() override;
	void Draw() override;

	/// <summary>
	/// 文字を描画
	/// </summary>
	/// <param name="_pos">		座標	</param>
	/// <param name="_size">	サイズ (デフォルト 1.0f,1.0f)	</param>
	/// <param name="_cenPos">	文字の描画開始位置 (基本的に0)</param>
	/// <param name="_angle">	回転値	</param>
	/// <param name="_kind">	フォントの種類</param>
	/// <param name="_rgb">		色		</param>
	/// <param name="_string">	文字	</param>
	void FontDraw(VECTOR2F _pos, VECTOR2F _size, float _angle, FONT_KIND _kind, Rgb _rgb, const char* _string, VECTOR2F _cenPos);

	///// <summary>
	///// 文字を描画
	///// </summary>
	///// <param name="_pos">		座標	</param>
	///// <param name="_size">	サイズ	(デフォルト 1.0f,1.0f)</param>
	///// <param name="_angle">	回転値	</param>
	///// <param name="_kind">	フォントの種類</param>
	///// <param name="_rgb">		色		</param>
	///// <param name="_string">	文字	</param>
	void FontDraw(VECTOR2F _pos, VECTOR2F _size,float _angle, FONT_KIND _kind, Rgb _rgb, const char* _string);

	//void CreateFontString(VECTOR2F _pos, VECTOR2F _size, VECTOR2F _cenPos, float _angle, FONT_KIND _kind,const char* _string);
	//void CreateFontChar();


	/// <summary>
	/// CreateFontHandleで作成されたフォントを使って画面に文字を表示する関数
	/// drawformatと同様に可変長引数に対応
	/// </summary>
	template<typename... Args>
	void FontDraw(VECTOR2F _pos,FONT_KIND _kind, Rgb _rgb, const char* _string, Args&&... args);
	/// <summary>
	/// フォントのサイズ拡張付き
	/// </summary>
	template<typename... Args>
	void FontDraw(VECTOR2F _pos, VECTOR2F _size, FONT_KIND _kind, Rgb _rgb, const char*_string, Args&&... args);
	/// <summary>
	/// フォントのサイズ拡張と角度変更付き
	/// </summary>
	template<typename... Args>
	void FontDraw(VECTOR2F _pos, VECTOR2F _size, float _angle, FONT_KIND _kind, Rgb _rgb, const char* _string, Args&&... args);
	

private:

	int GetFontHandle(FONT_KIND _kind);

	const char* string;

	int fontHandle[(int)FONT_KIND::MAX];
};

template<typename ...Args>
inline void Font::FontDraw(VECTOR2F _pos,FONT_KIND _kind, Rgb _rgb, const char* _string, Args && ...args)
{
	DrawFormatStringFToHandle(_pos.x, _pos.y, _rgb, GetFontHandle(_kind), _string, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Font::FontDraw(VECTOR2F _pos, VECTOR2F _size, FONT_KIND _kind, Rgb _rgb, const char* _string, Args && ...args)
{
	DrawRotaFormatStringFToHandle(_pos.x, _pos.y, _size.x, _size.y,0.0f,0.0f,0.0f, _rgb, GetFontHandle(_kind), 0xffffff, false, _string, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Font::FontDraw(VECTOR2F _pos, VECTOR2F _size, float _angle, FONT_KIND _kind, Rgb _rgb, const char* _string, Args && ...args)
{
	DrawRotaFormatStringFToHandle(_pos.x, _pos.y, _size.x, _size.y, 0.0f, 0.0f, _angle,_rgb, GetFontHandle(_kind), 0xffffff, false, _string, std::forward<Args>(args)...);
}
