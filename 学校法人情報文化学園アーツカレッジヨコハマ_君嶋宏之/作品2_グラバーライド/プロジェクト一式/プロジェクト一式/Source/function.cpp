#include "function.h"

namespace {
	bool slowKeyPush;
	bool slowMosion;

	int slowMosionCounter;
}


void ObjectPosisionMove(DxLib::VECTOR2F* _pos, float _speed)
{
#ifdef _DEBUG
	
	if (CheckHitKey(KEY_INPUT_NUMPAD2)) {
		_pos->y += _speed;
	}
	else if (CheckHitKey(KEY_INPUT_NUMPAD8)) {
		_pos->y -= _speed;
	}
	if (CheckHitKey(KEY_INPUT_NUMPAD6)) {
		_pos->x += _speed;
	}
	else if (CheckHitKey(KEY_INPUT_NUMPAD4)) {
		_pos->x -= _speed;
	}
#endif
}

void ObjectPosisionMove(VECTOR* _pos, float _speed)
{
#ifdef _DEBUG

	ObjectPosisionMove(V2PGet(_pos->x, _pos->y), _speed);

	if (CheckHitKey(KEY_INPUT_NUMPAD0)) {
		_pos->z += _speed;
	}
	else if (CheckHitKey(KEY_INPUT_NUMPAD5)) {
		_pos->z -= _speed;
	}
#endif
}


void Vector2Draw(const DxLib::VECTOR2I _drawPos, const DxLib::VECTOR2F position, const char* string)
{
#ifdef _DEBUG
	if (string == nullptr) {
		DrawFormatString(_drawPos.x, _drawPos.y, 0xffffff, "%.2f : %.2f", position.x, position.y);
	}
	else {
		DrawFormatString(_drawPos.x, _drawPos.y, 0xffffff, string, position.x, position.y);
	}
#endif
}

void VECTORDraw(const DxLib::VECTOR2I _drawPos, const VECTOR position, const char* string)
{
#ifdef _DEBUG
	if (string == nullptr) {
		DrawFormatString(_drawPos.x, _drawPos.y, 0xffffff, "%.2f  : %.2f  : %.2f", position.x, position.y,position.z);
	}
	else {
		DrawFormatString(_drawPos.x, _drawPos.y, 0xffffff, string, position.x, position.y,position.z);
	}
#endif
}

SlowMostionManager::SlowModeState SlowMosion()
{
	if (CheckHitKey(KEY_INPUT_0)) {
		if (!slowKeyPush) {
			if (!slowMosion) {
				slowKeyPush = true;
				slowMosion = true;
				return SlowMostionManager::SlowModeState::SLOW_START;
			}
			else {
				slowKeyPush = true;
				slowMosion = false;
				return SlowMostionManager::SlowModeState::SLOW_STOP;
			}
		}
	}
	else {
		slowKeyPush = false;
	}
	return SlowMostionManager::SlowModeState::NO_SLOW;
	/*if (slowMosion) {
		slowMosionCounter++;
		if (slowMosionCounter % 10 == 0) {
			return false;
		}
		return true;
	}
	return false;*/
}

void CreateMessageBox(const std::string& _error, const std::string& _title)
{
	MessageBox(NULL,_error.c_str(),_title.c_str(), MB_ICONERROR | MB_OK);
}

void DrawTimeAsImageH(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I colonPos, VECTOR2I cutColon)
{
	// 時・分・秒を取り出し、"hh:mm:ss" 文字列に整形
	int hours = (int)timeSec / 3600;
	int minutes = ((int)timeSec % 3600) / 60;
	int seconds = (int)timeSec % 60;
	
	char buf[9]; // "hh:mm:ss\0"
	std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, seconds);
	std::string timeStr(buf);

	// 末尾（秒の1桁目）から右詰めで描画
	for (int i = (int)timeStr.size() - 1; i >= 0; --i) {
		char c = timeStr[i];
		int srcX, srcY;
		int drawX = (int)pos.x - interval * ((int)timeStr.size() - 1 - i);
		int drawY = (int)pos.y;

		if (c == ':') {
			// コロンのテクスチャ座標
			srcX = colonPos.x;
			srcY = colonPos.y;
			DrawRectRotaGraph(drawX, drawY, srcX, srcY, cutColon.x, cutColon.y, scale, 0, handle, true, false);
		}
		else if (c >= '0' && c <= '9') {
			int digit = c - '0';
			srcX = digit * screen.x;
			srcY = screen.y;  // 数字テクスチャのY座標（固定）
			DrawRectRotaGraph(drawX, drawY, srcX, srcY, cutSize.x, cutSize.y, scale, 0, handle, true, false);
		}
		// それ以外の文字は無視
	}
}

