#include "anime.h"

//template<typename TAnime>
//Anime<TAnime>::Anime()
//{
//}
//template<typename TAnime>
//Anime<TAnime>::~Anime()
//{
//	animeInformation.clear();
//}
//template<typename TAnime>
//void Anime<TAnime>::Update()
//{
//	
//}
//
//template<typename TAnime>
//void Anime<TAnime>::AnimeRemove()
//{
//}
//
//template<typename TAnime>
//void Anime<TAnime>::BlendChange(float _blendRate)
//{
//	
//}
//template<typename TAnime>
//void Anime<TAnime>::ChangeFinish()
//{
//	
//}
//
//void Anime::SetAnime(int _animeKind, int _nowAnime)
//{
//	if (beforeAnimeIndex == -1) {
//		return;
//	}
//	MV1DetachAnim(meshHandle, beforeAnimeIndex);
//	beforeAnimeIndex = -1;
//	MV1SetAttachAnimBlendRate(meshHandle,animeAttackIndex, 1.0f);
//	
//	AnimeTotalTime = MV1GetAttachAnimTotalTime(meshHandle, animeAttackIndex);
//
//	attackNowTime = 0.0f;
//}
//
//void Anime::RemoveAnime(int _animeKind, int _nowAnime)
//{
//	if (_animeKind == _nowAnime) {
//		attackNowTime -= AnimeTotalTime;
//		return;
//	}
//	beforeAnimeIndex = MV1AttachAnim(meshHandle, 0, animeHandle[_nowAnime]);
//	beforeAnimeKind = _nowAnime;
//
//	animeAttackIndex = MV1AttachAnim(meshHandle, 0, animeHandle[_animeKind]);
//	animeKind = _animeKind;
//	
//	animeStartTime = 0;
//	//animeAttackIndex = -1;
//	AnimeTotalTime = 0.0f;
//	attackNowTime = 0.0f;
//	blendRate = 0.0f;
//}
//
//void Anime::BlendChange(float _blendRate)
//{
//	MV1SetAttachAnimBlendRate(meshHandle, beforeAnimeIndex,1.0f - _blendRate);
//	MV1SetAttachAnimBlendRate(meshHandle, animeAttackIndex, _blendRate);
//}
