#pragma once
#include "../../../../Library/GameObject.h"

class Fog : public GameObject
{
public:

	Fog();
	~Fog();

	void Update() override;

	/// <summary>
	///							フォグの色を設定
	/// </summary>
	/// <param name="_red">		赤				</param>
	/// <param name="_green">	緑				</param>
	/// <param name="_bule">	青				</param>
	void SetColor(unsigned char _red, unsigned char _green, unsigned char _blue);

	/// <summary>
	///							フォグの画面からの描画開始と終了距離を設定
	/// </summary>
	/// <param name="_start">	開始距離				</param>
	/// <param name="_end">		終了距離　				</param>
	void SetDrawStartAndEnd(const float& _start, const float& _end);

private:

	/// <summary>
	/// float配列の0.0~1.0fの値を0~255の値に変換して、フォグの色の設定をする
	/// </summary>
	void SetNormalizeColorToByteColor();

	/// <summary>
	/// フォグの描画データ
	/// </summary>
	struct DrawData
	{
		float start;			// フォグの開始距離
		float end;				// フォグの終了距離
		float density;			// フォグの濃さ
		bool isDistanceDraw;	// 距離でのフォグ描画をするかどうか

		DrawData() : start(0.0f), end(1.0f),density(0.00005f),isDistanceDraw(false) {}

		/// <summary>
		///									コンストラクタ
		/// </summary>
		/// <param name="_start">			フォグの開始距離					</param>
		/// <param name="_end">				フォグの終了距離					</param>
		/// <param name="_density">			フォグの濃さ						</param>
		/// <param name="_isDistanceDraw">	距離でのフォグ描画をするかどうか	</param>
		DrawData(const float& _start,const float& _end,const float& _density,bool _isDistanceDraw)
			: start(0.0f), end(1.0f), density(_density), isDistanceDraw(_isDistanceDraw){}
	};

	static const int COLOR_MAX = 3;		// 色の最大メンバ数
	float color[COLOR_MAX];				// フォグの色

	DrawData drawData;			// フォグの描画データ
};
