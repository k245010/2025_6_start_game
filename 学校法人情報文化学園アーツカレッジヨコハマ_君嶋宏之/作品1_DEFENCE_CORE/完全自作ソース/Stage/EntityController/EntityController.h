#pragma once
#include "../../../Library/GameObject.h"
#include "../../Transform/Transform.h"
#include "../../Stage/StageObjectData.h"
//#include "BulletInfo.h"
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <list>

class StageObjectBase;
class EntityBase;
class StageManager;
class ShotManager;
/// <summary>
/// エンティティを総括するクラス
/// </summary>
class EntityController
{
public:

	/// <summary>
	/// StageManagerクラスポインタの初期化
	/// </summary>
	/// <param name="_stageManager"> StageManagerクラスポインタ </param>
	void Init(StageManager* _stageManager);

	/// <summary>
	/// EntityControllerインスタンスを返す
	/// </summary>
	/// <returns></returns>
	static EntityController* GetEntityController();

	/// <summary>
	/// EntityControllerクラスのインスタンスを削除する
	/// </summary>
	static void DeleteEntityController();

	/// <summary>
	///				インスタンスがnewされているかどうか
	/// </summary>
	/// <returns>	true:newされている / false:newされていない　 </returns>
	static bool IsActiveInstance() { return isActiveInstance; }

	void Update();

	/// <summary>
	///						タレット情報のUI描画をするかどうか設定する
	/// </summary>
	/// <param name="_set">	タレット情報のUI描画をするかどうか	</param>
	void SetDrawTurretInfoUIFlag(bool _set) { isDrawTurretInfoUI = _set; }

	/// <summary>
	///							設置する罠を半透明で描画する	
	/// </summary>
	/// <param name="_mouse">	描画する罠のトランスフォーム		</param>
	/// <param name="_kind">	ステージオブジェクト(罠の種類)		</param>
	/// <param name="_alpha">	不透明度							</param>
	void DrawTrapPutPreview(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind,const float& _alpha);

	/// <summary>
	/// エンティティオブジェクトの削除　// 今後はprivateに移行　オブジェクトの破壊でdeleteしたいときは、damage関数的なのどで行う
	/// </summary>
	/// <param name="_obj"> ブロックオブジェクト </param>
	/// <returns> 成功：失敗 </returns>
	bool DeleteEntity(EntityBase* _obj);

