#pragma once
#include "../../Library/gameObject.h"
#include "../Struct.h"
#include "../Transform/Transform.h"

class Object3D : public GameObject
{
public:

	Object3D();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_trans"> トランスフォーム </param>
	Object3D(const Transform& _trans);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_trans"> トランスフォーム </param>
	/// <param name="_hModel"> モデルハンドル </param>
	Object3D(const Transform& _trans,const int& _hModel);
	virtual ~Object3D();

	virtual void Update() override;
	virtual void Draw() override;

	/// <summary>
	/// トランスフォームを返す
	/// </summary>
	/// <returns> トランスフォーム </returns>
	const Transform& GetTransform() const { return transform; }

	/// <summary>
	/// トランスフォームポインタを返す
	/// </summary>
	/// <returns> トランスフォームポインタ </returns>
	const Transform* GetTransfromPtr() const { return &transform; }

	/// <summary>
	/// 座標ポインタを返す
	/// </summary>
	/// <returns> 座標ポインタ </returns>
	const VECTOR3* GetPositionPtr() const { return &transform.position; }
	
	/// <summary>
	/// モデルハンドルを返
	/// す
	/// </summary>
	/// <returns> モデルハンドル </returns>
	int GetHmodel() const { return hModel; }

	/// <summary>
	/// ハンドルをdeleteしない　マネージャーがモデルを消したい時があるので
	/// </summary>
	void SetDontDeleteHmodel() { dontDeleteMeHmodel = true; };

protected:

	Transform transform;			// トランスフォーム
	int hModel;						// モデルハンドル	
	bool dontDeleteMeHmodel = false;// マネージャーがモデルを消したい時があるので、trueの時はMV1DeleteModelしない
	bool onModelDraw		= true;	// モデルを描画するかしないかフラグ
};	