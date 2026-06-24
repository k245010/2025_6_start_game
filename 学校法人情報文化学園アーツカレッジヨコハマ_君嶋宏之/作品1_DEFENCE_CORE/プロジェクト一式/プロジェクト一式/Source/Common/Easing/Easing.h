#pragma once
#include <functional>

/*
 西　作
 Easing h
*/

/// <summary>
/// easingを使うときはこの関数を使う
/// </summary>
namespace Easing {
    /// <summary>
    /// 二点の間の値を求める
    /// 等速
    /// </summary>
    template<typename T>
    T inline Lerp(T a, T b, float t) {
        return a + (b - a) * t;
    }
    /// <summary>
    /// 徐々に加速するLerp
    /// </summary>
    template<typename T>
    T inline EaseIn(T a, T b, float t) {
        t = t * t;
        return Lerp<T>(a,b,t);
    }

    /// <summary>
    /// 徐々に停止するLerp
    /// </summary>
    template<typename T>
    T inline EaseOut(T a, T b, float t) {
        t = 1 - (1 - t) * (1 - t);
        return Lerp<T>(a, b, t);
    }

    /// <summary>
    /// スムーズに加速し終わり際に減速
    /// </summary>
    template<typename T>
    T inline EaseInOut(T a, T b, float t) {
        if (t < 0.5f) {
            t = 2 * t * t;
        }
        else {
            t = 1 - ((t - 1) * (t - 1) * 2);
        }
        return Lerp<T>(a, b, t);
    }

    /// <summary>
    /// よりスムーズに変化
    /// </summary>
    template<typename T>
    T inline SmoothStep(T a, T b, float t) {
        t = t * t * (3 - 2 * t);
        return Lerp<T>(a, b, t);
    }

    template<typename T>
    T inline EaseInExpo(T a, T b, float t) {
        t = powf(2, 10 * t - 10);
        return Lerp<T>(a, b, t);
    }



    /// <summary>
    /// 逆方向に少し戻ってから最終位置で減速するイージング
    /// </summary>
    template<typename T>
    T inline EaseOutBack(T a, T b, float t) {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;

        t = 1 + c3 * powf(t - 1, 3) + c1 * powf(t - 1, 2);
        return Lerp<T>(a, b, t);
    }
    /// <summary>
    /// 弾むような振動で終了するイージング (Ease Out Elastic)
    /// </summary>
    template<typename T>
    T inline EaseOutElastic(T a, T b, float t) {
        const float c4 = (2.0f * DX_PI_F) / 3.0f;

        if (t == 0.0f) return a;
        if (t == 1.0f) return b;

        float easedT = powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
        return Lerp<T>(a, b, easedT);
    }
    /// <summary>
    /// 弾むような振動で開始するイージング (Ease In Elastic)
    /// </summary>
    template<typename T>
    T inline EaseInElastic(T a, T b, float t) {
        const float c4 = (2.0f * DX_PI_F) / 3.0f;

        if (t == 0.0f)
            return a;
        if (t == 1.0f)
            return b;

        float eased = -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
        return Lerp<T>(a, b, eased);
    }
    /// <summary>
    /// 軽く戻しを伴いながら加速して開始するイージング (Ease In Back)
    /// </summary>
    template<typename T>
    T inline EaseInBack(T a, T b, float t) {

        const float c1 = 1.70158f;
        const float c3 = c1 + 1;

        t = c3 * t * t * t - c1 * t * t;
        return Lerp<T>(a, b, t);

    }

    template<typename T>
    T inline SinCube(T a, T b, float t) {

        t = sinf(t * DX_PI_F);
        return Lerp<T>(a, b, t);

    }

    //sin波の90度まで(0～1にいって0に戻る動き)を行うための関数。
    template<typename T>
    T inline Sin90Cube(T a, T b, float t) {

        t = sinf(t * DX_PI_F / 2);
        return Lerp<T>(a, b, t);

    }

    template<typename T>
    T EasingFlow(float* _counter, float _maxTime, T min, T max, std::function<T(T, T, float)> _func) {
        if (*_counter <= 0.0f) {
            return T();
        }
        *_counter = max(*_counter - Time::GetGameDeltaTimeRate(), 0.0f);
        float rate = *_counter / _maxTime;
        return _func(min, max, rate);
    }

}