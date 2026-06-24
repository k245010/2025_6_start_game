#include "Debug.h"
#include "../PostEffect/PostEffect.h"
#include "../Input/Input.h"
#include "../../Shot/Bullet/Bullet/Bullet.h"
#include "../../Stage/StageManager/StageManager.h"
#include "../../Scene/Scenes/PlayScene.h"
#include "../../../ImGui/imgui.h"
#include "../../../Library/ObjectManager.h"
#include "../../../Library/SceneManager.h"
#include "../../../Library/App.h"

namespace 
{
	std::list<std::string>logger;

	float rate;
	Input* inputManager;
	bool debug;
	bool isChangeScene;

	float gameDeltaTimeRate;
}

void Debug::CreateMessageBox(const std::string& _error, const std::string& _title)
{
#if DebugIf
	MessageBox(NULL, _error.c_str(), _title.c_str(), MB_ICONERROR | MB_OK);
#endif
}

void Debug::InitDebug(Input* _input)
{
	inputManager		= _input;

	logger.clear();

	isChangeScene		= false;
	gameDeltaTimeRate	= 1.0f;

#ifdef _DEBUG
	debug = false;
#else
	debug = false;

#endif
}

void Debug::UpdateDebug()
{
#ifdef _DEBUG
	ImGui::Begin("debug");
	if (ImGui::TreeNode("debugLogger")) {
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(500, 200));
		for (std::string& str : logger) {
			ImGui::Text("%s", str.c_str());
			ImGui::Separator();
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
	if (logger.size() >= 100) {
		logger.pop_front();
	}
	ImGui::End();
#endif
}

void Debug::DebugUpdate()
{

//#ifdef _DEBUG

	if (inputManager->GetKeyConfigPut("_D_DEBUG_CHANGE"))
		debug = !debug;

	/*ImGui::Begin("DeltaTime");
	ImGui::End();*/

	if (!debug)
		return;

	ImGui::Begin("Debug");
	// 平均FPS
	ImGui::Text("FPS : %.1f",		Time::GetAverageFPS());
	// 最低FPS
	ImGui::Text("Worst FPS : %.1f", Time::GetWorstFPS());

	if (ImGui::Button("Reset FPS"))
	{
		// FPSをリセット
		Time::ResetSaveFPS();
	}

	ImGui::SliderFloat("gameDeltaTimeRate", &gameDeltaTimeRate, 0.0f, 10.0f);
	if (ImGui::Button("Reset"))
	{
		gameDeltaTimeRate = 1.0f;
	}

	Time::SetGameDeltaTimeRate(gameDeltaTimeRate);

#if USE_POST_EFFECT
	if (ImGui::Begin("PostEffect"))
	{
		auto& pe = PostEffect::GetParameters();

		ImGui::Separator();

		if (ImGui::Button("Reset PostEffect Params"))
		{
			PostEffect::ResetParameters();
		}

		ImGui::Separator();
		ImGui::Text("Bloom");
		ImGui::SliderInt("Bloom Threshold", &pe.bloomBrightThreshold, 0, 255);
		ImGui::SliderInt("Bloom Blur Width", &pe.bloomGaussPixelWidth, 1, 64);
		ImGui::SliderInt("Bloom Blur Strength", &pe.bloomGaussParam, 1, 2000);
		ImGui::SliderInt("Bloom Alpha", &pe.bloomAlpha, 0, 255);
		ImGui::SliderInt("Bloom Passes", &pe.bloomPasses, 1, 8);

		ImGui::Separator();
		ImGui::Text("Shared Edge Mask");
		ImGui::SliderInt("Edge Threshold", &pe.edgeThreshold, 0, 255);

		ImGui::Separator();
		ImGui::Text("Rim Light");
		ImGui::SliderInt("Rim Blur Width", &pe.rimSoftGaussWidth, 1, 64);
		ImGui::SliderInt("Rim Blur Strength", &pe.rimSoftGaussParam, 1, 2000);
		ImGui::SliderInt("Rim Alpha", &pe.rimAlpha, 0, 255);
		ImGui::SliderInt("Rim Passes", &pe.rimPasses, 1, 8);
		ImGui::SliderInt("Rim Color R", &pe.rimColorR, 0, 255);
		ImGui::SliderInt("Rim Color G", &pe.rimColorG, 0, 255);
		ImGui::SliderInt("Rim Color B", &pe.rimColorB, 0, 255);

		ImGui::Separator();
		ImGui::Text("AO Like");
		ImGui::SliderInt("AO Blur Width", &pe.aoSoftGaussWidth, 1, 64);
		ImGui::SliderInt("AO Blur Strength", &pe.aoSoftGaussParam, 1, 2000);
		ImGui::SliderInt("AO Alpha", &pe.aoAlpha, 0, 255);
		ImGui::SliderInt("AO Passes", &pe.aoPasses, 1, 8);

		PostEffect::ValidateParameters();
	}
	ImGui::End();
#endif

	// ObjectManagerが保持するインスタンス数
	//ImGui::Text("ObjectNum : %d", (int)ObjectManager::GetAllObject().size());

	////_ ステージ移動処理 _//

	//int selectStageNum = -1;

	//if (!isChangeScene)
	//{
	//	selectStageNum = PlayScene::startStageNumber;
	//	ImGui::SliderInt("selectStage", &selectStageNum, 1, StageManager::GetStageNumberMax());

	//	if (selectStageNum != PlayScene::startStageNumber)
	//	{
	//		// 一度タイトルへ移動
	//		SceneManager::ChangeScene("TITLE");	
	//		PlayScene::startStageNumber = selectStageNum;
	//		isChangeScene				= true;
	//	}
	//}
	//else
	//{
	//	SceneManager::ChangeScene("PLAY");
	//	isChangeScene = false;
	//}

	

	//ImGui::Text("DeleteObjectCount : %d", (int)ObjectManager::GetDeleteObjectCount());

	//std::list<BulletBase*> bullets = FindGameObjects<BulletBase>();
	//ImGui::Text("BulletObjectNum : %d", (int)bullets.size());

	ImGui::End();
	//UpdateLogger();
//#endif // _DEBUG
}

void Debug::DebugLog(std::string _log)
{
	logger.emplace_back(_log);
}

void Debug::DebugLogPrintfArgs(const TCHAR* _log,...)
{
	std::string name;
	name = printfString(_log);
	logger.emplace_back(name);
}

void Debug::DebugLogPrintf(const TCHAR* _log)
{
	std::string name;
	name = _log;
	logger.emplace_back(name);
}

void Debug::UpdateLogger()
{
	if (ImGui::TreeNode("logger")) {
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(500, 400));
		for (std::string& str : logger) {
			ImGui::Text("%s", str.c_str());
			ImGui::Separator();
		}
		ImGui::EndChild();
		ImGui::TreePop();
	}
	if (logger.size() >= 1000) {
		logger.pop_front();
	}
	
}

bool Debug::GetDebugMode()
{
	return debug;
}

void Debug::ClearLogger()
{
	logger.clear();
}


