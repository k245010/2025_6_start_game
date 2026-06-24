#pragma once
#include "../EnemyKind.h"
#include "../EnemyInfo.h"
#include "../../Stage/StageObjectData.h"
//#include "../../Navigation/Node/NodeData.h"
#include "../../../Library/GameObject.h"
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Player;
class EnemyBase;
class NavigationManager;
class StageManager;
class Transform;
/// <summary>
/// 敵を総括するクラス
/// </summary>
class EnemyManager : public GameObject
{
public:

	EnemyManager();
	~EnemyManager();

	/// <summary>
	///				インスタンスがnewされているかどうか
	/// </summary>
	/// <returns>	true:newされている / false:newされていない　 </returns>
	static bool IsActiveInstance() { return isActiveInstance; }

	/// <summary>
	/// クラスポインタの初期化
	/// </summary>
	void Init();

	void Update()	override;
	void Draw()		override;

	/// <summary>
	///							敵クラスをコンテナに追加する
	/// </summary>
	/// <param name="_enemy">	敵クラスポインタ 		</param>
	void Push(EnemyBase* _enemy);

	/// <summary>
	///							敵クラスをコンテナから削除する
	/// </summary>
	/// <param name="_enemy">	敵クラスポインタ 		</param>
	void Pop(EnemyBase* _enemy);

	/// <summary>
	/// 敵のナビゲーション情報をリロード
	/// </summary>
	void EnemyNavigationReload();

	/// <summary>
	/// 一番近くの敵のナンバーを返す
	/// </summary>
	/// <param name="_pos"></param>
	/// <returns></returns>
	int GetNearEnemyNumber(const VECTOR3& _pos);

	/// <summary>
	/// 一番近くの敵のトランスフォームを返す
	/// </summary>
	/// <param name="_pos"> 自身の座標 </param>
	/// <returns></returns>
	const Transform& GetNearEnemyTransform(const VECTOR3& _pos);

	/// <summary>
	/// 一番近くの敵の座標を返す
	/// </summary>
	/// <param name="_pos"> 自身の座標 </param>
	/// <returns></returns>
	const VECTOR3& GetNearEnemyPosition(const VECTOR3& _pos);

	/// <summary>
	/// 指定されたナンバーの敵の座標を返す
	/// </summary>
	/// <param name="_num"> 敵のナンバー </param>
	/// <returns> 座標 </returns>
	const VECTOR3& GetEnemyPosition(const int& _num);

	/// <summary>
	/// 指定されたナンバーの敵のヴェロシティを返す
	/// </summary>
	/// <param name="_num"> 敵のナンバー </param>
	/// <returns> ヴェロシティ </returns>
	const VECTOR3& GetEnemyVelocity(const int& _num);

	/// <summary>
	/// 敵の生成をリクエストする
	/// </summary>
	/// <param name="_pos">		座標		</param>
	/// <param name="_kind">	敵の種類	</param>
	/// <returns>				成功：失敗	</returns>
	bool RequestSummonEnemy(const VECTOR3& _pos, const ENEMY_KIND& _kind);

	/// <summary>
	///								次に召喚する敵の情報を設定　(全く同じ情報はマージして下さい)
	/// </summary>
	/// <param name="_enemyInfo">	std::unordered_multimap<int, VECTOR3I>でkey:敵の種類 value:敵の召喚座標	</param>
	void SetNextSummonEnemyInfo(std::unordered_multimap<int, VECTOR3I> _enemyInfo);

	/// <summary>
	///							次に召喚する敵のルートを描画するかどうか
	/// </summary>
	/// <param name="_isDraw">	true:描画する / false:描画しない		</param>
	void SetDrawNextSummonEnemyRoot(bool _isDraw) { isDrawNextSummonEnemyRoot = _isDraw; }

	/// <summary>
	/// 敵が召喚されている (ゲーム内に存在する) かどうか返す
	/// </summary>
	/// <returns> 召喚されてる:召喚されていない </returns>
	bool IsEnemySummon() const { return !enemies.empty(); }

