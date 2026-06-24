#pragma once
#include "../../../Library/GameObject.h"
#include "../../Transform/Transform.h"
#include "NodeData.h"
#include <unordered_map>
#include <set>
#include <vector>

/// <summary>
/// 基本コストに対する倍率　特定エリアでのNavPointのコストを変更する時に使う　例）泥の中 -> 倍率 2.0
/// </summary>
struct CostRate
{
	float costRate;	// コストに対する倍率1.0

	CostRate()			: costRate(1.0f)	{}
	CostRate(float _c)	: costRate(_c)		{}
};

/// <summary>
/// ノードのコスト
/// </summary>
struct Cost
{
	float g;		// スタートノードから現在のノードまでの実際の移動コスト
	float h;		// ヒューリスティックコスト(現在のノードからゴールノードまでの推定コスト) ユークリッド距離で計算
	
	Cost* parent;	// 親ノードへのポインタ

	Cost()
	{
		g = 0.0f;
		h = 0.0f;
		parent = nullptr;
	}

	/// <summary>
	///							コンストラクタ
	/// </summary>
	/// <param name="_g">		スタートノードから現在のノードまでの実際の移動コスト	</param>
	/// <param name="_h">		ヒューリスティックコスト								</param>
	/// <param name="_parent">	親ノードへのポインタ									</param>
	Cost(const float& _g, const float& _h, Cost* _parent = nullptr)
	{
		g		= _g;
		h		= _h;
		parent	= _parent;
	}

	// *オペレーターのオーバーライド　基本コストとの積
	//Cost operator*(CostRate _r) const { return Cost(g * _r.costRate, h); }
};

/// <summary>
/// NavPointの生成元モデル情報　生成元とは当たり判定をしなくていいので、不要な当たり判定を防ぎたい
/// </summary>
/// ※本来はポインターでの管理が理想だが、csvにNavPointを書き出すので考え中
struct OwnerModelData
{
	/// <summary>
	///						コンストラクタ
	/// </summary>
	/// <param name="_num"> 生成元のモデルごとの識別ナンバー							</param>
	/// <param name="_pos"> 生成元のモデルの座標データ 現在はポリゴンの最大座標を記録	</param>
	/// <param name="_ptr"> 生成元のモデルのインスタンスのポインタ						</param>
	OwnerModelData(const int& _num, const VECTOR3& _pos,const void* _ptr)	: number(_num), position(_pos), pointer(_ptr)	{}
	OwnerModelData()														: number(-1), position(VZero), pointer(nullptr) {}

	int number;							// 識別ナンバー
	VECTOR3 position;					// 座標
	const void* pointer;				// モデルのインスタンスのポインター
};	

/// <summary>
/// ノードの状態
/// </summary>
enum class NODE_STATE
{
	NONE = 0,	// 未初期化
	OPEN,		// オープン状態
	CLOSE,		// クローズ状態
};

/// <summary>
/// ノードの役割
/// </summary>
enum class NODE_ROLE
{
	NORMAL,	// 普通のノード
	GOAL,	// ゴールノード
};

/// <summary>
/// NavPointの種類　NAV_MESHがモデルのポリゴン情報から構築したもので、配置したものがWAY_POINTとなる
/// </summary>
enum class NAV_POINT_KIND
{
	NONE = 0,
	NAV_MESH,				// ナビゲーションメッシュ
	WAY_POINT,				// ウェイポイント
	VOXEL_WAY_POINT,		// 潰されたナビポイント情報をまとめるボクセルのウェイポイント

	MAX,
};

/// <summary>
/// 接続相手のデータ
/// </summary>
struct TargetData
{
	TargetData()																: kind(TARGET_CONNECT_KIND::NORMAL), blockState(NODE_BLOCK_STATE::FREE) {}
	TargetData(const TARGET_CONNECT_KIND& _kind)								: kind(_kind), blockState(NODE_BLOCK_STATE::FREE)						{}
	TargetData(const TARGET_CONNECT_KIND& _kind,const NODE_BLOCK_STATE& _block)	: kind(_kind), blockState(_block)										{}
	
	/// <summary>
	///								接続相手との間に、ボクセルが置かれた、NODE_BLOCK_STATEがBLOCKEDの時のボクセルキーを追加
	/// </summary>
	/// <param name="_voxelKey">	オーナーボクセルキー		</param>
	void PushVoxelOwnerKey(const int& _voxelKey)
	{
		voxelOwnerKeyList.emplace(_voxelKey);
	}

