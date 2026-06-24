#pragma once
#include "../CharaBase/CharaBase.h"
#include "../Gun/Gun/Gun.h"
#include <vector>
#include <string>
#include <unordered_map>

class Camera;
class SphereCollision;
class RayCollision;
class GameController;
class WaveController;
class StageManager;
class ShotManager;
class BulletManager;
class Physics;
class Input;
class Gauge;
class Animator;
class Gun;
/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public CharaBase
{
public:

	Player();
	~Player();

	void Init();
	void Update()	override;
	void Draw()		override;

	/// <summary>
	/// デバッグの状態
	/// </summary>
	enum class DEBUG_STATE
	{
		NORMAL = 0,
		DEBUG,

		MAX,
	};

	/// <summary>
	/// プレイヤーの状態
	/// </summary>
	enum class PLAYER_STATE
	{
		ACTION	= 0,
		DEATH,
		RESPAWN,

		MAX,
	};
	
	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	void SetPosition(const VECTOR3& _pos) { transform.position = _pos; }

	/// <summary>
	/// プレイヤーのカプセルの描画
	/// </summary>
	/// <param name="set"> 描画フラグ </param>
	void SetDrawPlayerCapsule(bool set) { onDrawCapsule = set; }

	const VECTOR3& GetDebugPosition() const { return dPosition; }

	/// <summary>
	/// スライム弾を最大につめる
	/// </summary>
	void PushSlimeBulletMax();

private:

	/// <summary>
	/// UI関連の描画
	/// </summary>
	void UIDraw();

	/// <summary>
	///							GunDataのファイルパスを返す
	/// </summary>
	/// <param name="_fileNum">	ファイルの番号			</param>
	/// <returns>				GunDataのファイルパス	</returns>
	std::string GetGunDataPath(const int& _fileNum);

	/// <summary>
	///								GunDataをロードしてデータを返す
	/// </summary>
	/// <param name="_saveFileNum">	ロードするファイルの番号	</param>
	/// <returns>					GunData						</returns>
	GunInfo::GunData LoadGunData(const int& _loadFileNum);

	/// <summary>
	///								gunsのGunDataをセーブする
	/// </summary>
	/// <param name="_saveFileNum">	セーブするファイルの番号	</param>
	/// <param name="_gunData">		保存するGunData				</param>
	void SaveGunData(const int& _saveFileNum,const GunInfo::GunData& _gunData);

	/// <summary>
	/// キャラクターが当たり判定で当たった時の処理
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	/// <returns> 自身の当たり判定クラスポインタを破棄する：破棄しない </returns>
	bool HitChara(const CollisionHitInfoData& _targetData) override;

	/// <summary>
	/// キャラクターの押し返し判定	ToDo: よりよい命名を考える
	/// </summary>
	/// <param name="_targetData"> 相手の当たり判定情報 </param>
	void PushBackOnCollision(const CollisionHitInfoData& _targetData);

	/// <summary>
	///								球の当たり判定での押し返しの処理を行う	(球の押し返しは、他の押し返しと比べて、複雑なので関数化しました)
	/// </summary>
	/// <param name="_hitPosCon">	当たった座標コンテナ									</param>
	/// <param name="_hitNormCon">	当たった法線コンテナ									</param>
	void PushBackOnCollisionSphere(const std::vector<VECTOR3>& _hitPosCon, const std::vector<VECTOR3>& _hitNormCon);


	enum class PLAYER_ATTACK_STATE
	{
		SLIME_PATCH_THROW = 0,
		NORMAL_BULLET_SHOT,

		MAX,
	};

	/// <summary>
	/// 回転がデッドゾーンを超えないようにセット
	/// </summary>
	/// <param name="rot"> 回転値 </param>
	/// <param name="mRot"> マウスの回転値 </param>
	void SetDeadViewRot(VECTOR3& rot,const VECTOR3& mRot);
	
	/// <summary>
	/// マウスの座標から移動量を返す
	/// </summary>
	/// <returns> 移動量 </returns>
	const VECTOR3& GetMouse();		

	DEBUG_STATE debugState;		// デバッグのステート

	GameController* gameController	= nullptr;
	WaveController* waveController	= nullptr;
	StageManager* stageManager		= nullptr;
	ShotManager* shotManager		= nullptr;
	BulletManager* bulletManager	= nullptr;
	RayCollision* rayColl			= nullptr;
	Physics* physics				= nullptr;
	Camera* camera					= nullptr;
	Input* input					= nullptr;
	Gauge* playerHPGauge			= nullptr;		// プレイヤーのHPゲージ
	Gauge* trapUpgradingNumGauge	= nullptr;		// 罠のアップグレード進行値ゲージ
	Animator* animation				= nullptr;		// アニメーションクラスのクラスポインタ
	std::vector<Gun*> guns;							// プレイヤーが持つ銃の種類分生成

	// マウスのデータ
	struct MouseData
	{
		//マウスポインタの座標
		VECTOR2I pos		= VECTOR2I();

		//動いた分の座標
		VECTOR2I movePos	= VECTOR2I();

		//mouseMoveの値を代入する
		VECTOR3 rePos;
	};
	MouseData mouse;

	bool getMouse;						// マウスの座標をゲットするかどうかのフラグ
	int inputPad;						// パッド変数

	int uiFunctionIDNumber = -1;		// UIDraw関数に対するIDナンバー

	//bool keyPut[10];

	/// <summary>
	/// 行動できる状態でのUpdate
	/// </summary>
	void ActionUpdate();

	/// <summary>
	/// 移動Update
	/// </summary>
	void MoveUpdate();

	/// <summary>
	/// 攻撃Update
	/// </summary>
	void AttckUpdate();

	/// <summary>
	/// スライムパッチを投げるUpdate
	/// </summary>
	//void ThrowUpdate();

	/// <summary>
	///								ショットUpdate
	/// </summary>
	/// <param name="_gunConIndex">	どのGunコンテナの要素にアクセスするかのインデックス	</param>
	void ShotUpdate(const int& _gunConIndex);

	/// <summary>
	/// 罠のメンテナンスをするUpdate
	/// </summary>
	void TrapMaintainUpdate();

	/// <summary>
	/// 罠を強化するUpdate
	/// </summary>
	void TrapUpgradeUpdate();

	/// <summary>
	/// 死亡状態でのUpdate
	/// </summary>
	void DeathUpdate();

	/// <summary>
	/// 復活処理
	/// </summary>
	void Respawn();

	/// <summary>
	/// HPの描画
	/// </summary>
	void HPDraw();

	/// <summary>
	/// 発射する弾の変更キーの描画
	/// </summary>
	void BulletChangeKeyDraw();

	/// <summary>
	///										弾情報の描画
	/// </summary>
	/// <param name="_playerAttackState">	現在のプレーヤーの攻撃ステート	</param>
	void BulletInfoDraw(const PLAYER_ATTACK_STATE& _playerAttackState);

	/// <summary>
	///										リロードの描画
	/// </summary>
	/// <param name="_playerAttackState">	現在のプレーヤーの攻撃ステート	</param>
	void ReloadDraw(const PLAYER_ATTACK_STATE& _playerAttackState);

	/// <summary>
	/// デバッグでの移動Update
	/// </summary>
	void DebugMove();

	enum UI_KIND
	{
		UI_UPGRADE_COST_LETTER,
		UI_UPGRADE_COST_NUM_LETTER,
		UI_SLIME_BULLET_NUM_LETTER,
		UI_NORMAL_BULLET_NUM_LETTER,
		UI_SLIME_ICON,
		UI_GUN_ICON,
		UI_R_KEY,
		UI_1_KEY,
		UI_2_KEY,
		
		MAX_UI,
	};

	int hImage[MAX_UI];				// UIハンドル配列　

	std::unordered_map<std::string, int> collOwnerNameList;						// 個々の当たり判定オブジェクトの名前を保存するコンテナ
	std::unordered_map<int, std::list<VECTOR3>> hitPositionInfoList;	// 当たり判定で当たった座標の情報を保存するコンテナ

	Transform collTrans;			// 応急処置球の当たり判定用トランスフォーム

	PLAYER_STATE state;				// プレイヤーの状態
	PLAYER_ATTACK_STATE attackState;// プレイヤーの攻撃状態
	
	float respawnCount		= 0.0f;	// リスポーンカウント

	bool onGround			= false;// 地面の上かどうか
	bool isInvincible		= false;// 無敵
	float invincibleCount	= 0.0f;	// 無敵時間

	bool canTrapUpgrade				= false;	// 現在罠のアップグレードが可能かどうか
	bool isTrapUpgrading			= false;	// 現在罠のアップグレードをしているかどうか
	float trapUpgradingReturnNum	= 0.0f;		// 罠の現在のアップグレード値
	VECTOR3 upgradeTrapPosition		= VZero;	// アップグレードする罠の座標
	int upgradeTrapKind				= -1;		// アップグレードする罠の種類

	bool isHitJumpPad		= false;			// ジャンプパッドに当たったかどうか

	bool isShotFail			= false;			// Gunの発射に失敗したかどうか
	float reloadKeyDrawTime;					// リロードキーの描画経過時間

	//-- デバッグ
	VECTOR3 dPosition;	
	VECTOR3 dRotation;	
	VECTOR3 dVelocity;		
	bool onDrawCapsule;				// プレイヤーのカプセルを描画するかのフラグ
};