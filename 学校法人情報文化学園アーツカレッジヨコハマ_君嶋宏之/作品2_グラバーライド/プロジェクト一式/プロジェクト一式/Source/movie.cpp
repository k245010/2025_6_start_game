#include "movie.h"
#include "config.h"

namespace
{
    //const float MOVIE_FRAME = 60 * 4.5f;//ムービーの長さ
}

Movie::Movie(SceneBase* _scene) : GameObject(_scene)
{
	hMovie = LoadGraph("data\\movie\\title_1.0000_1.mpg");
	//screenHandle = MakeScreen(SCREEN_WIDTH, SCREEN_HEIGHT, FALSE);
    
    position = V2Get(0);

    size = V2Get(SCREEN_WIDTH, SCREEN_HEIGHT);
  
    isActive = false;
    isDraw = false;
}

Movie::~Movie()
{
    DeleteGraph(screenHandle);
    DeleteGraph(hMovie);
}

void Movie::Draw()
{
    if (isDraw)
    {
       // SetDrawScreen(screenHandle);//MakeScreenで作った画面に切り替える
        DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hMovie, true);//画面いっぱいにムービーを拡縮
       // SetDrawScreen(DX_SCREEN_BACK);//裏画面に戻す
       // DrawExtendGraph(position.x, position.y, size.x, size.y, screenHandle, true);//スクリーンを拡縮してムービーのサイズを変える*/
    }

    /*DrawFormatString(100, 500, 0xff0000, "%d", TellMovieToGraphToFrame(hMovie));
    DrawFormatString(100, 700, 0xff0000, "%d", TellMovieToGraph(hMovie));*/
}

void Movie::Update()
{
    if (!isActive)
    {
       PlayMovieToGraph(hMovie);//ムービーの再生の開始
       isActive = true;
    }

    //if (!isDraw)
    //{
    //    isDraw = true;
    //    SeekMovieToGraph(hMovie, 0);//ムービーの再生位置を0にする
    //}

    //if (CheckHitKey(KEY_INPUT_X))
    //    PauseMovieToGraph(hMovie);//ムービーを止める

    int a = TellMovieToGraphToFrame(hMovie);

    if (isDraw)
    {
        if (GetMovieStateToGraph(hMovie) == -1)
            int a = 0;


        int a = GetMovieStateToGraph(hMovie);
        if (GetMovieStateToGraph(hMovie) != 1)
        {
            isDraw = false;
            isActive = false;
            PauseMovieToGraph(hMovie);//ムービーを止める
            //SeekMovieToGraph(hMovie, 0);//ムービーの再生位置を0にする

           
        }
    }

    //if (TellMovieToGraphToFrame(hMovie) >= 146)
    //{
    //    isDraw = false;
    //    PauseMovieToGraph(hMovie);//ムービーを止める
    //    SeekMovieToGraph(hMovie, 0);//ムービーの再生位置を0にする
    //}

    //WaitTimer(17);
}





