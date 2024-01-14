/*!
 * ルーレット
 * @author teco
 */

#include "fwd.h"
#include "side_action_roulette.h"
#include "side_action/actor/side_action_anim_data.h"
#include "common/map/action_map.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_switch.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -23.0f , -29.0f , 46.0f , 58.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(320,368,23,29),
};

static const TbRectF32Pod COLLISION_RECT = 
{
    SPRITE_RECT.x+1.0f , SPRITE_RECT.y+1 , SPRITE_RECT.width-2.0f , SPRITE_RECT.height-2.0f 
};

static TbFloat32 DEFAULT_SWITCH_SEC = 0.5f;

//////////////////////////////////
//  SideActionRoulettesManager
//////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionRouletteManager::SideActionRouletteManager()
    : m_Count(0)
{
    TbFill(m_Result,RESULT_NONE);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionRouletteManager::~SideActionRouletteManager()
{
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionRouletteManager::OnUpdateAction( TbFloat32 deltatime )
{
    if( m_Count == m_Max )
    {
        TbBool isSuccess = TB_TRUE;
        for( TbUint32 i = 0; i < m_Max; ++i ) {
            if( m_Result[i] != RESULT_SUCCESS ){
                isSuccess = TB_FALSE;
            }
        }
        if( isSuccess ){
            GetActionStage()->NotifyEvent("Key",SIDE_ACTION_EVENT_SWITCH_ON,nullptr);
            GetActionStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_WAKEUP,nullptr);
        }else{
            // やり直し
            GetActionStage()->NotifyEvent(nullptr,SIDE_ACTION_EVENT_RETURN,nullptr);
        }
        m_Count = 0;
        TbFill(m_Result,RESULT_NONE);
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionRouletteManager::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case  SIDE_ACTION_EVENT_RESULT:
        {
            if( param )
            {
                TbBool* resultParam = reinterpret_cast<TbBool*>(param);
                if( m_Count < m_Max ) 
                {
                    m_Result[m_Count] = *resultParam ? RESULT_SUCCESS : RESULT_FAILED;
                    ++m_Count;
                }
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

/*!
 * シーンに入った
 * @author teco
 */
void SideActionRouletteManager::OnEnter( void* userData )
{
    Super::OnEnter(userData);

    // 起床力
    SideActionStage* stage = static_cast<SideActionStage*>(GetActionStage());
    TbUint32 playerCount = stage->GetPlayerCount();
    TbUint32 wakeUpCount = 1;
    wakeUpCount = m_Max/playerCount;
    if( 0 < (m_Max%playerCount) ){
        ++wakeUpCount;
    }
    for( TbUint32 i = 0; i < playerCount; ++i ) {
        SideActionPlayer* player = stage->GetPlayer(i);
        player->SetWakeUpCount(wakeUpCount);
    }
    
}

//////////////////////////////////
//  SideActionRoulette
//////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionRoulette::SideActionRoulette()
    : Super()
    , m_CurrentIndex(0)
    , m_SwitchSec(DEFAULT_SWITCH_SEC)
    , m_SwitchTimer(0.0f)
    , m_Switch(nullptr)
{
    m_Candidate.SetEnableSetCapacityAuto(TB_TRUE);

    // スプライト設定
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_1);

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_MAP |
                              ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( COLLISION_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer( SIDE_ACTION_COLLISION_LAYER_ENEMY );

    // テキスト表示位置
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_6);
    AddComponent( &m_SpriteHooker );

    // スイッチ
    m_Switch = new SideActionSwitch(SideActionSwitch::TYPE_REVERSE);
    m_Switch->SetEnableRequestSleep();
    m_Switch->SetInitPos(TbVector2(0.0f,28.0f));
    m_Child.SetActor(m_Switch);
    AddComponent(&m_Child);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionRoulette::~SideActionRoulette()
{
    TB_SAFE_RELEASE(m_Switch);
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionRoulette::OnUpdateAction( TbFloat32 deltatime )
{
    if( !m_BitArray.Test(FLAG_SELECT) ) 
    {
        if( m_Switch->IsOn() )
        {
            m_BitArray.SetBit(FLAG_SELECT);
            TbBool isSuccess = IsSuccess();
            GetStage()->NotifyEvent("RouletteManager",SIDE_ACTION_EVENT_RESULT,&isSuccess);
        }
        else
        {
            m_SwitchTimer += deltatime;
            if( m_SwitchSec <= m_SwitchTimer ) {
                m_CurrentIndex = (m_CurrentIndex+1)%m_Candidate.GetCount();
                m_SwitchTimer = 0.0f;
            }
        }
    }
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionRoulette::NotifyEvent( TbUint32 event , void* param )
{
    switch( event )
    {
    case SIDE_ACTION_EVENT_RETURN:
        {
            m_BitArray.ResetBit(FLAG_SELECT);
            m_Switch->NotifyEvent(event,param);
        }
        break;
    default:
        break;
    }
    return 0;
}

/*!
 * ルーレット候補描画
 * @author teco
 */
void SideActionRoulette::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();

    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    const char* word = "A";
    if( m_CurrentIndex < m_Candidate.GetCount() ) {
        word = m_Candidate[m_CurrentIndex].GetBuffer();
    }
    fontBuffer->Printf( static_cast<TbSint32>(drawPos.GetX()),
                        static_cast<TbSint32>(drawPos.GetY()),
                        word);

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
