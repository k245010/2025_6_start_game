#pragma once
#include "../../../../Library/GameObject.h"
#include "../../../Stage/StageObjectData.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

/// <summary>
/// 対称ペアハッシュ　例）pairのint a,b == int b,a 順不同の組み合わせ比較
/// </summary>
struct SymmetricPairHash
{
	std::size_t operator()(const std::pair<int, int>& p) const noexcept // noexceptはこの関数は例外を投げませんの意思表示
	{
		std::size_t ha = std::hash<int>{}(p.first);
		std::size_t hb = std::hash<int>{}(p.second);
		
		// __ haの方が値を小さくする
		if (ha > hb)
			std::swap(ha, hb);

		// hb << 1 → 左に1ビットシフトしてビットを拡散
		// hb >> (ビット幅 - 1) → 上位ビットを下位に回す
		// 	| で OR → ビットを「ぐるっと回して混ぜる」
		return ha ^ ((hb << 1) | (hb >> ((sizeof(std::size_t) * 8) - 1)));
	}
};

class NavPointUpdater;
class StageManager;
class Player;
class PortalPoint;
struct PolygonData;
struct VertexData;
struct VertexDataHash;
struct PortalData;
class NavMeshController : public  GameObject
{
public:

	NavMeshController();
	~NavMeshController();

	/// <summary>
	/// NavMesh(PortalPoint)の初期化を行う
	/// </summary>
	/// <param name="_player"> プレイヤー </param>
	/// <param name="_stageManager"> ステージ </param>
	/// <param name="_doNavSetting"> NavMesh(PortalPoint)の初期化を行い、そのデータをファイル出力するかどうか </param>
	void Init(Player* _player, StageManager* _stageManager,bool _doNavSetting);

	/// <summary>
	/// NavMeshから初期化したPortalPointのコンテナを返す
	/// </summary>
	/// <returns> activePortalPointList(初期化時は全てactive状態のNavPointなのでactivePortalPointListを返す) </returns>
	std::unordered_map<int, PortalPoint*>& GetInitCreatePortalPointContainer() { return activePortalPointList; }

	void Update()	override;
	void Draw()		override;

private:

	/// <summary>
	/// ポリゴン情報からNavMeshを構築し、ポータルポイントの生成とポリゴン情報の設定を行う
	/// </summary>
	void PolygonForNavMeshDataSetting();

	/// <summary>	
	///											頂点2点(AB)を持つポリゴンナンバーを返す
	/// </summary>
	/// <param name="_localAllVerticesDataList">頂点情報コンテナ					</param>
	/// <param name="numA">						頂点A								</param>
	/// <param name="numB">						頂点B								</param>
	/// <returns>								ポリゴンナンバーが入ったコンテナ	</returns>
	std::unordered_set<int> GetPortalPointShaerdPolygonNumber(std::unordered_map<int, VertexData>& _localAllVerticesDataList,const int& _numA,const int& _numB);

	/// <summary>
	///											ポータルを構築する頂点2点(AB)を持つポリゴンの法線をコンテナで返す
	/// </summary>
	/// <param name="_localAllVerticesDataList">頂点情報コンテナ		</param>
	/// <param name="_localAllPolygonDataList">	ポリゴン情報コンテナ	</param>
	/// <param name="_portalPoint">				調べたいポータル		</param>
	/// <returns>								法線が入ったコンテナ	</returns>
	std::vector<VECTOR3> GetPortalPointShaerdPolygonNorm
	(
		std::unordered_map<int, VertexData>& _localAllVerticesDataList, std::unordered_map<int, PolygonData>& _localAllPolygonDataList, const PortalPoint& _portalPoint
	);

