/*!
 * 協力型ストップウォッチ
 * @author teco
 */

#include "fwd.h"
#include "side_action_coop_stop_watch.h"

#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

static const TbFloat32 WIDTH = 40.0f + 1.0f;
static const TbFloat32 HEIGHT = 15.0f + 1.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -WIDTH*2.0f , -HEIGHT*2.0f , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(320,432,WIDTH,HEIGHT)
};


static const TbRectF32Pod OUT_SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(368,432,WIDTH,HEIGHT)
};

static const TbRectF32Pod CLEAR_SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(416,432,WIDTH,HEIGHT)
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCoopStopWatch::SideActionCoopStopWatch()
    : Super()
    , m_Sec(0.0f)
    , m_InitSec(0.0f)

    , m_JudgeSecMax(1.0f)
    , m_JudgeSecMin(0.0f)
    , m_RetryTimerSec(0.0f)
{
    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_BACK_1);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawTime));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCoopStopWatch::~SideActionCoopStopWatch()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionCoopStopWatch::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_BitArray.Test(FLAG_END) ) {
        return;
    }

    static const TbFloat32 JUDGE_SEC = 2.0f;
    if( !m_BitArray.Test(FLAG_UPDATE) ) {
        if( !isCleared() ){
            GetMainSprite().SetUV(OUT_SPRITE_UV_RECT);
            m_RetryTimerSec += deltatime;
            if( JUDGE_SEC <= m_RetryTimerSec ){
                // やり直し
                GetActionStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_RETURN,nullptr);
                m_RetryTimerSec = 0.0f;
                GetMainSprite().SetUV(SPRITE_UV_RECT);
                m_BitArray.ResetBit(FLAG_FAILED);
            }
        }else{
            // クリア
            GetMainSprite().SetUV(CLEAR_SPRITE_UV_RECT);
            GetActionStage()->NotifyEvent("Key",SIDE_ACTION_EVENT_SWITCH_ON,nullptr);
            m_BitArray.SetBit(FLAG_END);
        }
    }else{
        m_RetryTimerSec = 0.0f;
    }
    m_BitArray.ResetBit(FLAG_UPDATE);
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionCoopStopWatch::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_PLAYER_PARAM:
        {
            SideActionEventPlayerParam* playerParam = reinterpret_cast<SideActionEventPlayerParam*>(param);
            // 時間を集める
            if( TB_FLOAT32_EQUAL_ZERO(playerParam->paramF32) ) {
                m_BitArray.SetBit(FLAG_FAILED);
            }
            m_TotalSec += playerParam->paramF32;
        }
        break;
    case SIDE_ACTION_EVENT_ALIVE:
        {
            m_BitArray.SetBit(FLAG_UPDATE);
        }
        break;
    }
    return 0;
}

/*!
 * ロジック更新 (派生クラスはこちらを継承するように)
 * @author teco
 */
void SideActionCoopStopWatch::OnPostAction( TbFloat32 deltatime )
{
    m_Sec = m_TotalSec;
    m_TotalSec = 0.0f;
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionCoopStopWatch::onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetFontSize(32.0f);
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    if( !m_BitArray.Test(FLAG_UPDATE) ){
        if( isCleared() ) {
            fontBuffer->SetColor(TbColor::FromRgbaU8(31,143,255,0xFF));
        }else{
            fontBuffer->SetColor(TbColor::FromRgbaU8(255, 140, 140, 0xFF));
        }
    }

    // 数
    TbSint32 sec = TbClamp<TbSint32>(static_cast<TbSint32>(m_Sec),0,99);
    TbSint32 msec = static_cast<TbSint32>(m_Sec*100.0f)%100;

    TbFloat32 x = static_cast<TbSint32>(drawPos.GetX()+SPRITE_RECT.x+SPRITE_RECT.width/2);
    TbFloat32 y = static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y+SPRITE_RECT.height/2);
    fontBuffer->Printf(x,y,
                      "%02d:%02d",sec,msec);

    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_RIGHT);
    fontBuffer->Printf(x-100,y,"%02d:%02d <",static_cast<TbSint32>(m_JudgeSecMin),static_cast<TbSint32>(m_JudgeSecMin*100.0f)%100);

    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_LEFT);
    fontBuffer->Printf(x+100,y,"< %02d:%02d",static_cast<TbSint32>(m_JudgeSecMax),static_cast<TbSint32>(m_JudgeSecMax*100.0f)%100);

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
