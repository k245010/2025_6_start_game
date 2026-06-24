#pragma once
#include <map>
#include <string>
#include <assert.h>

/// <summary>
/// アニメーションを再生する
/// </summary>
class Animator {
public:
	/// <summary>
	/// アニメーションクラスを作る
	/// </summary>
	/// <param name="model">アニメーションを付けるモデル</param>
	Animator(int model);
	~Animator();

	/// <summary>
	/// アニメーションの再生をする
	/// </summary>
	void Update();

	/// <summary>
	/// アニメーションファイルを追加する
	/// </summary>
	/// <param name="id">アニメーション番号</param>
	/// <param name="filename">アニメーションファイル</param>
	/// <param name="loop">ループする場合はtrue</param>
	/// <param name="speed">再生倍率（基本は1.0）</param>
	void AddFile(int id, std::string filename, bool loop, float speed = 1.0f);

	/// <summary>
	/// アニメーションファイルをアニメーションインデックスを指定して追加する
	/// </summary>
	/// <param name="id">アニメーション番号</param>
	/// <param name="filename">アニメーションファイル</param>
	/// <param name="loop">ループする場合はtrue</param>
	/// <param name="aniIndex">アニメーションの読み込むインデックス</param>
	/// <param name="speed">再生倍率（基本は1.0）</param>
	void AddFileSettingAniIndex(int id, std::string filename, bool loop, int aniIndex, float speed = 1.0f);

	/// <summary>
	/// アニメーションを再生する
	/// </summary>
	/// <param name="id">再生するアニメーションの番号</param>
	/// <param name="margeTime">前のアニメーションとのブレンド時間</param>
	void Play(int id, float margeTime = 0.2f);

	/// <summary>
	/// 全体のアニメーションの再生速度を変更する
	/// </summary>
	/// <param name="speed">再生速度（倍率）</param>
	void SetPlaySpeed(float speed);

	/// <summary>
	///							個々のアニメーションの再生速度を変更する
	/// </summary>
	/// <param name="_key">		アニメーションのkey </param>
	/// <param name="_speed">	再生速度（倍率）	</param>
	void SetPlaySpeed(int _key,float _speed);

	/// <summary>
	/// 今のアニメーションの再生速度を返す
	/// </summary>
	/// <returns>再生速度（倍率）</returns>
	float GetPlaySpeed();

	/// <summary>
	/// アニメーションが終ったか調べる
	/// ループしないアニメーションが、最後のフレームまで再生したものを、終りとする
	/// </summary>
	/// <returns>終っていればtrue</returns>
	bool IsFinish();

	/// <summary>
	/// 現在再生しているアニメーション番号
	/// </summary>
	/// <returns>アニメーション番号</returns>
	int GetCurrentID();

	/// <summary>
	/// 現在の再生フレーム
	/// </summary>
	/// <returns>再生フレーム</returns>
	float GetCurrentFrame();

	/// <summary>
	/// 現在の再生フレームを設定する
	/// </summary>
	/// <param name="_f"> 設定する再生フレーム </param>
	void SetCurrentFrame(const float& _f) 
	{
		if (_f < 0 || _f > GetMaxFrame())
			assert(false && "設定しようとした値は無効です");
		current.frame = _f;
	};

	/// <summary>
	/// 現在再生中アニメーションの最大フレームを返す
	/// </summary>
	/// <returns>最大フレーム</returns>
	float GetMaxFrame();
private:
	int baseModel;
	struct FileInfo {
		int hModel;
		bool loop;
		int aniIndex;
		float maxFrame;
		float playSpeed;
		FileInfo() : hModel(-1), loop(false), aniIndex(0), maxFrame(1.0f), playSpeed(1.0f) {}
	};
	std::map<int, FileInfo> fileInfos;

	struct PlayInfo {
		int fileID;
		int attachID;
		float frame;
		PlayInfo() : fileID(-1), attachID(-1), frame(0.0f) {}
	};
	PlayInfo current; // 今のアニメ
	PlayInfo before; // 前のアニメ

	float blendTime; // ブレンドのタイマー
	float blendTimeMax; // ブレンドのタイマー最大値

	bool finished;
	float playSpeed;
};