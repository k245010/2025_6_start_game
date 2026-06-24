#pragma once
#include "uiSelectBase.h"
#include "starData.h"
#include "stageScore.h"

class LoadManager;
class InputManager;

static const int RESULT_NUM = 4;

class ClearUi : public UiSelectBase
{
public:

	ClearUi(SceneBase* _scene);
	~ClearUi();

	void Update() override;
	void Draw() override;

	enum SterState
	{
		RATE,
		MOVE,
		FINISH,
	};

	void UIDraw(VECTOR2F _pos, VECTOR2F _size, float _angle, int _himage);
	void SlashDraw(VECTOR2F _pos,float _rate);

private:

	void StarMove();

	StageScore* score;
	VECTOR2F scoreUiPos[STAR_NUM];
	VECTOR2F scoreUiFontSize[STAR_NUM];
	bool getStar[STAR_NUM];

	int nowStage;
	float selectDistance;
	VECTOR2F goSelectTextPos;
	//VECTOR2F downStarPos[STAR_NUM];
	//float downStarDistance;

	LoadManager* loadManager;

	int starImage;
	int numberTextImage;
	int starAlpha[STAR_NUM];
	float starAngle[STAR_NUM];
	float starExrate[STAR_NUM];
	int starIndex;

	VECTOR2F starDrawPos[STAR_NUM];
	VECTOR2F firstDrawPos[STAR_NUM];
	SterState state;
	float moveTime[STAR_NUM];
	float selectAlpha;
	InputManager* input;

	//☆の獲得処理をスキップする
	void AllProductSkip();

	int resultImage[RESULT_NUM];
	bool sub;
	int loopNum[STAR_NUM];
	int beforeIndex;
	bool light[STAR_NUM];
	bool lightSub[STAR_NUM];
	bool allLight;
	SoundManager* sound;
	int speed;
	int counter;
	int numImage;
	VECTOR2F size;
	VECTOR2F start;
	int goSelectImage;
	bool productSkip;
};