	/// <summary>
	///								ボクセルキーのコンテナから引数のボクセルキーと一致するものをコンテナから削除し、コンテナが空になったか確認する　
	/// </summary>
	/// <param name="_voxelKey">	ボクセルキー												</param>
	/// <returns>					true:コンテナが空になった / false:コンテナにまだ要素がある	</returns>
	bool DeleteVoxelOwnerKeyAndEmptyConfirm(const int& _voxelKey)
	{
		std::set<int>::iterator itr = voxelOwnerKeyList.find(_voxelKey);

		if (itr != voxelOwnerKeyList.end())
		{
			itr = voxelOwnerKeyList.erase(itr);
		}
		return voxelOwnerKeyList.empty();
	}

	/// <summary>
	///				オーナーのボクセルキーコンテナを返す
	/// </summary>
	/// <returns>	オーナーのボクセルキーコンテナ		</returns>
	const std::set<int>& GetVoxelOwnerKeyContainer() const { return voxelOwnerKeyList; }

	/// <summary>
	///								引数のオーナーのボクセルキーを持っているのか返す
	/// </summary>
	/// <param name="_voxelKey">	オーナーのボクセルキー									</param>
	/// <returns>					true:持っている / false:持っていない					</returns>
	bool HasVoxelOwnerKey(const int& _voxelKey) const
	{
		return voxelOwnerKeyList.find(_voxelKey) != voxelOwnerKeyList.end();
	}

	TARGET_CONNECT_KIND kind;			// 接続の仕方の種類
	NODE_BLOCK_STATE blockState;		// オブジェクトに潰されているかの状態
	std::set<int> voxelOwnerKeyList;	// オーナーボクセルキーコンテナ　複数のボクセルに潰される可能性を考えてコンテナにしている
};

/// <summary>
/// ナビゲーションを行うナビゲーションの基底クラス
/// </summary>
class NavPointBase : public GameObject
{
public:

	/// <summary>
	///						コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid	</param>
	/// <param name="_pos"> 座標						</param>
	NavPointBase(const int& _num, const VECTOR3& _pos = VZero,const OwnerModelData& _ownerModel = OwnerModelData());

	/// <summary>
	///							コンストラクタ
	/// </summary>
	/// <param name="_num">		自身を識別するナンバーid	</param>
	/// <param name="_pos">		座標						</param>
	/// <param name="_state">	ノードの状態				</param>
	NavPointBase(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_STATE& _state);

	/// <summary>
	///						コンストラクタ
	/// </summary>
	/// <param name="_num"> 自身を識別するナンバーid	</param>
	/// <param name="_pos"> 座標						</param>
	/// <param name="_role">ノードの役割				</param>
	NavPointBase(const int& _num, const VECTOR3& _pos, const OwnerModelData& _ownerModel, const NODE_ROLE& _role);

	virtual ~NavPointBase();

	virtual void Update()	override;
	virtual void Draw()		override;

	/// <summary>
	///				座標を返す
	/// </summary>
	/// <returns>	座標		</returns>
	const VECTOR3& GetPosition() const { return transform.position; }

	/// <summary>
	///						ノードのコストを設定
	/// </summary>
	/// <param name="c">	ノードのコスト		</param>
	void SetCost(const Cost& c)
	{
		cost.g		= c.g;
		cost.h		= c.h;
		cost.parent = c.parent;
	}

	/// <summary>
	///						ノードコストの親ノードを設定
	/// </summary>
	/// <param name="p">	親ノードポインタ	</param>
	void SetCostParent(Cost* p) { cost.parent = p; }

	/// <summary>
	///						コストの倍率を設定する
	/// </summary>
	/// <param name="_b">	基本コスト	</param>
	void SetCostRate(const float& _b) { costRate.costRate = _b; }

	/// <summary>
	///				ノードのコストを返す
	/// </summary>	
	/// <returns>	倍率をかみしたコスト </returns>
	Cost GetCost() const { return cost; }

	/// <summary>
	///				ノードのコストの倍率を返す
	/// </summary>
	/// <returns>	コストの倍率			</returns>
	const CostRate& GetCostRate() const { return costRate; }

	/// <summary>
	///				生成元のモデルのデータを返す
	/// </summary>
	/// <returns>	生成元のモデルのデータ OwnerModelData </returns>
	const OwnerModelData& GetOwnerModelData() const { return ownerModelData; }

	/// <summary>
	///							ノードの状態を設定
	/// </summary>
	/// <param name="_state">	ノード状態	</param>
	void SetState(const NODE_STATE& _state) { state = _state; }

	/// <summary>
	/// ノードを潰したときのナビポイントを管理するボクセル(VoxelNavNodeData)のコンテナ(voxelNavList)のkey値をリセットする
	/// </summary>
	void ResetBlockOwnerVoxelKey() { blockedOwnerVoxelKey = -1; }

