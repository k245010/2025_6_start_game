#pragma once
#include "../../../Library/SceneBase.h"

/// <summary>
/// タイトルシーン
/// 
/// タイトルを表示して、キーを押したらプレイシーンに移行する。
/// </summary>
class Input;
class Fade;
class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene();
	void Update() override;
	void Draw() override;
private:

	/// <summary>
	///	プレイシーンを開始する
	/// </summary>
	void PlaySceneStart();

	bool isSceneChange = false;	// シーン変更フラグ

	enum UI_KIND
	{
		IMAGE_TITLE_LOGO,
		IMAGE_A_KEY,
		IMAGE_D_KEY,

		IMAGE_MAX
	};

	int uiImage[IMAGE_MAX];

	Input* input;
	Fade* fade;

	bool isFirstFrame = true;

	int lastStageNumber;						// 前回のステージナンバーを記録
	float selectKeyUIScaleChangeTime = 0.0f;	// ステージ選択キーUIの大きさの変化時間
	bool isPressedAKey = false;					// Akeyが押されたかどうか
	bool isPressedDKey = false;					// Dkeyが押されたかどうか
};
