#pragma once
#include "../../../Library/GameObject.h"

/** 有村 作
Input
ControllerInput
KeyboardInput
MouseInput
**/
//※ 一部機能は自身で作成

namespace ControllerInfo
{
	const float TRIGGER_DEADZONE = 0.6f;//何割以上トリガーが押し込まれたら入力を検知するか(GetPadInputで使用)
	const float DEFAULT_STICK_DEADZONE = 0.7f;//何割以上スティックが押し込まれたら入力を検知するか(GetPadInputで使用)
}

//1P～4Pのどの入力か
enum ControllerPlayer
{
	PLAYER_1 = DX_INPUT_PAD1,
	PLAYER_2 = DX_INPUT_PAD2,
	PLAYER_3 = DX_INPUT_PAD3,
	PLAYER_4 = DX_INPUT_PAD4,
};

enum StickDirections
{
	S_NO_DIRECTION=0,
	S_DOWN,
	S_DOWN_RIGHT,
	S_RIGHT,
	S_UP_RIGHT,
	S_UP,
	S_UP_LEFT,
	S_LEFT,
	S_DOWN_LEFT,

	S_DIRECTION_NUM,

};

//コントローラーの入力を全てまとめたもの
enum PadInputType
{
	P_NONE=-1,
	P_DPAD_UP=0,
	P_DPAD_DOWN,
	P_DPAD_LEFT,
	P_DPAD_RIGHT,
	P_START,
	P_BACK,
	P_L_STICK_PUSH,
	P_R_STICK_PUSH,
	P_L_SHOULDER,
	P_R_SHOULDER,
	P_BUTTON_A=12,
	P_BUTTON_B,
	P_BUTTON_X,
	P_BUTTON_Y,//ここまでは通常のXINPUTと同じ

	P_L_TRIGGER,
	P_R_TRIGGER,
	P_L_STICK_DOWN,
	P_L_STICK_DOWN_RIGHT,
	P_L_STICK_RIGHT,
	P_L_STICK_UP_RIGHT,
	P_L_STICK_UP,
	P_L_STICK_UP_LEFT,
	P_L_STICK_LEFT,
	P_L_STICK_DOWN_LEFT,
	P_R_STICK_DOWN,
	P_R_STICK_DOWN_RIGHT,
	P_R_STICK_RIGHT,
	P_R_STICK_UP_RIGHT,
	P_R_STICK_UP,
	P_R_STICK_UP_LEFT,
	P_R_STICK_LEFT,
	P_R_STICK_DOWN_LEFT,
};

//LRスティックの左右入力
struct StickInput
{
	DxLib::VECTOR2 leftStick;
	DxLib::VECTOR2 rightStick;
};

//LRスティックのbool
struct StickBool
{
	bool isLeftStick;
	bool isRightStick;
};

//LRスティックのfloat
struct StickFloat
{
	float leftStick;
	float rightStick;
};

struct StickDirection
{
	StickDirections leftStick;
	StickDirections rightStick;
};


class ControllerInput :public GameObject
{
public:
	ControllerInput();
	~ControllerInput();
	void Update()override;

	/// <summary>コントローラーのボタンが押されているか確認する</summary>
	/// <param name="_getStick">スティックの傾きも取るか　通常はfalse</param>
	/// <returns>いずれかのボタンが押されたらTrueを返す</returns>
	bool GetAnyButtonPush(bool _getStick = false);
	/// <summary>コントローラーのボタンが押されているか確認する</summary>
	/// <param name="_getStick">スティックの傾きも取るか 通常はfalse</param>
	/// <returns>いずれかのボタンが押されたらTrueを返す　前フレームも押されているならfalseを返す</returns>
	bool GetAnyButtonPut(bool _getStick = false);

	//ボタンを押している間True
	bool GetIsButtonPushingNow(int _button);
	//ボタンの押し始め1フレームのみTrue
	bool GetIsButtonPutNow(int _button);
	//ボタンを押して離した1フレームのみTrue
	bool GetIsButtonReleaseNow(int _button);

	/// <summary>
	/// コントローラーの入力を全て1つの関数で取れるようにしたもの
	/// </summary>
	/// <param name="_padInput">入力の種類</param>
	/// <returns>引数で指定した入力がある間Trueを返し続ける</returns>
	bool GetPadInputPushing(PadInputType _padInput);
	/// <summary>
	/// コントローラーの入力を全て1つの関数で取れるようにしたもの
	/// </summary>
	/// <param name="_padInput">入力の種類</param>
	/// <returns>引数で指定した入力が始まった1フレーム目のみTrueを返す</returns>
	bool GetPadInputPut(PadInputType _padInput);

