#pragma once
#include "../../../Library/GameObject.h"
#define DebugIf (1)
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

/*
 西　作
 Debug h,cpp
 //※ 一部機能は自身で作成
*/

class Input;

namespace Debug {

	//デバック用機能を備えたクラス

	/// <summary>///
	/// MassageBoxを簡易的に使いやすくしたものエラー文とタイトルを指定してメッセージを出すことが可能
	/// 本来のMassageBoxはさらにmassageに対するアクションを変えることが出来るが基本変えないので抜きました
	/// </summary>/// 
	/// <param name="_error">エラー文</param>
	/// /// <param name="_title">左上に出る文章</param>

	void CreateMessageBox(const std::string& _error, const std::string& _title);

	void InitDebug(Input* _input);

	void UpdateDebug();

	void DebugUpdate();

	/// <summary>
	/// Log出力する値を挿入する関数
	/// </summary>
	/// <param name="_log"></param>
	void DebugLog(std::string _log);
	/// <summary>
	/// Log出力する値を挿入する関数
	/// Printfみたいな形で出力したいときに使う
	/// 上の関数はエスケープシーケンス対応ver
	/// 下の関数は非対応ver
	/// </summary>
	/// <param name="_log"></param>
	/// <param name=""></param>
	void DebugLogPrintfArgs(const TCHAR* _log,...);
	void DebugLogPrintf(const TCHAR* _log);

	void UpdateLogger();

	bool GetDebugMode();

	//ログ出力をリセットする
	void ClearLogger();

	//printfのエスケープシーケンスの文字列をデバックログで出したいときに使う関数。
	inline const TCHAR* printfString(const TCHAR* _format, ...) {
		static TCHAR strPrintf[1024];
		va_list args;
		va_start(args, _format);
		_vstprintf_s(strPrintf, _format, args);
		return strPrintf;
	}

	inline void DebugOutPutPrintf(const char* fmt, ...)
	{
		char buf[1024];
		va_list ap;
		va_start(ap, fmt);
		vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
		va_end(ap);
		OutputDebugStringA(buf);
		OutputDebugStringA("\n");
	}

	/// <summary>
	///								分母に対する分子の進行度を出力する
	/// </summary>
	/// <param name="fmt">			文字					</param>
	/// <param name="_molecule">	分子					</param>
	/// <param name="_denominator">	分母					</param>
	inline void DebugOutPutProgressNumber(const char* fmt, const int _molecule,const size_t _denominator)
	{
		char c[1024];
		float percent = (float)_molecule / _denominator * 100;	// 現在の分母に対するパーセント

		// cに文字列として代入
		sprintfDx(c, "%s -- %d / %d >> %.1lf %%", fmt, _molecule, _denominator, percent);

		const char* fmtInited = c;

		// 出力 //
		OutputDebugStringA(fmtInited);
		OutputDebugStringA("\n");
	}
}