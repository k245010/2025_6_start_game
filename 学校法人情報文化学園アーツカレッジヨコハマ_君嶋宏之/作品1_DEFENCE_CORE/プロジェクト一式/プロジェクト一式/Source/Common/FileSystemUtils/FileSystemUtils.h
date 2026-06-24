#pragma once
#include <iostream>

/// <summary>
/// ファイル、ディレクトリの操作をする
/// </summary>
class FileSystemUtils
{
public:
	
	/// <summary>
	///								フォルダパス内のフォルダとファイルの総数を返す
	/// </summary>
	/// <param name="_filePath">	フォルダパス					</param>
	/// <returns>					フォルダとファイルの総数		</returns>
	static int GetDirectoryAndFileCount(const std::string_view& _filePath);

	/// <summary>
	///								フォルダパス内のフォルダの総数を返す
	/// </summary>
	/// <param name="_filePath">	フォルダパス		</param>
	/// <returns>					フォルダの総数		</returns>
	static int GetDirectoryCount(const std::string_view& _filePath);

	/// <summary>
	///								フォルダパス内のファイルの総数を返す
	/// </summary>
	/// <param name="_filePath">	フォルダパス		</param>
	/// <returns>					ファイルの総数		</returns>
	static int GetFileCount(const std::string_view& _filePath);

	/// <summary>
	///									フォルダパスとファイルを作成する
	///									すでにフォルダパスが存在した場合でも、フォルダ内容が上書きされることはない。
	/// </summary>
	/// <param name="_DirectoryPath">	作成するフォルダパス(ディレクトリパス)	｛例) data/stage/wave/｝ [※最後の / がないとstageと同じ階層にwave.txtが作られる]	</param>
	/// <param name="_fileName">		作成するファイル名						｛例) wave.txt｝																	</param>
	/// <returns>						true:成功 / false:失敗																										</returns>
	static bool CreateDirectoryAndFile(std::string_view _DirectoryPath, std::string_view _fileName);

	/// <summary>
	///								フォルダパスとファイルを作成する
	///								すでにフォルダパスが存在した場合でも、フォルダ内容が上書きされることはない。
	/// </summary>
	/// <param name="_filePath">	作成するフォルダパス(ディレクトリパス)	｛例) data/stage/wave/wave.txt｝	</param>
	/// <returns>					true:成功 / false:失敗														</returns>
	static bool CreateDirectoryAndFile(std::string_view _filePath);

	/// <summary>
	///									フォルダパスを作成する
	///									すでにフォルダパスが存在した場合でも、フォルダ内容が上書きされることはない。
	/// </summary>
	/// <param name="_DirectoryPath">	作成するフォルダパス(ディレクトリパス)	｛例) data/stage/wave｝	</param>
	/// <returns>						true:成功 / false:失敗											</returns>
	static bool CreateEntryDirectory(std::string_view _DirectoryPath);

	/// <summary>
	///								ファイルを作成する
	/// </summary>
	/// <param name="_filePath">	作成するファイルパス		｛例) data/stage/wave/wave.txt｝	</param>
	/// <returns>					true:成功 / false:失敗											</returns>
	static bool CreateEntryFile(std::string_view _filePath);

private:

	/// <summary>
	/// エントリのタイプ
	/// </summary>
	enum class ENTRY_TYPE
	{
		ALL			= 0,	// 全て
		DIRECTORY,			// ディレクトリ
		FILE,				// ファイル
	};

	/// <summary>
	///							指定されたタイプのフォルダパス内のエントリの総数を返す
	/// </summary>
	/// <param name="_filePath">フォルダパス												</param>
	/// <param name="_type">	指定エントリタイプ 											</param>
	/// <returns>				指定されたエントリタイプのフォルダパス内のエントリの総数	</returns>
	static int GetEntryCount(const std::string_view& _filePath, const ENTRY_TYPE _type);

	/// <summary>
	///									指定されたタイプのエントリの作成を行う
	/// </summary>
	/// <param name="_DirectoryPath">	フォルダパス								</param>
	/// <param name="_fileName">		ファイル名									</param>
	/// <param name="_type">			指定エントリタイプ							</param>
	/// <returns>						true:成功 / false:失敗						</returns>
	static bool CreateEntry(std::string_view _DirectoryPath, std::string_view _fileName, const ENTRY_TYPE _type);
};