	/// <summary>
	///						ノードを潰したときのナビポイントを管理するボクセル(VoxelNavNodeData)のコンテナ(voxelNavList)のkey値を設定する
	/// </summary>
	/// <param name="_key"> コンテナのkey値		</param>
	void SetBlockedOwnerVoxelKey(const int& _key) { blockedOwnerVoxelKey = _key; }

	/// <summary>
	///						ノードを潰したときのナビポイントを管理するボクセル(VoxelNavNodeData)のコンテナ(voxelNavList)のkey値を返す
	/// </summary>
	/// <returns>			コンテナのkey値		</returns>
	int GetBlockedOwnerVoxelKey() const { return blockedOwnerVoxelKey; }

	/// <summary>
	/// ノードを潰したときのナビポイントを管理するボクセルに自身が登録されているか返す
	/// </summary>
	bool IsSettingBlockedOwnerVoxelKey() const { return blockedOwnerVoxelKey != -1; }

	/// <summary>
	///				ノードの状態を返す
	/// </summary>
	/// <returns>	ノード状態			</returns>
	NODE_STATE GetState() const { return state; }

	/// <summary>
	///							ノードの潰されているかの状態を設定する
	/// </summary>
	/// <param name="_block">	NODE_BLOCK_STATE ノードの潰されているかの状態 </param>
	void SetBlockState(const NODE_BLOCK_STATE& _block) { blockState = _block; }

	/// <summary>
	///				ノードの潰されているかの状態を返す
	/// </summary>
	/// <returns>	NODE_BLOCK_STATE 潰されているかどうかの状態 </returns>
	const NODE_BLOCK_STATE& GetBlockState() const { return blockState; }

	/// <summary>
	/// ノードの役割をゴールノードに設定
	/// </summary>
	void SetGoalRole() { role = NODE_ROLE::GOAL; }

	/// <summary>
	///				トランスフォームポインタを返す
	/// </summary>
	/// <returns>	トランスフォームポインタ		</returns>
	Transform* GetTransformPtr() { return &transform; }

	/// <summary>
	/// デバッグでナンバーを表示するかどうかを切り替え
	/// </summary>
	void SetDrawNum() { isDrawNum = !isDrawNum; }

	/// <summary>
	///				アクティブかどうかを返す
	/// </summary>
	/// <returns>	true:アクティブ / false:アクティブでない </returns>
	bool IsActive() const { return isActive; }

	/// <summary>
	///						グローバルナンバーidを設定する
	/// </summary>
	/// <param name="_num"> 設定したい値		</param>
	void SetGlobalNumber(const int& _num) { globalNumber = _num; }

	/// <summary>
	///				グローバルナンバーidを返す
	/// </summary>
	/// <returns>	グローバルナンバーid		</returns>
	int GetGlobalNumber() const 
	{
		// 値が設定されていなかったら
		if (globalNumber < 0)
			assert(false);
		return globalNumber; 
	}

	/// <summary>
	///				ローカルナンバーidを返す　個々のコントローラーcppでの初期化時に設定する自身のナンバー　
	/// </summary>
	/// <returns>	グローバルナンバーid		</returns>
	int GetLocalNumber() const { return localNumber; }

	/// <summary>
	///				NavPointの種類を返す
	/// </summary>
	/// <returns>	NAV_POINT_KIND			</returns>
	NAV_POINT_KIND GetKind() const { return kind; }

	/// <summary>
	///							接続相手を追加する
	/// </summary>
	/// <param name="_target">	接続相手のポインタ		</param>
	/// <param name="_kind">	接続の仕方				</param>
	/// <returns>				true:成功 / false:失敗	</returns>
	bool PushTarget(NavPointBase* _target,TARGET_CONNECT_KIND _kind = TARGET_CONNECT_KIND::NORMAL);

	/// <summary>
	///							接続相手を保存したコンテナから引数のナンバーの接続相手を削除
	/// </summary>
	/// <param name="deNum">	削除するナンバー								</param>
	/// <returns>				true:削除できた / false:削除できなかった		</returns>
	bool DeleteTarget(const int& _targetNum);

	/// <summary>
	///							接続相手を保存したコンテナから引数のポインタの接続相手を削除
	/// </summary>
	/// <param name="deNum">	削除するNavPointBaseのポインタ									</param>
	/// <returns>				true:削除できた / false:削除できなかった						</returns>
	bool DeleteTarget(NavPointBase* _target);

	/// <summary>
	///							接続関係の状態を潰されているに設定する
	/// </summary>
	/// <param name="_key">		targetListコンテナkey				</param>
	/// <param name="_voxelKey">潰されている要因のボクセルキー		</param>
	void SetTargetDataBlock(NavPointBase* _key, const int& _voxelKey)
	{
		if (targetList.find(_key) == targetList.end())
			assert(false);

		targetList[_key].blockState = NODE_BLOCK_STATE::BLOCKED;
		targetList[_key].PushVoxelOwnerKey(_voxelKey);
	}

