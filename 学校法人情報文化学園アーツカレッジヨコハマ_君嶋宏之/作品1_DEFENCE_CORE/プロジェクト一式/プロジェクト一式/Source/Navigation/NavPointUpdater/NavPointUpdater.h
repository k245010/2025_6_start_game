#pragma once
#include "../../../Library/GameObject.h"
#include "../../Transform/Transform.h"
#include <unordered_map>
#include <set>

class StageObjectBase;
class Transform;
struct CollisionHitInfoData;
class NavigationManager;
class NavPointBase;
/// <summary>
/// NavPointの状態を更新するクラス	ステージとの当たり判定をして状態を更新
/// </summary>
class NavPointUpdater : public GameObject
{
	friend NavigationManager;

public:

	NavPointUpdater();
	~NavPointUpdater();

	/// <summary>
	///								ステージ初期時点でのステージオブジェクトとナビポイントの当たり判定を行い、当たったナビポイントのナンバーを返す
	/// </summary>
	/// <param name="_list">		ステージオブジェクトコンテナ	</param>
	/// <param name="_navManager">	ナビゲーションマネージャー		</param>
	/// <returns>					当たったナビポイントのナンバー  </returns>
	std::set<int> Init(const std::list<StageObjectBase*> _list, NavigationManager* _navManager);

	void Update()	override;
	void Draw()		override;

	/// <summary>
	/// 当たり判定で当たったNavPointをコンテナに保存するときの状態
	/// </summary>
	enum class IN_CONTAINER_STATE
	{
		INIT = 0,	// 初期化
		IN_CONT,	// 当たり判定で当たったNavPointをコンテナに保存している最中
		END			// 当たり判定で当たったNavPointをコンテナに保存し終わった
	};

	/// <summary>
	/// 当たり判定で当たったNavPointをコンテナに保存するときの状態を返す
	/// </summary>
	/// <returns> 当たり判定で当たったNavPointをコンテナに保存するときの状態 </returns>
	IN_CONTAINER_STATE GetInContainerState() const { return inContainerState; }

	/// <summary>
	/// sphereCollの全要素の当たり判定を行うかどうかのフラグをtrue
	/// 当たり判定で当たったNavPointのナンバーを保存するコンテナをリセット
	/// </summary>
	void ReloadCollision();

	/// <summary>
	///	当たり判定で当たったNavPointのナンバーを保存するコンテナをリセット
	/// </summary>
	/// <param name="_num"> NavPointナンバー </param>
	/// <param name="findChecked"> find関数でsphereCollListコンテナ内に要素が存在することを確かめたかどうか 確かめた:確かめていない </param>
	void ReloadCollision(const int& _num, bool findChecked = false);

	/// <summary>
	/// 当たり判定を行うコンテナにNavPointを登録する
	/// </summary>
	/// <param name="_ptr"> NavPointBaseポインタ </param>
	/// <returns></returns>
	bool CreateCollision(NavPointBase* _ptr);

	/// <summary>
	/// 当たり判定で当たったNavPointのナンバーを保存するコンテナをclear
	/// </summary>
	void ResetCollisionHitContainer();

	/// <summary>
	/// 当たり判定で当たったNavPointのナンバーを保存するコンテナを返す
	/// </summary>
	/// <returns> 当たったNavPointのナンバーを保存するコンテナ </returns>
	const std::set<int>& GetHitContainer() const { return hitNavPointList; }

	/// <summary>
	/// 当たり判定で当たっていないNavPointのナンバーを保存するコンテナを返す
	/// </summary>
	/// <returns> 当たっていないNavPointのナンバーを保存するコンテナ </returns>
	const std::set<int>& GetNoHitContainer() const { return noHitNavPointList; }

	/// <summary>
	/// 当たり判定で新たにNavPointコンテナに追加されたNavPointのナンバーを保存するコンテナを返す
	/// </summary>
	/// <returns> 新たにNavPointコンテナに追加されたNavPointのナンバーを保存するコンテナ </returns>
	const std::set<int>& GetNewPushContainer() const { return newPushNavPointContainer; }

