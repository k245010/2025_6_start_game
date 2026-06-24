#pragma once
#include "../../../Library/gameObject.h"

class Fade : public GameObject
{
public:

	Fade();
	~Fade();

	void Update()	override;
	

	/// <summary>
	///					フェード画面の色を設定する
	/// </summary>
	/// <param name="r">赤				</param>
	/// <param name="g">緑				</param>
	/// <param name="b">青				</param>
	void ChangeColor(unsigned char r, unsigned char g, unsigned char b);

	/// <summary>
	///						フェードインの開始
	/// </summary>
	/// <param name="_time">フェードイン時間	</param>
	void FadeIn(int _time) { Start(0, _time); }

	/// <summary>
	///						フェードアウトの開始
	/// </summary>
	/// <param name="_time">フェードアウト時間	</param>
	void FadeOut(int _time) { Start(255, _time); }

	/// <summary>
	///				フェードが終わったかどうか
	/// </summary>
	/// <returns>	true:フェードが終わった / false:フェードは終わていない	</returns>
	bool IsEnd();

	/// <summary>
	///				現在のフェードレベルを返す
	/// </summary>
	/// <returns>	現在のフェードレベル	</returns>
	unsigned char GetLevel();

private:
	
	/// <summary>
	/// フェードの描画
	/// </summary>
	void FadeDraw();

	/// <summary>
	///							画面の色を変化させる
	/// </summary>
	/// <param name="_level">	初めのアルファ値	</param>
	/// <param name="_time">	時間				</param>
	void Start(unsigned char _level, unsigned int _time);

	float current;
	unsigned int time;
	float target;
	unsigned char color;

	int fadeDrawFuncitionIDNumber = -1;	//  FadeDraw関数に対するIDナンバー
};