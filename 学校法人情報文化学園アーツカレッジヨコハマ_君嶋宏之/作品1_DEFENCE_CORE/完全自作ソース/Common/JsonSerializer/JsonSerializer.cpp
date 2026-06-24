#include "JsonSerializer.h"
#include "JsonDefine.h"
#include <sstream>		// stringstreamを使うため
#include <winsock.h>	// MultiByteToWideChar,WideCharToMultiByteを使うため
#include <tchar.h>		// try catchを使うため
#include <fstream>		// ofstream,ifstreamを使うため
//#include <filesystem>	
//#include <iostream>

#include <iostream>
#include <string>

#define USE_ORIGINAL_JSON_OUT (0)	// 自作のJsonでの出力方法を使用するかどうか

bool JsonSerializer::Save(std::string_view _jsonFilepath, const nlohmann::json& _json,bool _override)
{
	try 
	{
		std::ofstream writingFile;						// Json情報を格納するファイル
#if USE_ORIGINAL_JSON_OUT
		std::string writingStr = _json.dump();			// Json情報を文字列として初期化		
#else
		std::string writingStr = _json.dump(4);			// Json情報を文字列として初期化		引数の4で4半角分のインデント、コンマや{}での改行を行う
#endif
		std::string intervalStr;						// コンマや｛｝ごとにJson情報を代入する文字列		
		std::stringstream writingStrStream(writingStr);	// writingStrを操作する		[ stringstream ]文字列の操作を行う型

		std::list<char> curlyBrackets;					// {}を保存		保存された｛｝の順番などをみて、インデントを作成する

		if (_override)
		{
			// ファイルを開く  [ std::ios::out ]書き出しのためにファイルをオープンすることを指定
			writingFile.open(_jsonFilepath, std::ios::out);
		}
		else
		{
			// ToDo:末尾に対し、データを追加する

			assert(false && "実装途中の処理です。");

			// ファイルを開く  [ std::ios::app ]データの末尾に追加データを追加していくことを指定
			writingFile.open(_jsonFilepath, std::ios::app);
		}
		int indentCount = 0;

#if USE_ORIGINAL_JSON_OUT
		// 文字列を全て回す
		for (auto strItr = writingStr.begin(); strItr != writingStr.end();strItr++)
		{
			char str	= *strItr;	// char型のstringのイテレータの値
			intervalStr += str;		// 文字列の代入

			// 次の文字列が末尾でないとき
			if (std::next(strItr) != writingStr.end())
			{
				// 次の文字列が{}の時
				if (*std::next(strItr) == '{' || *std::next(strItr) == '}')
				{
					writingFile << intervalStr << std::endl;	// ファイルに文字列を格納し改行
					intervalStr.clear();
					continue;
				}
			}

			// コンマまたは{}の文字列だったら
			if ((str == '{') || (str == '}') || (str == ','))
			{
				if (str != ',')
				{
					bool canPush = true;

					if (!curlyBrackets.empty())
					{
						if (curlyBrackets.back() == str)
						{
							if (str == '{')
							{
								//　インデントを長くする
								indentCount++;
							}
							else
							{
								//　インデントを短くする
								indentCount--;
								canPush = false;
							}
						}
					}

					if (canPush)
					{
						// {}を保存
						curlyBrackets.emplace_back(str);
					}
				}

				if (str == '}')
				{
					// 次の文字列が末尾でないとき
					if (std::next(strItr) != writingStr.end())
					{
						// 次の文字列がコンマの時
						if (*std::next(strItr) == ',')
							continue;	// 以下の出力＆改行はしない	 「　}, 」の形で出力するため
					}
				}

				std::string indent = "    ";	// インデント文字列
				std::string addIndent;			// Json情報の文字列に追加するインデント

				// インデントが存在したらその分回す
				for (int i = 0;i < indentCount;i++)
				{
					addIndent += indent;
				}
				// ファイルに文字列を格納し改行
				writingFile << addIndent + intervalStr << std::endl;
				// 文字列をリセット
				intervalStr.clear();						
			}
		}
#else
		writingFile << writingStr << std::endl;
#endif
		writingFile.close();
	}
	MY_JSON_CATCH

	return true;
}

nlohmann::json JsonSerializer::Load(std::string_view _jsonFilePath)
{
	std::string jsonFilePath = std::string(_jsonFilePath);	// std::ifstreamにはstring_viewは渡せないので、std::stringにキャスト

	// Jsonファイルパスから.Jsonをロードする
	std::ifstream ifs(jsonFilePath);
	
	if (!ifs)
	{
		assert(false && "JSONファイルを開けませんでした。");
	}

	unsigned char BOMS[]	= { 0xEF, 0xBB, 0xBF };	// 文字コードUTF-8であることを示すBOM(Byte Order Mark)
	bool isUtf8				= true;					// 文字コードUTF-8であるかどうか

	for (const auto& bom : BOMS) 
	{
		// ファイル内の1~3文字目とBOMSの0~3要素を比較して、違っていたら	 [ get() ]一文字ずつ読み込む　※ この関数を呼ぶたびに、読み書き位置は末尾へと進む
		if (ifs.get() != bom) 
		{
			isUtf8 = false;
			break;	// UTF-8出ないことが確定したので、break
		}
	}

	// UTF-8でなかったら	※ UTF-8だったら３文字分、読み書き位置を進めた状態のままにする
	if (!isUtf8)
	{
		// ファイルの先頭からシークしてifs.get()したときの読み書き位置をもどす	[ seekg ]ファイルの読み書き位置を指定する
		ifs.seekg(std::ios_base::beg);
	}
	
	std::string jsonStr;	// Json情報の文字列 
	std::string line;		// 一行ごとの文字列　

	// ファイル末端まで回す
	while (std::getline(ifs, line))  // 1行ずつ読み込みlineに代入する
	{ 
		// lineの文字を追加
		jsonStr += line;
	}

	nlohmann::json loadJson;	// 返すJson

	try
	{
		// Jsonのパース
		loadJson = nlohmann::json::parse(jsonStr);
	}
	MY_JSON_CATCH

	return loadJson;
}

std::string JsonSerializer::UTF8ToShiftJis(std::string data)
{
	wchar_t wideChar[512];	// ワイド文字
	char shiftJis[512];		// Shift-JIS

	// マルチバイト文字からワイド文字に変換
	MultiByteToWideChar(CP_UTF8, 0, data.c_str(), (int)strlen(data.c_str()) + 1, wideChar, MAX_PATH);
	// ワイド文字からShift-JISに変換	// [ CP_ACP ]とはShift-JIS（ANSI）にワイド文字を変換するマクロ
	WideCharToMultiByte(CP_ACP, 0, wideChar, sizeof(wideChar) / sizeof(wideChar[0]), shiftJis, sizeof(shiftJis), NULL, NULL);

	return shiftJis;
}
