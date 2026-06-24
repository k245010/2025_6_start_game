#pragma once
#include "../Library/sceneBase.h"
#include "titleUi.h"
#include "inputManager.h"
/// <summary>
/// タイトルシーン
/// 
/// タイトルを表示して、キーを押したらプレイシーンに移行する。
/// </summary>
class Movie;
class Production;
class SoundManager;
class LoadManager;
class TitleScene : public SceneBase {
public:
	TitleScene();
	~TitleScene();
	void Update() override;
	void Draw() override;
	TitleUi* titleUi;
	InputManager* input;
	Movie* movie;
	Production* pro;
	SoundManager* sound;
	LoadManager* load;
	bool onPro;
};
