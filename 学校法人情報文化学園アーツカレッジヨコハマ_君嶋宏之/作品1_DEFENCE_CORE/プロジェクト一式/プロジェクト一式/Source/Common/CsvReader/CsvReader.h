#pragma once
#include <vector>
#include <string>

class CsvReader
{
public:

	CsvReader(std::string filename);
	~CsvReader();

	/// <summary>
	/// s‚ğ•Ô‚·
	/// </summary>
	/// <returns></returns>
	int GetLines();

	/// <summary>
	/// —ñ‚ğ•Ô‚·
	/// </summary>
	/// <param name="line"> s </param>
	/// <returns></returns>
	int GetColumns(int line);
	int GetInt(int line, int column);
	float GetFloat(int line, int column);

private:

	std::vector<std::vector<std::string>> record;
};