// main.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "fwd.h"

#include <base/io/tb_file_manager.h>
#include <base/io/resource/tb_resource_manager.h>
#include <framework/tb_framework_cui.h>
#include <sound/tb_sound.h>
#include <base/time/tb_tick.h>

using namespace toybox;

namespace
{
    TbSoundManager* s_SoundManager = NULL;   // サウンド管理
    TbSoundSource*  s_SoundSource = NULL;
    TbSint32        s_Phase = 0;
    TbSint32        s_PausePhase = 0;
    TbFloat32       s_ProcessSec = 0.0f;
}

void update()
{
    static TbFloat64 pre = 0.0f;
    if( TB_FLOAT32_EQUAL(pre,0.0f) ){
        pre = TbTick::GetMilliSeconds();
    }
    if (TB_FLOAT32_EQUAL(s_ProcessSec, 0.0f)) {
        s_ProcessSec = TbTick::GetMilliSeconds();
    }
    TbFloat64 playSec = 0.0;

    s_SoundManager->Update();

    enum {
        PHASE_INIT , 
        PHASE_WAIT , 
        PHASE_PLAY , 
        PHASE_IDLE , 
    };

    switch( s_Phase ) 
    {
    case PHASE_INIT:
        {
            // リソース読みこみ
            s_SoundManager->LoadSound( "sound/bgm.wav" , "bgm" , TB_SOUND_TYPE_STREAM );//
//            s_SoundManager->LoadSound( "sound/bgm.ogg" , "bgm" , TB_SOUND_TYPE_STREAM );//
            TbSoundId soundId = s_SoundManager->GetSoundId( "bgm" );
            ++s_Phase;
        }
        break;
    case PHASE_WAIT:
        {
            if( !s_SoundManager->IsReadySound("bgm") ){
                break;
            }
            // 音源作成/登録
            s_SoundSource = new TbSoundSource( "bgm" );
            s_SoundSource->SetEnableLoop(TB_TRUE);
            ++s_Phase;
        }
        break;
    case PHASE_PLAY:
        {
            if( s_SoundSource && s_SoundSource->IsReady() && !s_SoundSource->IsPlaying() ){
                // 再生準備が整ったら再生する
                s_SoundSource->Play();
                ++s_Phase;
            }
        }
        break;
    case PHASE_IDLE:
        {
            playSec = s_SoundSource->GetPlayTimeSec();
#if 1
            switch( s_PausePhase)
            {
            case 0:
                if (TbTick::GetMilliSeconds() - s_ProcessSec >= 2000.0f)
                {
                    s_SoundSource->Pause();
                    ++s_PausePhase;
                    s_ProcessSec = 0.0f;
                }
                break;
            case 1:
                if (TbTick::GetMilliSeconds() - s_ProcessSec >= 2000.0f)
                {
                    s_SoundSource->Play();
                    ++s_PausePhase;
                    s_ProcessSec = 0.0f;
                }
                break;
            }
#endif
        }
        break;
    }


    if( (TbTick::GetMilliSeconds() - pre) >= 300.0f ) {
        TbPrintf("%f\n",playSec);
        pre = 0.0f;
    }
}

namespace toybox
{

/*!
 * エントリーポイント
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFrameworkCui::OnEntryPoint( const TbChar8** argList , TbUint32 argCount ) 
{
    TbFrameworkCui::Initialize();

    // サウンド初期化
    TbSoundInitParam initParam;
    initParam.layerCount = 1;
    initParam.soundMax = 1;
    initParam.resourceListType = 0;
    s_SoundManager = new TbSoundManager( initParam );

    // メインループ
    TbFrameworkCui::StartMainLoop( NULL , update , NULL );
}

/*!
 * 終了
 * @author Miyake Shunsuke
 * @since 2012.07.18
 */
void TbFrameworkCui::OnExitPoint()
{
    s_SoundManager->RemoveSource(s_SoundSource);
    TB_SAFE_DELETE(s_SoundSource);
    TB_SAFE_DELETE(s_SoundManager);
    TbFrameworkCui::Finalize();
}

}