	/// <summary>
	/// NavigationManagerの関数を使いナビナンバーから座標を返す
	/// </summary>
	/// <param name="_num"> ナビナンバー </param>
	/// <returns> 座標：失敗時はnullを返す </returns>
	const VECTOR3* GetNavPointPosition(const int& _num);

	/// <summary>
	///						NavPointナンバーから使用用途種類を返す
	/// </summary>
	/// <param name="_num">	ナビポイントのナンバー				</param>
	/// <returns>			成功:使用用途種類 / 失敗:nullptr 	</returns>
	const NODE_USE_KIND* GetNavPointUseKind(const int& _num);

	/// <summary>
	///						NavPointナンバーからオブジェクトに潰されているかの状態
	/// </summary>
	/// <param name="_num">	ナビポイントのナンバー				</param>
	/// <returns>			成功:使用用途種類 / 失敗:nullptr	</returns>
	const NODE_BLOCK_STATE* GetNavPointBlockState(const int& _num);

	/// <summary>
	///								進むべきNavPointナンバーをコンテナにまとめて返す
	/// </summary>
	/// <param name="_mePos">		自身のいる座標（スタート座標）		</param>
	/// <param name="_goalPos">		目的地の座標（ゴール座標）			</param>
	/// <param name="_agentKind">	エージェントの種類					</param>
	/// <param name="_moveSpeed">	エージェントの移動スピード			</param>
	/// <param name="_nodeKind">	経路探索で使うノードの種類			</param>
	/// <param name="_dps">			1秒間に与えるダメージ数				</param>
	/// <param name="_mode">		経路探索するモード					</param>
	/// <returns>					進むべきNavPointナンバーコンテナ	</returns>
	std::vector<int> GetTargetMoveNavPointContainer(const VECTOR3& _mePos, const VECTOR3& _goalPos, const int& _agentKind, const float& _moveSpeed, const int& _nodeKind, const int& _mode = 0, const float& _dps = -1.0f);

	/// <summary>
	/// プレイヤーのトランスフォームを返す
	/// </summary>
	/// <returns> トランスフォーム </returns>
	const Transform& GetPlayerTransform();

	/// <summary>
	/// コアブロックの座標ポインタを返す
	/// </summary>
	/// <returns> コアブロックの座標ポインタ </returns>
	const VECTOR3* GetCorePositionPtr() const;

	/// <summary>
	/// プレイヤーの座標ポインタを返す
	/// </summary>
	/// <returns> プレイヤーの座標ポインタ </returns>
	const VECTOR3* GetPlayerPositionPtr() const;

	/// <summary>
	///				一番近い潰されたナビポイントの座標ポインタを返す
	/// </summary>
	/// <returns>	一番近い潰されたナビポイントの座標ポインタ	</returns>
	

	//const VECTOR3* GetNearBlockedNavPointPtr(const ) const;

	/// <summary>
	///							レイとステージオブジェクトの当たり判定
	/// </summary>
	/// <param name="pos1">		レイの始点 </param>
	/// <param name="pos2">		レイの終始 </param>
	/// <param name="_type">	どのステージオブジェクトタイプで当たり判定するか　デフォルトNONE -> すべて </param>
	/// <param name="hitP">		当たった座標 </param>
	/// <returns>				当たった：当たってない </returns>
	//bool StageObjectTypeRayCollision(const VECTOR3& _pos1, const VECTOR3& _pos2, const StageObjectData::TYPE& _type = StageObjectData::TYPE::NONE, VECTOR3* _hitP = nullptr);

	/// <summary>
	///							レイとステージオブジェクト種類ごとの当たり判定
	/// </summary>
	/// <param name="_pos1">	レイの始点																						</param>
	/// <param name="_pos2">	レイの終始																						</param>
	/// <param name="_type">	どのステージオブジェクト種類で当たり判定するかのコンテナ　コンテナが空の場合はすべて当たり判定	</param>
	/// <param name="_hitP">	当たった座標																					</param>
	/// <returns>				当たった：当たってない																			</returns>
	bool CheckRaycastStageObject(const VECTOR3& _pos1, const VECTOR3& _pos2, const std::set<int>& _kindList, VECTOR3* _hitP = nullptr);

