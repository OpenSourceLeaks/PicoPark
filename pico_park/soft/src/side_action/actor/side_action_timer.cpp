/*!
 * スイッチ
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_timer.h"

#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"
#include "side_action/stage/side_action_stage.h"

static const TbFloat32 WIDTH = 40.0f+1.0f;
static const TbFloat32 HEIGHT = 15.0f+1.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -WIDTH*2.0f , -HEIGHT*2.0f , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(320,432,WIDTH,HEIGHT)
};

static const TbFloat32 DEFAULT_FONT_SIZE = 32.0f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionTimer::SideActionTimer( Type type )
    : Super()
    , m_Type(type)
    , m_FontSize(DEFAULT_FONT_SIZE)
    , m_Sec(0.0f)
    , m_InitSec(0.0f)
    , m_TimUpInSec(0.0f)
    , m_TimeUpAction(TIMEUP_ACTION_NONE)
    , m_DispType(DISP_TYPE_DEFAULT)
{
    SetEnableScroll(TB_FALSE);

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
    TbFloat32 sec = m_Sec;
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
        if (m_TimeUpAction == TIMEUP_ACTION_DEAD || m_TimeUpAction == TIMEUP_ACTION_SWITCH ) {
            if (m_Sec < 10.0f)
            {
                if (static_cast<TbUint32>(sec - 0.01f) != static_cast<TbUint32>(m_Sec - 0.01f)) {
                    TbPlaySound("select");
                }
            }
        }
        if( TB_FLOAT32_EQUAL_ZERO(m_Sec) ){
            if( !m_BitArray.Test(FLAG_TIMEUP) ){
                onTimeUp();
            }
        }
        break;
    default:
        break;
    }
    if (m_BitArray.Test(FLAG_DISP_TIMEUP))
    {
        m_TimUpInSec += deltatime;
    }
    if (DEFAULT_FONT_SIZE < m_FontSize) {
        static TbFloat32 SCALE_DOWN = 0.98f;
        m_FontSize = TbMax(DEFAULT_FONT_SIZE, m_FontSize*SCALE_DOWN);
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
    case TIMEUP_ACTION_DEAD:
        GetActionStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_DEAD,nullptr);
        m_BitArray.SetBit(FLAG_DISP_TIMEUP);
        TbPlaySound("blip");
        break;
    case TIMEUP_ACTION_SWITCH:
        // スイッチイベントを送信する
        {
            static const TbSint32 NAME_OFFSET = TB_ARRAY_COUNT_OF("SwitchTimer")-1;
            TbStaticString32 key = GetName();
            key = key.Substr(NAME_OFFSET);
            TbSint32 id = 0;
            GetActionStage()->NotifyEvent(key.GetBuffer(),SIDE_ACTION_EVENT_SWITCH_ON,&id);
//            TbPlaySound("blip");
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
        if( !m_BitArray.Test(FLAG_TIMEUP) )
        {
            if (param)
            {
                static TbFloat32 SCALE_UP = 1.15f;
                TbUint32* time = static_cast<TbUint32*>(param);
                m_Sec += static_cast<TbFloat32>(*time);
                m_Sec = TbCeil(m_Sec);
                m_FontSize = DEFAULT_FONT_SIZE*SCALE_UP;
            }
            else
            {
                // ポーズ
                m_BitArray.SetBit(FLAG_PAUSE);
            }
        }
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
    fontBuffer->SetFontSize(m_FontSize);
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

    // タイムアップ表示
    if (m_BitArray.Test(FLAG_DISP_TIMEUP))
    {
        fontBuffer->SetEnableBorder(TB_TRUE);
        SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
        static const TbFloat32 IN_SEC = 1.0f;
        TbFloat32 posX = TbMin(1.0f, m_TimUpInSec / 1.0f);
        posX *= (stage->GetWindowWidth() / 2);
        TbFloat32 posY = (stage->GetWindowHeight() / 2);

        if (stage) {
            fontBuffer->SetColor(stage->GetClearFontColor());
        }
        fontBuffer->Printf(static_cast<TbSint32>(posX),
                           static_cast<TbSint32>(posY), "TIME UP");
    }

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
