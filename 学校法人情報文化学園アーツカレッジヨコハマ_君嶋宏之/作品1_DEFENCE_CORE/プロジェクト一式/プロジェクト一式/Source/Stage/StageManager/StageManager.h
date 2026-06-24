#pragma once
#include "../../../Library/GameObject.h"
#include "../../Transform/Transform.h"
#include "../StageObjectData.h"
#include <set>
#include <string>

class ITrap;
class NavigationManager;
class SphereCollision;
class EntityController;
class BlockController;
class EnemyManager;
class StageObjectBase;
class BlockBase;
class StageEditor;
/// <summary>
/// ステージの生成を総括するクラス
/// </summary>
class StageManager : public GameObject
{
	friend StageEditor;
	friend BlockController;

public:

	StageManager();
	~StageManager();

	/// <summary>
	/// クラスポインタの初期化
	/// </summary>
	/// <param name="_loadStageNum"> 読み込むステージナンバー </param>
	void Init(int _loadStageNum);

	void Update()	override;
	void Draw()		override;

	/// <summary>
	///						StageEditorでステージの編集をするかのフラグ設定
	/// </summary>
	/// <param name="_flag">true:編集する / false：編集しない	</param>
	void SetStageEditorFlag(bool _flag) { canStageEditor = _flag; }

	/// <summary>
	///				ステージの最大数を返す
	/// </summary>
	/// <returns>	ステージの最大数					</returns>
	static int GetStageNumberMax() { return StageInfo::MAX_STAGE_NUM; }

	/// <summary>
	///				インスタンスがnewされているかどうか
	/// </summary>
	/// <returns>	true:newされている / false:newされていない　 </returns>
	static bool IsActiveInstance() { return isActiveInstance; }

	/// <summary>
	/// ロードしたステージのナンバーを返す
	/// </summary>
	/// <returns> ステージナンバー </returns>
	int GetLoadStageNum() const { return loadStageNum; }

	/// <summary>
	///							ステージデータをcsvが読み込みステージを生成
	/// </summary>
	/// <param name="_stageNum">ステージナンバー 1～					</param>
	/// <returns>				true:読み込み成功 / false:読み込み失敗	</returns>
	bool CreateStage(const int& _stageNum);

	/// <summary>
	///							ステージデータをcsvに書き出し
	/// </summary>
	/// <param name="_stageNum">ステージナンバー 1～		</param>
	/// <param name="_override">データを上書きするかどうか	</param>
	void SaveStage(const int& _stageNum, bool _override = true);

	/// <summary>
	///				ステージに対するプレイヤーの初期座標をロードして返す
	/// </summary>
	/// <returns>	プレイヤーの初期座標 </returns>
	VECTOR3 LoadPlayerStartPosition();

	/// <summary>
	///				ステージマップの中心地点の座標を返す
	/// </summary>
	/// <returns>	中心座標 </returns>
	VECTOR3 GetStageCenterPosition() const;

	/// <summary>
	///							選択しているステージオブジェクトの設置ポイントを変更する
	/// </summary>
	/// <param name="_moveNum"> 変更する値 (-1,1) </param>
	/// <returns>				true:成功 / false:失敗 </returns>
	bool MoveStageObjectPutPointSelect(int _moveNum);

	/// <summary>
	/// 現在選択しているステージオブジェクトの設置ポイントの座標を返す
	/// </summary>
	const VECTOR3& GetStageObjectPutPointSelectPosition()
	{
		if (stageObjectPutPointCenterPositionList.find(stageObjectPutPointSelectData.number) == stageObjectPutPointCenterPositionList.end())
			assert(false);

		return stageObjectPutPointCenterPositionList[stageObjectPutPointSelectData.number];
	}

	/// <summary>
	/// ステージオブジェクトを設置するポイントの個々の配置インデックスのデータ
	/// </summary>
	struct PutPointIndexData
	{
		PutPointIndexData()																		: number(-1),	position(VZero),	putStageObjectKind(-1),		isPut(false)	{}
		PutPointIndexData(const int& _num, const VECTOR3& _pos)									: number(_num), position(_pos),		putStageObjectKind(-1),		isPut(false)	{}
		PutPointIndexData(const int& _num, const VECTOR3& _pos, const int& _kind,bool _isPut)	: number(_num), position(_pos),		putStageObjectKind(_kind),	isPut(_isPut)	{}

