#include "gauge.h"
#include <algorithm>

int Gauge::nextId = 0;
std::unordered_map<int, float> Gauge::delayMap;
template void Gauge::UpdateDelay<float>(float&, float, float);

Gauge::Gauge() : id(nextId++) 
{
    delayMap[id] = 0.0f;
    
    defaultGaugeDrawData = DefaultGaugeDrawData();
}

Gauge::Gauge(const VECTOR2& _pos, const float& _width, const float& _heigth, const float& _min, const float& _max) : id(nextId++)
{
    defaultGaugeDrawData = DefaultGaugeDrawData(_pos, _width, _heigth, _min, _max);
}

Gauge::~Gauge()
{
}

void Gauge::Draw(const VECTOR2& pos, float width, float height, float current, float min, float max, const int& backgroundColor, const int& fillColor, const int& delayColor, float delaySpeed, ScrollType mode, const int& borderColor, float borderThickness)
{
    float x = pos.x, y = pos.y;
    current = std::clamp(current, min, max);
    float& delayed = delayMap[id];
    UpdateDelay(delayed, current, delaySpeed);
    delayed = std::clamp(delayed, min, max);

    if (backgroundColor)
    {
        if (backgroundColorAlpha != 255)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, backgroundColorAlpha);

            DrawBoxAA(x, y, x + width, y + height, backgroundColor, TRUE);

            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        else
        {
            DrawBoxAA(x, y, x + width, y + height, backgroundColor, TRUE);
        }
    }
    if (borderThickness > 0)
        DrawBoxAA(x - borderThickness / 2, y - borderThickness / 2, x + width + borderThickness / 2, y + height + borderThickness / 2, borderColor, FALSE, borderThickness);

    float range     = max - min;
    float pctCurr   = (current - min) / range;
    float pctDelay  = (delayed - min) / range;

    if (mode == ScrollType::Horizontal) {

        if (isDrawDelayGauge)
            DrawBoxAA(x, y, x + width * pctDelay, y + height, delayColor, TRUE);
        DrawBoxAA(x, y, x + width * pctCurr, y + height, fillColor, TRUE);
    }
    else {
        if (isDrawDelayGauge)
            DrawBoxAA(x, y + height * (1 - pctDelay), x + width, y + height, delayColor, TRUE);
        DrawBoxAA(x, y + height * (1 - pctCurr), x + width, y + height, fillColor, TRUE);
    }
}

void Gauge::Draw(const VECTOR2& pos, const float& current)
{
    const DefaultGaugeDrawData& defaultData = defaultGaugeDrawData;
    Draw(pos, defaultData.width, defaultData.height, current, defaultData.minNum, defaultData.maxNum, defaultData.backgroundColor, defaultData.fillColor, defaultData.delayColor, defaultData.delaySpeed, defaultData.mode, defaultData.borderColor, defaultData.borderThickness);
}

void Gauge::Draw(const float& current)
{
    const DefaultGaugeDrawData& defaultData = defaultGaugeDrawData;
    Draw(defaultData.position, defaultData.width, defaultData.height, current, defaultData.minNum, defaultData.maxNum, defaultData.backgroundColor, defaultData.fillColor, defaultData.delayColor, defaultData.delaySpeed, defaultData.mode, defaultData.borderColor, defaultData.borderThickness);
}

void Gauge::ResetDelay(float initialValue) 
{
    delayMap[id] = initialValue;
}

void Gauge::SetDefaultFillColor(const int& _r, const int& _g, const int& _b)
{
    defaultGaugeDrawData.fillColor = GetColor(_r, _g, _b);
}

void Gauge::SetDefaultFillColor(const int& _color)
{
    defaultGaugeDrawData.fillColor = _color;
}

void Gauge::SetDrawBackgroundColorAlpha(const unsigned char& _alpha)
{
    assert((_alpha >= 0 && _alpha <= 255) && "不正なアルファ値です。");

    backgroundColorAlpha = _alpha;
}

template<typename T>
void Gauge::UpdateDelay(T& stored, T current, float speed) 
{
	// 線形補間で遅延値を更新
    stored += (current - stored) * speed;
}

