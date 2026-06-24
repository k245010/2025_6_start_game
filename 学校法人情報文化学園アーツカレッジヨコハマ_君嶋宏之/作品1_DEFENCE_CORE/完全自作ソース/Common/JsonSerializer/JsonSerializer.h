#pragma once
#include "../../../nlohmann/json.hpp"

class JsonSerializer
{
public:

	/// <summary>
	///								Jsonを書き出す
	/// </summary>
	/// <param name="_jsonFilePath">Jsonを書き出すパス		</param>
	/// <param name="_json">		書き出すJson			</param>
	/// <param name="_override">	上書き保存するかどうか	/ falseの場合、ファイル内の末尾に追加書き出しされる</param>
	/// <returns>					true:成功 / false:失敗	</returns>
	static bool Save(std::string_view _jsonFilePath, const nlohmann::json& _json,bool _override = true);

	/// <summary>
	///								JsonのファイルパスからJson情報を読み取りJsonとして返す
	/// </summary>
	/// <param name="_jsonFilePath">Jsonのファイルパス			</param>
	/// <returns>					読み取った情報が入ったJson	</returns>
	static nlohmann::json Load(std::string_view _jsonFilePath);

private:

	/// <summary>
	///						文字コードをUTF-8からShift-JISに変換し返す
	/// </summary>
	/// <param name="data">	文字コード									</param>
	/// <returns>			Shift-JISの文字コード						</returns>
	static std::string UTF8ToShiftJis(std::string data);
};
