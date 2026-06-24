#pragma once
#include "../Library/SceneBase.h"
#include <Dxlib.h>
#include "inputManager.h"
#include "commonFind.h"
#include "commonData.h"
#include "rgb.h"
#include "../Library/sceneManager.h"
#include "LoadData.h"
#include "selectCamera.h"
#include "light.h"
#include "soundManager.h"


//#include "fadeTransitor.h"
//#include "pushTransitor.h"
enum class SelectState {
	FadeIn,
	Select,
	FadeOut,
};
struct SelectData {
	VECTOR3 position;
	//VECTOR3 beforePos;
	Rgb color;
	
	bool operator==(const SelectData& _other) {
		return(position == _other.position && color == _other.color);
	}
};


/// <summary>
///ステージを選択するシーン
/// </summary>
class InputManager;
class CommonData;
class SoundManager;
class SelectCamera;
class BackGround;
class StageData;
class Production;
class LoadManager;
class SelectUi;

class SelectScene :public SceneBase {
public:
	SelectScene();
	~SelectScene();
	void Draw()override;
	void Update()override;
	
	//void MoveLerpStage(float movePos);
	//void MaxLarp(float& s,float e, float max);
private:

	void EaseUpdate();

	enum class SELECTSCENE_STATE
	{
		READY = 0,
		SELECT,
		PLAY_IN,
		END,
	};
	SELECTSCENE_STATE state;
	float stateWaitTime;

	/// <summary>
	/// プレイシーンからセレクトシーンに戻った時のステート
	/// </summary>
	enum class CLEARED_SELECT_STATE
	{
		NO_CLEAR = 0,
		CLEAR,
	};
	CLEARED_SELECT_STATE clearedState;

	StageData* stageData;
	std::vector<std::vector<float>> score;
	InputManager* input;
	CommonData* commonData;
	SelectCamera* camera;
	std::vector<SelectData> select;
	BackGround* backGround;
	void SetLerpData(VECTOR3 selectVelocity);
	void SetLerpData(const int& changeIndex);
	/// <summary>
	/// 背景とカメラの動く座標を決めるする
	/// </summary>
	/// <param name="changeIndex"> selectIndexの値 </param>
	/// <param name="set"> 現在の座標に代入:代入しない </param>
	void InitPosition(const int& changeIndex,bool set);
	int selectIndex;
	bool onSelect; // ステージの決定
	float selectLerp;
	VECTOR3 lerpVelocity;
	float inputWaitTime;
	bool isLerp;
	SoundManager* sound;
	VECTOR3 beforePos;
	//PushTransitor* pushTransitor;
	Production* pro;
	LoadManager* load;
	std::vector<SelectUi*> ui;
	int handle;
	struct BackGroundImageData
	{
		VECTOR2F position;
		VECTOR2F startPos;
		VECTOR2F endPos;
		float scale;
		int image;
	};
	BackGroundImageData backImage;

	float stageFontColor[3];

	int selectLevel;
	//デバック用の
	bool isDebug;

};