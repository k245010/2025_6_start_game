#pragma once
#include "../../Library/myDxLib.h"     
#include <unordered_map>

//* 上杉 作 (自身の改変あり) *//

/// <summary>
/// ゲージを描画するクラス
/// </summary>
class Gauge 
{
public:

    Gauge();

    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="_pos">     表示座標   </param>
    /// <param name="_width">   横幅       </param>
    /// <param name="_heigth">  縦幅       </param>
    /// <param name="_min">     最小値     </param>
    /// <param name="_max">     最大値     </param>
    Gauge(const VECTOR2& _pos, const float& _width, const float& _heigth, const float& _min, const float& _max);
    ~Gauge();

	/// ゲージのスクロールタイプ
	/// 横に表示するか縦に表示するか
    enum class ScrollType { Horizontal, Vertical };

    /// <summary>
	/// DrawBoxAAを使用してゲージを描画します。
    /// </summary>
    /// <param name="pos">              座標                  </param>
    /// <param name="width">            横幅                  </param>
    /// <param name="height">           縦幅                  </param>
    /// <param name="current">          現在の値              </param>
    /// <param name="min">              最小                  </param>
    /// <param name="max">              最大                  </param>
    /// <param name="backgroundColor">  ゲージ裏の色          </param>
    /// <param name="fillColor">        ゲージの色            </param>
    /// <param name="delayColor">       減るときのゲージの色  </param>
    /// <param name="delaySpeed">       減る速度              </param>
    /// <param name="mode">             ゲージの向き          </param>
    /// <param name="borderColor">      枠の色                </param>
    /// <param name="borderThickness">  枠の太さ              </param>
    void Draw(
        const VECTOR2& pos,
        float width, float height,
        float current, float min, float max,
        const int& backgroundColor = 0x444444,
        const int& fillColor = 0x00ff00,
        const int& delayColor = 0xff0000,
        float delaySpeed = 0.5f,
        ScrollType mode = ScrollType::Horizontal,
        const int& borderColor = 0xffffff ,
        float borderThickness = 1.0f
    );

    /// <summary>
    ///                         DrawBoxAAを使用してゲージを描画します。
    /// </summary>
    /// <param name="pos">      座標        </param>
    /// <param name="current">  現在の値    </param>
    void Draw(
        const VECTOR2& pos,
        const float& current
    );

    /// <summary>
    /// DrawBoxAAを使用してゲージを描画します。
    /// </summary>
    /// <param name="current"> 現在の値 </param>
    void Draw(
       const float& current
    );
    

    // テクスチャゲージ描画
   /* void DrawTexture(
        const VECTOR2F& pos,
        float width, float height,
        float current, float min, float max,
        int fillTex, int backgroundTex = -1,
        float delaySpeed = 0.5f,
        ScrollType mode = ScrollType::Horizontal,
        float uScale = 1.0f,
        float vScale = 1.0f
    );*/

	/// <summary>
	/// 遅延値をリセット。
    ///<summary>
    void ResetDelay(float initialValue = 0.0f);

    /// <summary>
    /// ゲージが減るときのゲージを描画するかを設定する
    /// </summary>
    void SetDrawDelay(bool _set) { isDrawDelayGauge = _set; }

    /// <summary>
    ///                         
    /// </summary>
    /// <param name="_maxNum"></param>
    void SetMaxNum(const float& _maxNum) { defaultGaugeDrawData.maxNum = _maxNum; }

    /// <summary>
    ///                     デフォルトのゲージの色を設定する
    /// </summary>
    /// <param name="_r">   赤 </param>
    /// <param name="_g">   緑 </param>
    /// <param name="_b">   青 </param>
    void SetDefaultFillColor(const int& _r, const int& _g, const int& _b);

    /// <summary>
    ///                         デフォルトのゲージの色を設定する
    /// </summary>
    /// <param name="_color">   デフォルトのゲージの色    </param>
    void SetDefaultFillColor(const int& _color);

