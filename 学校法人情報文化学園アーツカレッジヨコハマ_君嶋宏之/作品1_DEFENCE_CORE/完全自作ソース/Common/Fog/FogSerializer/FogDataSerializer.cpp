#include "FogDataSerializer.h"
#include "../../../Common/FileSystemUtils/FileSystemUtils.h"
#include "../../../Common/JsonSerializer/JsonSerializer.h"

const std::string FogDataSerializer::DIRECTORY_PATH								= "data/stage/stageData/";
std::unordered_map<int, FogInfo::FogData> FogDataSerializer::loadFogJsonList	= std::unordered_map<int, FogInfo::FogData>{};

void FogDataSerializer::Init()
{
	/*for (int i = 1;i < 7;i++)
	{
		FogInfo::FogData fogData = FogInfo::FogData(0.001f,WHITE);

		Save(i, fogData);
	}*/
}

void FogDataSerializer::Release()
{
	loadFogJsonList.clear();
}

FogInfo::FogData FogDataSerializer::Reload(const int& _stageNum)
{
	return Load(_stageNum);
}

FogInfo::FogData FogDataSerializer::GetFogData(const int& _stageNum)
{
	// keyが存在しなかった時(Jsonが読み込まれてなかった時)
	if (loadFogJsonList.find(_stageNum) == loadFogJsonList.end())
	{
		// key:ステージナンバー / value:敵のデータ
		loadFogJsonList.emplace(_stageNum, Load(_stageNum));
	}
	return loadFogJsonList[_stageNum];
}

void FogDataSerializer::Save(const int& _stageNum, const FogInfo::FogData& _fogData)
{
	// エントリの作成
	FileSystemUtils::CreateDirectoryAndFile(DIRECTORY_PATH, GetLoadJsonName(_stageNum));

	// Jsonの作成
	nlohmann::json json = _fogData;

	// Jsonの書き出し
	JsonSerializer::Save(DIRECTORY_PATH + GetLoadJsonName(_stageNum), json);
}

FogInfo::FogData FogDataSerializer::Load(const int& _stageNum)
{
	// Jsonデータの作成
	nlohmann::json json			= JsonSerializer::Load(DIRECTORY_PATH + GetLoadJsonName(_stageNum));
	// フォグデータの作成
	FogInfo::FogData fogData	= json;

	return fogData;
}

std::string FogDataSerializer::GetLoadJsonName(const int& _stageNum)
{
	char jsonName[CHAR_MAX];
	// ステージナンバーによってJsonの名前を設定
	sprintfDx(jsonName, "stage_%.2d/fog.json", _stageNum);

	return jsonName;
}