	/// <summary>
	///							エンティティを召喚をするための関数
	/// </summary>
	/// <param name="_trans">	トランスフォーム </param>
	/// <param name="_kind">	ステージオブジェクトの種類 </param>
	/// <returns>				成功：設置したオブジェクトのトランスフォームポインタ / 失敗:nullptr </returns>
	const Transform* SummonEntity(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	/// 一番近くの敵のトランスフォームを返す
	/// </summary>
	/// <param name="_pos"> 自身の座標 </param>
	/// <returns></returns>
	const Transform& GetNearEnemyTransform(const VECTOR3& _pos);

	/// <summary>
	/// EnemyManegrの関数を使い、一番近くの敵の座標を返す
	/// </summary>
	/// <param name="_pos"> 自身の座標 </param>
	/// <returns> 敵の座標 </returns>
	const VECTOR3& GetNearEnemyPosition(const VECTOR3& _pos);

	/// <summary>
	/// EnemyManegrの関数を使い、指定されたナンバーの敵の座標を返す
	/// </summary>
	/// <param name="_num"> 敵のナンバー </param>
	/// <returns> 敵の座標 </returns>
	const VECTOR3& GetEnemyPosition(const int _eneNumber);

	/// <summary>
	/// 敵が一体も召喚されているか返す
	/// </summary>
	/// <returns> 召喚されている：召喚されていない </returns>
	bool IsEnemySummon() const;

	/// <summary>
	///				プレイヤーのトランスフォームを返す
	/// </summary>
	/// <returns>	プレイヤーのトランスフォーム </returns>
	const Transform& GetPlayerTransform() const;

	/// <summary>
	/// すべてのエンティティを削除する
	/// </summary>
	void StageManagerForDeleteEntity();

	/// <summary>
	/// StageManager経由でShotManagerの関数を使い、発射間隔を設定　Shot関数を呼ぶ前に呼ぶ
	/// </summary>
	/// <param name="_pointer"> 識別用ポインタ(thisを入れる) </param>
	/// <param name="_interval"> 発射までの間隔 秒 </param>
	//void SetShotInterval(void* _pointer, const float _interval);

	/// <summary>
	/// Shotを使うオーナークラスポインタをコンテナから削除
	/// </summary>
	//void DeleteShotOwner(const void* _pointer);

	/// <summary>
	/// StageManager経由でShotManagerの関数を使い、通常単発射撃
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	/// <param name="_vel"> べロシティ </param>
	/// <param name="_bulletKind"> 弾の種類 </param>
	/// <param name="_pointer"> 識別用ポインタ(thisを入れる) </param>
	/// <returns> 成功：失敗 </returns>
	//bool Shot(const VECTOR3& _pos, const VECTOR3& _vel, const BULLET_KIND& _bulletKind, void* _pointer);

	/// <summary>
	/// エンティティオブジェクトを基底クラスコンテナで返す
	/// </summary>
	/// <returns> エンティティオブジェクトの基底クラスポインタ型ローカルコンテナ </returns>
	std::list<StageObjectBase*> GetAllStageObject();

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
	///							モデルハンドルを返す
	/// </summary>
	/// <param name="_kind">	ほしいモデルハンドルのステージオブジェクトの種類	</param>
	/// <returns>				モデルハンドル		</returns>
	const std::vector<int>& GetModelHandle(const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	///				ShotManagerクラスポインタを返す
	/// </summary>
	/// <returns>	ShotManagerクラスポインタ			</returns>
	ShotManager* GetShotManager() const { return shotManager; }

	/// <summary>
	///				StageManagerクラスポインタを返す
	/// </summary>
	/// <returns>	StageManagerクラスポインタ			</returns>
	StageManager* GetStageManager() const { return stageManager; }

	/// <summary>
	///				円形のモデルハンドルを返す
	/// </summary>
	/// <returns>	円形のモデルハンドル	</returns>
	//int GetCircleModel() const { return circleModel; }

	/// <summary>
	///								エンティティのテクスチャハンドルを返す
	/// </summary>
	/// <param name="_fileName">	ファイル名	.pngはなし						</param>
	/// <returns>					エンティティのテクスチャハンドルのコンテナ	</returns>
	int GetEntityTextureHandle(std::string_view _fileName) const
	{
		if (entityTextureHandleList.find(std::string(_fileName)) == entityTextureHandleList.end())
			assert(false && "ファイルが見つかりませんでした。");

		return entityTextureHandleList.at(std::string(_fileName));
	}

private:

	EntityController();
	~EntityController();

	/// <summary>
	///							エンティティの召喚
	/// </summary>
	/// <param name="_trans">	トランスフォーム </param>
	/// <param name="_kind">	ステージオブジェクトの種類											</param>
	/// <returns>				成功:生成したオブジェクトのトランスフォームポインタ	/ 失敗:nullptr	</returns>
	const Transform* CreateEntity(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	/// タレット情報のUI描画処理
	/// </summary>
	void TurretInfoUIDraw();

	/// <summary>
	/// グリッド配置する時の座標を返す
	/// </summary>
	/// <param name="_trans"> トランスフォーム </param>
	/// <returns> 座標 </returns>
	VECTOR3 GetPutGridPosition(const Transform& _trans);

	static EntityController* objectMe;					// EntityControllerのインスタンス
	static bool isActiveInstance;						// インスタンスがnewされているかのフラグ
	StageManager* stageManager	= nullptr;				// StageManagerのクラスポインタ
	ShotManager* shotManager	= nullptr;

	std::unordered_set<EntityBase*> entities;			// エンティティのコンテナ
	std::unordered_map<int, std::vector<int>> hModel;	// エンティティの種類とモデルハンドルのコンテナ
	std::unordered_map<int, int> hPreviewModel;			// エンティティの種類とモデルハンドルの半透明専用コンテナ

	bool isDrawTurretInfoUI		= false;				// タレットの情報のUI描画をするかどうか

	std::unordered_map<std::string, int> entityTextureHandleList;	// エンティティのテクスチャハンドルのコンテナ

	int turretUIFunctionIDNumber = -1;	// TurretInfoUIDraw関数に対するIDナンバー
};
