#pragma once
#include "../../Library/GameObject.h"

namespace CAMERA_INFO
{
	constexpr float NEAR_NUM	= 10.0f;
	constexpr float FAR_NUM		= 50000.0f;
	const float VIEW_ANGLE		= DegToRad * 95.0f;	// 視野角
}

class Camera : public GameObject
{
public:

	Camera();
	~Camera();

	/// <summary>
	/// クラスポインタの初期化
	/// </summary>
	void Init();

	void Update() override;

	/// <summary>
	/// 注視点の操作
	/// </summary>
	void Control();
	void Draw() override;

	/// <summary>
	/// カメラをセットする　基本的に、Updateで行っているけど、次フレームにセットを持ち越したくないときに使用
	/// </summary>
	void SettingCamera() const;

	/// <summary>
	/// カメラ座標を返す
	/// </summary>
	/// <returns></returns>
	const VECTOR3& GetPosition() const { return position; }

	/// <summary>
	/// カメラ座標を設定する
	/// </summary>
	/// <param name="_pos"> 座標 </param>
	void SetPosition(const VECTOR3& _pos) { position = _pos; }

	/// <summary>
	/// ターゲット座標を返す
	/// </summary>
	/// <returns></returns>
	const VECTOR3& GetTarget() const { return target; }

	/// <summary>
	/// ターゲット座標を設定する
	/// </summary>
	/// <param name="_tar"> ターゲット座標 </param>
	void SetTarget(const VECTOR3& _tar) { target = _tar; }

	/// <summary>
	/// 一人称のカメラのセット
	/// </summary>
	/// <param name="_playerPos"> プレイヤーの座標 </param>
	/// <param name="_rot"> 回転ベクトル </param>
	void FirstPersonCamera(VECTOR3 _playerPos, VECTOR2 _rot);

	/// <summary>
	/// カメラの状態
	/// </summary>
	enum class STATE
	{
		DEBUG = 0,
		NORMAL,
		FIRST,

		MAX,
	};

	/// <summary>
	/// カメラの状態を返す
	/// </summary>
	/// <returns> カメラの状態 </returns>
	STATE GetState() { return state; }

	/// <summary>
	/// カメラの状態をセット
	/// </summary>
	/// <param name="_state"> セットしたいカメラの状態 </param>
	void SetState(const STATE& _state) { state = _state; }

private:

	STATE state;			// カメラの状態
	VECTOR3 position;		// 座標
	VECTOR3 target;			// 注視点　ターゲット
	VECTOR3 debugPosition;	// デバッグの座標


	//bool isSetDebug2Pos;
};