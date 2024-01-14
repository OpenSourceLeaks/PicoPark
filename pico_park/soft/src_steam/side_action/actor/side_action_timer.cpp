/*!
 * スイッチ
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_timer.h"

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

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionTimer::SideActionTimer( Type type )
    : Super()
    , m_Type(type)
    , m_Sec(0.0f)
    , m_InitSec(0.0f)
    , m_TimeUpAction(TIMEUP_ACTION_NONE)
    , m_DispType(DISP_TYPE_DEFAULT)
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
 * @since 2013.09.19
 */
SideActionTimer::~SideActionTimer()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionTimer::OnUpdateAction( TbFloat32 deltatime )
{
    switch( m_Type )
    {
    case TYPE_UP:
        if( !m_BitArray.Test(FLAG_PAUSE) ){
            m_Sec += deltatime;
        }
        break;
    case TYPE_DOWN:
        if( !m_BitArray.Test(FLAG_PAUSE) ){
            m_Sec -= deltatime;
        }
        m_Sec = TbMax(0.0f,m_Sec);
        if( TB_FLOAT32_EQUAL_ZERO(m_Sec) ){
            if( !m_BitArray.Test(FLAG_TIMEUP) ){
                onTimeUp();
            }
        }
        break;
    default:
        break;
    }
}

/*!
 * タイムアップ
 * @author teco
 */
void SideActionTimer::onTimeUp()
{
    switch( m_TimeUpAction )
    {
    case TIMEUP_ACTION_NONE:
        break;
    case TIMEUP_ACTION_RETRY:
        GetStage()->RequestRetry();
        break;
    case TIMEUP_ACTION_SWITCH:
        // スイッチイベントを送信する
        {
            static const TbSint32 NAME_OFFSET = TB_ARRAY_COUNT_OF("SwitchTimer")-1;
            TbStaticString32 key = GetName();
            key = key.Substr(NAME_OFFSET);
            TbSint32 id = 0;
            GetActionStage()->NotifyEvent(key.GetBuffer(),SIDE_ACTION_EVENT_SWITCH_ON,&id);
        }
        break;
    }
    m_BitArray.SetBit(FLAG_TIMEUP);
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionTimer::NotifyEvent( TbUint32 event , void* param )
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
            m_BitArray.ResetBit(FLAG_TIMEUP);
        }
        break;
    }
    return 0;
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionTimer::onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    // 数
    switch( m_DispType )
    {
    case DISP_TYPE_SEC:
        {
            TbSint32 minute = static_cast<TbSint32>(TbCeil(m_Sec))/60;
            TbSint32 sec = static_cast<TbSint32>(TbCeil(m_Sec))%60;
            fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()+SPRITE_RECT.x+SPRITE_RECT.width/2),
                               static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y+SPRITE_RECT.height/2),
                               "%02d:%02d",minute,sec);
        }
        break;
    case DISP_TYPE_MSEC:
        {
            TbFloat32 srcSec = TbClamp<TbFloat32>(m_Sec,0.0f,99.99f);
            TbSint32 sec = TbClamp<TbSint32>(static_cast<TbSint32>(srcSec),0,99);
            TbSint32 msec = static_cast<TbSint32>(srcSec*100.0f)%100;
            fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()+SPRITE_RECT.x+SPRITE_RECT.width/2),
                               static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y+SPRITE_RECT.height/2),
                               "%02d:%02d",sec,msec);
        }
        break;
    }

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
