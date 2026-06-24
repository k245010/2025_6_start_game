#pragma once
#include "../../../Library/GameObject.h"
#include "../Node/WayPoint/WayPointController.h"
#include "../Node/NavMesh/NavMeshController.h"
#include "../NavPointUpdater/NavPointUpdater.h"
#include "../Node/NodeData.h"
#include <set>

/// <summary>
/// ノードのスコアを管理
/// </summary>
struct OpenNode
{
	int num;		// ノードのナンバー
	float score;	// ノードのスコア

	/// <summary>
	///							コンストラクタ
	/// </summary>
	/// <param name="_num">		ノードのナンバー		</param>
	/// <param name="_score">	ノードのスコア			</param>
	OpenNode(const int& _num, const float& _score) : num(_num), score(_score) {}

	// scoreの比較演算子オーバーロード　（OpenNodeA < OpenNodeB の時score値で比較）
	bool operator < (const OpenNode& other) const
	{
		// ※ 逆にする（priority_queueは「大きい方が先」なので、これで小さい順にする）＜　を　＞にして返す
		return score > other.score;
	}
};

/// <summary>
/// 経路探索するモード
/// </summary>
enum class NAVIGATION_MODE
{
	ACTIVE		= 0,	// アクティブなノードのみの経路探索
	IN_BLOCKED,			// 潰されたノードも含めた経路探索	主にオブジェクトを壊して進みたいときの経路探索に使用

	MAX,
};

class Player;
class EnemyManager;
class StageManager;
class NavPointBase;
class WayPointController;
class NavMeshController;
class NavPointUpdater;
/// <summary>
/// ナビゲーションを総括するクラス
/// </summary>
class NavigationManager : public GameObject
{
	friend NavPointUpdater;

public:

	NavigationManager();
	~NavigationManager();

	/// <summary>
	/// クラスポインタの初期化
	/// </summary>
	/// <param name="_doNavSetting"> NavMesh(PortalPoint)の初期化を行い、そのデータをファイル出力するかどうか </param>
	void Init(bool _doNavSetting);

	void Update()	override;
	void Draw()		override;

	/// <summary>
	/// ナビポイントナンバーから座標を返す
	/// </summary>
	/// <param name="_num"> ナビポイントナンバー </param>
	/// <returns> 成功:座標 / 失敗:nullptr </returns>
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
	///							引数の座標から一番近い、NavPointナンバー を返す
	/// </summary>
	/// <param name="_pos">		座標 </param>
	/// <param name="_kind">	経路探索で使うノードの種類 </param>
	/// <returns>				NavPointナンバー </returns>
	const int GetPositionForNearNavPointNumber(const VECTOR3& _pos ,const int& _nodeKind,const int& _mode);

	/// <summary>
	///								進むべきNavPointナンバーをコンテナにまとめて返す
	/// </summary>
	/// <param name="_mePos">		自身のいる座標（スタート座標）		</param>
	/// <param name="_goalPos">		目的地の座標（ゴール座標）			</param>
	/// <param name="_agentKind">	エージェントの種類					</param>
	/// <param name="_moveSpeed">	エージェントの移動スピード			</param>
	/// <param name="_nodeKind">	経路探索で使うノードの種類			</param>
	/// <param name="_mode">		経路探索するモード					</param>
	/// <param name="_dps">			1秒間に与えるダメージ数				</param>
	/// <returns>					進むべきNavPointナンバーコンテナ	</returns>
	std::vector<int> GetTargetMoveNavPointContainer(const VECTOR3& _mePos, const VECTOR3& _goalPos, const int& _agentKind, const float& _moveSpeed, const int& _nodeKind,const int& _mode = 0, const float& _dps = -1.0f);

	/// <summary>
	/// NavPointのコンテナが変化したかどうか
	/// </summary>
	/// <returns> 変化した：変化していない </returns> // ToDo 変化したコンテナをreturnする用に「変更したほうがいいかも
	bool IsChangedNavPointContainer() const { return isChangedContainer; }

