#include "WaveDataSerializer.h"
#include "../WaveInfo.h"
#include "../../Scene/Scenes/PlayScene.h"
#include "../../Common/JsonSerializer/JsonSerializer.h"
#include "../../Common/FileSystemUtils/FileSystemUtils.h"
#include "../../../nlohmann/json.hpp"

void WaveDataSerializer::Save(const int& _waveNum, const WaveInfo::WaveData& _waveData,bool _override)
{
	// エントリの作成
	FileSystemUtils::CreateDirectoryAndFile(GetWaveDataFilePath(_waveNum));

	nlohmann::json json = _waveData;					
	JsonSerializer::Save(GetWaveDataFilePath(_waveNum), json,_override);	// Jsonを書き出し
}

WaveInfo::WaveData WaveDataSerializer::Load(const int& _waveNum)
{
	nlohmann::json json			= JsonSerializer::Load(GetWaveDataFilePath(_waveNum));	// ウェーブデータを読み込み、Jsonへ代入
	WaveInfo::WaveData waveData = json;

	return waveData;
}

std::string WaveDataSerializer::GetWaveDataFilePath(const int& _waveNum)
{
	char jsonName[CHAR_MAX];
	// ウェーブナンバーによってJsonの名前を設定
	sprintfDx(jsonName, "data/stage/stageData/stage_%.2d/wave/wave_%.2d.json", PlayScene::startStageNumber, _waveNum);

	return std::string(jsonName);
}

bool WaveDataSerializer::DeleteWaveDataFile(const int& _waveNum)
{
	// ファイルの削除　削除できなかった場合は、falseを返す
	return std::filesystem::remove_all(GetWaveDataFilePath(_waveNum));
}

