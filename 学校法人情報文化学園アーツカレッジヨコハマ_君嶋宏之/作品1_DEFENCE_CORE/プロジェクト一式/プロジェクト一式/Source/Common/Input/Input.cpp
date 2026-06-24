#include "Input.h"
#include "../../../ImGui/imgui.h"
#include "../../../Library/DebugNew.h"

namespace
{
	constexpr int MAX_LOG_RECORD = 1000;	// ログを記録する最大数
}

Input::Input(bool _useController, bool _useKeyboard, bool _useMouse)
{
	controller = nullptr;
	keyboard = nullptr;
	mouse = nullptr;
	if (_useController)
	{
		controller = new ControllerInput();
	}
	if (_useKeyboard)
	{
		keyboard = new KeyboardInput();
	}
	if (_useMouse)
	{
		mouse = new MouseInput();
	}

	DontDestroyOnSceneChange(true);

	for (int i = 0; i < sizeof(InputInfo::DEFAULT_KEY_CONFIGS)/sizeof(KeyConfigData); i++)
	{
		keyConfig.emplace(InputInfo::DEFAULT_KEY_CONFIGS[i].keyName, InputInfo::DEFAULT_KEY_CONFIGS[i].inputData);
	}

	inputLog.clear();
}

Input::~Input()
{
	keyConfig.clear();
	inputLog.clear();
}

void Input::Update()
{
	//_ 古いログを削除 _//

	if (inputLog.size() > MAX_LOG_RECORD)
	{
		int logCount = 1;
		for (auto itr = inputLog.end();itr != inputLog.begin();)
		{
			if (logCount > MAX_LOG_RECORD)
			{
				// ログの削除
				itr = inputLog.erase(itr);
				logCount--;
			}
			else
			{
				itr--;
				logCount++;
			}
		}
	}

#ifdef _DEBUG
	
	ImGui::Begin("InputLog");
	for (const auto& log : inputLog)
	{
		// ログの出力
		ImGui::TextUnformatted(log.c_str());
	}
	// ログの一番下へスクロール　常に最新のログを見るため
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
	ImGui::End();
#endif // _DEBUG
}

bool Input::GetKeyConfigPush(std::string _keyName)
{
	//std::unordered_map<std::string, InputConfigData>::iterator it = keyConfig.find(_keyName);
	std::_List_const_iterator it= keyConfig.find(_keyName);

	//要素が見つからなかったらfalseを返す
	if (it == keyConfig.end())
	{
		assert(false && "キーが見つかりませんでした");
		return false;
	}
	
	//コントローラー、キーボード、マウスで値が入っているものに対して入力チェックを行うが、いずれかがTrueならTrueで返す
	InputConfigData inputData = (*it).second;
	bool ret = false;
	if (controller != nullptr && !ret && inputData.padInput >= 0)
	{
		ret = controller->GetPadInputPushing(inputData.padInput);
	}
	if (keyboard != nullptr && !ret && inputData.keyboardInput >= 0)
	{
		ret = keyboard->GetIsKeyboardPushing(inputData.keyboardInput);
	}
	if (mouse != nullptr && !ret && inputData.mouseInput >= 0)
	{
		ret = mouse->GetMouseControlPushing(inputData.mouseInput);
	}

	if (ret)
		inputLog.push_back(_keyName);

	return ret;
}

bool Input::GetKeyConfigPut(std::string _keyName)
{
	//std::unordered_map<std::string, InputConfigData>::iterator it = keyConfig.find(_keyName);
	std::_List_const_iterator it = keyConfig.find(_keyName);
		
	//要素が見つからなかったらfalseを返す
	if (it == keyConfig.end())
	{
		assert(false && "キーが登録されていません");
		return false;
	}

	//コントローラー、キーボード、マウスで値が入っているものに対して入力チェックを行うが、いずれかがTrueならTrueで返す
	InputConfigData inputData = (*it).second;
	bool ret = false;
	if (controller != nullptr && !ret && inputData.padInput >= 0)
	{
		ret = controller->GetPadInputPut(inputData.padInput);
	}
	if (keyboard != nullptr && !ret && inputData.keyboardInput >= 0)
	{
		ret = keyboard->GetIsKeyboardPut(inputData.keyboardInput);
	}
	if (mouse != nullptr && !ret && inputData.mouseInput >= 0)
	{
		ret = mouse->GetMouseControlPut(inputData.mouseInput);
	}

	if (ret)
		inputLog.push_back(_keyName);

	return ret;
}

int Input::CreateOrChangekeyConfig(KeyConfigData _configData)
{
	auto it = keyConfig.find(_configData.keyName);
	
	//キーが存在しないなら新しく作る
	if (it == keyConfig.end())
	{
		keyConfig.emplace(_configData.keyName,_configData.inputData);
		return 1;
	}
	else
	{//存在しているならその要素を書き換える
		(*it).second = _configData.inputData;
		return 0;
	}

	return -1;
}

ControllerInput* Input::GetControllerInput()
{
	if (controller != nullptr)
	{
		return controller;
	}
	return nullptr;
}

KeyboardInput* Input::GetKeyboardInput()
{
	if (keyboard != nullptr)
	{
		return keyboard;
	}
	return nullptr;
}

MouseInput* Input::GetMouseInput()
{
	if(mouse!=nullptr)
	{
		return mouse;
	}
	return nullptr;
}

void Input::AddInputLog(const std::string _log)
{
	inputLog.push_back(_log);
}

/*bool Input::GetIsKeyOrButtonPushingNow(int _key, int _button)
{
	if(GetIsKeyPushingNow(_button)||GetIsKeyboardPushing(_key))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Input::GetIsKeyOrButtonPutNow(int _key, int _button)
{
	if (GetIsKeyPutNow(_button) || GetIsKeyboardPut(_key))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Input::GetIsKeyOrButtonReleaseNow(int _key, int _button)
{
	if (GetIsKeyReleaseNow(_button) || GetIsKeyboardRelease(_key))
	{
		return true;
	}
	else
	{
		return false;
	}
}*/