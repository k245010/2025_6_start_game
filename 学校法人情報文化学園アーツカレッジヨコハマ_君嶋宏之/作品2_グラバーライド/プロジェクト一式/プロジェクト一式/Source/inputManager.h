#pragma once
#include <list>
#include "../Library/gameObject.h"
#include "struct.h"

namespace
{
	//何フレーム前の入力まで覚えておくか
	const int SAVE_FRAME_MAX = 600;
	//キーボードの入力を保存しておく配列の大きさ
	const int KEYBOARD_BUF_NUM = 256;
	//スティック入力の最大値
	const int STICK_INPUT_MAX = 32767;
}

//1P～4Pのどの入力か
enum ControllerPlayer
{
	PLAYER_1= DX_INPUT_PAD1,
	PLAYER_2=DX_INPUT_PAD2,
	PLAYER_3 = DX_INPUT_PAD3,
	PLAYER_4 = DX_INPUT_PAD4,
};

enum StickDirections
{
	S_NO_DIRECTION,
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

//キーボード入力のリスト用
struct KeyboardInput
{
	char kInput[KEYBOARD_BUF_NUM];
};

//LRスティックの左右入力
struct StickInput
{
	DxLib::VECTOR2F leftStick;
	DxLib::VECTOR2F rightStick;
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

//プレイヤーの操作の入力を管理する
class InputManager :public GameObject
{
public:
	InputManager(SceneBase* _scene);
	~InputManager();
	void Update()override;
	void Draw()override;

	bool GetInputA();
	
	/// <summary>コントローラーのボタンが押されているか確認する</summary>
	/// <param name="_getStick">スティックの傾きも取るか　通常はfalse</param>
	/// <returns>いずれかのボタンが押されたらTrueを返す</returns>
	bool GetAnyKeyPush(bool _getStick=false);
	/// <summary>コントローラーのボタンが押されているか確認する</summary>
	/// <param name="_getStick">スティックの傾きも取るか 通常はfalse</param>
	/// <returns>いずれかのボタンが押されたらTrueを返す　前フレームも押されているならfalseを返す</returns>
	bool GetAnyKeyPut(bool _getStick = false);

	///<summary>指定したキーボードのキーかコントローラーのボタンが押されている間Trueを返す</summary>
	bool GetIsKeyOrButtonPushingNow(int _key, int _button);
	///<summary>指定したキーボードのキーかコントローラーのボタンの押し始め1F目のみTrueを返す</summary>
	bool GetIsKeyOrButtonPutNow(int _key, int _button);
	///<summary>指定したキーボードのキーかコントローラーのボタンを押して離し始めた1F目のみTrueを返す</summary>
	bool GetIsKeyOrButtonReleaseNow(int _key, int _button);

	//ボタンを押している間True
	bool GetIsKeyPushingNow(int _button);
	//ボタンの押し始め1フレームのみTrue
	bool GetIsKeyPutNow(int _button);
	//ボタンを押して離した1フレームのみTrue
	bool GetIsKeyReleaseNow(int _button);

	///<summary>ボタンを押している間True(過去のフレームを取れる)
	/// ※指定したフレーム分前の入力が保存されていなかった場合、無条件でfalseを返す</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <param name="_frameNum">何フレーム前の入力を取るか(SAVE_FRAME_MAX-1が最大値)</param>
	bool GetIsKeyPushingPast(int _button, int _frameNum);
	///<summary>指定したフレームがボタンを押し始めた1Fの場合True(過去のフレームを取れる)
	/// ※指定したフレーム分前の入力が保存されていなかった場合、無条件でfalseを返す</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <param name="_frameNum">何フレーム前の入力を取るか(SAVE_FRAME_MAX-2が最大値)</param>
	bool GetIsKeyPutPast(int _button, int _frameNum);
	///<summary>指定したフレームがボタンを押して離した最初の1Fの場合True(過去のフレームを取れる)
	/// ※指定したフレーム分前の入力が保存されていなかった場合、無条件でfalseを返す</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <param name="_frameNum">何フレーム前の入力を取るか(SAVE_FRAME_MAX-2が最大値)</param>
	bool GetIsKeyReleasePast(int _button, int _frameNum);

	///<summary>指定したボタンが何フレーム間押し続けられているかを確認する</summary>
	/// <param name="_button">ボタンの種類</param>
	/// <returns>押されているフレームの長さ(SAVE_FRAME_MAXが最大値)</returns>
	int GetPushKeyTime(int _button);

	///<summary>キーボード用　指定したキーが押されている間trueを返す</summary>
	/// <param name="_keyCode">キーの種類</param>
	bool GetIsKeyboardPushing(int _keyCode);
	///<summary>キーボード用　指定したキーが押され始めた1Fのみ間trueを返す</summary>
	/// <param name="_keyCode">キーの種類</param>
	bool GetIsKeyboardPut(int _keyCode);
	///<summary>キーボード用　指定したキーを押して離した1Fのみ間trueを返す</summary>
	/// <param name="_keyCode">キーの種類</param>
	bool GetIsKeyboardRelease(int _keyCode);

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

	///<summary>コントローラーの振動を行う</summary>
	/// <param name="_power">振動の強さ(0~1000)</param>
	///	<param name="_timeSecond">振動時間(秒数)</param>
	void ControlVibrationStartTime(int _power ,int _timeSecond);

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
	bool GetAnyKeyPush(bool _getStick,int _backFrame);

	const int BUTTON_MAX = 16;//XINPUTで使うボタンの数
	const float ANY_KEY_TRIGGER_DEADZONE = 0.5f;//AnyKeyPushで使うトリガーのデッドゾーン
	const float ANY_KEY_STICK_DEADZONE = 0.5f;//AnyKeyPushで使うスティックのデッドゾーン

	StickDirections GetDirection(float _rot);

	//何Pか
	ControllerPlayer playerNumber;

	//現フレームの入力
	XINPUT_STATE inputNow;
	KeyboardInput keyboardInputNow;

	//過去の入力をまとめて保存しておくリスト
	std::list<XINPUT_STATE> input;
	std::list<KeyboardInput>keyboardInput;

};