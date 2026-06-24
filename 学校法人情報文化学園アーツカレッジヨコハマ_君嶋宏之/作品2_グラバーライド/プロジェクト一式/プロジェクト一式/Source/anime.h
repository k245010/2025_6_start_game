#pragma once
#include "../Library/gameObject.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "function.h"
#include "loadManager.h"

enum class PlayerAnimesionKind
{
	None = -1,
	Wait = 0,
	Walk,
	Zero_Gravity,
	Jump_Up,
	Jump_Down,
	Land,
	Damage,
	Gravity_Move,
	Die,
	Attack,
	Goal,
	Rail,
};

enum class EnemyNomalAnimationKind
{
	None = -1,
	//Wait = 0,
	Walk,
	Turn,
};

enum class EnemyFlyAnimationKind
{
	None = -1,
	//Wait = 0,
	Walk,
};

/// <summary>
///	アニメーションクラスで作成するときの手順
/// ① newで作る。(CreateGameObjectはつかわない)
/// ② 次にAnimeInformationを呼ぶ。
/// ③ AnimeStartHandleを呼ぶ。
/// そこからアニメーションを切り替えたいならAnimeChangeを呼ぶ。
/// 下の関数の拡張機能を使えばスピードを変えたりできる。
/// </summary>
/// <typeparam name="TAnime"></typeparam>

template <typename TAnime>
class Anime
{
public:
	Anime(LoadManager* _motion) {
		previewAnime = AnimeReset(previewAnime.animeKind);
		beforeAnime = AnimeReset(beforeAnime.animeKind);
		animeTime = 0.0f;
		motion = _motion;
		plus = true;
		//hModel = MV1LoadModel("data\\model\\animesion\\Walking (2).mv1");
		//hModel2 = MV1LoadModel("data\\model\\animesion\\Flying.mv1");
	};
	~Anime() { 
		AnimeRemove();
		animeInformation.clear(); 
	}
	void Update() {
		if (previewAnime.index >= 0) {
			if (plus) {
				previewAnime.time += previewAnime.speed;
				if (previewAnime.time >= previewAnime.totalTime) {
					previewAnime.time = previewAnime.totalTime;
				}
				if (beforeAnime.index >= 0) {
					if (blendNum >= 1.0f) {
						ChangeFinish();
						beforeAnime.time = 0;
					}
					else {
						blendNum += blendAmoumt;
						if (blendNum >= 1.0f) {
							blendNum = 1.0f;
						}
						BlendChange(blendNum);
					}
				}
				int animeNum = static_cast<int>(previewAnime.animeKind);
				if (previewAnime.time >= previewAnime.totalTime) {
					if (previewAnime.loop) {
						previewAnime.time = previewAnime.time - previewAnime.totalTime;
					}
					else {
						//AnimeRemove();
						previewAnime.time = previewAnime.totalTime;
					}
				}
				if (previewAnime.time < 0.0f) {
					previewAnime.time = 0.0f;
				}
			}
			MV1SetAttachAnimTime(previewAnime.hHandle, previewAnime.index, previewAnime.time);
		}
	};
	//void Draw();

	/// <summary>
	/// アニメーションに関する情報を格納しておくvector配列に値を保存しておく関数。
	/// ここでは格納するだけなのでソートは行わない。（別の関数でソートされるようになっているので順番は気にしなくてよい）
	/// ここで入れた情報からアニメーションを再生させられるのでこのクラスをnewしたらすぐに値を入れるのが望ましい
	/// </summary>
	/// <param name="_handle">アニメーションを行うハンドル</param>
	/// <param name="loop">このアニメーションはループ再生をするかどうか</param>
	/// <param name="kind">アニメーションの種類</param>
	//void AnimeInformation(int _handle, bool loop, TAnime kind, float _speed) {
	//	int anime = static_cast<int>(kind);
	//	animeInformation.push_back(InitAnime(_handle, loop, kind, _speed));
	//	//animeInformation[kind] = InitAnime(_handle, loop, kind);
	//}

	void AnimeInformation(int _handle, bool loop, TAnime kind, float _speed, std::string _string) {
		int anime = static_cast<int>(kind);
		animeInformation.push_back(InitAnimeHandle(_handle, loop, kind, _speed, _string));
	}

