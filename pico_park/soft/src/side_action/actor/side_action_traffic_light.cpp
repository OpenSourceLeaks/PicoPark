/*!
 * 信号
 * @author teco
 */

#include "fwd.h"
#include "side_action_traffic_light.h"

#include <graphics/render/tb_draw_2d.h>
#include <graphics/sprite/tb_sprite_material.h>

#include "side_action/actor/side_action_player.h"
#include <crayon/stage/cr_action_stage.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbFloat32 WIDTH = 32.0f+1.0f;
static const TbFloat32 HEIGHT = 20.0f+1.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -WIDTH*2.0f , 0 , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(544,432,WIDTH*2.0f,HEIGHT*2.0f)
};

static const TbFloat32 SEC_DEFAULT[] =
{
    10.0f,
    7.0f
};

static TB_NDEBUG_CONST TbFloat32 BLINK_SEC_BEGIN = 3.2f;
static TB_NDEBUG_CONST TbFloat32 BLINK_SEC_INTERVAL = 0.2f;

/*!
 * コンストラクタ
 * @author teco
 */
SideActionTrafficLight::SideActionTrafficLight()
    : Super()
    , m_State(STATE_MOVE)
    , m_BlinkTimer(0.0f)
{
    SetEnableScroll(TB_FALSE);

    TbCopyArray(SEC_DEFAULT,m_ActiveSec);
    setState(STATE_MOVE);

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_6);

    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::OnDrawRect));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionTrafficLight::~SideActionTrafficLight()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionTrafficLight::Setup(const CrActorFactory::UserData& userData)
{
    enum {
        PARAM_MOVE_SEC ,
        PARAM_STOP_SEC ,
        PARAM_MOVE_OFFSET_SEC , 
        PARAM_MAX
    };
    if (PARAM_MAX <= userData.GetCount() ) {
        TbFloat32 moveSec = userData.AsFloat32(PARAM_MOVE_SEC);
        TbFloat32 stopSec = userData.AsFloat32(PARAM_STOP_SEC);
        TbFloat32 moveOffsetSec = userData.AsFloat32(PARAM_MOVE_OFFSET_SEC);
        moveSec += moveOffsetSec * TbMax<TbFloat32>(0.0f,Root::GetInstance().GetPlayerCount()-CMN_PLAYER_MIN);
        SetSwitchSec(moveSec,stopSec);
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionTrafficLight::OnUpdateAction( TbFloat32 deltatime )
{
    m_Timer -= deltatime;
    if( m_Timer <= 0.0f )
    {
        switch( m_State )
        {
        case STATE_MOVE:
            {
                setState(STATE_STOP);
            }
            break;
        case STATE_STOP:
            {
                setState(STATE_MOVE);
            }
            break;
        }
    }
    else if( m_Timer < BLINK_SEC_BEGIN )
    {
        m_BlinkTimer += deltatime;
        if( BLINK_SEC_INTERVAL <= m_BlinkTimer ){
            m_BitArray.SetBit(FLAG_BLINK_OFF,!m_BitArray.Test(FLAG_BLINK_OFF));
            m_BlinkTimer = 0.0f;
        }
    }
    if( STATE_STOP == m_State )
    {
        // 動いているキャラがいたら死亡
        SideActionStage* stage = static_cast<SideActionStage*>(GetActionStage());
        TbUint32 count = stage->GetPlayerCount();
        for( TbUint32 i = 0; i < count; ++i )
        {
            SideActionPlayer* player = stage->GetPlayer(i);
            TbFloat32 veloLengthSq = player->GetVelo().LengthSq();
            if( 0.05f <= veloLengthSq ){
                if (SideActionPlayer::STATE_DEAD != player->GetState())
                {
                    TbPlaySound("hit");
                    player->NotifyEvent(SIDE_ACTION_EVENT_DEAD, nullptr);
                }
            }
        }
    }
}

/*!
 * 前景描画
 * @author teco
 */
void SideActionTrafficLight::OnDrawRect( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    struct ColorPair
    {
        TbColor activeColor;
        TbColor inactiveColor;
    };
    static const ColorPair COLOR_TABLE[] =
    {
        { TbColor::FromRgbaU8(69,154,224,255) , TbColor::FromRgbaU8(0,0,60,255) } , 
        { TbColor::FromRgbaU8(255,140,140,255) , TbColor::FromRgbaU8(60,0,0,255) } , 
    };
    static const TbRectF32 RECT_TABLE[] =
    {
        TbRectF32(-58.0f,4.0f,58.0f,58.0f) , 
        TbRectF32(0.0f,4.0f,58.0f,58.0f) , 
    };

    TbVector2 drawPos = GetDrawPos();
    for( TbUint32 i = 0; i < TB_ARRAY_COUNT_OF(RECT_TABLE); ++i )
    {
        TbRectF32 rect = RECT_TABLE[i];
        rect.x += drawPos.GetX();
        rect.y += drawPos.GetY();
        TbColor color;
        if( i == m_State && !m_BitArray.Test(FLAG_BLINK_OFF) ) {
            color = COLOR_TABLE[i].activeColor;
        }else{
            color = COLOR_TABLE[i].inactiveColor;
        }
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512 - 8, 0, 8, 8));
        material->Draw(rect,uvRect,color, poseMatrix);
    }
}

