#pragma once
#include "../../../Library/GameObject.h"

class EffekseerObject : public GameObject
{
public:

	/// <summary>
	///								エフェクトの初期化
	/// </summary>
	/// <param name="pPos">			初期座標														</param>
	/// <param name="_data">		エフェクトデータ												</param>
	/// <param name="_data">		再生速度														</param>
	/// <param name="parentPos">	追従座標ポインタ												</param>
	/// <param name="changePos">	追従座標ポインタに対する変更座標 (描画座標ポインタに加算する)	</param>
	EffekseerObject(const VECTOR3& pPos, const int& _data, const float& _spped, const VECTOR3* parentPos = nullptr,const VECTOR3& _changePos = VZero);
	~EffekseerObject();

	
	void Init();

	void Update() override;
	void SetSpeed(float _speed) { speed = _speed; }

	/// <summary>
	/// エフェクトの再生を中断する
	/// </summary>
	void StopEffekseer();

private:

	int hEffekseerData;
	int hPlayingEffekseer;
	float appearanceTime;		// 存在時間
	VECTOR3 position;			// 座標
	const VECTOR3* parentPos;	// 追従処理のためのエフェクト描画座標ポインタ
	VECTOR3 changePos;			// 描画座標ポインタに対する変更座標　描画座標ポインタに加算する
	float speed;				// 再生速度

};