	/// <summary>
	/// この関数でソートをしているので順番はどうなっていても数さえあればかまわない
	/// </summary>
	void AnimeSort() {
		std::sort(animeInformation.begin(), animeInformation.end());
	}

	
	//TAnime AnimeStart(TAnime kind) {
	//	/*if (previewAnime.animeKind == beforeAnime.animeKind) {
	//		return  static_cast<TAnime>(-1);
	//	}*/
	//	int animeNum = static_cast<int>(kind);
	//	previewAnime = GetAnimesion(animeNum);
	//	previewAnime.index = MV1AttachAnim(previewAnime.hHandle, animeNum);
	//	return kind;
	//}
	/// <summary>
	/// 仮置き用のモデルスタート
	/// </summary>
	/// <param name="kind"></param>
	/// <returns></returns>
	/// /// <summary>
	///	アニメーションをスタートさせる関数。
	/// これにアニメーションの種類を引数に入れれば再生が可能になる。
	/// 先ほどのAnimeInformation関数を呼びだし、情報を格納しないとで再生することができない。
	/// </summary>
	/// <param name="kind"></param>
	/// <returns></returns>
	TAnime AnimeStartHandle(TAnime kind) {
		//if (previewAnime.animeKind == beforeAnime.animeKind) {
			//return  static_cast<int>(-1);
		//}
		int animeNum = static_cast<int>(kind);
		previewAnime = GetAnimesion(animeNum);
		previewAnime.index = MV1AttachAnim(previewAnime.hHandle, 0, previewAnime.animeHandle);
		previewAnime.totalTime = MV1GetAttachAnimTotalTime(previewAnime.hHandle, previewAnime.index);
		return kind;
	}

	void AnimeRemove() {
		MV1DetachAnim(previewAnime.hHandle, previewAnime.index);
		MV1DetachAnim(beforeAnime.hHandle, beforeAnime.index);
		previewAnime = AnimeReset(previewAnime.animeKind);
		beforeAnime = AnimeReset(beforeAnime.animeKind);
	}
	/// <summary>
	/// アニメーションを変えるときに使う関数でチェンジするアニメーションの種類を引数に入れる。
	/// またブレンドにかかる時間を入れることによって、好きな時間でブレンドを行うことができる。
	/// </summary>
	/// <param name="kind"></param>
	/// <param name="_time"></param>
	/// <returns></returns>
	TAnime AnimeChange(TAnime kind, float _time) {
		/*if (previewAnime.animeKind == beforeAnime.animeKind) {
			return static_cast<TAnime>(-1);
		}*/
		/*if (blendNum < 1.0f && blendNum != 0.0f) {
			return static_cast<TAnime>(-1);
		}*/
		if (previewAnime.animeKind == kind) {
			return static_cast<TAnime>(-1);
		}
		if (beforeAnime.index != -1) {
			next = kind;
			return static_cast<TAnime>(-1);
		}
		beforeAnime = previewAnime;
		next = static_cast<TAnime>(-1);
		blendNum = 0.0f;
		//blendAnimeTime = _time;
		blendAmoumt = abs(1.0f / _time);
		AnimeStartHandle(kind);
		beforeAnime.speed = previewAnime.speed;
		return kind;
	}

	//void RemoveAnime(int _animeKind, int _nowAnime);
	void BlendChange(float _blendRate) {
		MV1SetAttachAnimBlendRate(beforeAnime.hHandle, beforeAnime.index, 1.0f - _blendRate);
		MV1SetAttachAnimBlendRate(previewAnime.hHandle, previewAnime.index, _blendRate);
	}

	void ChangeFinish() {
		MV1DetachAnim(beforeAnime.hHandle, beforeAnime.index);
		beforeAnime = AnimeReset(beforeAnime.animeKind);
		blendNum = 0.0f;
	}


	struct Animesion
	{
		int hHandle;
		int animeHandle;
		float totalTime;
		int index;
		bool loop;
		TAnime animeKind;
		float speed;
		float time;

		bool operator< (const Animesion& a)const {
			return static_cast<int>(a.animeKind) > static_cast<int>(animeKind);
		}
	};

