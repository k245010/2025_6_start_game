#pragma once
#include "uiBase.h"
class Font;
class Movie;
class TitleUi :public UiBase 
{
public:
	TitleUi(SceneBase*_scene);
	~TitleUi();
	void Update()override;
	void Draw()override;
	void Init();
	void BackCircleInit();

	/// <summary>
	/// 演出が終わっているかどうか
	/// </summary>
	/// <returns></returns>
	bool GetTitleUiStateReady() const;
	/// <summary>
	/// タイトルの表示が終わっているか
	/// </summary>
	void GoTitleUiState() { titleUiState = TitleUiState::GO; itemSelectBlendValue = 255; };
	/// <summary>
	/// ボタンを押した後の演出が終わっているか
	/// </summary>
	/// <returns></returns>
	bool GetGoProEnd()const { return goProEnd; };

private:

	enum ItemSelectRate{
		BEGIN,
		READY,
		GOSCALE,
		GOMOVE,
		MAX,
	};

	enum class TitleUiState {
		BEGIN,
		READY,
		GO,
	};

	enum class BeginState {
		TITLE,
		ITEMSELECT,
	};
	Font* font;
	Movie* movie;

	enum class END_STATE
	{
		SMALL,
		BIG,
		END,
	};

	enum class TITLE_KIND
	{
		FONT = 0,
		BACK_CIRCLE,

		MAX,
	};

	struct TitleData
	{
		VECTOR2F position;
		int hImage;//タイトル用ハンドル
		float angle;
		float scale;
		float rate;
	};
	TitleData title[(int)TITLE_KIND::MAX];
	float maxScale;
	float minScale;
	float copyScale;
	bool initScale[2];


	int titleSelectImage;
	TitleUiState titleUiState;

	float beginProRate;
	
	VECTOR2F selectItemPosition;
	BeginState beginState;
	END_STATE endState;

	std::vector<float>itemSelectRate;

	int itemSelectBlendValue;
	VECTOR2F itemSelectScale;
	float itemSelectScaleF;
	float itemSelectWaitTime;


	bool goProEnd;
};