	/// <summary>
	/// アクティブ化するNavPointのナンバーコンテナを返す
	/// </summary>
	/// <returns> アクティブ化するNavPointのナンバーコンテナ </returns>
	//std::set<int> GetActivatingNavPointContainer() const { return activatingNavPointList; }

	/// <summary>
	/// 非アクティブ化するNavPointのナンバーコンテナを返す
	/// </summary>
	/// <returns> 非アクティブ化するNavPointのナンバーコンテナ </returns>
	//std::set<int> GetDeactivatingNavPointContainer() const { return deactivatingNavPointList; }

	/// <summary>
	/// 当たり判定関係のNavPointのナンバーを保存するコンテナに値が入ったかどうか返す
	/// </summary>
	/// <returns> 値が入った：値が入っていない </returns>
	bool InContainer() const { return inContainer; }

	/// <summary>
	/// 当たり判定を行うかのフラグをtrueにする
	/// </summary>
	/// <param name="_num"></param>
	//void CollisionActiving(const int& _num);

	/// <summary>
	/// 当たり判定を行うか返す
	/// </summary>
	/// <param name="_num"></param>
	/// <returns> true : false </returns>
//	bool IsCollisionActive(const int& _num);

	/// <summary>
	///							ステージのオブジェクトを置いた範囲（BOX）を送る　ナビゲーションのNavPointを非アクティブ化する
	/// </summary>
	/// <param name="_trans">	ステージのオブジェクトのトランスフォーム									</param>
	/// <param name="_hp">		ステージオブジェクトのHP													</param>
	/// <returns>				生成された範囲の識別ナンバー　StageObjectPutAreaBoxHPChangeの引数の使用		</returns>
	int StageObjectPutAreaBox(const Transform& _trans, const int& _hp);

	/// <summary>
	///								ステージのオブジェクトを置いた範囲（BOX）を送る　ナビゲーションのNavPointを非アクティブ化する
	/// </summary>
	/// <param name="_startPos">	BOXの始めの座標																</param>
	/// <param name="_endPos">		BOXの終わりの座標															</param>
	/// <param name="_hp">			ステージオブジェクトのHP													</param>
	/// <returns>					生成された範囲の識別ナンバー　StageObjectPutAreaBoxHPChangeの引数の使用		</returns>
	int StageObjectPutAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos, const int& _hp);

	/// <summary>
	///								ステージのオブジェクトを削除した範囲（BOX）を送る　ナビゲーションのNavPointが範囲内にあった場合NavPointをアクティブ化する
	/// </summary>
	/// <param name="_startPos">	BOXの始めの座標															</param>
	/// <param name="_endPos">		BOXの終わりの座標															</param>
	void StageObjectClearAreaBox(const Transform& _trans);

	/// <summary>
	///								ステージのオブジェクトを削除した範囲（BOX）を送る　ナビゲーションのNavPointが範囲内にあった場合NavPointをアクティブ化する
	/// </summary>
	/// <param name="_startPos">	BOXの始めの座標																</param>
	/// <param name="_endPos">		BOXの終わりの座標															</param>
	void StageObjectClearAreaBox(const VECTOR3& _startPos, const VECTOR3& _endPos);

	/// <summary>
	///							ステージのオブジェクトを置いた範囲（BOX）のHP情報を変更する
	/// </summary>
	/// <param name="_trans">	StageObjectPutAreaBoxで返ってきた識別ナンバー				</param>
	/// <param name="_hp">		変更するHP													</param>
	void StageObjectPutAreaBoxHPChange(const int& _number, const int& _hp);

	/// <summary>
	/// ボクセル(ナビゲーションマネージャーのVoxelNavNodeData)のエリアデータ
	/// </summary>
	struct AreaData
	{
		AreaData()												: transform(Transform()), stageObjectHp(0)		{}
		AreaData(const Transform& _trans, const int& _costRate)	: transform(_trans), stageObjectHp(_costRate)	{}

		Transform transform;	// トランスフォーム
		int stageObjectHp;	// コスト倍率
	};

