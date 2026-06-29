#pragma once
#include "../../../Library/GameObject.h"
#include "../../Transform/Transform.h"
#include "../StageObjectData.h"
//#include "StageManager.h"
#include <unordered_map>
#include <vector>
#include <list>



/// <summary>
/// 生成の方法
/// </summary>
enum class STAGE_OBJECT_CREATE_WAY
{
	STATIC = 0, // 静的配置　ステージ
	DYNAMIC,    // 動的配置　プレイヤーが配置
};

class StageObjectBase;
class BlockBase;
class StageManager;
/// <summary>
/// ブロックの生成を総括するクラス
/// </summary>
class BlockController
{
	friend StageManager;

public:

	/// <summary>
	/// StageManagerクラスポインタを初期化。
	/// </summary>
	/// <param name="_stageManager"> 初期化する StageManager オブジェクトへのポインタ </param>
	void Init(StageManager* _stageManager);

	/// <summary>
	/// BlockControllerクラスのインスタンスを返す
	/// </summary>
	/// <returns></returns>
	static BlockController* GetBlockController();

	/// <summary>
	/// BlockControllerクラスのインスタンスを削除する
	/// </summary>
	static void DeleteBlockController();

	/// <summary>
	///				インスタンスがnewされているかどうか
	/// </summary>
	/// <returns>	true:newされている / false:newされていない　 </returns>
	static bool IsActiveInstance() { return isActiveInstance; }

	/// <summary>
	///							設置する罠を半透明で描画する	
	/// </summary>
	/// <param name="_mouse">	描画する罠のトランスフォーム		</param>
	/// <param name="_kind">	ステージオブジェクト(罠の種類)		</param>
	/// <param name="_alpha">	不透明度							</param>
	void DrawTrapPutPreview(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const float& _alpha);

	/// <summary>
	///						ブロックオブジェクトの削除　// 今後はprivateに移行　オブジェクトの破壊でdeleteしたいときは、damage関数的なのどで行う
	/// </summary>
	/// <param name="_obj"> ブロックオブジェクト </param>
	/// <returns>			true:成功 / false:失敗 </returns>
	bool DeleteBlock(BlockBase* _obj);

	/// <summary>
	///						ブロックオブジェクトの削除
	/// </summary>
	/// <param name="_kind">削除したいブロックのステージオブジェクトの種類	</param>
	/// <returns>			true:成功 / false:失敗							</returns>
	bool DeleteBlock(const StageObjectData::STAGE_OBJECT_KIND& _kind);
	
	/// <summary>
	///							ユーザーが配置をするための関数
	/// </summary>
	/// <param name="_trans">	トランスフォーム							</param>
	/// <param name="_kind">	ステージオブジェクトの種類					</param>
	/// <returns>				StageObjectBaseポインタ:成功 / nullptr:失敗	</returns>
	StageObjectBase* PutBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	/// すべてのブロックオブジェクトを削除する
	/// </summary>
	void StageManagerForDeleteBlock();

	/// <summary>
	/// 特定の方法で生成されたブロックオブジェクトをすべて削除する
	/// </summary>
	/// <param name="createWay"></param>
	void StageManagerForDeleteBlock(const STAGE_OBJECT_CREATE_WAY& createWay);

	/// <summary>
	/// グロックオブジェクトのコンテナを返す
	/// </summary>
	/// <returns> グロックオブジェクトポインタ型ローカルコンテナ </returns>
	const std::list<BlockBase*> GetAllBlockObject();

	/// <summary>
	/// グロックオブジェクトを基底クラスでコンテナを返す
	/// </summary>
	/// <returns> ブロックオブジェクトの基底クラスポインタ型ローカルコンテナ </returns>
	std::list<StageObjectBase*> GetAllStageObject();

	/// <summary>
	/// コアブロックの座標ポインタを返す
	/// </summary>
	/// <returns> コアブロックの座標ポインタ </returns>
	const VECTOR3* GetCorePosition() const;

	/// <summary>
	/// コアが壊れたかどうか返す
	/// </summary>
	/// <returns> true:壊れた false:壊れていない </returns>
	bool IsCoreBroken();

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
	///							トランスフォーム(BOX)内のナビゲーションを改訂する
	/// </summary>
	/// <param name="_trans">	ステージオブジェクトのトランスフォームポインタ			</param>
	/// <param name="_hp">		ステージオブジェクトのHP								</param>
	void NavigationAreaBoxHPChange(const Transform* _trans, const int& _hp);

