#pragma once
#include "../LightInfo.h"
#include "../../../../nlohmann/json.hpp"

class LightDataSerializer
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
	///							ライトデータを再読み込み
	/// </summary>
	/// <param name="_stageNum">ステージナンバー		</param>
	/// <returns>				ライトデータ			</returns>
	static LightInfo::LightData Reload(const int& _stageNum);

	/// <summary>
	///							読み込んだのライトデータを返す	読み込んでなかったら読み込みを行う
	/// </summary>
	/// <param name="_stageNum">ステージナンバー		</param>
	/// <returns>				ライトデータ			</returns>
	static LightInfo::LightData GetLightData(const int& _stageNum);

	/// <summary>
	///							ライトのデータを書き込み
	/// </summary>
	/// <param name="_stageNum">ステージナンバー			</param>
	/// <param name="_light">	ライトデータ				</param>
	static void Save(const int& _stageNum, const LightInfo::LightData& _light);

private:

	/// <summary>
	///							ライトデータを読み込み
	/// </summary>
	/// <param name="_stageNum">ステージナンバー		</param>
	/// <returns>				ライトデータ			</returns>
	static LightInfo::LightData Load(const int& _stageNum);

	/// <summary>
	///							ライトのロードするJsonの名前を返す　例)stage_00 / light.json
	/// </summary>
	/// <param name="_stageNum">ロードする敵の種類		</param>
	/// <returns>				Jsonの名前				</returns>
	static std::string GetLoadJsonName(const int& _stageNum);

	static const std::string DIRECTORY_PATH;								// フォグデータの保存フォルダパス
	static std::unordered_map<int, LightInfo::LightData> loadFogJsonList;	// ライトデータをロードしたJsonのコンテナ
};