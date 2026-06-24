#include "GunDataSerializer.h"
#include "../../Common/JsonSerializer/JsonSerializer.h"
#include "../../Common/FileSystemUtils/FileSystemUtils.h"

const std::string GunDataSerializer::GetGunDataDirectoryPath()
{
	return "data/gun/";
}

GunInfo::GunData GunDataSerializer::LoadGunData(std::string_view _filepath)
{
	// Jsonをロードする
	nlohmann::json json			= JsonSerializer::Load(_filepath);
	// Jsonから構造体へ変換
	GunInfo::GunData gunData	= json;

	return gunData;
}

void GunDataSerializer::SaveGunData(std::string_view _filepath, const GunInfo::GunData& _gunData)
{
	// エントリの作成
	FileSystemUtils::CreateDirectoryAndFile(_filepath);
	// Jsonの書き出し
	JsonSerializer::Save(_filepath, nlohmann::json(_gunData));
}
