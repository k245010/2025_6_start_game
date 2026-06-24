#pragma once
#include "../Library/gameObject.h"
#include "config.h"
#include "object.h"

namespace {
	typedef std::vector<Object*> ObjectList;
}

enum class ObjectType
{
	NOMAL_A,//通常
	NOMAL_B,
	GRAVITY_A,//重力オブジェクト
	POINT, //中間点
	COIN,
	GOAL, //ゴール
	AIR_BARRIER,
	SPIKE,
	FENCE,
	SWITCH,
	BUMPER,
	RAIL,
	IRON_DOOR,
	HEAL_ITEM,
	GHEAL_ITEM,
	BREAKABLE,
	GRAVITY_AREA,

	PLAYER,
	WALK_ENEMY=20,
	FLY_ENEMY=22,

	SIGNBOARD,
	WALLHANGING,
	BORAD,
	BORAD2,
	DESK,
	PC,

	AIRVENT,
	PLUMB,
	PLUMB2,

	CAMERA,
	BOX,
	MACHINE,
	CAMERA_AREA,
	MAX_NUN, 
};

/// <summary>
/// ステージのオブジェクトを作るクラス
/// </summary>
class StageObjectManager:public GameObject
{
public:
	StageObjectManager(SceneBase* _scene);
	~StageObjectManager();
	void Update()override;
	void Draw()override;

	/// <summary>
	/// objectの座標取得
	/// </summary>
	/// <param name="ob"></param>
	/// <returns></returns>
	VECTOR GetObjectPosition(const Object* ob);

	/// <summary>
	/// 指定したオブジェクトの削除
	/// </summary>
	/// <param name="ob"></param>
	void DeleteObject(Object* ob);
	void DeleteObject();
	void DeletePlayerOtherObject();
	void DeletePointToCoinToPlayerOtherObject();


	/// <summary>
	/// オブジェクトの作成
	/// </summary>
	/// <param name="_pos"></座標>
	/// <param name="scale"></大きさ>
	/// <param name="rotation"></回転>
	/// <param name="type"></オブジェクトの種類>
	/// <returns></returns>
	int CreateTerraObject(const VECTOR& _pos,const VECTOR& scale,const VECTOR& rotation,const int& type,const int& collisionId,const std::vector<int>_slot);
	int CreateCharaObject(const VECTOR& _pos, const VECTOR& scale, const VECTOR& rotation, const int& type, const int& collisionId,const std::vector<int>_slot);
	/// <summary>
	/// オブジェクトの種類を取得
	/// </summary>
	/// <param name="ob"></param>
	/// <returns></returns>
	int GetObjectType(const Object* ob)const;

	/// <summary>
	/// 読み込まれたモデルを取得
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	//int GetHModel(int index) const { return hModel[index]; }

	ObjectList& GetObjectList();
	void SetObjectList(ObjectList& list);
	void NextCheckPoint();
	VECTOR GetCurrentCpPosition()const;
	/// <summary>
	/// リストサイズの取得
	/// </summary>
	/// <returns></returns>
	size_t GetObjectNum() { return objectList.size(); }

	void SetCollisionId(bool active) { isCollisionId = active; }
	
	int GetSwitchObjectId();
	int GetCoinNum()const { return coinNum; };
	VECTOR3 GetCameraAreaPullAddPosition(int cameraAreaid);
	
	void SetSwitchObjectState(int id, int state);
private:
	//チェックポイントの数をカウントしてクラスに渡す
	int checkPointCount;
	//現在のチェックポイントとクラスが持っているidを比較して座標を返すのに使う変数
	int currentCp;
	
	ObjectList objectList;

	//デバック用
	//コリジョンidを表示するか
	bool isCollisionId;
	//エリアを表示させるか
	bool areaFlg;

	int coinNum;

};