	/// <summary>
	/// navigationコンテナにあるactiveなNavPointをinactiveコンテナに移動  
	/// </summary>
	/// <param name="_num"> NavPointナンバー </param>
	/// <returns> 成功 削除イテレータ：失敗 イテレータend </returns>
	std::unordered_map<int, NavPointBase*>::iterator ActiveForInactivePushNavPoint(const int& _num);

	/// <summary>
	/// inactiveに移されたNavPointを元のnavigationコンテナに戻す 
	/// </summary>
	/// <param name="_num"> NavPointナンバー </param>
	/// <returns> 成功 削除イテレータ：失敗 イテレータend </returns>
	std::unordered_map<int, NavPointBase*>::iterator InactiveForActivePushNavPoint(const int& _num);

	/// <summary>
	///								ステージのオブジェクトを置いた範囲（BOX）を送る　ナビゲーションのNavPointを非アクティブ化する
	/// </summary>
	/// <param name="_startPos">	BOXの始めの座標																</param>
	/// <param name="_endPos">		BOXの終わりの座標															</param>
	/// <param name="_hp">			ステージオブジェクトのHP													</param>
	/// <returns>					生成された範囲の識別ナンバー　StageObjectPutAreaBoxHPChangeの引数の使用		</returns>
	int StageObjectPutAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos,const int& _hp);

	/// <summary>
	///								ステージのオブジェクトを削除した範囲（BOX）を送る　ナビゲーションのNavPointが範囲内にあった場合NavPointをアクティブ化する
	/// </summary>
	/// <param name="_startPos">	BOXの始めの座標				</param>
	/// <param name="_endPos">		BOXの終わりの座標			</param>
	void StageObjectClearAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos);

	/// <summary>
	/// オブジェクトに潰されたナビポイントを一つのボクセルとして管理する	
	/// １オブジェクト(このプロジェクトの場合罠のオブジェクト)に１つ	
	/// </summary>
	struct VoxelNavNodeData
	{
		Transform voxelTransfrom;										// ボクセルのトランスフォーム
		int hp;															// ボクセルのHP
		
		// ボクセルの底面の中心座標　
		// ボクセルの中心座標よりも底面のほうが元々あたナビポイントの座標に近い 
		// ToDo 平均座標のほうが適切な可能性あるので平均座標に変更
		VECTOR3 voxelBottomPosition;																					
		NODE_STATE state;												// ボクセルのノード状態

		std::unordered_map<int, std::list<int>> blockedNavPointList;	// 潰されたナビポイントのナンバーと接続相手のナンバーを保持
		std::set<int> targetList;										// 自身とほかのボクセルの接続情報を保持

		VoxelNavNodeData(const std::unordered_map<int, std::list<int>>& _blockedCon, const std::set<int>& _targetCon, const Transform& _trans, const int& _hp)
			: blockedNavPointList(_blockedCon), targetList(_targetCon), voxelTransfrom(_trans), hp(_hp)
		{
			voxelBottomPosition		= _trans.position;
			voxelBottomPosition.y	-= _trans.scale.y * _trans.size.y;
			state					= NODE_STATE::NONE;
		}

		VoxelNavNodeData(const std::unordered_map<int, std::list<int>>& _blockedCon, const Transform& _trans, const int& _hp)
			: blockedNavPointList(_blockedCon), voxelTransfrom(_trans), hp(_hp)
		{
			targetList.clear();

			voxelBottomPosition		= _trans.position;
			voxelBottomPosition.y	-= _trans.scale.y * _trans.size.y;
			state					= NODE_STATE::NONE;
		}

		VoxelNavNodeData()
		{
			blockedNavPointList.clear();
			targetList.clear();
			voxelTransfrom		= Transform();
			hp					= 0;
			voxelBottomPosition = VZero;
			state				= NODE_STATE::NONE;
		}

		/// <summary>
		///							ボクセル同士の接続相手ナンバーの設定
		/// </summary>
		/// <param name="_target">	接続相手ナンバー				</param>
		void PushTarget(const int& _target) { targetList.emplace(_target); }
	};

	/// <summary>
	///							voxelNavListに保存したボクセルのHP情報を変更
	/// </summary>
	/// <param name="_key">		PushVoxelNavNodeで返ってきたコンテナのkey		</param>
	/// <param name="_hp">		変更するHP										</param>
	void VoxelNavNodeHPChange(const int& _key, const int& _hp);