	/// <summary>
	///													モデルに対するポータルポイントコンテナの設定をする
	/// </summary>
	/// <param name="_localVerticesAndTargetDataList">	モデルの種類分ローカル頂点の座標を見て自身と接続している相手を設定するコンテナ	</param>
	/// <param name="_localPortalPointList">			モデルに対するポータルポイントコンテナ											</param>
	/// <param name="_localAllVerticesDataList">		頂点情報コンテナ																</param>
	/// <param name="_localAllPolygonDataList">			ポリゴン情報コンテナ															</param>
	/// <param name="_portalPairList">					ポリゴン同士で共有しているポータル頂点の順不同のペアコンテナ					</param>
	/// <param name="_modelMaxPosition">				モデルのポリゴンの最大座標														</param>
	/// <param name="_modelMatrix">						モデルの行列																	</param>
	/// <param name="_modelOwnerPtr">					モデルハンドルを保持した、クラスのポインタ										</param>
	void ModelPortalPointListSetting
	(
		const std::unordered_map<int, std::unordered_set<VertexData, VertexDataHash>>& _localVerticesAndTargetDataList,
		std::unordered_map<int, PortalPoint*>& _localPortalPointList,
		std::unordered_map<int, VertexData>& _localAllVerticesDataList,
		std::unordered_map<int, PolygonData>& _localAllPolygonDataList,
		std::unordered_set<std::pair<int, int>, SymmetricPairHash>& _portalPairList,
		const VECTOR3& _modelMaxPosition,
		const MATRIX& _modelMatrix,
		const void* _modelOwnerPtr
	);

	/// <summary>
	///												モデルに対するポータルポイントの接続相手を設定する
	/// </summary>
	/// <param name="_localPortalPointList">		モデルに対するポータルポイントコンテナ			</param>
	/// <param name="_modelPortalPointCreateNum">	モデルに対するポータルポイントの生成ナンバー	</param>
	/// <param name="_localAllPolygonDataList">		ポリゴンのローカルコンテナ						</param>
	/// <param name="_localAllVerticesDataList">	頂点データのローカルコンテナ					</param>
	/// <param name="_firstNum">					ナンバーの値が小さい方の頂点ナンバー A			</param>
	/// <param name="_secondNum">					ナンバーの値が大きい方の頂点ナンバー B			</param>
	void ModelPortalPointTargetSetting(std::unordered_map<int, PortalPoint*>& _localPortalPointList, const int& _modelPortalPointCreateNum, std::unordered_map<int, PolygonData>& _localAllPolygonDataList, std::unordered_map<int, VertexData>& _localAllVerticesDataList, const int& _firstNum,const int& _secondNum);

	/// <summary>
	///										法線コンテナを見て、自身と相手を接続するかどうか返す
	/// </summary>
	/// <param name="_meNormContainer">		自身のポータルポイントの法線コンテナ					</param>
	/// <param name="_targetNormContainer">	相手のポータルポイントの法線コンテナ					</param>
	/// <returns>							true:接続できる / false:接続できない					</returns>
	bool CanConnectPortalPoint(const std::vector<VECTOR3>& _meNormContainer, const std::vector<VECTOR3>& _targetNormContainer);

	/// <summary>
	///											worldVerticesDataListに頂点データをpushする				
	/// </summary>
	/// <param name="_localVerticesDataList">	頂点データのローカルコンテナ	</param>
	/// <param name="_blockTrans">				モデルののトランスフォーム		</param>
	void CreateWorldVerticesDataList(std::unordered_map<int, VertexData>& _localVerticesDataList, const Transform& _modelTrans);
	
	/// <summary>
	///											worldPoygonDataListにポリゴンデータをpushする				
	/// </summary>
	/// <param name="_localAllPolygonDataList">	ポリゴンのローカルコンテナ	</param>
	/// <param name="_blockTrans">				モデルのトランスフォーム	</param>
	void CreateWorldPoygonDataList(std::unordered_map<int, PolygonData>& _localAllPolygonDataList, const Transform& _modelTrans);

	/// <summary>
	///												ローカルポータルポイントコンテナを適用する
	/// </summary>
	/// <param name="_modelForLocalPortalPointList">モデルのポリゴンから構築された、ポータルコンテナ	</param>
	/// <param name="_modelKind">					モデルの種類										</param>
	/// <param name="_modelTrans">					モデルのトランスフォーム							</param>
	/// <param name="_localAllVerticesDataList">	頂点情報コンテナ									</param>
	/// <param name="_localAllPolygonDataList">		ポリゴン情報コンテナ								</param>
	/// <param name="_initPortalPointList">			適用するローカルポータルポイントコンテナ			</param>
	void ApplyInitPortalPointList(std::unordered_map<int, std::unordered_map<int, PortalPoint*>>& _modelForLocalPortalPointList, const int& _modelKind, const Transform& _modelTrans, std::unordered_map<int, VertexData>& _localAllVerticesDataList, std::unordered_map<int, PolygonData>& _localAllPolygonDataList, std::unordered_map<int, PortalPoint*>& _initPortalPointList);

