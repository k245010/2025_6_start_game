#pragma once
#include "loadData.h"
#include "slowMostionManager.h"

/// <summary>
/// オブジェクトの配置を細かく変えたいときに使う関数。
/// Vector2でもVECTORでも使えるようにしました。
/// </summary>



void ObjectPosisionMove(DxLib::VECTOR2F* _pos,float _speed);
void ObjectPosisionMove(VECTOR* _pos, float _speed);

/// <summary>
/// Vector2やVECTORのポジションを写すときにいちいち書くのはめんどくさいので関数化しました。
/// ここにいれればVECTORの位置が見えるようになるので上にある関数と連携して使ってください。
/// </summary>
/// <param name="_drawPos">ポジションを描画させる場所</param>
/// <param name="position">見せる対象となる位置</param>
/// <param name="string">nullptrを入れればデフォルトの設定に文字を入れれば好きな文字を描画させられる。</param>

void Vector2Draw(const DxLib::VECTOR2I _drawPos, const DxLib::VECTOR2F position,const char* string);
void VECTORDraw(const DxLib::VECTOR2I _drawPos, const VECTOR position, const char* string);

SlowMostionManager::SlowModeState SlowMosion();

/// <summary>///
/// MassageBoxを簡易的に使いやすくしたものエラー文とタイトルを指定してメッセージを出すことが可能
/// 本来のMassageBoxはさらにmassageに対するアクションを変えることが出来るが基本変えないので抜きました
/// </summary>/// 
/// <param name="_error">エラー文</param>
/// /// <param name="_title">左上に出る文章</param>
void CreateMessageBox(const std::string& _error, const std::string& _title);

/// <summary>
/// 二点の間の値を求める
/// 等速
/// </summary>
template<typename T>
T Lerp(T a, T b, float t) {
    return a + (b - a) * t;
}
/// <summary>
/// 徐々に加速するLerp
/// </summary>
template<typename T>
T EaseIn(T a, T b, float t) {
    t = t * t;
    return a + (b - a) * t;
}

/// <summary>
/// 徐々に停止するLerp
/// </summary>
template<typename T>
T EaseOut(T a, T b, float t) {
    t = 1 - (1 - t) * (1 - t);
    return a + (b - a) * t;
}

/// <summary>
/// スムーズに加速し終わり際に減速
/// </summary>
template<typename T>
T EaseInOut(T a, T b, float t) {
    if (t < 0.5f) {
        t = 2 * t * t;
    }
    else {
        t = 1 - ((t - 1) * (t - 1) * 2);
    }
    return a + (b - a) * t;
}

/// <summary>
/// よりスムーズに変化
/// </summary>
template<typename T>
T SmoothStep(T a, T b, float t) {
    t = t * t * (3 - 2 * t);
    return a + (b - a) * t;
}

template<typename T>
T EaseInExpo(T a, T b, float t) {
    t = powf(2, 10 * t - 10);
    return a + (b - a) * t;
}



/// <summary>
/// 逆方向に少し戻ってから最終位置で減速するイージング
/// </summary>
template<typename T>
T EaseOutBack(T a, T b, float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;

    t = 1 + c3 * powf(t - 1, 3) + c1 * powf(t - 1, 2);
    return a + (b - a) * t;
}
/// <summary>
/// 弾むような振動で終了するイージング (Ease Out Elastic)
/// </summary>
template<typename T>
T EaseOutElastic(T a, T b, float t) {
    const float c4 = (2.0f * DX_PI_F) / 3.0f;

    if (t == 0.0f) return a;
    if (t == 1.0f) return b;

    float easedT = powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
    return a + (b - a) * easedT;
}
/// <summary>
/// 弾むような振動で開始するイージング (Ease In Elastic)
/// </summary>
template<typename T>
T EaseInElastic(T a, T b, float t) {
    const float c4 = (2.0f * DX_PI_F) / 3.0f;

    if (t == 0.0f)
        return a;
    if (t == 1.0f)
        return b;

    float eased = -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
    return a + (b - a) * eased;
}
/// <summary>
/// 軽く戻しを伴いながら加速して開始するイージング (Ease In Back)
/// </summary>
template<typename T>
T EaseInBack(T a, T b, float t) {

    const float c1 = 1.70158f;
    const float c3 = c1 + 1;

    t = c3 * t * t * t - c1 * t * t;
    return a + (b - a) * t;

}

template<typename T>
T SinCube(T a, T b, float t) {

    t = sinf(t * DX_PI_F);
    return a + (b - a) * t;

}


