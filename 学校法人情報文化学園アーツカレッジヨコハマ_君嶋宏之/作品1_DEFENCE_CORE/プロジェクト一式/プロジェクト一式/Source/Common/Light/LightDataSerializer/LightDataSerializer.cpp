#include "LightDataSerializer.h"
#include "../../../Common/FileSystemUtils/FileSystemUtils.h"
#include "../../../Common/JsonSerializer/JsonSerializer.h"

const std::string LightDataSerializer::DIRECTORY_PATH = "data/stage/stageData/";
std::unordered_map<int, LightInfo::LightData> LightDataSerializer::loadFogJsonList = std::unordered_map<int, LightInfo::LightData>{};

void LightDataSerializer::Init()
{
	for (int i = 1;i < 7;i++)
	{
		LightInfo::LightData LightData = LightInfo::LightData(VZero, LightInfo::DEFAULT_LIGHT_DIR, Colors::WHITE, Colors::WHITE, Colors::WHITE);

		Save(i, LightData);
	}
}

void LightDataSerializer::Release()
{
	loadFogJsonList.clear();
}

LightInfo::LightData LightDataSerializer::Reload(const int& _stageNum)
{
	return Load(_stageNum);
}

LightInfo::LightData LightDataSerializer::GetLightData(const int& _stageNum)
{
	// keyが存在しなかった時(Jsonが読み込まれてなかった時)
	if (loadFogJsonList.find(_stageNum) == loadFogJsonList.end())
	{
		// key:ステージナンバー / value:敵のデータ
		loadFogJsonList.emplace(_stageNum, Load(_stageNum));
	}
	return loadFogJsonList[_stageNum];
}

void LightDataSerializer::Save(const int& _stageNum, const LightInfo::LightData& _LightData)
{
	// エントリの作成
	FileSystemUtils::CreateDirectoryAndFile(DIRECTORY_PATH, GetLoadJsonName(_stageNum));

	// Jsonの作成
	nlohmann::json json = _LightData;

	// Jsonの書き出し
	JsonSerializer::Save(DIRECTORY_PATH + GetLoadJsonName(_stageNum), json);
}

LightInfo::LightData LightDataSerializer::Load(const int& _stageNum)
{
	// Jsonデータの作成
	nlohmann::json json = JsonSerializer::Load(DIRECTORY_PATH + GetLoadJsonName(_stageNum));
	// フォグデータの作成
	LightInfo::LightData lightData = json;

	return lightData;
}

std::string LightDataSerializer::GetLoadJsonName(const int& _stageNum)
{
	char jsonName[CHAR_MAX];
	// ステージナンバーによってJsonの名前を設定
	sprintfDx(jsonName, "stage_%.2d/light.json", _stageNum);

	return jsonName;
}