	/// <summary>
	///							navigationAreaBoxNumberListに保存したステージオブジェクトのトランスフォームポインタを削除する
	/// </summary>
	/// <param name="_trans">	トランスフォームポインタ </param>
	void DeleteNavigationAreaBoxTransformPtr(const Transform* _trans);

	/// <summary>
	///				ロードしたハンドルコンテナを返す
	/// </summary>
	///				const std::unordered_map<int, std::vector<int>>型のハンドル	valueのコンテナには複数のモデルが入っている
	/// 
	///				key:ステージオブジェクトのブロックナンバー / value:ハンドル
	const std::unordered_map<int, std::vector<int>>& GetModelHandles() const { return hModels; }

	/// <summary>
	///				モデルをロードし、ロードしたハンドルコンテナを返す
	/// </summary>
	/// <returns>
	///				const std::unordered_map<int, std::vector<int>>型のハンドル	valueのコンテナには複数のモデルが入っている
	/// 
	///				key:ステージオブジェクトのブロックナンバー / value:ハンドル
	/// </returns>
	const std::unordered_map<int, std::vector<int>> GetRawModelHandles();

	/// <summary>
	///				スポナーのポインタが入ったコンテナを返す
	/// </summary>
	/// <returns>	const std::list<const StageObjectBase*>&	</returns>
	const std::list<const StageObjectBase*>& GetSpawnerList() const { return spawnerList; }

private:

	BlockController();
	~BlockController();

	/// <summary>
	///									ブロックオブジェクトの生成
	/// </summary>
	/// <param name="_trans">			トランスフォーム							</param>
	/// <param name="_kind">			ステージオブジェクトの種類					</param>
	/// <param name="_useModelIndex">	使用するモデルのインデクス番号				</param>
	/// <returns>						StageObjectBaseポインタ:成功 / nullptr:失敗	</returns>
	StageObjectBase* CreateBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _useModelIndex);

	/// <summary>
	///									ブロックオブジェクトの生成
	/// </summary>
	/// <param name="_trans">			トランスフォーム								</param>
	/// <param name="_kind">			ステージオブジェクトの種類						</param>
	/// <param name="_useModelIndex">	使用するモデルのインデクス番号					</param>
	/// <param name="createWay">		生成の方法										</param>
	/// <returns>						BlockBaseポインタ:成功 / nullptr:失敗			</returns>
	BlockBase* CreateBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const int& _useModelIndex, const STAGE_OBJECT_CREATE_WAY& _createWay);

	/// <summary>
	///							グリッド配置する時の座標を返す
	/// </summary>
	/// <param name="_trans">	トランスフォーム		</param>
	/// <returns>				グリッド変換した座標	</returns>
	VECTOR3 GetPutGridPosition(const Transform& _trans);

	/// <summary>
	///									スポナーのポインタをコンテナにpushする
	/// </summary>
	/// <param name="_spawnerPointer">	スポナーのStageObjectBaseポインタ	</param>
	/// <returns>						true:成功 / false:失敗				</returns>
	bool PushSpawnerToList(const StageObjectBase* _spawner);

	/// <summary>
	///									スポナーのポインタをPushSpawnerToList関数でpushしたコンテナから削除をリクエストする
	/// </summary>
	/// <param name="_spawnerPointer">	スポナーのStageObjectBaseポインタ	</param>
	/// <returns>						true:成功 / false:失敗				</returns>
	bool DeleteRequestSpawnerFormList(const StageObjectBase* _spawner);

	static BlockController* objectMe;	// BlockControllerクラスのインスタンス
	static bool isActiveInstance;		// インスタンスがnewされているかのフラグ
	StageManager* stageManager;			// StageManagerクラスポインタ

	std::unordered_map<BlockBase*, STAGE_OBJECT_CREATE_WAY> blocks;	// ブロックオブジェクトと生成の方法を保存するコンテナ
	std::unordered_map<int, std::vector<int>> hModels;				// ブロックオブジェクトの種類とモデルハンドルのコンテナ
	std::unordered_map<int, int> hPreviewModel;						// ブロックオブジェクトの種類とモデルハンドルの半透明専用コンテナ

	std::list<const StageObjectBase*> spawnerList;					// スポナーを登録するコンテナ  StageManagerがスポナーの位置を把握するため  
};
