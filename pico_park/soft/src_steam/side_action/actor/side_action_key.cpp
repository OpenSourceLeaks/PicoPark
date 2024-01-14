/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "root.h"
#include "side_action_key.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -16.0f , -28.0f, 32.0f , 56.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(112,640,16,28),
};

static TB_NDEBUG_CONST TbFloat32 APPROACH_RATE = 0.1f;

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionKey::SideActionKey( Type type )
    : Super()
    , m_Type(type)
    , m_AttachTarget(nullptr)
    , m_State(STATE_READY)
{
    AddComponent(&m_SpriteHooker);

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_4);

    TbUint32 pushBackTarget = 0;
    ActionCollision* collision = CreateCollision( SPRITE_RECT , pushBackTarget , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_KEY);
        collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided));
    }

    SetActive(m_Type != TYPE_BREAKOUT);
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionKey::~SideActionKey()
{
    TB_SAFE_RELEASE(m_AttachTarget);
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionKey::OnUpdateAction( TbFloat32 deltatime )
{
    switch( m_State )
    {
    case STATE_READY:
        if( TYPE_BREAKOUT == m_Type )
        {
            if( !IsActive() && !existsBreakBlock() ){
                SetActive(TB_TRUE);
#if CMN_DEPRECATED
                SideActionStage* stage = static_cast<SideActionStage*>(GetActionStage());
                stage->SetDisableGameOver();
#endif
            }
        }
        break;
    case STATE_REQ_END:
        {
            SetActive(TB_FALSE);
            m_State = STATE_END;
            // もしかしたら全体通知した方がいいかもしれないが、現状不要なのでアタッチ対象のみに通知
            if( m_AttachTarget ) {
                m_AttachTarget->NotifyEvent(SIDE_ACTION_EVENT_KEY_END,nullptr);
            }
            TB_SAFE_RELEASE(m_AttachTarget);
        }
        break;
    default:
        break;
    }

    if( m_AttachTarget ){
        // アタッチ済みの対象がいたら近づく
        TbVector2 target( m_AttachTarget->GetPosX() + m_AttachRect.x , m_AttachTarget->GetPosY() + m_AttachRect.y );
        
        if( !m_AttachTarget->IsLookingRight() ){
            target.Add( m_AttachRect.width , 0.0f );
        }

        TbVector2 delta( target.GetX() - GetPosX() , 
                            target.GetY() - GetPosY() );
        TbFloat32 deltaLen = delta.LengthSq();
        if( deltaLen < 0.1f ) {
            SetPos( target.GetX() , target.GetY() );
        }else{
            AddPos( delta.GetX()*APPROACH_RATE , delta.GetY()*APPROACH_RATE ,0.0f);
        }
    }
}

/*! 
 * イベント通知
 * @author teco
 */
TbSint32 SideActionKey::NotifyEvent( TbUint32 event , void* param )
{
    if( event == SIDE_ACTION_EVENT_ACTIVATE ||
        event == SIDE_ACTION_EVENT_SWITCH_ON ) 
    {
        if( STATE_READY == m_State ){
            SetActive( TB_TRUE );
        }
    }
    return 0;
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionKey::onCollided(ActionCollision* target)
{
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_GOAL:
        {
            // 消える
            m_State = STATE_REQ_END;
        }
        break;
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
    case SIDE_ACTION_COLLISION_LAYER_KEY_TARGET:
        {
            if( !m_AttachTarget && !m_SwitchTarget.IsEmpty() ) {
                GetStage()->NotifyEvent(m_SwitchTarget.GetBuffer(),SIDE_ACTION_EVENT_SWITCH_ON,nullptr);
            }
            // このレイヤーにはプレイヤーしかいないので、その前提でキャストする
            TB_SAFE_RELEASE(m_AttachTarget);
            m_AttachTarget = target->GetActor();
            m_AttachTarget->Retain();

            if( SIDE_ACTION_COLLISION_LAYER_PLAYER == target->GetLayer() ){
                m_AttachRect = target->GetRect();
            }else{
                m_AttachRect = TbRectF32();
            }
        }
        break;
    }
}

/*!
 * 初期位置計算
 * @author teco
 * @since 2014.07.23
 */
TbVector2 SideActionKey::CalcInitPos( const TbVector2& pos , TbFloat32 offset )
{
    static TB_NDEBUG_CONST TbFloat32 PLAYER_COUNT_SCALE = 0.8f;
    TbVector2 result = pos;
    TbFloat32 scale = static_cast<TbFloat32>(Root::GetInstance().GetPlayerCount()) * PLAYER_COUNT_SCALE;
    result.Add( 0.0f , scale * offset );
    return result;
}

/*!
 * 文字描画
 * @author teco
 * @since 2014.12.28
 */
void SideActionKey::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
#if CMN_DEPRECATED
    if( IsHint() && GetMainSprite().IsVisible() && !m_AttachTarget){
        const TbVector2& drawPos = GetDrawPos();
        TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
        fontBuffer->PushSetting();
        fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

        // ヒント
        fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
                           static_cast<TbSint32>(drawPos.GetY()+SPRITE_RECT.y-36.0f),"1.GET");

        fontBuffer->PopSetting();
        fontBuffer->Flush();
    }
#endif
}

/*! 
 * アクティブ設定
 * @author teco
 */
void SideActionKey::SetActive( TbBool isActive )
{
    ActionCollision* collision = GetCollision();
    if( collision ){
        collision->SetActive(isActive);
    }
    GetMainSprite().SetVisible(isActive);
    m_BitArray.SetBit(FLAG_ACTIVE,isActive);
}


/*!
 * 壊せるブロックが残っているか
 * @author teco
 * @since 2014.08.02
 */
TbBool SideActionKey::existsBreakBlock()
{
    const ActionMap& map = GetActionStage()->GetMap();
    return map.Exists(ACTION_MAP_CHIP_BREAK_BEGIN,ACTION_MAP_CHIP_BREAK_END);
}