	/// <summary>
	///												ローカルポータルポイントコンテナの接続相手の適用をする
	/// </summary>
	/// <param name="_initPortalPointList">			適用するローカルポータルポイントコンテナ			</param>
	void ApplyInitPortalPointTarget(std::unordered_map<int, PortalPoint*>& _initPortalPointList);

	/// <summary>
	///										ローカルポータルポイントコンテナから、activePortalPointListのポータルポイントコンテナを生成する
	/// </summary>
	/// <param name="_initPortalPointList">	ローカルポータルポイントコンテナ	</param>
	void CreatePortalPoint(std::unordered_map<int, PortalPoint*>& _initPortalPointList);

	/// <summary>
	///									NavMesh(PortalPoint)の初期化を行う
	/// </summary>
	/// <param name="_doNavSetting">	NavMesh(PortalPoint)を初期化を行い、そのデータをファイル出力するかどうか </param>
	void ModelForPortalPointInit(bool _doNavSetting);

	/// <summary>
	///						NavPointをファイルからロードしコンテナに初期化
	/// </summary>
	/// <param name="_file">ファイル名								</param>
	bool LoadNavPoint(const int& _file);

	/// <summary>
	///							NavPointの接続相手をファイルからロードしNavPointにロードする
	/// </summary>
	/// <param name="_file">	ファイル名							</param>
	bool LoadTargetNavPoint(const int& _file);

	/// <summary>
	///							navPointの情報をセーブする
	/// </summary>
	/// <param name="_file">	ファイル名							</param>
	void SaveNavPoint(const int& _file);
	
	StageManager* stageManager;
	Player* player;

	std::vector<int> hModel;									// モデルハンドル配列

	std::unordered_map<int, MV1_REF_POLYGONLIST> modelForPolygonDataList;						// モデルハンドルごとのポリゴン情報コンテナ
	std::unordered_map<int,	std::unordered_map<int, std::unordered_set<VertexData, VertexDataHash>>> modelForLocalVerticesAndTargetDataList;// モデルの種類分ローカル頂点の座標を見て自身と接続している相手を設定する　ポータルの生成で使う　近い座標同士に頂点があったら１つにまとめてカウント
	std::unordered_map<int, std::unordered_map<int, PolygonData*>> modelForLocalPolygonDataList;// モデルの種類分ローカル頂点の座標を見て頂点の数分idを設定する　ワールド座標の頂点定義で使う　近い座標同士に頂点があったら１つにまとめてカウント
	std::unordered_map<int, std::unordered_map<int, PortalPoint*>> modelForLocalPortalList;		// モデルの種類分ローカルポータルの数分idを設定する
	std::unordered_map<int, PolygonData*> worldPoygonDataList;									// ワールド座標に変換したポリゴンデータコンテナ 生成されたステージオブジェクトすべてのポリゴンデータを管理
	std::unordered_map<int, VertexData*> worldVerticesDataList;									// ワールド座標に変換した頂点データコンテナ 座標を見て頂点の数分idを設定する　ポータル定義で使う　
	
	//std::unordered_set<std::pair<int,int>, SymmetricPairHash> portalList;						// ポリゴン同士で共有しているポータル頂点の順不同のペアを保存
	std::unordered_map<int, PortalPoint*> activePortalPointList;								// 経路探索に使える状態のポータルポイントのコンテナ 生成されたステージオブジェクトすべてのポリゴンのポータルを管理
	std::unordered_map<int, PortalPoint*> inactivePortalPointList;								// 経路探索に使えない状態のポータルポイントのコンテナ 生成されたステージオブジェクトすべてのポリゴンのポータルを管理

	//std::unordered_set<PortalPoint*> modelEndPortalPointList;									// モデルの端っこのポータルポイント（接続相手の法線が自身と85度以上あったとき）を保存するコンテナ

	static int polygonCreateNum;		// ワールド変換したポリゴンの生成ナンバー
	static int vertexCreateNum;			// ワールド変換したの生成ナンバー
	static int portalPointCreateNum;	// ポータルポイントの生成ナンバー
	static int ownerModelSaveNum;		// NavPointの生成元のモデルナンバーを設定、記録　モデルごとにNavPointのグループをつくる　当たり判定を生成元のモデルと自身で行わないようにする


	//bool isChangedContainer;			// WayPointのコンテナが変化したかどうか

	bool isFrameDraw;					// モデルのフレーム描画フラグ
	bool isPortalPointRootDraw;			// ポータルポイント同士の接続（経路）の描画フラグ

	std::unordered_map<std::string,int> debugInfoCountList;
};