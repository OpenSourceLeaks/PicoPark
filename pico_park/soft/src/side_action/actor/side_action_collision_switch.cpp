/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action/actor/side_action_collision_switch.h"
#include "side_action/actor/side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCollisionSwitch::SideActionCollisionSwitch( TbFloat32 width , TbFloat32 height )
    : Super()
    , m_IsEnd(TB_FALSE)
{
    TbRectF32 rect(0.0f,0.0f,width,height);
    CrActionCollision* collision = CreateCollision( rect , 0 , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
        collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided));
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCollisionSwitch::~SideActionCollisionSwitch()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionCollisionSwitch::OnUpdateAction( TbFloat32 deltatime )
{
}

/*!
 * 描画
 * @author teco
 * @since 2013.09.19
 */
void SideActionCollisionSwitch::OnDraw( TbFloat32 deltatime )
{
#if 0
    TbDraw2dBegin();
    TbDrawRectParam param;
    param.SetColor(TbColor::Red());
    TbRectF32 rect = GetCollision()->GetRect();
    TbVector2 pos = GetDrawPos();
    param.SetRect(pos.GetX()+rect.x,pos.GetY()+rect.y,rect.width,rect.height);
    TbDrawRect(param);
    TbDraw2dEnd();
#endif
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionCollisionSwitch::onCollided(CrActionCollision* target)
{
    if( m_IsEnd ) {
        return;
    }
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
        {
            if( !m_TargetName.IsEmpty() ){
                GetStage()->NotifyEvent(m_TargetName.GetBuffer(),SIDE_ACTION_EVENT_SWITCH_ON,nullptr);
            }
        }
        break;
    }
    m_IsEnd = TB_TRUE;
}

///////////////////////////////
// 触れるとプレイヤーが変化する
///////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionCollisionChangePlayer::SideActionCollisionChangePlayer()
    : Super()
    , m_IsEnd(TB_FALSE)
    , m_IsEnableRepeat(TB_FALSE)
    , m_IsOn(TB_FALSE)
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionCollisionChangePlayer::~SideActionCollisionChangePlayer()
{
}

/*!
* セットアップ
* @author teco
*/
void SideActionCollisionChangePlayer::Setup(const CrActorFactory::UserData& userData)
{
    TbRectF32 rect(0.0f, 0.0f, userData.AsFloat32(0), userData.AsFloat32(1));
    CrActionCollision* collision = CreateCollision(rect, 0, TB_TRUE);
    if (TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
        collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));
    }
    if( 2 <= userData.GetCount() && 0 < userData.AsSint32(2) ){
        m_IsEnableRepeat = TB_TRUE;
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionCollisionChangePlayer::OnUpdateAction(TbFloat32 deltatime)
{
    if (m_IsEnableRepeat) {
        if(m_IsOn ){
            m_IsOn = TB_FALSE;
        }else if (m_IsEnd) {
            m_IsEnd = TB_FALSE;
        }
    }
}

/*!
* 衝突コールバック
* @author teco
*/
void SideActionCollisionChangePlayer::onCollided(CrActionCollision* target)
{
    m_IsOn = TB_TRUE;
    if (m_IsEnd) {
        return;
    }
    if (target->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER) {
        return;
    }
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    if (!stage) {
        return;
    }
    TbUint32 systemPlayerCount = Root::GetInstance().GetPlayerCount();
    TbUint32 playerCount = stage->GetPlayerCount();
    if (CMN_PLAYER_MIN < systemPlayerCount) {
        TbPlaySound("generate");
    }
    for( TbUint32 i = 0; i < playerCount; ++i  ) {
        SideActionPlayer* player = stage->GetPlayer(i);
        TbUint32 index = stage->GetPlayerShuffleIndex( player->GetPlayerIndex() );
        index = (index+playerCount) % systemPlayerCount;
        index = stage->GetPlayerIndexFromShufferIndex(index);
        player->SetPlayerIndex(index);
    }
    m_IsEnd = TB_TRUE;
}

///////////////////////////////
// 触れるとアクター生成
///////////////////////////////

/*!
* コンストラクタ
* @author teco
*/
SideActionCollisionActorCreator::SideActionCollisionActorCreator()
    : Super()
    , m_IsEnd(TB_FALSE)
    , m_IsSilent(TB_FALSE)
{
}

/*!
* デストラクタ
* @author teco
*/
SideActionCollisionActorCreator::~SideActionCollisionActorCreator()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionCollisionActorCreator::Setup(const CrActorFactory::UserData& userData)
{
    static const TbUint32 DEFAULT_COUNT = 5;
    TbRectF32 rect(0.0f, 0.0f, userData.AsFloat32(0), userData.AsFloat32(1));
    CrActionCollision* collision = CreateCollision(rect, 0, TB_TRUE);
    if (TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
        collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));
    }
    TB_RESQUE(DEFAULT_COUNT <=userData.GetCount());
    m_CreateParam.typeName = userData.AsString(2);
    m_CreateParam.pos.SetX(userData.AsFloat32(3));
    m_CreateParam.pos.SetY(userData.AsFloat32(4));
    TbUint32 userDataCount = userData.GetCount()-DEFAULT_COUNT;
    m_CreateParam.userData.SetCount(userDataCount);
    using DataType = CrActorFactory::UserDataUnit::Type;
    for (TbUint32 i = 0; i < userDataCount; ++i) {
        switch (userData.GetType(i+DEFAULT_COUNT))
        {
        case DataType::TYPE_STRING:
            m_CreateParam.userData.SetString(i,userData.AsString(i+DEFAULT_COUNT));
            break;
        case DataType::TYPE_F32:
        default:
            m_CreateParam.userData.SetF32(i, userData.AsFloat32(i + DEFAULT_COUNT));
            break;
        }
    }
}