	Animesion InitAnime(int _handle, bool loop, TAnime kind, float _speed) {
		Animesion anime;
		anime.hHandle = _handle;
		anime.loop = loop;
		int animeNum = static_cast<int>(kind);
		anime.index = MV1AttachAnim(_handle, animeNum);
		anime.totalTime = MV1GetAttachAnimTotalTime(_handle, anime.index);
		anime.index = MV1DetachAnim(_handle, anime.index);
		anime.index = -1;
		anime.animeKind = kind;
		anime.speed = _speed;
		anime.time = 0;
		//anime.animeHandle = hModel2;
		return anime;
	}

	Animesion InitAnimeHandle(int _handle, bool loop, TAnime kind, float _speed, std::string _string) {
		Animesion anime;
		anime.hHandle = _handle;
		anime.animeHandle = motion->GetHandle(_string);
		anime.loop = loop;
		int animeNum = static_cast<int>(kind);
		anime.index = MV1AttachAnim(_handle, 0, anime.animeHandle);
		anime.totalTime = MV1GetAttachAnimTotalTime(_handle, anime.index);
		anime.index = MV1DetachAnim(_handle, anime.index);
		anime.index = -1;
		anime.animeKind = kind;
		anime.speed = _speed;
		anime.time = 0.0f;
		return anime;
	}

	Animesion GetAnimesion(int kind) {
		Animesion anime;
		anime = animeInformation[kind];
		return anime;
	}

	Animesion AnimeReset(TAnime kind) {
		Animesion anime;
		int animeNum = static_cast<int>(kind);
		//anime.index = MV1DetachAnim(anime.hHandle,animeNum);
		anime.hHandle = -1;
		anime.animeHandle = -1;
		anime.loop = false;
		anime.totalTime = 0;
		anime.index = -1;
		anime.animeKind = static_cast<TAnime>(-1);
		anime.time = 0.0f;
		return anime;
	}

	/// <summary>
	/// 途中でアニメーションの速度を変えるときに必要になってくる関数。
	/// 引数にスピードを入れればスピードが変わるようになっている。
	/// </summary>
	/// <param name="_speed"></param>
	/// <returns></returns>
	int AnimesionChangeSpeed() {
		if (previewAnime.index == -1) {
			return -1;
		}
		previewAnime.speed *= -1.0f;
		return static_cast<int>(previewAnime.speed);
	}
	int AnimesionChangeSpeed(float _speed) {
		if (previewAnime.index == -1) {
			return -1;
		}
		previewAnime.speed = _speed;
		return static_cast<int>(previewAnime.speed);
	}
	/// <summary>
	/// アニメーションでもし移動する場合があった時にそのボーンの位置にポジションを合わせなければいけないので
	/// その時にこの関数を使う。
	/// ボーン番号を引数に書けばそのボーンのローカル座標が出るのでそこで位置をとってくる。
	/// </summary>
	/// <param name="_index"></param>
	/// <returns></returns>
	VECTOR AnimesionFramePositionIndex(int _index) {
		if (MV1GetFrameName(previewAnime.hHandle, _index) == NULL) {
			return VGet(0.0f, 0.0f, 0.0f);
		}
		return MV1GetAttachAnimFrameLocalPosition(previewAnime.hHandle, previewAnime.index, _index);
	}

	Animesion GetAnimesion() {
		return previewAnime;
	}

	Animesion GetBeforeAnimesion() {
		return beforeAnime;
	}

	float GetTime() { return animeTime; }

	TAnime GetNext() { return next; }

	/// <summary>
	/// これの値をfalseにするとアニメーションを止めれるようにする機能の追加。
	/// </summary>
	/// <param name="_plus"></param>
	void SetPlus(bool _plus) { plus = _plus; }

	/// <summary>
	/// アニメーションを最初に戻したいときに使う。
	/// </summary>
	void AnimeFirstBack() {
		previewAnime.time = 0.0f;
	}

private:

	std::vector<Animesion> animeInformation;
	//std::vector<Animesion> beforeAnime;

	Animesion previewAnime;
	Animesion beforeAnime;

	//float blendAnimeTime;
	float blendAmoumt;
	float blendNum;

	float animeTime;
	LoadManager* motion;
	TAnime next;
	bool plus;
	//int hModel;
	//int hModel2;
};