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
	/// ブロックオブジェクトの削除　// 今後はprivateに移行　オブジェクトの破壊でdeleteしたいときは、damage関数的なのどで行う
	/// </summary>
	/// <param name="_obj"> ブロックオブジェクト </param>
	/// <returns> 成功：失敗 </returns>
	bool DeleteBlock(BlockBase* _obj);
	
	/// <summary>
	/// ユーザーが配置をするための関数
	/// </summary>
	/// <param name="_trans"> トランスフォーム </param>
	/// <param name="_kind"> ステージオブジェクトの種類 </param>
	/// <returns> 成功 生成したオブジェクトのトランスフォームポインタ：失敗 nullptr </returns>
	const Transform* PutBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	/// ブロックオブジェクトの生成
	/// </summary>
	/// <param name="_trans"> トランスフォーム </param>
	/// <param name="_kind"> ステージオブジェクトの種類 </param>
	void CreateBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind);

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
	/// <returns> 
	///				const std::unordered_map<int, std::vector<int>>型のハンドル	valueのコンテナには罠用のレベル１～のモデルが入っている
	/// 
	///				key:ステージオブジェクトのブロックナンバー / value:ハンドル
	/// </returns>
	const std::unordered_map<int, std::vector<int>> GetRawModelHandle();

private:

	BlockController();
	~BlockController();

	/// <summary>
	/// ブロックオブジェクトの生成
	/// </summary>
	/// <param name="_trans"> トランスフォーム </param>
	/// <param name="_kind"> ステージオブジェクトの種類 </param>
	/// <param name="createWay"> 生成の方法 </param>
	/// <returns> 生成したオブジェクトのトランスフォームポインタ </returns>
	const Transform* CreateBlock(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, const STAGE_OBJECT_CREATE_WAY& createWay);

	/// <summary>
	/// グリッド配置する時の座標を返す
	/// </summary>
	/// <param name="_trans"></param>
	/// <returns></returns>
	const VECTOR3 GetPutGridPosition(const Transform& _trans);

	static BlockController* objectMe;								// BlockControllerクラスのインスタンス
	static bool isActiveInstance;									// インスタンスがnewされているかのフラグ
	StageManager* stageManager;										// StageManagerクラスポインタ

	std::unordered_map<BlockBase*, STAGE_OBJECT_CREATE_WAY> blocks;	// ブロックオブジェクトと生成の方法を保存するコンテナ
	std::unordered_map<int, std::vector<int>> hModel;				// ブロックオブジェクトの種類とモデルハンドルのコンテナ
	std::unordered_map<int, int> hPreviewModel;						// ブロックオブジェクトの種類とモデルハンドルの半透明専用コンテナ
};
