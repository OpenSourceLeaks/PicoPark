/*!
 * スイッチ
 * @author teco
 */

#include "fwd.h"
#include "side_action_stop_watch.h"

#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"
#include "common/cmn_util.h"

#include "side_action/side_action_types.h"

static const TbFloat32 WIDTH  = 40.0f + 1.0f;
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
    CMN_TEX_UV_RECT(464,464,WIDTH,HEIGHT)
};

// 苦肉の対応 (ステージ頭で生成されるので)
static TbUint32 s_CountSEValue = 0;
static void PlayCountSE(TbUint32 value)
{
    if (s_CountSEValue != value)
    {
        TbPlaySound("select");
        s_CountSEValue = value;
    }
}

/*!
 * コンストラクタ
 * @author teco
 */
SideActionStopWatch::SideActionStopWatch()
    : Super()
    , m_Sec(0.0f)
    , m_InitSec(0.0f)
    , m_PlayerIndex(0)
{
    s_CountSEValue = 0;
    m_FontColor = CmnUtil::GetMainColor();

    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawTime));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionStopWatch::~SideActionStopWatch()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionStopWatch::OnUpdateAction( TbFloat32 deltatime )
{
    TbFloat32 sec = m_Sec;
    if( !m_BitArray.Test(FLAG_PAUSE) ){
        m_Sec -= deltatime;
    }
    m_Sec = TbMax(0.0f,m_Sec);

    if( TB_FLOAT32_EQUAL_ZERO(m_Sec) )
    {
        if( !m_BitArray.Test(FLAG_DEAD) ){
            m_BitArray.SetBit(FLAG_DEAD);
        }
    }
    else
    {
        if (isVisibleTime())
        {
            TbUint32 value = static_cast<TbUint32>(m_Sec - 0.01f);
            if (static_cast<TbUint32>(sec - 0.01f) != value) {
                PlayCountSE(value);
            }
        }
    }

    if( !m_ParentName.IsEmpty() ){
        if( !m_BitArray.Test(FLAG_DEAD) && !m_BitArray.Test(FLAG_PAUSE) ){
            GetActionStage()->NotifyEvent(m_ParentName.GetBuffer(),SIDE_ACTION_EVENT_ALIVE,nullptr);
        }
        SideActionEventPlayerParam param;
        param.playerIndex = m_PlayerIndex;
        param.paramF32 = m_Sec;
        GetActionStage()->NotifyEvent(m_ParentName.GetBuffer(),SIDE_ACTION_EVENT_PLAYER_PARAM,&param);
    }
}


/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionStopWatch::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_SWITCH_ON:
        // ポーズ
        m_BitArray.SetBit(FLAG_PAUSE);
        break;
    case SIDE_ACTION_EVENT_SWITCH_OFF:
        // ポーズ解除
        m_BitArray.ResetBit(FLAG_PAUSE);
        break;
    case SIDE_ACTION_EVENT_RETURN:
        {
            // 状態復帰
            m_Sec = m_InitSec;
            m_BitArray.ResetBit(FLAG_PAUSE);
            m_BitArray.ResetBit(FLAG_DEAD);
            GetMainSprite().SetUV(SPRITE_UV_RECT);
            s_CountSEValue = 0;
        }
        break;
    case SIDE_ACTION_EVENT_SET_COLOR:
        {
            TbColor* color = reinterpret_cast<TbColor*>(param);
            m_FontColor = *color;
            GetMainSprite().SetUV(OUT_SPRITE_UV_RECT);
            GetMainSprite().SetAlpha(color->GetAlphaF32());
        }
        break;
    }
    return 0;
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionStopWatch::onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetFontSize(32.0f);
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    if( m_BitArray.Test(FLAG_DEAD)) {
        fontBuffer->SetEnableLineThrough(TB_TRUE);
//        fontBuffer->SetColor(TbColor::FromRgbaU8(255,140,140,0xFF));
    }
    fontBuffer->SetColor(m_FontColor);

    TbBool isVisible = isVisibleTime();
    if (isVisible)
    {
        TbSint32 sec = TbClamp<TbSint32>(static_cast<TbSint32>(m_Sec),0,99);
        TbSint32 msec = static_cast<TbSint32>(m_Sec*100.0f)%100;
        fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()+SPRITE_RECT.x+SPRITE_RECT.width/2),
                            static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y+SPRITE_RECT.height/2),
                            "%02d:%02d",sec,msec);
    }

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}

/*!
 * 時間表示するか
 * @author teco
 */
TbBool SideActionStopWatch::isVisibleTime() const
{
    TbBool isVisible = TB_TRUE;
    if (m_BitArray.Test(FLAG_INVISIBLE))
    {
        if (!m_BitArray.Test(FLAG_PAUSE) && !m_BitArray.Test(FLAG_DEAD))
        {
            if (m_Sec < 3.0f)
            {
                isVisible = TB_FALSE;
            }
        }
    }
    return isVisible;
}
