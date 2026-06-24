#pragma once
#include <filesystem>
#include <string>
#include <vector>

class PathReader
{
public:
	PathReader();
	~PathReader();

	//void ReadPath(std::string _directory);

	/// <summary>
	/// 指定フォルダ内に含まれる全ファイルの名前を取得(拡張子は消してます)
	/// </summary>
	/// <param name="_directory">指定するフォルダ</param>
	void ReadPath(std::string _directory);

	/// <summary>
	/// ファイルの名前の全要素vector<string>
	/// </summary>
	/// <returns></returns>
	const std::vector<std::string>& GetFolder() const { return folder; }
	void ReleaseFolder();
private:
	std::vector<std::string> folder;
};