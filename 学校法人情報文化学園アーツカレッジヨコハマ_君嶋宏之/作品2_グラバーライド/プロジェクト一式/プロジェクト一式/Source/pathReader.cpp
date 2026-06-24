#include "pathReader.h"

PathReader::PathReader()
{
}

PathReader::~PathReader()
{
	folder.clear();
}

void PathReader::ReadPath(std::string _directory)
{
	
	std::string f;
	for (auto const& entry : std::filesystem::recursive_directory_iterator(_directory)) {
		// ファイルのみ対象
		if (!entry.is_regular_file())
			continue;

		// stem() は拡張子を除いたファイル名を返す
		std::string name = entry.path().stem().string();

		folder.emplace_back(std::move(name));
	}
}

void PathReader::ReleaseFolder()
{
	if (!folder.empty())
	{
		for(auto i = folder.begin(); i < folder.end(); i++)
		{
			folder.erase(i);
		}
	}
}