    /// <summary>
    ///                         ゲージ裏の色のアルファ値
    /// </summary>
    /// <param name="_alpha">   アルファ値  0~255の範囲   </param>
    void SetDrawBackgroundColorAlpha(const unsigned char& _alpha);

private:

    /// <summary>
    /// デフォルトのゲージの描画情報
    /// </summary>
    struct DefaultGaugeDrawData
    {
        VECTOR2 position;                           // ゲージが表示される座標
        float width;                                // 横幅
        float height;                               // 縦幅
        float minNum;                               // 最小値
        float maxNum;                               // 最大値
        int backgroundColor = 0x444444;             // ゲージ裏の色
        int fillColor = 0x00ff00;                   // ゲージの色
        int delayColor = 0xff0000;                  // 減るときのゲージの色 
        float delaySpeed = 0.5f;                    // 減る速度
        ScrollType mode = ScrollType::Horizontal;   // ゲージの向き
        int borderColor = 0xffffff;                 // 枠の色
        float borderThickness = 0.0f;               // 枠の太さ

        DefaultGaugeDrawData()
            : position(VECTOR2(0, 0)), 
            width(0.0f), height(0.0f), 
            minNum(0.0f), maxNum(0.0f),
            backgroundColor(0x444444),
            fillColor(0x00ff00),
            delayColor(0xff0000),delaySpeed(0.5f),
            mode(ScrollType::Horizontal),
            borderColor(0xffffff),borderThickness(0.0f) {};

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="_pos">             表示座標              </param>
        /// <param name="width">            横幅                  </param>
        /// <param name="height">           縦幅                  </param>
        /// <param name="current">          現在の値              </param>
        /// <param name="min">              最小                  </param>
        /// <param name="max">              最大                  </param>
        /// <param name="backgroundColor">  ゲージ裏の色          </param>
        /// <param name="fillColor">        ゲージの色            </param>
        /// <param name="delayColor">       減るときのゲージの色  </param>
        /// <param name="delaySpeed">       減る速度              </param>
        /// <param name="mode">             ゲージの向き          </param>
        /// <param name="borderColor">      枠の色                </param>
        /// <param name="borderThickness">  枠の太さ              </param>
        DefaultGaugeDrawData(
            const VECTOR2& _pos,
            const float& _width,
            const float& _heigth,
            const float& _min,
            const float& _max,
            const int& _bgColor = 0x444444,
            const int& _fillColor = 0x00ff00,
            const int& _delayColor = 0xff0000,
            const float& _delaySpeed = 0.5f,
            const ScrollType& _mode = ScrollType::Horizontal,
            const int& _borderColor = 0xffffff,
            const float& _borderThickness = 1.0f)
            : position(_pos), 
            width(_width), height(_heigth), 
            minNum(_min), maxNum(_max),
            backgroundColor(_bgColor),
            fillColor(_fillColor),
            delayColor(_delayColor),delaySpeed(_delaySpeed),
            mode(_mode),
            borderColor(_borderColor),borderThickness(_borderThickness)
        {
            // 枠はゲージの大きさに影響するので、計算してから初期化
            borderThickness = height * 0.1f;    // ゲージの高さに対して、10分の1の太さで設定
        };
    };

    DefaultGaugeDrawData defaultGaugeDrawData;      // デフォルトのゲージの描画情報

	int id;                                         // ゲージID
	static int nextId;                              // 次のID
	static std::unordered_map<int, float> delayMap; // 遅延値マップ
    bool isDrawDelayGauge = true;                   // ゲージが減るときのゲージを描画するかのフラグ
    unsigned char backgroundColorAlpha = 255;       // 裏側のゲージの色のアルファ値

    /// <summary>
	/// 遅延値を更新
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="stored"></param>
    /// <param name="current"></param>
    /// <param name="speed"></param>
    template<typename T>
    void UpdateDelay(T& stored, T current, float speed);
};