/*!
* 衝突コールバック
* @author teco
*/
void SideActionCollisionActorCreator::onCollided(CrActionCollision* target)
{
    if (m_IsEnd) {
        return;
    }
    if (target->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER) {
        return;
    }
    if (!m_IsSilent) {
        TbPlaySound("generate");
    }
    GetStage()->CreateActor(m_CreateParam);

    m_IsEnd = TB_TRUE;
}

///////////////////////////////
// 触れている間は移動可能
///////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 */
SideActionCollisionConstraintMove::SideActionCollisionConstraintMove()
    : Super()
{
}

/*!
* デストラクタ
* @author teco
*/
SideActionCollisionConstraintMove::~SideActionCollisionConstraintMove()
{
}

/*!
* セットアップ
* @author teco
*/
void SideActionCollisionConstraintMove::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        ARG_WIDTH , 
        ARG_HEIGHT,
        ARG_TARGET,
    };
    static const TbUint32 DEFAULT_COUNT = 5;
    TbRectF32 rect( 0.0f, 
                    0.0f, 
                    userData.AsFloat32(ARG_WIDTH), 
                    userData.AsFloat32(ARG_HEIGHT));

    CrActionCollision* collision = CreateCollision(rect, 0, TB_TRUE);
    if (TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
        collision->SetCollidedCallback(TbCreateMemFunc(this, &Self::onCollided));
    }
    if ( ARG_TARGET < userData.GetCount()) {
        m_Target = userData.AsString(ARG_TARGET);
    }
}

/*!
* 衝突コールバック
* @author teco
*/
void SideActionCollisionConstraintMove::onCollided(CrActionCollision* target)
{
    if (target->GetLayer() != SIDE_ACTION_COLLISION_LAYER_PLAYER) {
        return;
    }
    SideActionPlayer* player = TbDynamicCast<SideActionPlayer*>(target->GetActor());
    if (!m_Target.IsEmpty() && player)
    {
        TbUint32 playerIndex = player->GetPlayerIndex();
        GetStage()->NotifyEvent(m_Target.GetBuffer(),SIDE_ACTION_EVENT_ENABLE_MOVE,&playerIndex);
    }
}