	/// <summary>
	///							targetListコンテナからボクセルキーを削除する
	///							ボクセルコンテナが空になったらNODE_BLOCK_STATEをFREEにする
	/// </summary>
	/// <param name="_key">		targetListコンテナkey				</param>
	/// <param name="_voxelKey">潰されている要因のボクセルキー		</param>
	void DeleteTargetDataVoxelKey(NavPointBase* _key, const int& _voxelKey)
	{
		if (targetList.find(_key) == targetList.end())
			assert(false);

		// ボクセルコンテナからボクセルキーを削除して、コンテナが空になったら
		if (targetList[_key].DeleteVoxelOwnerKeyAndEmptyConfirm(_voxelKey))
			targetList[_key].blockState = NODE_BLOCK_STATE::FREE;
	}

	/// <summary>
	///				相手を記録したコンテナを返す
	/// </summary>
	/// <returns>	相手を記録したコンテナ			</returns>
	const std::unordered_map<NavPointBase*, TargetData>& GetTargetContainer() const { return targetList; }

	/// <summary>
	///						引数で渡された接続相手のナビポイントの接続データを返す
	/// </summary>
	/// <param name="_nav">	自身(this)の接続相手のナビポイントポインタ										</param>
	/// <returns>			引数の接続相手が存在した：接続データ / 引数の接続相手が存在しなかった:nullptr	</returns>
	const TargetData* GetTargetData(const NavPointBase* _nav);

	/// <summary>
	///							接続相手の個々に対して、引数のオーナーのボクセルキーがあるか返す
	/// </summary>
	/// <param name="_target">	接続相手のポインタ													</param>
	/// <param name="_voxelKey">オーナーのボクセルキー												</param>
	/// <returns>				true:キーがある / false:キーがない									</returns>
	bool HasTargetDataVoxelOwnerKey(NavPointBase* _target,const int& _voxelKey)
	{
		if (targetList.find(_target) == targetList.end())
			assert(false);

		return targetList[_target].HasVoxelOwnerKey(_voxelKey);
	}

	/// <summary>
	///				接続相手のナンバーコンテナを返す
	/// </summary>
	/// <returns>	接続相手のナンバーコンテナ		</returns>
	std::list<int> GetTargetNumberContainer();

	/// <summary>
	/// ノードの使用用途を返す
	/// </summary>
	const NODE_USE_KIND& GetUseKind() const { return useKind; }

	/// <summary>	
	///							ノードの使用用途を設定する
	/// </summary>
	/// <param name="_user">	設定したい使用用途			</param>
	void SetUseKind(const NODE_USE_KIND _use) { useKind = _use; }

protected:

	/// <summary>
	///							接続相手を保存したコンテナ内に引数のナンバーがあるか返す
	/// </summary>
	/// <param name="_target">	調べたいナンバー </param>
	/// <returns>				true:イテレータ / false:イテレータend </returns>
	std::unordered_map<NavPointBase*,TargetData>::iterator IsPushTargetNumber(const int& _targetNum);

	int globalNumber;							// 自身を識別するグローバルナンバーid　	managerが識別する
	int localNumber;							// 自身を識別するローカルナンバーid		controllerが識別する
	NAV_POINT_KIND kind;						// NavPointの種類

	Cost cost;									// ノードのコスト
	CostRate costRate;							// ノードの基本コスト	通常は0
	OwnerModelData ownerModelData;				// 生成元のモデルのデータ
	NODE_STATE state;							// ノードの状態
	NODE_BLOCK_STATE blockState;				// ノードの潰されているかの状態
	int blockedOwnerVoxelKey = -1;				// ノードを潰したときのナビポイントを管理するボクセル(VoxelNavNodeData)のコンテナ(voxelNavList)のkey値
	NODE_ROLE role;								// ノードの役割
	NODE_USE_KIND useKind;						// ノードの使用用途種類
	Transform transform;						// トランスフォーム
	VECTOR3 norm;								// 設置面の法線方向　計算である程度で求めるため場所によっては不正確

	// このコンテナにサイズ制限を設ける場合、一時的に接続相手がinactiveになり、このコンテナから削除され、
	// 別の接続相手がpushされた場合、一時的にinactiveになった接続相手が再びpushされなくなってしまう可能性がある
	std::unordered_map<NavPointBase*, TargetData> targetList;		// 接続相手を保存する

	bool isDrawNum;								// デバッグでナンバーを表示するかどうかのフラグ
	bool isActive;								// アクティブかどうかのフラグ
};