#pragma once
#include "../../Library/GameObject.h"
#include "../Stage/StageObjectData.h"
#include "../Transform/Transform.h"
#include <unordered_map>

// ゲーム関連マクロ
#define GAME_FIRST_PERSON (0) // 一人称ありのゲーム

class Input;
class Camera;
class NavigationManager;
class StageObjectController;
class StageManager;
class WaveController;
class EnemyManager;
class Player;
class Transform;
class HoldCoin;
class Fade;
/// <summary>
/// ゲーム状態操作するクラス
/// </summary>
class GameController : public GameObject
{
public:

	GameController();
	~GameController();

	void Init();

	void Update()	override;
	void Draw()		override;


	/// <summary>
	/// ゲームの状態
	/// </summary>
	enum class GAME_STATE
	{
		START = 0,	// ゲームスタート		
		SETUP,		// 準備 オブジェクトの設置など
		DEFENCE,	// 防衛
		DEFEAT,		// 敗北
		VICTORY,	// 勝利

		MAX,
	};

	/// <summary>
	/// ゲームの状態を返す
	/// </summary>
	/// <returns> ゲームの状態 </returns>
	const GAME_STATE& GetGameState() const { return gameState; }

	/// <summary>
	/// ゲームプレイの状態
	/// </summary>
	enum class PLAY_STATE
	{
		OBJECT_PUT,		// オブジェクト設置モード
		FIRST_PERSON,	// 一人称モード
	};

	/// <summary>
	/// ゲームプレイの状態を返す
	/// </summary>
	/// <returns> ゲームプレイの状態 </returns>
	const PLAY_STATE& GetPlayState() const { return playState; }

	/// <summary>
	/// ブロックの配置状態
	/// </summary>
	enum class SETTING_WAY_STATE
	{
		GRID = 0,	// グリッド配置
		FREE,		// フリー配置

		MAX,
	};

	/// <summary>
	/// 配置モード　blockを置くモード、経路探索のゴールを置くモードのステート
	/// </summary>
	enum class SETTING_STATE
	{
		STAGE_OBJ_SET,	// ブロック設置
		GOAL_SET,		// 経路探索のゴール設置

		MAX,
	};

	/// <summary>
	/// 配置モードを返す
	/// </summary>
	/// <returns>  </returns>
	const SETTING_STATE& GetControllState() const { return settingState; }

	/// <summary>
	/// wayPointの当たり判定のリセット　今後は経路も再探索する
	/// </summary>
	//void NavigationReload();

	/// <summary>
	/// ゲームプレイ中か返す
	/// </summary>
	/// <returns> ゲームプレイ中：ゲームプレイ中でない </returns>
	bool InGame() { return gameState == GAME_STATE::DEFENCE || gameState == GAME_STATE::SETUP; }

	/// <summary>
	/// 保持しているコインを1枚増やす
	/// </summary>
	void AddHoldCoin();

	/// <summary>
	///						保持しているコインを減らす
	/// </summary>
	/// <param name="_sub">	減らす枚数				</param>
	void SubHoldCoin(const int& _sub);

	/// <summary>
	///				保持しているコイン数を返す
	/// </summary>
	/// <returns>	保持しているコイン数			</returns>
	int GetHoldCoin();

	/// <summary>
	/// 消費量に対してコインが不足していたことを伝える
	/// </summary>
	void SetInsufficientCoin();

	/// <summary>
	///						デバックモードかを設定
	/// </summary>
	/// <param name="_set">	true:デバックモード / false:デバックモードでない	</param>
	void SetDebugFlag(bool _set) { isDebug = _set; }

private:

	/// <summary>
	/// UIの描画
	/// </summary>
	void UIDraw();

	/// <summary>
	/// ステージオブジェクトの配置をするUpdate
	/// </summary>
	void SettingUpdate();

	/// <summary>
	/// 俯瞰視点でのカメラ操作のUpdate
	/// </summary>
	void CameraControllUpdate();

	/// <summary>
	/// 配置するステージオブジェクトUIの描画
	/// </summary>
	void SettingUIDraw();

	/// <summary>
	/// 配置するステージオブジェクトモデルの描画
	/// </summary>
	void SettingModelDraw();

	/// <summary>
	/// ウェーブスタートの情報UIの描画
	/// </summary>
	void WaveStartInfoDraw();

	/// <summary>
	/// ウェーブの管理を行う
	/// </summary>
	void WaveUpdate();

	/// <summary>
	/// ゲームの状態を返す
	/// </summary>
	/// <param name="_state"> ゲームの状態 </param>
	void SetGameState(const GAME_STATE& _state) { gameState = _state; }

	/// <summary>
	/// マウスポインタの座標から3Dゲーム内座標に変換
	/// </summary>
	/// <param name="_screenPos"> VECTOR2Iマウス座標 </param>
	/// <param name="_mode"> 配置モード </param>
	/// <returns> 3Dゲーム内座標 </returns>
	VECTOR3 GetScreenForWorldPosI(const VECTOR2I& _screenPos, const SETTING_WAY_STATE& _mode);