inline VECTOR Lerp(const VECTOR& start, const VECTOR& end, float t) {
    return VAdd(VScale(start, 1.0f - t), VScale(end, t));
}

/// <summary>///
/// 数字画像を使用して描画をする関数
/// </summary>/// 
/// <param name="_x  _y">一番右に描画する文字の中央座標</param>
/// <param name="_num">描画したい数値</param>
/// <param name="_interval">文字同士の間隔</param>
/// <param name="_numImage">描画に使用する数字の画像のハンドル</param>
/// <param name="_width _height">画像の縦横の大きさ</param>
/// <param name="_exRate">画像の拡大率</param>
/// <param name="_angle">画像の傾き角度</param>
/// <param name="_defaultZeroNum">0を何桁まで初期表示させるか　3を入力した場合001と表示される</param>
void DrawRotaNum(float _x, float _y, int _num, int _interval, int _numImage, int _width, int _height, float _exRate, float _angle,int _defaultZeroNum=1);
void DrawRotaNumStart(float _x, float _y, int _num, int _interval, int _numImage, int _width, int _height,int _startY, float _exRate, float _angle,int _defaultZeroNum=1);

/// <summary>
/// タイムh:m:sを画像で描画する関数
/// </summary>
/// <param name="pos"> 描画開始位置（右端）</param>
/// <param name="timeSec"> 描画したい時刻（秒）</param>
/// <param name="interval"> 文字間隔 </param>
/// <param name="handle"> グラフィックハンドル </param>
/// <param name="scale"> 拡大率 </param>
/// <param name="screen"> 数字１桁分のテクスチャ幅・高さ </param>
/// <param name="cutSize"> 切り出しサイズ </param>
/// <param name="colonPos"> コロンのテクスチャ左上座標 </param>
/// <param name="cutColon"> 切り出しコロンのテクスチャ左上座標 </param>
void DrawTimeAsImageH(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I colonPos, VECTOR2I cutColon);

/// <summary>
/// タイムm:sを画像で描画する関数
/// </summary>
/// <param name="pos"> 描画開始位置（右端）</param>
/// <param name="timeSec"> 描画したい時刻（秒）</param>
/// <param name="interval"> 文字間隔 </param>
/// <param name="handle"> グラフィックハンドル </param>
/// <param name="scale"> 拡大率 </param>
/// <param name="screen"> 数字１桁分のテクスチャ幅・高さ </param>
/// <param name="cutSize"> 切り出しサイズ </param>
/// <param name="colonPos"> コロンのテクスチャ左上座標 </param>
/// <param name="cutColon"> 切り出しコロンのテクスチャ左上座標 </param>
void DrawTimeAsImageMS(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I colonPos, VECTOR2I cutColon, VECTOR2I pointPos, VECTOR2I cutPoint);

/// <summary>
/// タイムm:s.msを画像で描画する関数
/// </summary>
/// <param name="pos"> 描画開始位置（右端）</param>
/// <param name="timeSec"> 描画したい時刻（秒）</param>
/// <param name="interval"> 文字間隔 </param>
/// <param name="handle"> グラフィックハンドル </param>
/// <param name="scale"> 拡大率 </param>
/// <param name="screen"> 数字１桁分のテクスチャ幅・高さ </param>
/// <param name="cutSize"> 切り出しサイズ </param>
/// <param name="colonPos"> コロンのテクスチャ左上座標 </param>
/// <param name="cutColon"> 切り出しコロンのテクスチャ左上座標 </param>
void DrawTimeAsImageM(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I colonPos, VECTOR2I cutColon, VECTOR2I pointPos, VECTOR2I cutPoint);

/// <summary>
/// タイムs.msを画像で描画する関数
/// </summary>
/// <param name="pos"> 描画開始位置（右端）</param>
/// <param name="timeSec"> 描画したい時刻（秒）</param>
/// <param name="interval"> 文字間隔 </param>
/// <param name="handle"> グラフィックハンドル </param>
/// <param name="scale"> 拡大率 </param>
/// <param name="screen"> 数字１桁分のテクスチャ幅・高さ </param>
/// <param name="cutSize"> 切り出しサイズ </param>
/// <param name="colonPos"> コロンのテクスチャ左上座標 </param>
/// <param name="cutColon"> 切り出しコロンのテクスチャ左上座標 </param>
void DrawTimeAsImageS(const VECTOR2F& pos, float timeSec, int interval, int handle, float scale, VECTOR2I screen, VECTOR2I cutSize, VECTOR2I pointPos, VECTOR2I cutPoint);