	/*///<summary>ボタンを押している間True(過去のフレームを取れる)
	/// ※指定したフレーム分前の入力が保存されていなかった場合、無条件でfalseを返す</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <param name="_frameNum">何フレーム前の入力を取るか(SAVE_FRAME_MAX-1が最大値)</param>
	bool GetIsButtonPushingPast(int _button, int _frameNum);
	///<summary>指定したフレームがボタンを押し始めた1Fの場合True(過去のフレームを取れる)
	/// ※指定したフレーム分前の入力が保存されていなかった場合、無条件でfalseを返す</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <param name="_frameNum">何フレーム前の入力を取るか(SAVE_FRAME_MAX-2が最大値)</param>
	bool GetIsButtonPutPast(int _button, int _frameNum);
	///<summary>指定したフレームがボタンを押して離した最初の1Fの場合True(過去のフレームを取れる)
	/// ※指定したフレーム分前の入力が保存されていなかった場合、無条件でfalseを返す</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <param name="_frameNum">何フレーム前の入力を取るか(SAVE_FRAME_MAX-2が最大値)</param>
	bool GetIsButtonReleasePast(int _button, int _frameNum);*/

	///<summary>指定したボタンが何フレーム間押し続けられているかを確認する</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <returns>押されているフレームの長さ(SAVE_FRAME_MAXが最大値)</returns>
	int GetPushButtonTime(int _button);

	///<summary>スティックの倒し具合を(-1~1)取る</summary>
	StickInput GetStickInput();
	///<summary>スティックの倒し具合を(-1~1)取る</summary>
	/// <param name="_backFrame">何フレーム前の入力を取るか</param>
	StickInput GetStickInput(int _backFrame);

	///<summary>スティックが倒されている方向を調べる</summary>
	/// <param name="_deadZone">デッドゾーン(全体の何割が倒れていたら反応させるか)</param>
	StickDirection GetStickKnocking(float _deadZone);
	///<summary>スティックが倒されている方向を調べる</summary>
	/// <param name="_deadZone">デッドゾーン(全体の何割が倒れていたら反応させるか)</param>
	/// /// <param name="_backFrame">何フレーム前の入力を取るか</param>
	StickDirection GetStickKnocking(float _deadZone, int _backFrame);

	///<summary>スティックが倒されている方向を調べる</summary>
	/// <param name="_deadZone">デッドゾーン(全体の何割が倒れていたら反応させるか)</param>
	/// <returns>前回のフレームと今のフレームで倒した角度が違うなら値を返し、同じならS_NO_DIRECTIONが返る</returns>
	StickDirection GetStickKnockingPut(float _deadZone);

	///<summary>倒されているスティックの角度を入手する</summary>
	StickFloat GetStickRot();
	///<summary>倒されているスティックの角度を入手する</summary>
	/// <param name="_backFrame">何フレーム前の入力を取るか</param>
	StickFloat GetStickRot(int _backFrame);

	///<summary>現フレームのLRトリガーの押し込まれ具合を入手する(0~1)</summary>
	StickFloat GetTriggerNow();

	/// <summary>
	/// 現フレームのLRトリガーの押し込まれ具合を入手する(0~1)
	/// </summary>
	/// <returns>前フレームも押されているなら0を返す</returns>
	StickFloat GetTriggerPut(float _deadzone);

	///<summary>コントローラーの振動を行う</summary>
	/// <param name="_power">振動の強さ(0~1000)</param>
	///	<param name="_timeSecond">振動時間(秒数)</param>
	void ControlVibrationStartTime(int _power, int _timeSecond);

	///<summary>コントローラーの振動を行う(1秒=60フレームで計算)</summary>
	/// <param name="_power">振動の強さ(0~1000)</param>
	///	<param name="_timeFrame">振動時間(フレーム数)</param>
	void ControlVibrationStartFrame(int _power, int _timeFrame);

	//何Pのコントローラーの情報を入手するかを設定する(何もしなかったら1P)
	void SetControllerPlayer(ControllerPlayer _playerNumber) { playerNumber = _playerNumber; }

private:
	/// <summary>コントローラーのボタンが押されているか確認する</summary>
	/// <param name="_getStick">スティックの傾きも取るか</param>
	/// <param name="_backFrame">何フレーム前のものを取るか</param>
	/// <returns>いずれかのボタンが押されたらTrueを返す</returns>
	bool GetAnyButtonPush(bool _getStick, int _backFrame);

	StickDirections GetDirection(float _rot);

	const int BUTTON_MAX = 16;//XINPUTで使うボタンの数
	const float ANY_KEY_TRIGGER_DEADZONE = 0.5f;//AnyButtonPushで使うトリガーのデッドゾーン
	const float ANY_KEY_STICK_DEADZONE = 0.5f;//AnyButtonPushで使うスティックのデッドゾーン

	//何Pか
	ControllerPlayer playerNumber;

	//現フレームの入力
	XINPUT_STATE inputNow;

	//過去の入力をまとめて保存しておくリスト
	std::list<XINPUT_STATE> input;
};