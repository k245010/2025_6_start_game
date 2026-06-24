#pragma once
#include "uiSelectBase.h"
#include "starData.h"
#include <vector>

class StageScore;
class SelectUi : public UiSelectBase
{
public:

	SelectUi(SceneBase* _scene);
	~SelectUi();

	/// <summary>
	/// ステージの写真とフレーム画像の初期化
	/// </summary>
	/// <param name="_index"> ステージIndex </param>
	/// <param name="_frameImage"> フレームハンドル </param>
	void Init(const int& _index,const int& _frameImage);

	void Update() override;

	void Draw(const VECTOR& pos,const int& _selectIndex);

	/// <summary>
	/// 描画するステージで取得した星の数をセットする
	/// </summary>
	/// <param name="_num"> 星の数 </param>
	/// <param name="_index"> 星の種類index </param>
	void SetStarNum(const int& _num,const int& _index) { data.starNum[_index] = _num; }

	/// <summary>
	/// 描画するクリアしたベストタイムをセットする
	/// </summary>
	/// <param name="_time"> タイム </param>
	void SetBestTime(const float& _time) { data.bestTime = _time; }
	
	/// <summary>
	/// 描画する取得したコイン数をセットする
	/// </summary>
	/// <param name="_num"> コインの数 </param>
	void SetCoinNum(const int& _num) { data.coinNum = _num; }

	/// <summary>
	/// ステージで取得できるコインの最大数をセットする
	/// </summary>
	/// <param name="_num"> コインの最大数 </param>
	void SetMaxCoinNum(const int& _num) { coinMax = _num; }

	/// <summary>
	/// プレイヤーがステージをクリアして、セレクトシーンに戻ったかセットする
	/// </summary>
	void Clear() { clear = true; }

	/// <summary>
	/// ステージ文字の色をセットする
	/// </summary>
	/// <param name="rgb"> Rgb変数 </param>
	void SetStageFontColor(Rgb rgb) { stage.color = rgb; }

private:

	struct ScoreData
	{
		int starNum[STAR_NUM];
		int coinNum;
		float bestTime;
	};
	ScoreData data;

	struct BackData
	{
		float copyScale;
		float scale;
		float angle;
		int hImage;
	};
	BackData back;

	struct PictureData
	{
		float scale;
		int image;
		int frameImage;
	};
	PictureData pic;

	struct StageFontData
	{
		VECTOR2F position;
		float scale;
		float time;
		Rgb color;
		int image;
	};
	StageFontData stage;
	int stageFontImage;

	bool selecting;
	int selectIndex;
	int meIndex;
	float rate;

	int coinMax;

	int scoreFontImage[STAR_NUM];
	int scoreImage[STAR_NUM];

	bool clear;
};