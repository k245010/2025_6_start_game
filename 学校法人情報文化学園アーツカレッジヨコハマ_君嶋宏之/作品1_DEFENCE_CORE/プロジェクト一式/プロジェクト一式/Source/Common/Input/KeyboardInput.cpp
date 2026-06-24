#include "keyboardInput.h"
#include "../../../Library/DebugNew.h"

KeyboardInput::KeyboardInput()
{
	DontDestroyOnSceneChange(true);
}

KeyboardInput::~KeyboardInput()
{
	keyboardInput.clear();
}

void KeyboardInput::Update()
{
	GetHitKeyStateAllEx(keyboardInputNow.kInput);

	keyboardInput.push_front(keyboardInputNow);
	if (keyboardInput.size() > InputInfo::SAVE_FRAME_MAX)
	{
		keyboardInput.pop_back();
	}
}

bool KeyboardInput::GetIsKeyboardPushing(int _keyCode)
{
	assert(_keyCode > 0 && _keyCode < KeyboardInfo::KEYBOARD_BUF_NUM);

	if (keyboardInput.front().kInput[_keyCode] > 0)
	{
		return true;
	}
	return false;
}

bool KeyboardInput::GetIsKeyboardPut(int _keyCode)
{
	if(keyboardInput.size()>1)
	{
		auto it = keyboardInput.begin();
		KeyboardInputData now = *it;
		std::advance(it, 1);
		KeyboardInputData lastFrame = *it;

		if (now.kInput[_keyCode] > 0 && lastFrame.kInput[_keyCode] <= 0)
		{
			return true;
		}
	}
	return false;
}

bool KeyboardInput::GetIsKeyboardRelease(int _keyCode)
{
	if (keyboardInput.size() > 1)
	{
		std::list<KeyboardInputData>::iterator it = keyboardInput.begin();
		KeyboardInputData now = *it;
		std::advance(it, 1);
		KeyboardInputData lastFrame = *it;

		if (!now.kInput[_keyCode] && lastFrame.kInput[_keyCode])
		{
			return true;
		}
		else
		{
			return false;
		}

	}
	return false;
}