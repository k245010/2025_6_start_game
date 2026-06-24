#pragma once
#include <list>
#include <string>
#include <unordered_map>
#include "../../../Library/gameObject.h"
#include "controllerInput.h"
#include "keyboardInput.h"
#include "mouseInput.h"

/** 有村 作 
Input
ControllerInput
KeyboardInput
MouseInput
**/
//※ 一部機能は自身で作成

class ControllerInput;
class KeyboardInput;
class MouseInput;

struct InputConfigData
{
	PadInputType padInput;
	int keyboardInput;
	MouseInputType mouseInput;

	InputConfigData() { padInput = P_NONE; keyboardInput = -1; mouseInput = M_NO_CONTROL; }
	InputConfigData(PadInputType _padInput, int _keyInput, MouseInputType _mouseInput) { padInput = _padInput; keyboardInput = _keyInput; mouseInput = _mouseInput; }
};

struct KeyConfigData
{
	std::string keyName;
	InputConfigData inputData;

	KeyConfigData() {};
	KeyConfigData(std::string _name, PadInputType _padInput, int _keyInput, MouseInputType _mouseInput)
	{
		keyName = _name;
		InputConfigData data(_padInput, _keyInput, _mouseInput);
		inputData = data;
	}
};

namespace InputInfo
{
	//何フレーム前の入力まで覚えておくか
	static const int SAVE_FRAME_MAX = 600;
	//スティック入力の最大値
	static const int STICK_INPUT_MAX = 32767;

	//キーコンフィグ　ここの値を使って操作入力を行うと後からデータの変更が容易

	// * デバッグのボタンは「_D_」が先頭文字列についています

	inline KeyConfigData DEFAULT_KEY_CONFIGS[]
	{
		KeyConfigData("NEXT",							P_NONE,				KEY_INPUT_RETURN,	M_NO_CONTROL),
		KeyConfigData("MODE_CHANGE",					P_NONE,				KEY_INPUT_E,		M_NO_CONTROL),
		KeyConfigData("WAVE_START",						P_NONE,				KEY_INPUT_RETURN,	M_NO_CONTROL),
		KeyConfigData("MOVE_FRONT",						P_L_STICK_UP,		KEY_INPUT_W,		M_NO_CONTROL),
		KeyConfigData("MOVE_BACK",						P_L_STICK_DOWN,		KEY_INPUT_S,		M_NO_CONTROL),
		KeyConfigData("MOVE_RIGHT",						P_L_STICK_RIGHT,	KEY_INPUT_D,		M_NO_CONTROL),
		KeyConfigData("MOVE_LEFT",						P_L_STICK_LEFT,		KEY_INPUT_A,		M_NO_CONTROL),
		KeyConfigData("MOVE_UP",						P_NONE,				KEY_INPUT_SPACE,	M_NO_CONTROL),
		KeyConfigData("SHOT",							P_NONE,				-1,					M_LEFT_CLICK),
		KeyConfigData("RELOAD",							P_NONE,				KEY_INPUT_R,		M_NO_CONTROL),
		KeyConfigData("TRAP_MAINTAIN",					P_NONE,				KEY_INPUT_F,		M_NO_CONTROL),
		KeyConfigData("TRAP_UPGRADE",					P_R_TRIGGER,		-1,					M_RIGHT_CLICK),
		KeyConfigData("SELECT_1",						P_NONE,				KEY_INPUT_1,		M_NO_CONTROL),
		KeyConfigData("SELECT_2",						P_NONE,				KEY_INPUT_2,		M_NO_CONTROL),
		KeyConfigData("SELECT_3",						P_NONE,				KEY_INPUT_3,		M_NO_CONTROL),
		KeyConfigData("SELECT_4",						P_NONE,				KEY_INPUT_4,		M_NO_CONTROL),
		KeyConfigData("SELECT_5",						P_NONE,				KEY_INPUT_5,		M_NO_CONTROL),
		KeyConfigData("SELECT_6",						P_NONE,				KEY_INPUT_6,		M_NO_CONTROL),
		KeyConfigData("SELECT_7",						P_NONE,				KEY_INPUT_7,		M_NO_CONTROL),
		KeyConfigData("PUT_TRAP",						P_R_TRIGGER,		-1,					M_LEFT_CLICK),
		KeyConfigData("DELETE_TRAP",					P_L_TRIGGER,		-1,					M_RIGHT_CLICK),
		KeyConfigData("_D_SELECT_STAGE_OBJECT",			P_NONE,				-1,					M_LEFT_CLICK),
		KeyConfigData("_D_PUT_STAGE_OBJECT",			P_NONE,				-1,					M_RIGHT_CLICK),
		KeyConfigData("_D_MOVE_DOWN",					P_NONE,				KEY_INPUT_LSHIFT,	M_NO_CONTROL),
		KeyConfigData("_D_DEBUG_CAM_CHANGE",			P_NONE,				KEY_INPUT_N,		M_NO_CONTROL),
		KeyConfigData("_D_DEBUG_POS_TO_PLAYER_POS_SET",	P_NONE,				KEY_INPUT_Y,		M_NO_CONTROL),
		KeyConfigData("_D_DO_GET_MOUSE",				P_NONE,				KEY_INPUT_B,		M_NO_CONTROL),
		KeyConfigData("_D_ADD_COIN",					P_NONE,				KEY_INPUT_C,		M_NO_CONTROL),
		KeyConfigData("_D_DEBUG_CHANGE",				P_NONE,				KEY_INPUT_F1,		M_NO_CONTROL),
		KeyConfigData("_D_SELECT_STAGE_NUM_1",			P_NONE,				KEY_INPUT_1,		M_NO_CONTROL),
		KeyConfigData("_D_SELECT_STAGE_NUM_2",			P_NONE,				KEY_INPUT_2,		M_NO_CONTROL),
		KeyConfigData("_D_SELECT_STAGE_NUM_3",			P_NONE,				KEY_INPUT_3,		M_NO_CONTROL),
		KeyConfigData("_D_SELECT_STAGE_NUM_4",			P_NONE,				KEY_INPUT_4,		M_NO_CONTROL),
		KeyConfigData("_D_SELECT_STAGE_NUM_5",			P_NONE,				KEY_INPUT_5,		M_NO_CONTROL),
		KeyConfigData("_D_SELECT_STAGE_NUM_6",			P_NONE,				KEY_INPUT_6,		M_NO_CONTROL),
		KeyConfigData("_D_SELECT_STAGE_NUM_7",			P_NONE,				KEY_INPUT_7,		M_NO_CONTROL),
	};
}