		int number;				// 生成ナンバー
		VECTOR3 position;		// 座標
		int putStageObjectKind;	// 設置したステージオブジェクトの種類　設置していなかったら-1
		bool isPut;				// ステージオブジェクトを配置しているかどうか
	};

	/// <summary>
	///						ステージオブジェクトの設置ポイントにステージオブジェクトを生成する
	/// </summary>
	/// <param name="_cm">	カメラ座標 </param>
	/// <param name="_kind">ステージオブジェクトの種類 </param>
	/// <param name="_m">	マウスポインタ座標 </param>
	/// <returns>			true:成功 / false:失敗 </returns>
	bool CreatePutPointStageObject(const VECTOR3& _cm, const StageObjectData::STAGE_OBJECT_KIND& _kind, const VECTOR2I& _m);

	/// <summary>
	///						ステージオブジェクトの設置ポイントにあるステージオブジェクトを削除する
	/// </summary>
	/// <param name="_cm">	カメラ座標																				</param>
	/// <param name="_m">	マウスポインタ座標																		</param>
	/// <returns>			true:成功 / false:削除されたステージオブジェクトの種類をintにキャストした値　失敗:-1	</returns>
	int DeletePutPointStageObject(const VECTOR3& _cm, const VECTOR2I& _m);

	/// <summary>
	///							設置する罠を半透明で描画する	
	/// </summary>
	/// <param name="_mouse">	マウス座標							</param>
	/// <param name="_kind">	ステージオブジェクト(罠の種類)		</param>
	/// <param name="_alpha">	不透明度							</param>
	void DrawTrapPutPreview(const VECTOR2I& _mouse, const StageObjectData::STAGE_OBJECT_KIND& _kind,const float& _alpha);

	/// <summary>
	///						罠の影響範囲を描画するかを設定
	/// </summary>
	/// <param name="_set">	true:描画する / false:描画しない	</param>
	void SetDrawTrapImpactRadiusFlag(bool _set) { isDrawTrapImpactRadius = _set; }

	/// <summary>
	///								ステージオブジェクトを生成する
	/// </summary>
	/// <param name="_pos">			生成座標														</param>
	/// <param name="_kind">		ステージオブジェクトの種類										</param>
	/// <param name="_velocity">	指定した座標から1フレーム前の座標の方向ベクトル　設置判定で使う </param>
	/// <returns>					true:成功 / false:失敗											</returns>
	//bool CreateStageObject(const VECTOR3& _pos, const StageObjectData::STAGE_OBJECT_KIND& _kind, const VECTOR3& _velocity = VZero);

