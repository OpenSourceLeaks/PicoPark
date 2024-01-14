/*!
 * スイッチ
 * @author teco
 */

#include "fwd.h"
#include "side_action_stop_watch.h"

#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action/side_action_types.h"

static const TbFloat32 WIDTH = 44.0f+1.0f;
static const TbFloat32 HEIGHT = 20.0f+1.0f;

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

/*!
 * コンストラクタ
 * @author teco
 */
SideActionStopWatch::SideActionStopWatch()
    : Super()
    , m_Sec(0.0f)
    , m_InitSec(0.0f)
{
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
    if( !m_BitArray.Test(FLAG_PAUSE) ){
        m_Sec -= deltatime;
    }
    m_Sec = TbMax(0.0f,m_Sec);

    if( TB_FLOAT32_EQUAL_ZERO(m_Sec) )
    {
        if( !m_BitArray.Test(FLAG_DEAD) ){
            GetMainSprite().SetUV(OUT_SPRITE_UV_RECT);
            m_BitArray.SetBit(FLAG_DEAD);
        }
    }
    if( !m_ParentName.IsEmpty() ){
        if( !m_BitArray.Test(FLAG_DEAD) && !m_BitArray.Test(FLAG_PAUSE) ){
            GetActionStage()->NotifyEvent(m_ParentName.GetBuffer(),SIDE_ACTION_EVENT_ALIVE,nullptr);
        }
        GetActionStage()->NotifyEvent(m_ParentName.GetBuffer(),SIDE_ACTION_EVENT_F32,&m_Sec);
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
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    if( m_BitArray.Test(FLAG_DEAD)) {
        fontBuffer->SetColor(TbColor::FromRgbaU8(255,140,140,0xFF));
    }

    TbSint32 sec = TbClamp<TbSint32>(static_cast<TbSint32>(m_Sec),0,99);
    TbSint32 msec = static_cast<TbSint32>(m_Sec*100.0f)%100;
    fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()+SPRITE_RECT.x+SPRITE_RECT.width/2),
                        static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y+SPRITE_RECT.height/2),
                        "%02d:%02d",sec,msec);

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
