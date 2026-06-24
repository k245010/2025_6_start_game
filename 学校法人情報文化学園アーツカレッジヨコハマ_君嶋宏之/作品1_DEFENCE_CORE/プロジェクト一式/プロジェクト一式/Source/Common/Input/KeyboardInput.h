#pragma once
#include "../../../Library/GameObject.h"
#include "Input.h"

/** 有村 作
Input
ControllerInput
KeyboardInput
MouseInput
**/
//※ 一部機能は自身で作成

namespace KeyboardInfo
{
	//キーボードの入力を保存しておく配列の大きさ
	const int KEYBOARD_BUF_NUM = 256;
}

//キーボード入力のリスト用
struct KeyboardInputData
{
	int kInput[KeyboardInfo::KEYBOARD_BUF_NUM];
};

class KeyboardInput : public GameObject
{
public:

	KeyboardInput();
	~KeyboardInput();
	void Update()override;

	///<summary>キーボード用　指定したキーが押されている間trueを返す</summary>
	/// <param name="_keyCode">キーの種類</param>
	bool GetIsKeyboardPushing(int _keyCode);
	///<summary>キーボード用　指定したキーが押され始めた1Fのみ間trueを返す</summary>
	/// <param name="_keyCode">キーの種類</param>
	bool GetIsKeyboardPut(int _keyCode);
	///<summary>キーボード用　指定したキーを押して離した1Fのみ間trueを返す</summary>
	/// <param name="_keyCode">キーの種類</param>
	bool GetIsKeyboardRelease(int _keyCode);
private:
	KeyboardInputData keyboardInputNow;
	std::list<KeyboardInputData>keyboardInput;

};