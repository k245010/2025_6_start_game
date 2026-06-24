#include "EnemyDataSerializer.h"
#include "../../Common/FileSystemUtils/FileSystemUtils.h"
#include "../../Common/JsonSerializer/JsonSerializer.h"

const std::string EnemyDataSerializer::DIRECTORY_PATH							= "data/enemy/";
std::unordered_map<int,nlohmann::json> EnemyDataSerializer::loadEnemyJsonList	= std::unordered_map<int,nlohmann::json>{};

void EnemyDataSerializer::Init()
{
	// 敵データがまだ決定していない場合は、ここでデフォルトコンストラクタを使ってデータを書き出す

	/*for (auto i = 0;i < (int)ENEMY_KIND::MAX;i++)
		EnemyDataSerializer::Save((ENEMY_KIND)i, EnemyInfo::EnemyData());*/
}

void EnemyDataSerializer::Release()
{
	loadEnemyJsonList.clear();
}

EnemyInfo::EnemyData EnemyDataSerializer::Reload(const ENEMY_KIND& _kind)
{
	return Load(_kind);
}

std::string EnemyDataSerializer::GetLoadJsonName(const ENEMY_KIND& _kind)
{
	char jsonName[CHAR_MAX];
	// 敵の種類によってJsonの名前を設定
	sprintfDx(jsonName, "enemy_%.2d.json", (int)_kind);

	return jsonName;
}

EnemyInfo::EnemyData EnemyDataSerializer::Load(const ENEMY_KIND& _kind)
{
	// Jsonデータの作成
	nlohmann::json json				= JsonSerializer::Load(DIRECTORY_PATH + GetLoadJsonName(_kind));
	// 敵データの作成
	EnemyInfo::EnemyData enemyData	= json;

	return enemyData;
}

EnemyInfo::EnemyData EnemyDataSerializer::GetEnemyData(const ENEMY_KIND& _kind)
{
	assert(_kind < ENEMY_KIND::MAX && (int)_kind >= 0);

	// keyが存在しなかった時(Jsonが読み込まれてなかった時)
	if (loadEnemyJsonList.find((int)_kind) == loadEnemyJsonList.end())
	{
		// key:敵の種類 / value:敵のデータ
		loadEnemyJsonList.emplace((int)_kind, Load(_kind));
	}
	return loadEnemyJsonList[(int)_kind];
}

void EnemyDataSerializer::Save(const ENEMY_KIND& _kind, const EnemyInfo::EnemyData& _enemyData)
{
	// エントリの作成
	FileSystemUtils::CreateDirectoryAndFile(DIRECTORY_PATH, GetLoadJsonName(_kind));

	// Jsonの作成
	nlohmann::json json = _enemyData;

	// Jsonの書き出し
	JsonSerializer::Save(DIRECTORY_PATH + GetLoadJsonName(_kind), json);
}