void DrawTimeAsImageMS(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I colonPos, VECTOR2I cutColon, VECTOR2I pointPos, VECTOR2I cutPoint)
{
	// 分・秒・ミリ秒を取り出し、"mm:ss.ms" 文字列に整形
	int minutes = ((int)timeSec % 3600) / 60;
	int seconds = (int)timeSec % 60;

	char buf[8]; // "mm:ss.m\0"
	std::snprintf(buf, sizeof(buf), "%01d:%02d", minutes, seconds);
	std::string timeStr(buf);

	// 末尾（秒の1桁目）から右詰めで描画
	for (int i = (int)timeStr.size() - 1; i >= 0; --i) {
		char c = timeStr[i];
		int srcX, srcY;
		float drawX = pos.x - interval * ((int)timeStr.size() - 1 - i);
		float drawY = pos.y;

		if (c == ':') {
			// コロンのテクスチャ座標
			srcX = colonPos.x;
			srcY = colonPos.y;
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutColon.x, cutColon.y, scale, 0, handle, true, false);
		}
		else if (c == '.')
		{
			// ポイント（ピリオド）のテクスチャ座標
			srcX = pointPos.x;
			srcY = pointPos.y;
			drawY = drawY + (scale * cutSize.y) / 3.2f;
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutPoint.x, cutPoint.y, scale, 0, handle, true, false);
		}
		else if (c >= '0' && c <= '9') {
			int digit = c - '0';
			srcX = digit * screen.x;
			srcY = screen.y;  // 数字テクスチャのY座標（固定）
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutSize.x, cutSize.y, scale, 0, handle, true, false);
		}
		// それ以外の文字は無視
	}
}

void DrawTimeAsImageM(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I colonPos, VECTOR2I cutColon, VECTOR2I pointPos, VECTOR2I cutPoint)
{
	// 分・秒・ミリ秒を取り出し、"mm:ss.ms" 文字列に整形
	int minutes = ((int)timeSec % 3600) / 60;
	int seconds = (int)timeSec % 60;
	int mmSecond = (timeSec - (int)timeSec) * 10;

	char buf[8]; // "mm:ss.m\0"
	std::snprintf(buf, sizeof(buf), "%02d:%02d.%01d", minutes, seconds, mmSecond);
	std::string timeStr(buf);

	// 末尾（秒の1桁目）から右詰めで描画
	for (int i = (int)timeStr.size() - 1; i >= 0; --i) {
		char c = timeStr[i];
		int srcX, srcY;
		float drawX = pos.x - interval * ((int)timeStr.size() - 1 - i);
		float drawY = pos.y;

		if (c == ':') {
			// コロンのテクスチャ座標
			srcX = colonPos.x;
			srcY = colonPos.y;
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutColon.x, cutColon.y, scale, 0, handle, true, false);
		}
		else if (c == '.')
		{
			// ポイント（ピリオド）のテクスチャ座標
			srcX = pointPos.x;
			srcY = pointPos.y;
			drawY = drawY + (scale * cutSize.y) / 3.2f;
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutPoint.x, cutPoint.y, scale, 0, handle, true, false);
		}
		else if (c >= '0' && c <= '9') {
			int digit = c - '0';
			srcX = digit * screen.x;
			srcY = screen.y;  // 数字テクスチャのY座標（固定）
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutSize.x, cutSize.y, scale, 0, handle, true, false);
		}
		// それ以外の文字は無視
	}
}

