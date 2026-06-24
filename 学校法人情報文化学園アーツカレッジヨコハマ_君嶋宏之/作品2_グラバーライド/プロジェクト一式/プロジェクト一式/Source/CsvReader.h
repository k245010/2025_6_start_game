#pragma once
#include  <vector>
#include <string>


class CsvReader
{
public:
	CsvReader();
	~CsvReader();
	/// <summary>
	/// データを読み込む
	/// </summary>
	/// <param name="_filename"></param>
	/// <returns></returns>
	bool Load(std::string _filename);
	/// <summary>
	/// 全データを一括で書き込む（上書き）
	/// </summary>
	/// <param name="_filename"></param>
	/// <param name="all"></param>
	void SaveAll(std::string _filename,const std::vector<std::vector<std::string>>& all) const;
	/// <summary>
	/// 一ラインを書き換える
	/// </summary>
	/// <param name="_filename"></param>
	/// <param name="targetLine"></param>
	/// <param name="newLine"></param>
	void SaveLine(std::string _filename, int targetLine, const std::vector<std::string>& newLine) const;
	/// <summary>
	/// セル一箇所だけ書き換える
	/// </summary>
	/// <param name="_filename"></param>
	/// <param name="line"></param>
	/// <param name="column"></param>
	/// <param name="newValue"></param>
	void SaveCell(std::string _filename, int line, int column, const std::string& newValue)const;
	
	/// <summary>
	/// 全てのセルを1種類の文字で埋める
	/// </summary>
	/// <param name="_filename"></param>
	/// <param name="_all"></param>
	void ResetAll(std::string _filename, const std::string& _allText);

	int GetLines();
	int GetColums(int line);
	int GetInt(int line, int column);
	float GetFloat(int line, int column);

private:
	std::vector<std::vector<std::string>> record;
	
};