	/// <summary>
	///								ステージオブジェクトを設置する
	/// </summary>
	/// <param name="_pos">			設置座標						</param>
	/// <param name="_kind">		ステージオブジェクトの種類		</param>
	/// <returns>					true:成功 / false:失敗			</returns>
	bool PutStageObject(const VECTOR3& _pos, const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	///								ステージオブジェクトを生成する
	/// </summary>
	/// <param name="_trans">		トランスフォーム						</param>
	/// <param name="_kind">		ステージオブジェクトの種類				</param>
	/// <returns>					true:成功 / false:失敗					</returns>
	bool PutStageObject(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind);
	
	/// <summary>
	///						敵のスポナーを設置する
	/// </summary>
	/// <param name="_pos">	設置座標										</param>
	/// <returns>			設置したスポナーのステージオブジェクトポインタ	</returns>
	const StageObjectBase* PutSpawner(const VECTOR3& _pos);

	/// <summary>
	///							敵のスポナーを削除する
	/// </summary>
	/// <param name="_spawner">	StageObjectBaseポインタ		</param>
	/// <returns>				true:成功 / false:失敗		</returns>
	bool DeleteSpawner(StageObjectBase* _spawner);

	/// <summary>
	/// 全てのスポナーを削除する
	/// </summary>
	void DeleteAllSpawner();

	/// <summary>
	/// ステージオブジェクトの設置ポイント関連の移動方向の種類
	/// </summary>
	/*enum class PUT_POINT_MOVE_DIRECTION_KIND
	{
		PUT = 0,
		DOWN,
		RIGHT,
		LEFT,
	};*/

	/// <summary>
	/// ステージオブジェクトの設置ポイントのインデックスの移動
	/// </summary>
	/// <param name="_move"> 移動方向 </param>
	/// <returns> 成功：失敗 </returns>
	//bool StageObjectPutPointIndexMove(const PUT_POINT_MOVE_DIRECTION_KIND& _move);

	/// <summary>
	/// ステージオブジェクトの設置ポイントの移動
	/// </summary>
	/// <param name="_move"> 移動方向 </param>
	/// <returns> 成功：失敗 </returns>
	//bool StageObjectPutPointMove(const PUT_POINT_MOVE_DIRECTION_KIND& _move);

	/// <summary>
	/// ステージオブジェクトを削除する範囲のトランスフォームを送り、その範囲内のステージオブジェクトを削除する	// 現在は球での当たり判定での対応
	/// </summary>
	/// <param name="_trans"> 範囲のトランスフォーム </param>
	//void DeleteStageObjectInTransfrom(const Transform& _trans);

	/// <summary>
	/// ブロックオブジェクトのコンテナを返す
	/// </summary>
	/// <returns> ブロックオブジェクトポインタコンテナ </returns>
	const std::list<BlockBase*> GetAllBlockObject();

	/// <summary>
	/// ステージオブジェクトのコンテナを返す
	/// </summary>
	/// <returns> ステージオブジェクトコンテナ </returns>
	const std::list<StageObjectBase*> GetAllStageObject();


	/// <summary>
	///							レイとステージオブジェクトタイプごとの当たり判定
	/// <para>					非対応タイプ:																</para>
	/// <para>					STAGE_OBJECT_KIND::CORE_BLOCK												</para>
	/// <para>					STAGE_OBJECT_KIND::SLIME_PATCH												</para>
	/// <para>					STAGE_OBJECT_KIND::SLIME_PATCH												</para>
	/// </summary>
	/// <param name="_pos1">	レイの始点																	</param>
	/// <param name="_pos2">	レイの終始																	</param>
	/// <param name="_type">	どのステージオブジェクトタイプで当たり判定するか　デフォルトNONE -> すべて	</param>
	/// <param name="_hitP">	当たった座標																</param>
	/// <returns>				当たった：当たってない														</returns>
	//bool StageObjectTypeRayCollision(const VECTOR3& _pos1, const VECTOR3& _pos2, const StageObjectData::TYPE& _type = StageObjectData::TYPE::NONE, VECTOR3* _hitP = nullptr);

	/// <summary>
	///							レイとステージオブジェクト種類ごとの当たり判定
	/// </summary>
	/// <param name="_pos1">	レイの始点																	</param>
	/// <param name="_pos2">	レイの終始																	</param>
	/// <param name="_type">	どのステージオブジェクト種類で当たり判定するか　デフォルトNONE -> すべて	</param>
	/// <param name="_hitP">	当たった座標																</param>
	/// <returns>				当たった：当たってない														</returns>
	//bool StageObjectKindRayCollision(const VECTOR3& _pos1, const VECTOR3& _pos2, const StageObjectData::STAGE_OBJECT_KIND& _kind, VECTOR3* _hitP = nullptr);

	/// <summary>
	///							レイとステージオブジェクト種類ごとの当たり判定
	/// </summary>
	/// <param name="_startPos">レイの始点																						</param>
	/// <param name="_endPos">	レイの終始																						</param>
	/// <param name="_type">	どのステージオブジェクト種類で当たり判定するかのコンテナ　コンテナが空の場合はすべて当たり判定	</param>
	/// <param name="_hitP">	当たった座標																					</param>
	/// <returns>				true:当たっていた / false:当たっていない														</returns>
	bool CheckRaycastStageObject(const VECTOR3& _startPos, const VECTOR3& _endPos, const std::set<int>& _kindList, VECTOR3* _hitPos = nullptr);

	/// <summary>
	///								レイの当たり判定をして、メンテナンスをすることができる罠を返す
	/// </summary>
	/// <param name="_startPos">		レイの始点																														</param>
	/// <param name="_endPos">		レイの終始																														</param>
	/// <returns>					レイキャストをしてメンテナンスできる罠に当たっていたら当たったステージオブジェクトを返す / 当たっていなかったらnullptrを返す	</returns>
	const StageObjectBase* RaycastGetCanMaintainTrap(const VECTOR3& _startPos, const VECTOR3& _endPos);

	/// <summary>
	///								罠のメンテナンスをする
	/// </summary>
	/// <param name="_stageObj">	メンテナンスする罠のStageObjectBaseポインタ															</param>
	/// <param name="_add">			進行させるメンテナンス値	0.0f～1.0f(0～100%)														</param>
	/// <returns>					レイキャストをして当たっていたら現在のメンテナンスの進行値 0.0f～1.0f / 当たっていなかったら-1.0f	</returns>
	float TrapMaintaining(const StageObjectBase* _stageObj, const float& _add);

	/// <summary>
	///							レイの当たり判定をして、アップグレードをすることができる罠を返す
	/// </summary>
	/// <param name="_startPos">レイの始点																							</param>
	/// <param name="_endPos">	レイの終始																							</param>
	/// <returns>				StageObjectBaseポインタ:レイキャストをしてアップグレードできる罠のポインタ / nullptr:当たっていない	</returns>
	const StageObjectBase* RaycastGetCanUpgradTrap(const VECTOR3& _startPos, const VECTOR3& _endPos);

	/// <summary>
	///								罠のアップグレードをする
	/// </summary>
	/// <param name="_stageObj">	アップグレードする罠のStageObjectBaseポインタ														</param>
	/// <param name="_add">			進行させるアップグレード値	0.0f～1.0f(0～100%)														</param>
	/// <returns>					レイキャストをして当たっていたら現在のアップグレードの進行値 0.0f～1.0f / 当たっていなかったら-1.0f	</returns>
	float TrapUpgrade(const StageObjectBase* _stageObj, const float& _add);

	/// <summary>
	///								罠の現在のレベルを返す
	/// </summary>
	/// <param name="_stageObj">	ステージオブジェクトの種類							</param>
	/// <returns>					罠が存在したら:現在のレベル / 存在しなかったら:-1	</returns>
	int GetTrapLevel(const StageObjectBase* _stageObj);

	/// <summary>
	///						ステージオブジェクトの設置ポイントにあるステージオブジェクトがコントローラー側で削除された際に、削除したことを知らせる
	/// </summary>
	/// <param name="_pos"> 削除されたステージオブジェクトのトランスフォーム </param>
	void SendDeletePutPointStageObject(const Transform& _trans);

	/// <summary>
	/// ステージに変化が起きたときに呼ぶ
	/// </summary>
	//void ChangedStageObject();

	/// <summary>
	///				ステージオブジェクトを設置するポイントの中心座標コンテナを返す
	/// </summary>
	/// <returns>	ステージオブジェクトを設置するポイントの中心座標コンテナ </returns>
	std::vector<VECTOR3> GetStageObjectPutPointList() const;

	/// <summary>
	///				コアブロックの座標ポインタを返す
	/// </summary>
	/// <returns>	コアブロックの座標ポインタ </returns>
	const VECTOR3* GetCorePosition() const;

	/// <summary>
	///				コアが壊れたかどうか返す
	/// </summary>
	/// <returns>	true:壊れた / false:壊れていない </returns>
	bool IsCoreBroken();

	/// <summary>
	///						一番近くの敵のトランスフォームを返す
	/// </summary>
	/// <param name="_pos"> 自身の座標							</param>
	/// <returns>			一番近くの敵のトランスフォーム		</returns>
	const Transform& GetNearEnemyTransform(const VECTOR3& _pos) const;

	/// <summary>
	///						EnemyManagerの関数を使い、一番近くの敵の座標を返す
	/// </summary>
	/// <param name="_pos"> 自身の座標	</param>
	/// <returns>			敵の座標	</returns>
	const VECTOR3& GetNearEnemyPosition(const VECTOR3& _pos);

	/// <summary>
	///						EnemyManagerの関数を使い、指定されたナンバーの敵の座標を返す
	/// </summary>
	/// <param name="_num"> 敵のナンバー	</param>
	/// <returns>			敵の座標		</returns>
	const VECTOR3& GetEnemyPosition(const int& _eneNumber);

	/// <summary>
	///				EnemyManagerの関数を使い、敵が召喚されているかどうか返す
	/// </summary>
	/// <returns>	true:召喚されてる / false:召喚されていない </returns>
	bool IsEnemySummon() const;

	/// <summary>
	/// 罠の操作の種類
	/// </summary>
	enum class TRAP_CONTROL_KIND
	{
		PUT = 0,	// 罠の設置
		CLAER,		// 罠の削除
	};

	/// <summary>
	///							設置したステージオブジェクトトランスフォーム(BOX)内のナビゲーションを改訂する
	/// </summary>
	/// <param name="_trans">	ステージオブジェクトトランスフォームポインタ	</param>
	/// <param name="_hp">		ステージオブジェクトのHP						</param>
	void NavigationPutAreaBox(const Transform* _trans, const int& _hp);

	/// <summary>
	///							削除された範囲トランスフォーム(BOX)内のナビゲーションを改訂する
	/// </summary>
	/// <param name="_trans">	トランスフォーム								</param>
	void NavigationClearAreaBox(const Transform& _trans);

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
	///				ハンドルをロードしてロードしたハンドルコンテナを返す
	/// </summary>
	/// <returns> 
	///				const std::unordered_map<int, int>型のハンドル
	/// 
	///				key:ステージオブジェクトのブロックナンバー / value:ハンドル
	/// </returns>
	const std::unordered_map<int, int> GetRawModelHandles();

	/// <summary>
	///								引数のStageObjectBaseポインタがITrap(罠のインターフェイス)クラスを持っていたらコンテナにpushする
	/// </summary>
	/// <param name="_stageObje">	StageObjectBaseポインタ															</param>
	/// <returns>					true:ITrapを持ったかつコンテナに追加できた / false:コンテナに追加できなかった	</returns>
	bool PushInterfaceTrapList(StageObjectBase* _stageObj);

	/// <summary>
	///								引数のStageObjectBaseポインタをITrap(罠のインターフェイス)クラスを管理しているコンテナから削除する
	/// </summary>
	/// <param name="_stageObje">	StageObjectBaseポインタ															</param>
	void DeleteInterfaceTrapList(StageObjectBase* _stageObj);

	/// <summary>
	///							一番近い罠オブジェクトを返す
	/// </summary>
	/// <param name="_pos">		自身の座標																							</param>
	/// <param name="_max">		捜索距離の最大値																					</param>
	/// <returns>				一番近い罠オブジェクトをみけられたら:一番近い罠オブジェクトポインタ / 見つけられなかったら:nullptr	</returns>
	const ITrap* GetNearTrap(const VECTOR3& _pos,const float& _max);

	/// <summary>
	///							プレイヤーのトランスフォームを送る　毎フレームプレイヤーが呼ぶ
	/// </summary>
	/// <param name="_trans">	プレイヤーのトランスフォーム </param>
	void SendPlayerTransform(const Transform& _trans) { playerTransform = _trans; }

	/// <summary>
	///				プレイヤーのトランスフォームを返す
	/// </summary>
	/// <returns>	プレイヤーのトランスフォーム </returns>
	const Transform& GetPlayerTransform() const { return playerTransform; }

	/// <summary>
	///							タレット情報のUI描画を行うかのフラグ
	/// </summary>
	/// <param name="_set">　	true:描画する / false:描画しない　</param>
	void SetTurretInfoUIDrawFlag(bool _set) { isDrawTurretInfoUI = _set; }

private:

	/// <summary>
	///									ステージオブジェクトを生成する
	/// </summary>
	/// <param name="_pos">				生成座標															</param>
	/// <param name="_kind">			ステージオブジェクトの種類											</param>
	/// <param name="_useModelIndex">	使用するモデル										</param>
	/// <param name="_isYPosAdjustment">Y座標を調整するかのフラグ											</param>
	/// <returns>						StageObjectBaseポインタ:生成したオブジェクトポインタ / nullptr:失敗	</returns>
	StageObjectBase* CreateStageObject(const VECTOR3& _pos, const StageObjectData::STAGE_OBJECT_KIND& _kind, bool _isYPosAdjustment = true);

	/// <summary>
	///								ステージオブジェクトを生成する
	/// </summary>
	/// <param name="_trans">		トランスフォーム														</param>
	/// <param name="_kind">		ステージオブジェクトの種類												</param>
	/// <returns>					StageObjectBaseポインタ::生成したオブジェクトポインタ / nullptr:失敗	</returns>
	StageObjectBase* CreateStageObject(const Transform& _trans, const StageObjectData::STAGE_OBJECT_KIND& _kind, bool _isYPosAdjustment = true);

	/// <summary>
	///										ステージオブジェクトを削除する
	/// </summary>
	/// <param name="_deleteStageObject">	削除するStageObjectBaseポインタ	</param>
	/// <returns>							true:成功 / false:失敗			</returns>
	bool DeleteStageObject(StageObjectBase* _deleteStageObject);

	/// <summary>
	///							ステージデータをcsvに書き出し
	/// </summary>
	/// <param name="_stageNum">ステージナンバー 1～				</param>
	/// <param name="_stageNum">true:削除の成功 / false:削除の失敗	</param>
	bool DeleteStage(const int& _stageNum);

	/// <summary>
	///											ステージオブジェクトの設置ポイントにステージオブジェクトを生成する
	/// </summary>
	/// <param name="_putPointIndexDataPtr">	設置するポイントの個々の配置インデックスのデータのポインタ		</param>
	/// <param name="_kind">					ステージオブジェクトの種類										</param>
	/// <returns>								true:設置できた / false:設置できていない						</returns>
	bool CreatePutPointStageObject(PutPointIndexData* _putPointIndexDataPtr, const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	///							ステージオブジェクトの設置ポイントにステージオブジェクトを生成する
	/// </summary>
	/// <param name="_putPos">	設置座標(一番近い設置ポイントの座標に補間される)	</param>
	/// <param name="_kind">	ステージオブジェクトの種類							</param>
	/// <returns>				true:設置できた / false:設置できていない			</returns>
	bool CreatePutPointStageObject(const VECTOR3& _putPos, const StageObjectData::STAGE_OBJECT_KIND& _kind);

	/// <summary>
	///							レイとステージオブジェクト種類ごとの当たり判定
	/// </summary>
	/// <param name="_pos1">	レイの始点																						</param>
	/// <param name="_pos2">	レイの終始																						</param>
	/// <param name="_type">	どのステージオブジェクト種類で当たり判定するかのコンテナ　コンテナが空の場合はすべて当たり判定	</param>
	/// <param name="_hitP">	当たった座標																					</param>
	/// <returns>				StageObjectBaseポインタ:当たったオブジェクトポインタ / nullptr:当たっていない					</returns>
	StageObjectBase* RaycastStageObject(const VECTOR3& _startPos, const VECTOR3& _endPos, const std::set<int>& _kindList, VECTOR3* _hitPos = nullptr);

	/// <summary>
	///							レイの当たり判定をして、罠のステージオブジェクトポインタを返す
	/// </summary>
	/// <param name="_startPos">レイの始点																</param>
	/// <param name="_endPos">	レイの終始																</param>
	/// <returns>				StageObjectBaseポインタ:当たった罠のポインタ / nullptr:当たっていない	</returns>
	const StageObjectBase* RaycastGetTrap(const VECTOR3& _startPos, const VECTOR3& _endPos);

	/// <summary>
	///				ステージオブジェクトのコンテナを返す
	/// </summary>
	/// <returns>	ステージオブジェクトコンテナ </returns>
	std::list<StageObjectBase*> GetStageObjectContainer();

	/// <summary>
	///							ステージオブジェクトを設置するポイントを読み込みコンテナに代入する
	/// </summary>
	/// <param name="_stageNum">ステージナンバー 1～					</param>
	/// <returns>				true:読み込み成功 / false:読み込み失敗	</returns>
	bool LoadStageObjectPutPointData(const int& _stageNum);

	/// <summary>
	///							csvから読み込んだ値を引数Transformに代入
	/// </summary>
	/// <param name="_trans">	Transform	</param>
	/// <param name="_num">		代入値		</param>
	/// <param name="col">		列			</param>
	void SetTransformToCsvColumn(Transform& _trans, const float& _num, const int& _col);

	/// <summary>
	///							csvから読み込んだ値を引数VECTORに座標として代入
	/// </summary>
	/// <param name="_trans">	VECTOR3		</param>
	/// <param name="_num">		代入値		</param>
	/// <param name="col">		列			</param>
	void SetPositionFromCsvColumn(VECTOR3& _pos, const float& _num, const int& _col);

	/// <summary>
	/// csvから読み込んだ現在選択されているステージオブジェクトを設置するポイントのインデックスの座標を設定する
	/// </summary>
	/// <param name="_num">		ポイントのナンバー	</param>
	/// <param name="_line">	csvデータの列		</param>
	/// <param name="_columns"> csvデータの行		</param>
	/// <param name="_cenPos">	ポイントの中心座標	</param>
	//void InitStageObjectPointIndexToIndexPosition(const int& _num,const int& _line,const int& _columns,const VECTOR3& _cenPos);

	/// <summary>
	///							マウスポインタ座標からワールド座標に変換した座標とカメラの座標で方向ベクトルをつくってステージオブジェクトと当たり判定をする
	/// </summary>
	/// <param name="_musePos"> マウス座標							</param>
	/// <param name="_hitPos">	当たった座標ポインタ				</param>
	/// <returns>				true:当たった / false:当たってない	</returns>
	bool GetScreenPosToWorldPosRayColl(const VECTOR2I& _musePos,VECTOR3* _hitPos);

	/// <summary>
	///									引数の座標から近くの、ステージオブジェクトを設置するポイントのインデックスポインタを返す
	/// </summary>
	/// <param name="_pos">				座標																															</param>
	/// <param name="_putedReturnNull"> 設置されていたときにnullptrを返すかどうか																						</param>
	/// <returns>						PutPointIndexDataポインタ:近くのPutPointIndexDataポインタ / nullptr:すでに設置されているまたは、明らかにポイントから遠いとき	</returns>
	StageManager::PutPointIndexData* GetNearPutPointDataPtr(const VECTOR3& _pos, bool _putedReturnNull = false);

	/// <summary>
	///							罠の影響範囲を描画する　攻撃範囲など
	/// </summary>
	/// <param name="_pos">		原点座標				</param>
	/// <param name="_radius">	範囲の半径				</param>
	void DrawTrapImpactRadius(const VECTOR3& _pos, const float& _radius) const;

	static bool isActiveInstance;							// インスタンスがnewされているかのフラグ
	
	//GameController* gameController;
	StageEditor* stageEditor						= nullptr;
	EnemyManager* enemyManager						= nullptr;
	NavigationManager* navigationManager			= nullptr;
	SphereCollision* deleteStageObjectCollision		= nullptr;		// ステージオブジェクトを削除するための当たり判定
	Transform deleteStageObjectCollisionTransform	= Transform();	// ステージオブジェクトを削除するための当たり判定のトランスフォーム
	
	bool canStageEditor = false;
	
	std::unordered_map<int, std::vector<StageManager::PutPointIndexData>> stageObjectPutPointIndexPosList;	// ステージオブジェクトを設置するポイントの個々の配置インデックスのデータコンテナ
	std::unordered_map<int, VECTOR3> stageObjectPutPointCenterPositionList;									// ステージオブジェクトを設置するポイントの中心座標のコンテナ 
	
	std::unordered_map<const Transform*, int> navigationAreaBoxNumberList;									// ナビゲーションへ送ったステージオブジェクトのトランスフォーム範囲の識別ナンバーを保存する
	std::unordered_map<const StageObjectBase*, ITrap*> interfaceTrapList;									// ITrap(罠のインターフェース)を管理するコンテナ

	/// <summary>
	/// 選択されている設置ポイントのデータ 今後データが少なかったら削除
	/// </summary>
	struct PutPointSelectData
	{
		PutPointSelectData() : number(0) {}
		PutPointSelectData(int ind,int x,int z) : number(ind) {}

		int number;	// ポイントのナンバー
	};

	PutPointSelectData stageObjectPutPointSelectData;	// 現在選択されているステージオブジェクトの配置をするポイントの位置データ

	int loadStageNum			= -1;		// ステージナンバー

	Transform playerTransform;				// プレイヤー座標
	bool isDrawTrapImpactRadius = false;	// 罠の影響範囲を描画するかのフラグ
	bool isDrawTurretInfoUI		= false;	// 罠のトラップの情報描画のフラグ

	int circleModel				= -1;		// 円形のモデルハンドル
};