private:

	/// <summary>
	/// ステージのオブジェクトが生成or削除されたときに当たり判定を行う際の、ステージのオブジェクト状態の種類
	/// </summary>
	enum class STAGE_OBJECT_CHANGED_KIND
	{
		PUT = 0,	// 生成された
		CLEAR,		// 削除された
	};

	/// <summary>
	///				当たり判定の処理を回す
	/// </summary>
	/// <returns>	当たった:ボクセルのオーナーキー / 当たってない:-1	</returns>
	int CollisionUpdate();

	/// <summary>
	///							OBBと点の当たり判定を行う
	/// </summary>
	/// <param name="_stageTr"> ステージのトランスフォーム			</param>
	/// <param name="_navP">	点の座標							</param>
	/// <returns>				true:当たった / false:当たってない	</returns>
	bool OBBToNavPointColl(const Transform& _stageTr, const VECTOR3& _navP) const;

	/// <summary>
	///							AABBとレイを使ったエッジの当たり判定を行う　ToDo OBBに対応
	/// </summary>
	/// <param name="_trans">	AABBのオブジェクトトランスフォーム		</param>
	/// <param name="_navP1">	レイの始点ナビポイント					</param>
	/// <param name="_navP2">	レイの終点ナビポイント					</param>
	/// <returns>				true:当たっている　false:当たっていない </returns>
	bool AABBToNavPointEdgeColl(const Transform& _trans, const VECTOR3& _navP1, const VECTOR3& _navP2) const;
	
	/// <summary>
	///								当たり判定で当たったときの処理を行う
	/// </summary>
	/// <param name="_num">			ナンバー												</param>
	/// <param name="_targetData">	相手の当たり判定情報									</param>
	/// <returns>					自身の当たり判定クラスポインタを破棄する：破棄しない	</returns>
	//bool HitNavPoint(const int& _num, const CollisionHitInfoData& _targetData);

	/// <summary>
	///								２点の座標からBOXを構築したトランスフォームを返す
	/// </summary>
	/// <param name="_startPos">	BOXの始めの座標			</param>
	/// <param name="_endPos">		BOXの終わりの座標		</param>
	/// <returns>					BOXトランスフォーム		</returns>
	Transform GetPositionToTransform(const VECTOR3& _startPos, const VECTOR3& _endPos) const;

	NavigationManager* navigationManager = nullptr;

	std::list<StageObjectBase*> stageObjList;						// ステージオブジェクトのコンテナ
	std::unordered_map<int,NavPointBase*> navPointList;				// NavPointのコンテナ　	voxelWayPointは含まない

	std::unordered_multimap<int, AreaData> stageObjectChangedArea;	// ステージのオブジェクトが生成or削除された範囲をAreaDataとして保存するコンテナ
	//std::unordered_map<Transform,int> stageObjectChangedTransform;	// ステージのオブジェクトが生成or削除された範囲をトランスフォームとして保存するコンテナ

	//std::list<StageObjectBase*> stageObjectList;					// ステージのオブジェクト全てを保持するリスト

	std::set<int> hitNavPointList;			// 当たり判定で当たったNavPointを保存するコンテナ（一度当たったらReload関数を呼ぶかメモリ解放まで値を保持）
	std::set<int> noHitNavPointList;		// 当たり判定で当たっていないNavPointを保存するコンテナ
	std::set<int> newPushNavPointContainer;	// 当たり判定で新たにNavPointコンテナにpushされたNavPointを保存するコンテナ　ステージが変化したことによってNavPointのアクティブ、非アクティブが変化したか見る

	std::set<int> activatingNavPointList;	// NavigationManager側でアクティブ化するNavPointのナンバーコンテナ 要素は１フレームだけ保持（NavigationManagerで毎フレームこのコンテナを見てNavPointのコンテナを操作するため）　	ステージオブジェクトが削除されて、かぶさらなくなったら追加
	std::set<int> deactivatingNavPointList;	// NavigationManager側で非アクティブ化するNavPointのナンバーコンテナ 要素は１フレームだけ保持（NavigationManagerで毎フレームこのコンテナを見てNavPointのコンテナを操作するため）　	ステージオブジェクトとかぶさったら追加

	IN_CONTAINER_STATE inContainerState = IN_CONTAINER_STATE::INIT;	// 当たり判定で当たったNavPointをコンテナに保存するときの状態
	bool inContainer					= false;					// コンテナに値が入ったかどうか

	//-- デバッグ
	std::list<Transform> saveTr;
};