#pragma once
#include "../Library/gameObject.h"

class Fader : public GameObject {
public:
	Fader(SceneBase* _scene);
	~Fader(){}
	void Update() override;
	void Draw() override;

	/// <summary>
	/// フェードする色を変更する（各色0～255）
	/// </summary>
	/// <param name="r">赤</param>
	/// <param name="g">緑</param>
	/// <param name="b">青</param>
	void ChangeColor(unsigned char r, unsigned char g, unsigned char b);

	/// <summary>
	/// フェードを開始する
	/// </summary>
	/// <param name="level">フェードレベル(0～255)</param>
	/// <param name="time">フェード時間（フレーム）</param>
	void Start(unsigned char level, unsigned int time);

	/// <summary>
	/// フェードインする
	/// </summary>
	/// <param name="time">フェード時間（フレーム）</param>
	void FadeIn(int time) { Start(0,time); }

	/// <summary>
	/// フェードアウトする
	/// </summary>
	/// <param name="time">フェード時間（フレーム）</param>
	void FadeOut(int time) { Start(255, time); }

	/// <summary>
	/// フェードが終わっているか
	/// </summary>
	/// <returns>終わっていればtrue</returns>
	bool IsEnd() const;

	/// <summary>
	/// フェードレベルを取得する
	/// </summary>
	/// <returns>フェードレベル</returns>
	unsigned char GetLevel() const;
private:
	float current;
	unsigned int time;
	float target;
	unsigned int color;
};