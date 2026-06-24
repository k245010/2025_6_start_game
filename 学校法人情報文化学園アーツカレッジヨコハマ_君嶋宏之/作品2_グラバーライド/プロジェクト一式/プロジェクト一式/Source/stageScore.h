#pragma once
#include "../Library/gameObject.h"
#include "starData.h"
#include "commonData.h"

class CommonData;
class GameManager;
class StageData;

class StageScore :public GameObject
{
public:
	StageScore(SceneBase* _scene);
	~StageScore();
	void Update()override;
	void Draw()override;

	//経過時間を入手する
	float GetGameTime()const { return gameTimer; }
	//取得コイン数を入手する
	int GetCoinNum()const { return hasCoinNum; }
	//受けたダメージ数を入手する
	int GetDamageNum()const { return damageNum; }
	//☆を取得できたかを入手する
	bool GetCanStar(StarType _starType)const { return canGetStar[_starType]; }

	float GetTargetTime()const { return starTarget.targetTime; }
	int GetTargetCoin()const { return starTarget.targetCoin; }

	///<summary>☆の取得条件を設定する</summary>
	/// <param name="_targetTime">目標タイム</param>
	/// <param name="_targetCoinNum">目標コイン取得数</param>
	/// <param name="_targetDamageNum">目標ダメージ数</param>
	void SetStarTarget(StarTarget _target);

	void SetCoinTarget(int _num);

	///<summary>コインの取得枚数を増加させる</summary>
	///<param name="_addNum">追加する枚数</param>
	void AddGetCoin(int _addNum = 1);
	///<summary>ダメージを受けた回数を増加させる</summary>
	///<param name="_addNum">増加させる数</param>
	void AddDamageNum(int _addNum = 1);

	//今の経過時間、コインの取得枚数、ダメージを受けた回数、スコアを全て0にリセットする
	void ResetNowState();

private:
	//スターを入手出来るかのフラグを更新する
	void UpdateCanGetStar();

	const float GAMETIME_MAX = 999.9f;//秒数のカンスト値

	int nowStage;//現在のステージ

	float gameTimer;//現在時間
	int score;//現在スコア
	int hasCoinNum;//現在コイン入手数
	int damageNum;//現在ダメージ数

	//float targetTime;//☆を取るための目標タイム
	//float targetCoinNum;//☆を取るための目標コイン枚数
	//float targetDamageNum;//☆を取るための目標ダメージ数

	//☆を取るための条件
	StarTarget starTarget;

	bool canGetStar[STAR_NUM];//リザルトで表示される☆を入手出来るかのフラグ

	bool doSaveCommonData;//commonDataにクリアした時の情報を送ったかどうか

	GameManager* gameManager;
	CommonData* commonData;
	StageData* stageData;
};