#ifdef _DEBUG

	void DebugDeleteVoxelWayPointNumberTarget(int _key) { debugTargetDataDeletedVoxelWayPointNumber.emplace(_key); }
	
	
	static std::unordered_set<int> debugDeleteNavPointNumberCheckList;

#endif // _DEBUG



private:
	
	/// <summary>
	///				経路探索に使う全てのNavPointが入ったnavigationNavPointListコンテナを返す
	/// </summary>
	/// <returns>	navigationNavPointList			</returns>
	std::unordered_map<int, NavPointBase*>& GetNavigationPointContainer() { return navigationNavPointList; }

	/// <summary>
	///									削除される予定のナビポイントの接続相手だったナビポイントから引数の削除される予定のナビポイントとの接続をやめる
	/// </summary>
	/// <param name="_deleteNavPoint">	削除される予定のナビポイント					</param>
	void DeleteNavPointToTargetDisconnect(NavPointBase* _deleteNavPoint);

	/// <summary>
	///							voxelNavListにナビポイント情報をpushする
	/// </summary>
	/// <param name="_voxel">	pushするナビポイント情報(key:生成ナンバー / value:接続相手ナンバーコンテナ)		</param>
	/// <param name="_voxel">	pushするボクセルのエリアデータ													</param>
	/// <returns>				pushした際のコンテナのkey														</returns>
	int PushVoxelNavNode(std::unordered_map<int,std::list<int>>& _navCon,const NavPointUpdater::AreaData& _voxel);

	/// <summary>
	///							voxelNavListに保存したナビポイント情報を削除
	/// </summary>
	/// <param name="_key">		PushVoxelNavNodeで返ってきたコンテナのkey		</param>
	/// <param name="_num">		削除するナビポイントナンバー					</param>
	void DeleteVoxelInNavNode(const int& _key,const int& _num);

	/// <summary>
	///	voxelNavListを返す
	/// </summary>
	/// <returns></returns>
	const std::unordered_map<int, VoxelNavNodeData>& GetVoxelNavList() { return voxelNavList; }

	/// <summary>
	///							ボクセルのノードデータ同士の接続をリクエストする	
	/// </summary>
	/// <param name="_me">		自身のボクセルのノードデータ					</param>
	/// <param name="_target">	相手のボクセルのノードデータ					</param>
	void RequestConnectVoxelNavNode(VoxelNavNodeData& _me, VoxelNavNodeData& _target);

	/// <summary>
	///								引数で渡されたNODE_USE_KINDが経路探索で使うことができるか　int値のNODE_USE_KINDに含まれるか返す
	/// </summary>
	/// <param name="_settingKind">	使うノードの種類NODE_USE_KINDを一つのint値に入れた値 </param>
	/// <param name="_checkKind">   使えるかどうか調べるNODE_USE_KIND </param>
	/// <returns>					true：使える false:使えない </returns>
	bool CanUseNavPoint(const int& _useKind, const NODE_USE_KIND& _checkKind);

	/// <summary>
	///								ナビポイントの状態から経路探索に使えるかどうか返す
	/// </summary>
	/// <param name="_mode">		現在設定されているNAVIGATION_MODEのint値	</param>
	/// <param name="_checkMode">	ナビポイントの潰されているかの状態			</param>
	/// <returns></returns>
	bool CanUseTypeNavPoint(const int& _mode, const NODE_BLOCK_STATE& _checkMode);

	/*/// <summary>
	/// 潰された
	/// </summary>
	/// <param name="_agentKind">			</param>
	/// <param name="_voxelKey"> 			</param>
	/// <returns>							</returns>
	float GetCostRateNavPoint(const int& _agentKind, const int& _voxelKey);*/

	

	/// <summary>
	///						コンテナ内に含まれるナビポイントがどのNODE_USE_KINDなのか総数を返す
	/// </summary>
	/// <param name="_con"> 調べるナビポイントコンテナ			</param>
	/// <returns>			key:NODE_USE_KIND / value:総数		</returns>
	std::unordered_map<int, int> GetInContainerNodeUseKindNum(const std::unordered_map<int, NavPointBase*>& _con);

	/// <summary>
	///						GetInContainerNodeUseKindNum関数の値を描画	この関数の前にImgui::Begin後にImgui::Endを呼んでください
	/// </summary>
	/// <param name="_con"> GetInContainerNodeUseKindNum関数の値 </param>
	void ImGuiDrawInContainerNodeUseKindNum(const std::unordered_map<int, int>& _con);

	WayPointController* wayPointController	= nullptr;
	NavMeshController* navMeshController	= nullptr;
	NavPointUpdater* navPointUpdater		= nullptr;
	EnemyManager* enemyManager				= nullptr;
	StageManager* stageManager				= nullptr;
	Player* player							= nullptr;

	int navPointCreateNum = 0;				// NavPointの生成数

	// 経路探索に使える状態のNavPointを管理するコンテナ
	std::unordered_map<int, NavPointBase*> activeNavPointList;		

	// 経路探索に使えない状態のNavPointを管理するコンテナ
	std::unordered_map<int, NavPointBase*> inactiveNavPointList;	

	// VoxelNavNodeDataを持ったナビポイントを保持する　１オブジェクトに１つのナビポイントデータを持つことによってナビポイントの削減につながる
	// オブジェクトが潰された時に潰されたナビポイント情報を保持したナビポイントを生成管理
	// key:生成ナンバー(navPointCreateNum) / value:潰されたナビポイント情報
	std::unordered_map<int, VoxelNavNodeData> voxelNavList;

	// activeNavPointListとvoxelNavListをNavPointとして扱ったNavPointを管理するコンテナ
	// 経路探索で潰されたナビポイントも含めた経路探索をするときに使用　オブジェクトを取り除き、ナビポイントが復活したときのルート予想が可能
	std::unordered_map<int, NavPointBase*> navigationNavPointList;

	bool isChangedContainer = false;			// ナビゲーションに

	bool isNavPointRootDraw = false;			// NavPointのルートの描画
	int drawRootKind		= 0;				// 描画するルートの種類
	bool isNavPointDraw		= false;			// NavPointの描画

	//_ デバッグ _//

	std::unordered_map<int, std::list<int>> debugNavgationList;
	std::unordered_set<int> debugOpenedNavPointList;
	std::unordered_set<int> debugVoxelList;

	std::unordered_map<NavPointBase*, std::set<NavPointBase*>> debugCostRateUpNavPointList;	// 経路探索時にコスト倍率が上げられたナビポイントを保存

	std::unordered_map<int, std::list<Cost>> debugNavPointCostList;							// 経路探索時のナビポイントのコストを記録

	std::unordered_map<int, std::list<int>> debugNavCountList;								// 経路探索にナビポイントのナンバーが何回使われたかカウント

	std::set<int> debugDeletedVoxelWayPointNumber;											// 削除されたVoxelWayPointのナンバーを記録
	std::set<int> debugTargetDataDeletedVoxelWayPointNumber;								// TagretDataから削除されたVoxelWayPointのナンバーを記録
};