	/// <summary>
	///						コインをドロップする
	/// </summary>
	/// <param name="_pos">	ドロップする座標	</param>
	void DropCoin(const VECTOR3& _pos);

private:

	static bool isActiveInstance;						// インスタンスがnewされているかのフラグ

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	/// <param name="kind"> 敵の種類 </param>
	void CreateEnemy(const VECTOR3& _pos, const ENEMY_KIND& _kind);

	/// <summary>
	/// 次のウェーブで召喚される敵のルートのナビポイントナンバーコンテナを設定する　
	/// </summary>
	void SetNextSummonEnemyRootNumberList();

	/// <summary>
	/// 敵データの変更を行う
	/// </summary>
	void ChangeEnemyDataUpdate();

	/// <summary>
	///										敵データの当たり判定データの変更を行う
	/// </summary>
	/// <param name="_collisionDataList">	敵データの当たり判定データコンテナ参照	</param>
	void ChangeEnemyCollisionDataList(std::unordered_map<COLLISION_OBJECT_KIND, float>& _collisionDataList);

	StageManager* stageManager				= nullptr;						
	NavigationManager* navigationManager	= nullptr;		// NavigationManagerのクラスポインタ
	Player* player							= nullptr;		// Playerのクラスポインタ

	static int enemyNumber;									// 敵のナンバーを管理する静的変数
	std::unordered_map<int, EnemyBase*> enemies;			// 生成された敵を保存するコンテナ

	int hModels[(int)ENEMY_KIND::MAX];						// 敵のモデルハンドル

	bool navAlways							= false;		// 常に経路探索
	int navTargetObj						= 0;			// 経路探索したどり着く相手の種類
	bool keyPut								= false;		// 連打防止フラグ

	std::unordered_multimap<int, VECTOR3I> nextSummonEnemyInfoList;		// 次のウェーブで召喚する敵の生成情報コンテナ	key:敵の種類 / value:召喚座標	(全く同じ情報はコンテナに存在しないようにする)
	std::set<std::vector<int>> nextSummonEnemyRootNumberList;			// 次のウェーブで召喚される敵のルートのWayPointナンバーコンテナ　std::vector敵のルートのWayPointナンバー　(全く同じルートはコンテナに存在しないようにする)
	bool isDrawNextSummonEnemyRoot			= false;					// 次のウェーブで召喚される敵のルートを描画するかどうか
	int summonedSinceTimeCount				= 0;						// 敵を生成してからのカウント

	bool isDrawEnemyRoot					= true;						// 敵のルートを描画するかどうか　敵の召喚前、召喚後のルート全てを含む

	int setEnemyKind									= (int)ENEMY_KIND::WALKER;		// 設定する敵データの種類
	int setSelectCollisionDataMember					= 0;							// 設定する敵データのどのCollisionDataのメンバを選択するか設定する メンバにはbodyCollTargetKindListとattackCollTargetKindListがある
	COLLISION_OBJECT_KIND setCollisionDataTargetKind	= COLLISION_OBJECT_KIND::NONE;	// 設定する敵データのCollisionDataの種類
	float setCollisionDataTargetDamge					= 0.0f;							// 設定する敵データのCollisionDataのダメージ量
	bool isSelectCollisionData							= false;						// 設定する敵データのCollisionDataを選択したかどうか
	std::string selectCollisionObjectKindStr;											// 選択した当たり判定オブジェクトの種類の文字列
	std::vector<EnemyInfo::EnemyData> enemyDataSetList;		// 設定する敵データのコンテナ
	std::vector<EnemyInfo::EnemyData> enemyDataAttachList;	// 適用する敵データのコンテナ
	std::vector<EnemyInfo::EnemyData> enemyDataRawList;		// 元の敵データのコンテナ
};