	/// <summary>
	/// マウスポインタの座標から3Dゲーム内座標に変換
	/// </summary>
	/// <param name="_screenPos"> VECTOR2マウス座標 </param>
	/// <param name="_mode"> 配置モード </param>
	/// <returns> 3Dゲーム内座標 </returns>
	VECTOR3 GetScreenForWorldPosF(const VECTOR2& _screenPos, const SETTING_WAY_STATE& _mode);

	/// <summary>
	/// ステージオブジェクトの配置
	/// </summary>
	/// <param name="_mode"> 配置モード </param>
	/// <returns> 配置した：していない </returns>
	bool StageObjectSetting(const SETTING_WAY_STATE& _mode);

	/// <summary>
	/// ステージオブジェクトの配置
	/// </summary>
	void PutStageObject();

	/// <summary>
	/// ステージオブジェクトの削除
	/// </summary>
	void DeleteStageObject();

	
	// クラスポインタを参照
	Input* inputManager						= nullptr;
	NavigationManager* navigationManager	= nullptr;
	StageManager* stageManager				= nullptr;
	WaveController* waveController			= nullptr;
	EnemyManager* enemyManager				= nullptr;
	Camera* camera							= nullptr;
	Fade* fade								= nullptr;

	// このクラスでnew
	HoldCoin* holdCoin						= nullptr;

	bool isDebug				= false;// デバッグフラグ

	bool isSceneChange			= false;// シーン変更フラグ

	int coinUIFunctionIDNumber	= -1;	// HoldCoinクラスのCoinDraw関数に対するIDナンバー
	int uiFunctionIDNumber		= -1;	// UIDraw関数に対するIDナンバー


	bool useImGuiMouse				= false;	// マウスポインタがImGuiで使用されているかのフラグ
	int inputPad;								// パッドの入力
	bool pushChangeCameraKey		= false;	// カメラ変更のキーを押したかどうか

	VECTOR2I screenPosition;					// マウスポインタ座標
	Transform putTransform;						// ブロック設置トランスフォーム
	SETTING_STATE settingState			= SETTING_STATE::STAGE_OBJ_SET;	// 配置モード
	SETTING_WAY_STATE settingWayState	= SETTING_WAY_STATE::GRID;		// 配置方法
	MATRIX camMoveRotYMatrix			= MGetIdent();					// カメラの移動用のY回転行列
	int setCamMoveRotYMatrixAngle		= 0;							// 設定するカメラの移動用のY回転行列の角度値
	float camTargetToPosDistance		= 0.0f;							// カメラのターゲット(注視点)からカメラの座標までの距離

	// 配置する罠のデータ
	struct PutTrapData
	{
		PutTrapData() : kind(0), cost(0) {};
		PutTrapData(const int& _kind, const int& _cost) : kind(_kind), cost(_cost) {};

		int kind;		// 配置する罠の種類
		int cost;		// 配置の消費量　（コインを消費）
	};

	std::unordered_map<int, PutTrapData> putTrapDataList;		// 配置する罠のデータコンテナ key:STAGE_OBJECT_KINDのint型 / value:PutTrapData
	int putTrapKind = (int)StageObjectData::STAGE_OBJECT_KIND::WALL_BLOCK;	// 配置する罠

	GAME_STATE gameState;										// ゲームの状態
		
	PLAY_STATE playState;										// ゲームプレイの状態
	PLAY_STATE copyPlayState;									// ゲームプレイの状態のコピー


	bool changedState = false;									// ゲームの状態が変化したかどうかフラグ

	bool firstFrameWaveEnd = false;								// ウェーブが終わった初めのフレームかどうか

	enum UI_KIND
	{
		GAM_DEFEAT_LETTER,			// DEFEATの文字
		GAM_VICTORY_LETTER,			// VICTORYの文字
		GAM_PUT_TRAP_LETTER,		// PUT TRAPの文字
		GAM_WALL_LETTER,			// WALLの文字
		GAM_SPIKE_LETTER,			// SPIKEの文字
		GAM_ASSAULT_TURRET_LETTER,	// ASSAULT TURRETの文字
		GAM_SHOTGUN_TURRET_LETTER,	// SHOTGUN TURRETの文字
		GAM_JUMP_PAD_LETTER,		// JUMP_PADの文字
		GAM_WAVE_START_LETTER,		// WAVE_STARTの文字
		GAM_CHANGE_CAMERA_LETTER,	// CHANGE CAMERAの文字
		GAM_COST_NUM,				// コスト数字
		GAM_E_KEY,					// Eキー
		GAM_A_KEY,					// Aキー
		GAM_D_KEY,					// Dキー
		GAM_ENTER_KEY,				// Enterキー
		GAM_1_KEY,					// 1キー
		GAM_3_KEY,					// 3キー
		GAM_4_KEY,					// 4キー
		GAM_DEFEAT_CORE,			// 敗北コア
		GAM_VICTORY_CORE,			// 勝利コア
		GAM_MOUSE,					// マウスポインタ

		GAM_MAX,
	}; 

	/// <summary>
	///							putTrapKindから現在選択されている罠の種類の文字ハンドルを返す
	/// </summary>
	/// <param name="_kind">	putTrapKind				</param>
	/// <returns>				罠の文字ハンドル		</returns>
	int GetPutTrapImageHandle(const int& _kind);

	int gameUIImage[UI_KIND::GAM_MAX];	// GameControllerが管理するUIハンドル配列
};