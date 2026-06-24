#include "CsvReader.h"
#include <fstream>
#include <Windows.h>
#include <assert.h>

CsvReader::CsvReader(std::string filename)
{
	std::ifstream file(filename);
	std::string str;
	record.clear();

	if (!file)
		assert(false && "ファイルを開くことができませんでした");

	while (getline(file, str))	// 一行ずつ読み取り、strに代入
	{
		OutputDebugString(str.c_str());
		OutputDebugString("\n");
		// strを , ごとにばらす
		std::vector<std::string> line;	// 行の文字列コンテナ　sizeは列の数
		size_t idx;						// 一文（カンマで区切られた文字列）の長さ

		// 文字の末端まで回す	
		while ((idx = str.find(',')) != std::string::npos)	// コンマが見つかったらidxに文字の長さを代入する	strが " 1234, ... " の場合、4が代入される　
		{
			// 先頭から見つかったコンマまでの文字列をs1に代入（一文を代入）
			std::string s1	= str.substr(0, idx);	
			// コンマから次の文字列を代入し、一文ずつ文字を見ていく
			str				= str.substr(idx + 1);
			// 一文をpush
			line.push_back(s1);
		}
		// この行の最後の文字をpush
		line.push_back(str);
		// 出来上がったこの行の文字列のvectorをpush
		record.push_back(line);
	}
	file.close();
}

CsvReader::~CsvReader()
{
}

int CsvReader::GetLines()
{
	assert(record.size() < INT_MAX && "行の数がintの最大値を超えています");

	return (int)record.size();
}

int CsvReader::GetColumns(int line)
{
	assert(record[line].size() < INT_MAX && "列の数がintの最大値を超えています");

	return (int)record[line].size();
}

int CsvReader::GetInt(int line, int column)
{
	assert(line >= 0 && line < record.size());
	assert(column >= 0 && column < record[line].size());
	std::string s = record[line][column];
	if (s.length()==0)
		return 0;
	return stoi(s);
}

float CsvReader::GetFloat(int line, int column)
{
	assert(line >= 0 && line < record.size());
	assert(column >= 0 && column < record[line].size());
	std::string s = record[line][column];
	if (s.length() == 0)
		return 0;
	return stof(s);
}
