#pragma once
#include "../Library/gameObject.h"
#include "LoadData.h"
#include "player.h"
#include "uiBase.h"

static const int UI_GRAVITY_GAUGE_ARROW_MAX = 8;
static const int UI_HP_MAX = PLAYER_HP_MAX;
static const int UI_HP_IMAGE_MAX = PLAYER_HP_MAX + 1;//瀕死の時用のテクスチャのために+1


class GameManager;
class SoundManager;
class EffectManager;
class PlayerUi : public UiBase
{
public:
	PlayerUi(SceneBase* _scene);
	~PlayerUi();
	void Update()override;
	void Draw()override;

	void SetGameManagerState(const GameManager::GAME_STATE& state) { gameManagerState = state; }

	/// <summary>
	/// 主にHPのUi変数のリセット
	/// </summary>
	void ResetUi();

	void PlayerGravityStateInit(GravityState& _state);
	void PlayerMode(Player::PlayerMode& _state) { playerModeState = &_state; }

	void PlayerGaugeAdd(float _add);
	void PlayerGaugeUse(float _sub);

	float GetGravityMoveGaugeNum() { return gravityMoveGaugeNum; }

	void SetHp(int _hp);
	void SetNoGravityZone(bool set) { noGravity = set; }
	void GravityGaugeMax();

	void PlayerUiShake();
	void SetDrawArrowAngle();
	void AloowDraw(VECTOR _screenPosition, int animeNum);

	void InGoal() { inGoal = true; }

private:

	void GravityGaugeUiOffset();
	enum class WARNING_GRAVITY_VALUE_KIND
	{
		VISION = 0,
		GAUGE,
	};
	/// <summary>
	/// 重力値に対し警告するかどうか
	/// </summary>
	/// <param name="_kind"> 重力値によって警告されるもの </param>
	/// <returns> 警告する:警告しない </returns>
	bool IsWaringGravityValue(WARNING_GRAVITY_VALUE_KIND _kind);

	void GravityMoveGaugeNumInit();

	enum class VISION_KIND
	{
		GRAVITY_VALUE = 0,	//重力低下
		HP_VALUE,			//HP低下

		MAX,
	};
	/// <summary>
	/// 引数のvalueにfinishValueまでrateを加算する
	/// </summary>
	/// <param name="value"> _______加算される値 </param>
	/// <param name="rate"> ________加算値 </param>
	/// <param name="finishValue"> _最大値 </param>
	/// <param name="kind"> ________visionの種類 </param>
	/// <returns> 値が達した:値が達してない </returns>
	bool VisionValueInfo(float& value,const float rate,const float finishValue, int _kind);

	VECTOR2F GetDrawPosition(const VECTOR2F& vec);
	bool LigthingGravityArrow(const float& _angle);
	bool LigthingGravityArrow(const float& _angle,int i,int j);

	/// <summary>
	/// HPの加算合成変数をリセット
	/// </summary>
	void ResetHpDataBlendAdd();

	/// <summary>
	/// HPが1の時のステート
	/// </summary>
	enum class HP_ONE 
	{
		NORMAL = 0,
		ONE,		
	};
	/// <summary>
	/// 描画のHPが1の時の処理
	/// </summary>
	/// <param name="one"> HPが1:HPが1でない </param>
	void SetHpOneInfo(bool one);

	enum class HP_CHANGING_STATE
	{
		NORMAL = 0,
		ADD,
		SUB,
	};
	/// <summary>
	/// 描画のHPの増減
	/// </summary>
	/// <param name="state"> 増やすのか、減らすのか指定 </param>
	void SetHpChangeInfo(const HP_CHANGING_STATE& _state, int index);

	/// <summary>
	/// 描画HPのステートをセット
	/// </summary>
	void SetHpChangeState(int index);

	bool inGoal;//playerがゴールしたか

	float gravityMoveGaugeNum;//重力ゲージの値
	bool gravityMoveGaugeNumMax;
	bool gravityMoveGaugeNumInfo;
	int gravityMoveGaugeNumCount;
	int gravityMoveGaugeImage;
	float gravityGaugeSub;

	VECTOR2F uiPosition;

	VECTOR2F uiGlobalPosition;

	struct VisionData
	{
		bool onVision;
		float time;
		float alpha;
		int hImage;
	};
	VisionData vision[(int)VISION_KIND::MAX];

	enum class HP_CHANGE_STATE
	{
		CHANGE = 0,
		SET
	};

	struct HpDrawData
	{
		//VECTOR2F position;
		float scale;
		float count;
		float alpha;
		bool onAlpha;
		float add;
		float addScale;
		int addLevel;
		bool onAddDraw; // 加算合成
		bool onDraw;
		bool onUp;
		bool onDown;
		
		HP_CHANGING_STATE changingState;
	};
	struct HpData
	{
		HpDrawData drawData[UI_HP_MAX] ;
		VECTOR2F basePosition;
		int num;
		bool oneHp;
		int backImage[UI_HP_MAX];//後ろの枠
		int image[UI_HP_IMAGE_MAX];

		HP_CHANGE_STATE changeState;
		HP_ONE oneHpState;
	};
	HpData hp;
	int copyHpNum;
	bool onChangeHpNum; // hpの数字を変更するか

	enum class GRAVITY_GAUGE
	{
		BASE = 0,
		BASE_IN,
		CENTER,
		GAUGE,

		MAX,
	};
	enum class GRAVITY_GAUGE_ARROW
	{
		ARROW_SMALL = 0,
		ARROW_BIG,

		MAX,
	};
	struct GravityGaugeData
	{
		VECTOR2F position;
		VECTOR2F size;
		Rgb brightColor;
		int hImage;
		float time;
	};
	GravityGaugeData gGaugeUiData[(int)GRAVITY_GAUGE::MAX];
	struct GravityGaugeArrowData
	{
		VECTOR2F position[UI_GRAVITY_GAUGE_ARROW_MAX / 2];
		VECTOR2F size;
		float angle[UI_GRAVITY_GAUGE_ARROW_MAX / 2];
		int hImage;

		float test[UI_GRAVITY_GAUGE_ARROW_MAX / 2];
	};
	GravityGaugeArrowData gArrowUiData[(int)GRAVITY_GAUGE_ARROW::MAX];

	Rgb arrowUiColorNormal;
	Rgb arrowUiColorLight;

	VECTOR2F gGaugeUiGlobalPosition;


	float drawRateScaleInit;
	float drawRateScale;
	float arrowRate;
	int light;
	float time;

	bool gGaugeChange[(int)GRAVITY_GAUGE::MAX];
	int gGaugeChangeNum[(int)GRAVITY_GAUGE::MAX];
	int gGaugeChangeNumCopy[(int)GRAVITY_GAUGE::MAX];
	
	bool gravityUse;
	int useCounter;
	SoundManager* sound;
	EffectManager* effectManager;
	GameManager* gameManager;

	GravityState* playerGravityState;
	Player::PlayerMode* playerModeState;

	VECTOR2F test;

	float allowAngle;
	int allowImage;
	VECTOR allowPos;

	float setAngle[8];

	float testNum[100];
	int animeCounter;

	bool noGravity;
	GameManager::GAME_STATE gameManagerState;
};