/// <summary>
/// プレイヤーの操作の入力を管理するクラス
///	シーンを跨いで存在できるようにしているためBootSceneで生成推奨
/// </summary>
class Input : public GameObject
{
public:

	/// <summary>
	/// 使用したいデバイスのみ生成推奨
	/// 生成したデバイスはGet関数で取得可能
	/// </summary>
	/// <param name="_useController">コントローラー入力を取るか</param>
	/// <param name="_useKeyboard">キーボード入力を取るか</param>
	/// <param name="_useMouse">マウス入力を取るか</param>
	Input(bool _useController,bool _useKeyboard,bool _useMouse);
	~Input();
	void Update()override;

	/// <summary>
	/// keyconfigを使用して入力を受け取る
	/// </summary>
	/// <param name="_keyName">keyconfigの名前</param>
	/// <returns>そのキーが押されているかどうか</returns>
	bool GetKeyConfigPush(std::string _keyName);
	/// <summary>
	/// keyconfigを使用して入力を受け取る
	/// </summary>
	/// <param name="_keyName">keyconfigの名前</param>
	/// <returns>そのキーが押され始めた1フレーム目のみTrue</returns>
	bool GetKeyConfigPut(std::string _keyName);

	/// <summary>
	/// キーコンフィグを新たに生成するか、既に指定したキーの要素があるならその要素を書き換える
	/// </summary>
	/// <param name="_ConfigData">キーコンフィグのデータ</param>
	/// <returns>新しく生成したなら1、既にあるものを変えたなら0を返す</returns>
	int CreateOrChangekeyConfig(KeyConfigData _ConfigData);

	///<summary>指定したキーボードのキーかコントローラーのボタンが押されている間Trueを返す</summary>
	//bool GetIsKeyOrButtonPushingNow(int _key, int _button);
	///<summary>指定したキーボードのキーかコントローラーのボタンの押し始め1F目のみTrueを返す</summary>
	//bool GetIsKeyOrButtonPutNow(int _key, int _button);
	///<summary>指定したキーボードのキーかコントローラーのボタンを押して離し始めた1F目のみTrueを返す</summary>
	//bool GetIsKeyOrButtonReleaseNow(int _key, int _button);

	ControllerInput* GetControllerInput();
	KeyboardInput* GetKeyboardInput();
	MouseInput* GetMouseInput();

private:

	/// <summary>
	/// 入力したログを記録する
	/// </summary>
	/// <param name="_log"> キーコンフィグ </param>
	void AddInputLog(const std::string _log);

	ControllerInput* controller;
	KeyboardInput* keyboard;
	MouseInput* mouse;
	std::unordered_map<std::string, InputConfigData>keyConfig;
	std::list<std::string> inputLog;
};