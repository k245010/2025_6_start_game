#pragma once

/// <summary>
/// Windowの定義をします
/// </summary>
#define FIX_FRAME_RATE  // フレームレートを固定にする
namespace Screen
{
	//static const int WIDTH			= 2560;
	//static const int HEIGHT			= 1440;
	static const int WIDTH				= 1920;
	static const int HEIGHT				= 1080;
	/*static const int WIDTH				= 1080;
	static const int HEIGHT				= 720;*/
	static const int WIDTH_CENTER		= WIDTH / 2;
	static const int HEIGHT_CENTER		= HEIGHT / 2;
	static const BOOL WINDOW_MODE		= TRUE;
	static const char* WINDOW_NAME		= "DEFENCE_CORE";
	static const float WINDOW_EXTEND	= 1.0f;
	static const float FRAME_RATE		= 60.0f; // 0にすると、ハードに従う
};
