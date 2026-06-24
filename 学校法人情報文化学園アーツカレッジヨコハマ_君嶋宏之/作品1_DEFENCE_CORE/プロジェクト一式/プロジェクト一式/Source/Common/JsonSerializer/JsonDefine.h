#pragma once
#include "../../../nlohmann/json.hpp"

#define MY_JSON_CATCH catch (nlohmann::detail::exception ex){ MessageBox(NULL, _TEXT(CreateExceptionText(ex).c_str() ), std::to_string(ex.id).c_str() , MB_OK);exit(0); }	// Jsonパース時のcatchマクロ																												

/// <summary>
///						Jsonのパース時に例外が発生したときの表示テキスト文を返す
/// </summary>
/// <param name="ex">	nlohmann::detail::exception(Jsonのexception)	</param>
/// <returns>			例外時のエラー文								</returns>	
std::string CreateExceptionText(nlohmann::detail::exception _exception)
{
	std::string ret = "JSONのパースに失敗しました。\n";
	// 例外内容の代入
	ret				+= _exception.what();
	ret				+= "\n\n";		

	switch (_exception.id)	// 例外の種類
	{
	case 101:
	case 102:
	case 103:
		ret += "ファイルに誤りがある可能性があります。";
		break;
	case 212:
	case 213:
	case 214:
	case 301:
	case 302:
	case 303:
		ret += "パース時の変数の名前、型を確認してください。";
		break;
	default:
		ret += "上記のメッセージを参照してください";
		break;
	}
	return ret;
}