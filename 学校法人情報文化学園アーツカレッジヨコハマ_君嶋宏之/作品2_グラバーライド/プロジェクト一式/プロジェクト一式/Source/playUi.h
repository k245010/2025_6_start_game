#pragma once
#include "../Library/gameObject.h"
#include "stageScore.h"
#include "gameManager.h"


/// <summary>
/// ステージ数、ゲームプレイ中のタイム、コイン取得数を表示するクラス
/// </summary>
class SoundManager;
class CommonData;
class PlayUi :public GameObject
{
public:

	PlayUi(SceneBase* _scene);
	~PlayUi();
	void Update()override;
	void Draw()override;

private:
	
	StageScore* stageScore;
	GameManager* gameManager;
	SoundManager* sound;
	CommonData* commonData;

	/// <summary>
	/// UIの表示するステート
	/// </summary>
	enum class READY_STATE
	{
		STAY = 0,
		STAGE,		// ステージ数文字描画
		START,		// プレイ中
		END,
	};
	READY_STATE state;

	/// <summary>
	/// ステージ数文字の描画ステート
	/// </summary>
	enum class RATE_STATE
	{
		START = 0,	// 中心に向かう
		STOP,		// 止まる
		END,
	};

	/// <summary>
	/// ステージ文字の描画データ
	/// </summary>
	struct ReadyData
	{ 
		RATE_STATE state;
		VECTOR2F position;
		float rate;
		float scale;
		bool draw;
		int time;
		int image[2];
	};
	ReadyData stageFont;
	int randIndex;			// 乱数の保存	
	float stageFontColor[3];

	VECTOR2F coinTextPos;
	int coinFontImage;
	int coinImage;
	int crossImage;
	bool allCoinGetSound;	// 全てのコインを取得した時の音を鳴らしたかどうか
	int waitTime;			// 全取得の音を鳴らすまでの時間

	VECTOR2F timeTextPos;
	int timeFontImage;
	
	/// <summary>
	/// コインの裏の光の描画データ
	/// </summary>
	struct CoinMaxData
	{
		VECTOR2F position;
		bool onMax;
		float angle;
		float scale;
		float time;
		int image;
	};
	CoinMaxData coinMaxData;

	bool onDraw; // UIの描画フラグ
};	