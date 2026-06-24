#include "csvReader.h"
#include <stdio.h>
#include <fstream>

#include <Windows.h>
#include <cassert>


CsvReader::CsvReader()
{
}

CsvReader::~CsvReader()
{
}

bool CsvReader::Load(std::string _filename)
{
	std::ifstream file(_filename);
	std::string str;
	record.clear();
	while (std::getline(file, str))
	{
		//OutputDebugString(str.c_str());
		//OutputDebugString("\n");
		//str‚ð,‚²‚Æ‚É‚Î‚ç‚·
		std::vector<std::string> line;
		size_t idx;
		while ((idx = str.find(",")) != std::string::npos) {
			std::string s1 = str.substr(0, idx);
			str = str.substr(idx + 1);
			line.push_back(s1);
		}
		line.push_back(str);
		record.push_back(line);
	}
	file.close();
	return true;
}

void CsvReader::SaveAll(std::string _filename,const std::vector<std::vector<std::string>>& all) const
{
	std::ofstream file(_filename, std::ios::out);
	for (const auto& row : all) {
		for (size_t i = 0; i < row.size(); ++i) {
			file << row[i];
			if (i + 1 < row.size()) file << ',';
		}
		file << '\n';
	}
}

void CsvReader::SaveLine(std::string _filename, int targetLine, const std::vector<std::string>& newLine) const
{
	
	std::vector<std::vector<std::string>> temp = record;

	if (targetLine < 0 || targetLine >= temp.size()) return;

	temp[targetLine] = newLine;

	SaveAll(_filename, temp);
}

void CsvReader::SaveCell(std::string _filename, int line, int column, const std::string& newValue)const
{
	if (line < 0 || line >= record.size()) return;
	if (column < 0 || column >= record[line].size()) return;

	std::vector<std::vector<std::string>> temp = record;
	temp[line][column] = newValue;
	SaveAll(_filename, temp);
}
void CsvReader::ResetAll(std::string _filename, const std::string& _allText)
{
	Load(_filename);
	std::vector<std::vector<std::string>>all;
	for (int line = 0; line < GetLines(); line++)
	{
		std::vector<std::string> oneLine;
		for (int colum = 0; colum < GetColums(line); colum++)
		{
			oneLine.push_back(_allText);
		}
		all.push_back(oneLine);
	}
	SaveAll(_filename,all);
}
int CsvReader::GetLines()
{
	return record.size();
}

int CsvReader::GetColums(int line)
{
	return record[line].size();
}

int CsvReader::GetInt(int line, int column)
{
	assert(line >= 0 && line < record.size());
	assert(column >= 0 && column < record[line].size());
	std::string s = record[line][column];
	if (s.length() == 0)
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