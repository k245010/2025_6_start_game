#pragma once
#include "../EnemyInfo.h"
#include "../../../nlohmann/json.hpp"
#include <list>

class EnemyDataSerializer
{
public:

	/// <summary>
	/// 初期化　
	/// </summary>
	static void Init();

	/// <summary>
	/// コンテナが保持している全てのデータを開放する
	/// </summary>
	static void Release();

	/// <summary>
	/// 更新	// EnemyManagerクラスが呼んでいます
	/// </summary>
	//static void Update();

	/// <summary>
	///							敵のデータを再読み込み
	/// </summary>
	/// <param name="_kind">	敵の種類			</param>
	/// <returns>				敵のデータ			</returns>
	static EnemyInfo::EnemyData Reload(const ENEMY_KIND& _kind);

	/// <summary>
	///							読み込んだ敵のデータを返す	読み込んでなかったら読み込みを行う
	/// </summary>
	/// <param name="_kind">	敵の種類			</param>
	/// <returns>				敵のデータ			</returns>
	static EnemyInfo::EnemyData GetEnemyData(const ENEMY_KIND& _kind);

	/// <summary>
	///						敵のデータを書き込み
	/// </summary>
	/// <param name="_kind">敵の種類				</param>
	/// </param>			敵のデータ				</param>
	static void Save(const ENEMY_KIND& _kind,const EnemyInfo::EnemyData& _enemyData);

private:

	/// <summary>
	///							敵のデータを読み込み
	/// </summary>
	/// <param name="_kind">	敵の種類			</param>
	/// <returns>				敵のデータ			</returns>
	static EnemyInfo::EnemyData Load(const ENEMY_KIND& _kind);

	/// <summary>
	///							敵のロードするJsonの名前を返す　例)enemy_00.json
	/// </summary>
	/// <param name="_kind">	ロードする敵の種類	</param>
	/// <returns>				Jsonの名前			</returns>
	static std::string GetLoadJsonName(const ENEMY_KIND& _kind);

	static const std::string DIRECTORY_PATH;	// 敵データの保存フォルダパス

	static std::unordered_map<int,nlohmann::json> loadEnemyJsonList;	// 敵の情報をロードしたJsonのコンテナ
};
