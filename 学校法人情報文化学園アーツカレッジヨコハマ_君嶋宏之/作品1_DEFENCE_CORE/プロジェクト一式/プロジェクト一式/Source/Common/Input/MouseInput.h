#pragma once
#include "../../../Library/GameObject.h"
#include "../../../Library/myDxLib.h"
#include <list>

/** 有村 作
Input
ControllerInput
KeyboardInput
MouseInput
**/
//※ 一部機能は自身で作成

namespace MouseInfo
{
	const float CHECK_CURSOR_MOVE_VEC_SIZE = 3.0f;//前フレームと座標の差がいくつ以上あったらその方向に移動していると認識するか
}

//マウスの操作を全てまとめたもの
enum MouseInputType
{
	M_NO_CONTROL = -1,
	M_RIGHT_CLICK = 0,
	M_LEFT_CLICK,
	M_WHEEL_BUTTON,
	M_WHEEL_UP,//ホイールを上に動かす
	M_WHEEL_DOWN,//ホイールを下に動かす
	M_CURSOR_RIGHT,//マウスカーソルを右に動かす
	M_CURSOR_LEFT,//マウスカーソルを左に動かす
	M_CURSOR_UP,//マウスカーソルを上に動かす
	M_CURSOR_DOWN,//マウスカーソルを下に動かす
};

class MouseInput :public GameObject
{
public:
	//マウスのボタンの種類
	enum MouseButton
	{
		NO_BUTTON=-1,
		RIGHT_CLICK = 0,
		LEFT_CLICK,
		WHEEL_BUTTON,
		BUTTON_4,
		BUTTON_5,
		BUTTON_6,
		BUTTON_7,
		BUTTON_8,

		BUTTON_NUM,
	};
	
	//カーソルの動きの向きの種類
	enum CursorMoveDirectionType
	{
		NO_DIRECTION=-1,
		CURSOR_RIGHT,
		CURSOR_LEFT,
		CURSOR_UP,
		CURSOR_DOWN,
	};

	struct MouseButtonInput
	{
		bool input[BUTTON_NUM];
	};

	MouseInput();
	~MouseInput();
	void Update()override;

	///<summary>マウスカーソルの位置を取得する</summary>
	/// <param name="_canScreenOut">TRUEなら画面外の座標も得られる,FALSEなら画面端が最大(最小)値になる</param>
	/// <returns>マウスカーソルの座標</returns>
	VECTOR2 GetMousePos(bool _canScreenOut);
	///<summary>マウスカーソルの位置を移動させる</summary>
	/// <param name="_pos">マウスカーソルの移動先の座標</param>
	void SetMousePos(VECTOR2 _pos);
	/// <summary>
	/// マウス操作全てを今現在行っているかを一括で取れる関数
	/// </summary>
	/// <param name="_controlType">必要な操作の種類</param>
	/// <returns></returns>
	bool GetMouseControlPushing(MouseInputType _controlType);
	/// <summary>
	/// マウス操作全てを今現在行っているかを一括で取れる関数
	/// </summary>
	/// <param name="_controlType">必要な操作の種類</param>
	/// <returns>ボタン系のみ連続で入力がされても1フレーム目しかTrueを返さない</returns>
	bool GetMouseControlPut(MouseInputType _controlType);

	///<summary>現在のフレームのマウスホイールの移動量を取得する</summary>
	/// <returns>奥に動かしたなら正の値・手前に動かしたなら負の値が返り動かしていないなら0が返される</returns>
	int GetMouseWheelFrame()const;

	///<summary>マウスカーソルと引数で示した正方形の範囲が当たっているかを取得する</summary>
	/// <param name="_centerPos">範囲の中心の座標</param>
	/// <param name="_size">当たり判定を取りたい中心からの範囲</param>
	/// <returns>当たったならTrue,当たっていないならFalse</returns>
	bool IsCursorSquareCollision(VECTOR2 _centerPos, VECTOR2 _size);
	/// <summary>
	/// マウスカーソルが指定した方向に動いているかどうかを確認する
	/// </summary>
	/// <param name="_cursorDirection">マウスカーソルの方向</param>
	/// <param name="_setCursorCenter">マウスカーソルを中央に固定するか</param> 
	/// <returns></returns>
	bool IsCursorMove(CursorMoveDirectionType _cursorDirection,bool _setCursorCenter=true);

	/// <summary>
	/// 前フレームからカーソルがどれだけ動いたかを得る
	/// </summary>
	VECTOR2 GetCursorDif()const { return cursorPosDif; }

	/// <summary>
	/// カーソル位置を_posに戻し、currentCursorPosもリセットする
	/// </summary>
	void ResetCursorPos(VECTOR2 _pos);

	/// <summary>
	/// マウス座標を設定するためにはこの値をTrueにする必要がある
	/// </summary>
	/// <param name="_can"></param>
	void SetCanSetCursorPos(bool _can) { canSetCursorPos = _can;};
	/// <summary>
	/// この値がTrueのときのみマウスの座標を設定出来る
	/// </summary>
	/// <param name="_can"></param>
	bool GetCanSetCursorPos()const { return canSetCursorPos; }

	/// <summary>
	/// マウスを表示するかどうかを設定する
	/// </summary>
	/// <param name="_isDraw"></param>
	void SetDrawMouseCursor(bool _isDraw) { drawCursor = _isDraw; SetMouseDispFlag(_isDraw); }
	/// <summary>
	/// マウスが画面に表示されているかを入手する
	/// </summary>
	/// <returns></returns>
	bool GetDrawMouseCursor()const { return drawCursor; }

private:
	//現フレームのボタン入力記録する
	void SetCurrentButtonInput();
	//現フレームのボタン入力をリストに記録する
	void PushInputList();

	///<summary></summary>
	/// <param name="_button"></param>
	/// <returns></returns>
	bool GetMousePushing(MouseButton _button);
	///<summary></summary>
	/// <param name="_button"></param>
	/// <returns></returns>
	bool GetMousePut(MouseButton _button);

	int nowMouseWheelVol;

	VECTOR2 currentCursorPos;//現在のカーソルの座標(画面外も含む)
	VECTOR2 cursorPosDif;//前フレームとのカーソルの座標の差分
	
	std::list<MouseButtonInput>buttonInput;
	MouseButtonInput currentButtonInput;
	bool canSetCursorPos;//カーソルの座標を設定出来るようにするか
	bool drawCursor;//マウスカーソルが表示されているかどうか
};