void DrawTimeAsImageS(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I pointPos, VECTOR2I cutPoint)
{
	// 秒・ミリ秒を取り出し、"ss.ms" 文字列に整形
	int seconds = (int)timeSec;
	int mmSecond = (timeSec - (int)timeSec) * 10;
	bool drawPoint = false;

	char buf[8]; // "mm:ss.m\0"
	std::snprintf(buf, sizeof(buf), "%01d.%01d", seconds, mmSecond);
	std::string timeStr(buf);

	// 末尾（秒の1桁目）から右詰めで描画
	for (int i = (int)timeStr.size() - 1; i >= 0; --i) {
		char c = timeStr[i];
		int srcX, srcY;
		float drawX = pos.x - interval * ((int)timeStr.size() - 1 - i);
		if (c != '.')
		{
			if(drawPoint)
			{
				drawX += cutPoint.x;
			}
			else
			{
				drawX -= cutPoint.x;
			}
		}
		float drawY = pos.y;

		if (c == '.')
		{
			// ポイント（ピリオド）のテクスチャ座標
			srcX = pointPos.x;
			srcY = pointPos.y;
			drawY = drawY + (scale * cutSize.y) / 3.2f;
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutPoint.x, cutPoint.y, scale, 0, handle, true, false);
			drawPoint = true;
		}
		else if (c >= '0' && c <= '9') {
			int digit = c - '0';
			srcX = digit * screen.x;
			srcY = screen.y;  // 数字テクスチャのY座標（固定）
			DrawRectRotaGraphF(drawX, drawY, srcX, srcY, cutSize.x, cutSize.y, scale, 0, handle, true, false);
		}
		// それ以外の文字は無視
	}
}

void DrawTimeAsImageHM(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I colonPos, VECTOR2I cutColon, VECTOR2I pointPos, VECTOR2I cutPoint)
{
	// 時・分・秒・ミリ秒を取り出し、"hh:mm:ss.ms" 文字列に整形
	int hours = (int)timeSec / 3600;
	int minutes = ((int)timeSec % 3600) / 60;
	int seconds = (int)timeSec % 60;
	int mmSecond = (timeSec - (int)timeSec) * 10;

	char buf[11]; // "hh:mm:ss.m\0"
	std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%01d", hours, minutes, seconds, mmSecond);
	std::string timeStr(buf);

	// 末尾（秒の1桁目）から右詰めで描画
	for (int i = (int)timeStr.size() - 1; i >= 0; --i) {
		char c = timeStr[i];
		int srcX, srcY;
		int drawX = (int)pos.x - interval * ((int)timeStr.size() - 1 - i);
		int drawY = (int)pos.y;

		if (c == ':') {
			// コロンのテクスチャ座標
			srcX = colonPos.x;
			srcY = colonPos.y;
			DrawRectRotaGraph(drawX, drawY, srcX, srcY, cutColon.x, cutColon.y, scale, 0, handle, true, false);
		}
		else if (c == '.')
		{
			// ポイント（ピリオド）のテクスチャ座標
			srcX = pointPos.x;
			srcY = pointPos.y;
			drawY = drawY + (scale * cutSize.y) / 3.2f;
			DrawRectRotaGraph(drawX, drawY, srcX, srcY, cutPoint.x, cutPoint.y, scale, 0, handle, true, false);
		}
		else if (c >= '0' && c <= '9') {
			int digit = c - '0';
			srcX = digit * screen.x;
			srcY = screen.y;  // 数字テクスチャのY座標（固定）
			DrawRectRotaGraph(drawX, drawY, srcX, srcY, cutSize.x, cutSize.y, scale, 0, handle, true, false);
		}
		// それ以外の文字は無視
	}
}

void DrawRotaNum(float _x, float _y, int _num, int _interval, int _numImage, int _width, int _height, float _exRate, float _angle, int _defaultZeroNum)
{
	int tmp = _num;
	int dig = 0;
	int commaNum = 0;
	do
	{
		DrawRectRotaGraphF(_x - (_interval * dig) - commaNum * 8, _y, tmp % 10 * _width, 0, _width, _height, _exRate, _angle, _numImage, true, false);
		tmp = tmp / 10;
		dig++;
	} while (tmp > 0||dig<_defaultZeroNum);
}

void DrawRotaNumStart(float _x, float _y, int _num, int _interval, int _numImage, int _width, int _height, int _startY, float _exRate, float _angle, int _defaultZeroNum)
{
	int tmp = _num;
	int dig = 0;
	int commaNum = 0;
	do
	{
		DrawRectRotaGraph(_x - (_interval * dig) - commaNum * 8, _y, tmp % 10 * _width,_startY , _width, _height, _exRate, _angle, _numImage, true, false);
		tmp = tmp / 10;
		dig++;
	} while (tmp > 0 || dig < _defaultZeroNum);
}
