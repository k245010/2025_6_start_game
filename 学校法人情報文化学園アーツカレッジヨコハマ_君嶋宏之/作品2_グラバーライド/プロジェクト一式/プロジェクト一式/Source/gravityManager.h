#pragma once
#include "../Library/gameObject.h"

class Object;

enum GravityState
{
	NO_GRAVITY = 0,
	DOWN_GRAVITY_START,
	DOWN_GRAVITY,
	DOWN_RIGHT_GRAVITY_START,
	DOWN_RIGHT_GRAVITY,
	RIGHT_GRAVITY_START,
	RIGHT_GRAVITY,
	UP_RIGHT_GRAVITY_START,
	UP_RIGHT_GRAVITY,
	UP_GRAVITY_START,
	UP_GRAVITY,
	UP_LEFT_GRAVITY_START,
	UP_LEFT_GRAVITY,
	LEFT_GRAVITY_START,
	LEFT_GRAVITY,
	DOWN_LEFT_GRAVITY_START,
	DOWN_LEFT_GRAVITY,

	GRAVITY_NUM,
};

enum GravityDirection
{
	G_NO,
	G_DOWN,
	G_DOWN_RIGHT,
	G_RIGHT,
	G_UP_RIGHT,
	G_UP,
	G_UP_LEFT,
	G_LEFT,
	G_DOWN_LEFT,

	G_DIRECTION_NUM,
};

struct GravityObject
{
	Object* object;
	GravityState gravityState;

};



class GravityManager :public GameObject
{
public:

	enum GravityChangeMode
	{
		WORLD,
		PLAYER_CHANGE_MODE,
		ALL_CHANGE,
	};
	GravityManager(SceneBase* _scene);
	~GravityManager();

	void Update()override;
	void Draw()override;

	GravityState GetGravityState()const { return gravityState; }
	void SetGravityState(GravityState _gState) { gravityState = _gState; }
	GravityState GetWorldGravityState()const { return worldGravityState; }
	GravityState GetBeforeWorldGravityState()const { return beforeWorldGravityState; }
	static float GetWorldGravity() { return worldGravity; }
	static float GetBeforeWorldGravity() { return beforeWorldGravity; }

	void CreateGravityObject(Object* _object);
	void CreateGravityObject(Object* _object, GravityState _gState);

	///<summary>指定したオブジェクトの重力を指定したものに変更する</summary>
	/// <param name="_object">変更するオブジェクトのポインタ</param>
	/// <param name="_gState">変更する重力の方向</param>
	/// <returns>成功なら1,失敗なら-1</returns>
	int ChangeGravityObject(Object* _object, GravityState _gState);

	//指定したobjectのポインタを持つリストの要素を削除する
	void RemoveGravityObject(Object* _object);

	//gravityObjectリストを空にする
	void ClearGravityObject();
	/// <summary>
	/// プレイヤーの重力変更モードを使用する際に使われるものなので使わなくていい関数。
	/// </summary>
	/// <param name="_mode"></param>
	/// <param name="_gravityState"></param>
	void SetGravityChangeMode(GravityChangeMode _mode, GravityState _gravityState);

	/// <summary>
	/// 新しく重力状態を付与させたいときに使う関数。
	/// 例でいうとプレイヤーは無重力、その他は世界の重力というときはPLAYER_CHANGE_MODEになっているように、
	/// 重力をオブジェクトごとに分けたいときにモードを作り、設定をすると出来るようになる関数。
	/// </summary>
	/// <param name="_mode"></param>
	void SetGravityChangeMode(GravityChangeMode _mode);

	/// <summary>
	/// 世界の重力を変えたいときはこの関数を使う
	/// </summary>
	/// <param name="_gravityState"></param>
	void ChangeWorldGravity(GravityState _gravityState);

	/// <summary>
	/// GravityStateから角度を取りたいときはこの関数を使う。
	/// </summary>
	/// <param name="_state">角度を知りたい重力のstate</param>
	/// <returns>stateの角度が返ってくる</returns>
	float GetGravityAngle(GravityState _state);

	/// <summary>
	/// 引数に入れたStateの逆の重力のStateを取ってくる関数。
	/// 角度を取りたい場合は先ほどのGetGravityAngleと併用して使ってください。
	/// 例 RIGHT_GRAVITY リターンは LEFT_GRAVITYになる
	/// </summary>
	/// <param name="_state"></param>
	/// <returns>引数の逆の重力</returns>
	/// 
	GravityState GetReverceGravity(GravityState _state);

	/// <summary>
	/// プレイヤーの重力変更モードで使われる関数。
	/// 世界の重力からプレイヤーの移動のモードを入れると移動のモードに対応した重力に対応してくれる関数
	/// この関数をプレイヤー以外で使うことはないと思うので見なくてよい.
	/// </summary>
	/// <param name="_nowState"></param>
	/// <returns></returns>
	GravityState MultGravityState(GravityState _nowState);
	GravityState MultGravityState(GravityState _nowState,GravityState _world);

	/// <summary>
	/// プレイヤーの参照した値の逆の重力を返す関数。
	/// これもプレイヤー関連以外では使うことがないと思うのでとくに見なくてよい。
	/// </summary>
	/// <param name="_state"></param>
	/// <returns></returns>
	GravityState GetPlayerReverceChangeGravity(GravityState _state);

	/// <summary>
	/// Angleに値を入れたらその近くの値の重力のstateが返ってくる関数
	/// 試験段階なのでバグが発生する確率が高いので今は使わないことを推奨する。
	/// </summary>
	/// <param name="_angle"></param>
	/// <param name="_add"></param>
	/// <returns></returns>
	GravityState NearGravityNumState(float _angle,float _add);

	/// <summary>
	/// gravityのアングルからgravityStateを取ってくるときに役に立つ関数で、
	/// ラジアンの値を入れていけば、それに見合ったStateをとれる。
	/// </summary>
	/// <param name="_angle">Stateを取りたいangle</param>
	/// <param name="_start">GRAVITY_DOWN_STARTのこのStartの部分を取るか否か</param>
	/// <returns>State</returns>
	GravityState AngleToGravityStateGet(float _angle, bool _start);

private:
	GravityState gravityState;
	GravityState worldGravityState;
	GravityState beforeWorldGravityState;
	GravityChangeMode mode;
	std::list<GravityObject> gravityObject;
	static float worldGravity;
	static float beforeWorldGravity;
	std::string catsString;
	const char* debugDraw;
};