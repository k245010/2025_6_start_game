#pragma once
#include "uiSelectBase.h"
#include "loadManager.h"
#include <vector>

class EffectManager;

class PauseUi : public UiSelectBase
{
public:


	enum PauseState
	{
		Wait,
		Move,
		MoveWait,
		Menu,
		None,
	};

	PauseUi(SceneBase* _scene);
	~PauseUi();

	void Update() override;
	void Draw() override;

	void RateSelect();
	void ResetMove();

	void PushReset();
	void PushIndex();

	void FontScaling(float* _counter, bool* rate,float _up,float _down);

	void UIDraw(VECTOR2F _pos, VECTOR2F _size, float _angle, int _himage);

private:

	//int aloow;
	bool buttonMove;
	VECTOR2F keepButtonPos;
	float buttonCounter;
	float aloowDistance;
	int rateIndex;
	float rateCounter;
	PauseState state;
	GravityManager* gravityManager;
	EffectManager* effectManager;
	VECTOR2F moveInitPosition;
	float waitCounter;
	bool finalMove;
	float countNum;
	float pushCount;
	int pushRateIndex;
	bool pushRate;

	LoadManager* load;
	const int UI_NUM = 7;
	std::vector<int> image;
};