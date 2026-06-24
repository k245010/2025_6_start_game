#pragma once
#include "../FogInfo.h"
#include "../../../../nlohmann/json.hpp"

class FogDataSerializer
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
	///							フォグのデータを再読み込み
	/// </summary>
	/// <param name="_stageNum">ステージナンバー		</param>
	/// <returns>				フォグのデータ			</returns>
	static FogInfo::FogData Reload(const int& _stageNum);

	/// <summary>
	///							読み込んだのフォグデータを返す	読み込んでなかったら読み込みを行う
	/// </summary>
	/// <param name="_stageNum">ステージナンバー		</param>
	/// <returns>				フォグのデータ			</returns>
	static FogInfo::FogData GetFogData(const int& _stageNum);

	/// <summary>
	///							フォグのデータを書き込み
	/// </summary>
	/// <param name="_stageNum">ステージナンバー			</param>
	/// <param name="_fogData">	フォグのデータ				</param>
	static void Save(const int& _stageNum,const FogInfo::FogData& _fogData);

private:

	/// <summary>
	///							フォグのデータを読み込み
	/// </summary>
	/// <param name="_stageNum">ステージナンバー		</param>
	/// <returns>				フォグのデータ		</returns>
	static FogInfo::FogData Load(const int& _stageNum);

	/// <summary>
	///							フォグのロードするJsonの名前を返す　例)stage_00 / fog.json
	/// </summary>
	/// <param name="_stageNum">ロードするフォグの種類	</param>
	/// <returns>				Jsonの名前			</returns>
	static std::string GetLoadJsonName(const int& _stageNum);

	static const std::string DIRECTORY_PATH;							// フォグデータの保存フォルダパス
	static std::unordered_map<int, FogInfo::FogData> loadFogJsonList;	// フォグのデータをロードしたJsonのコンテナ
};