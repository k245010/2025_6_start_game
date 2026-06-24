#pragma once
#include "../Library/gameObject.h"

/// <summary>
/// ムービーを流す用のクラス
/// </summary>
class Movie :public GameObject 
{
public:
	Movie(SceneBase* _scene);
	~Movie();
	void Draw()override;
	void Update()override;

	/// <summary>
	/// 動画を再生する
	/// Updateで呼んでOK！
	/// </summary>
	void PlayMovie() 
	{
		if (!isDraw)
		{
			isDraw = true;
			SeekMovieToGraph(hMovie, 0);//ムービーの再生位置を0にする
		}
	}

	bool GetIsDraw() { return isDraw; }

private:
	
	VECTOR2I position;
	VECTOR2I size;

	int hMovie;//movieを読み込むハンドル
	int screenHandle;//SetDrawScreenで使う用のハンドル
	bool isDraw;
	bool isActive;
};