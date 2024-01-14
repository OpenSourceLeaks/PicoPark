/*!
 * ブロック崩し用ボール
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_link.h"
#include "side_action_key.h"
#include <crayon/input/cr_input.h>
#include <crayon/collision/cr_action_collision.h>
#include <crayon/map/cr_action_map.h>
#include "root.h"

#include "side_action/side_action_types.h"

static const TbFloat32 BLOCK_SIZE = 32.0f;

static const TbFloat32 VELO = 2.0f;

enum
{
    UV_DEFAULT ,
    UV_UP ,
    UV_DOWN ,
    UV_LEFT ,
    UV_RIGHT
};

static const TbRectF32Pod SPRITE_UV_RECT[] = 
{
    CMN_TEX_UV_RECT(224+16,576,16,16),
    CMN_TEX_UV_RECT(288,576,16,16),
    CMN_TEX_UV_RECT(288,592,16,16),
    CMN_TEX_UV_RECT(224,576,16,16),
    CMN_TEX_UV_RECT(224+32,576,16,16),
};

///////////////////////////////////
// SideActionLinkBlock
///////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
SideActionLinkBlock::SideActionLinkBlock(TbUint32 uvIndex , TbUint32 w, TbUint32 h)
    : m_State(STATE_BEGIN)
    , m_DelayCounter(0)
    , m_Link(nullptr)
{
    const TbRectF32Pod SPRITE_RECT =
    {
        0.0f , 0.0f , static_cast<TbFloat32>(w) , static_cast<TbFloat32>(h)
    };
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_BACK_1);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT[uvIndex]);

    // コリジョン
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision( SPRITE_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT_2);
    collision->SetEnablePushedFromFixedPosCollision(TB_FALSE);
    collision->SetAttribute(SIDE_ACTION_COLLISION_ATTR_NOT_MOVE);
}

/*!
 * デストラクタ
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
SideActionLinkBlock::~SideActionLinkBlock()
{
}

/*!
 * 目標位置設定
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
void SideActionLinkBlock::SetTargetPos( const TbVector2& pos )
{
    m_TargetPos = pos;
}

/*!
 * ロジック更新
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
void SideActionLinkBlock::OnUpdateAction( TbFloat32 deltatime )
{
    if (m_BitArray.Test(FLAG_GO_PRE_TARGET_POS)) {
        SetPos(m_PreTargetPos);
        m_BitArray.ResetBit(FLAG_GO_PRE_TARGET_POS);
    } else {
        switch( m_State )
        {
        case STATE_BEGIN:
        case STATE_END:
            break;
        case STATE_ACTIVE:
            {
                TbVector2 dest = (m_TargetPos + GetInitPos());
                if (move(dest,1.0f)) {
                    m_State = STATE_END;
                }
#if 0
                TbVector2 pos( GetPos().GetX() , GetPos().GetY() );
                TbVector2 deltaPos = dest - pos;
                if( m_BitArray.Test(FLAG_INTERPOLATE) && (deltaPos.LengthSq() > TB_FLOAT32_EPSILON) ) {
                    TbVector2 velo = deltaPos;
                    velo.Normalize();
                    velo *= VELO;
                    pos += velo;
                    SetPos( pos.GetX() , pos.GetY() );
                    TbVector2 newDeltaPos = dest - pos;
                    if( deltaPos.Dot(newDeltaPos) <= 0.0f ){
                        SetPos(dest.GetX(),dest.GetY());
                        m_State = STATE_END;
                    }
                }else{
                    SetPos(dest.GetX(),dest.GetY());
                    m_State = STATE_END;
                }
#endif
            }
            break;
        case STATE_RETURN:
            {
                TbVector2 dest = GetInitPos();
                if (move(dest,0.5f)) {
                    m_State = STATE_BEGIN;
                }
            }
            break;
        };
    }
    m_PreTargetPos = GetPosXY();
}

/*!
 * 座標押し戻し後処理
 * @author Miyake_Shunsuke
 */ 
void SideActionLinkBlock::OnPushedBack(const toybox::TbVector2& pos, const toybox::TbVector2& pushBackVec)
{
    Super::OnPushedBack(pos, pushBackVec);
    if (m_Link) {
        m_Link->OnPushedBackBlock();
    }
}

/*!
 * 移動
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
TbBool SideActionLinkBlock::move(const TbVector2& dest, TbFloat32 speedScale )
{
    if (!IsFront()) {
        if (m_DelayCounter < 4) {
            // 先頭以外遅らせる
            ++m_DelayCounter;
            return TB_FALSE;
        }
    }
    TbVector2 pos( GetPos().GetX() , GetPos().GetY());
    TbVector2 deltaPos = dest - pos;
    if (m_BitArray.Test(FLAG_INTERPOLATE) && (deltaPos.LengthSq() > TB_FLOAT32_EPSILON)) {
        TbVector2 velo = deltaPos;
        velo.Normalize();
        velo *= VELO;
        velo *= speedScale;
        pos += velo;

        // 横の場合は押し出す
        if( IsFront() && IsEnablePush() && STATE_ACTIVE == m_State )
        {
            CrActionActor::MoveContactOption option;
            option.isEnableIgnoreTargetVelo = TB_TRUE;
            TbFloat32 movX = velo.GetX();
            TbVector2 mov(movX, 0.0f);
            if (movX > 0.0f) {
                MoveContactActor(mov, CR_CONTACT_RIGHT, option);
            }
            else {
                MoveContactActor(mov, CR_CONTACT_LEFT, option);
            }
        }

        SetPos(pos.GetX(), pos.GetY());
        TbVector2 newDeltaPos = dest - pos;
        if (deltaPos.Dot(newDeltaPos) <= 0.0f) {
            SetPos(dest.GetX(), dest.GetY());
            return TB_TRUE;
        }
    }
    else {
        SetPos(dest.GetX(), dest.GetY());
        return TB_TRUE;
    }
    return TB_FALSE;
}

///////////////////////////////////
// SideActionLink
///////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionLink::SideActionLink( Type type , 
                                TbUint32 blockCount ,
                                TbUint32 blockSize ,
                                const TbVector2& dir , 
                                TbBool enablePush)
    : Super()
    , m_Type(type)
    , m_InitOffsetCount(0)
    , m_BlockSize(blockSize)
    , m_IsEnableOpenByKey(TB_FALSE)
{
    if (0 == m_BlockSize) {
        m_BlockSize = BLOCK_SIZE;
    }
    TB_ASSERT(blockCount>0);
    m_Direction = dir;
    TbUint32 w = m_BlockSize;
    TbUint32 h = m_BlockSize;
    if (!TB_FLOAT32_EQUAL_ZERO(m_Direction.GetY())) {
        h += 1.0f; // すきま対策
    }else {
        w += 1.0f;
    }
    for( TbUint32 i = 0; i < blockCount; ++i ) {
        TbUint32 uvIndex = UV_DEFAULT;
        if (i == 0) {
            if (m_Direction.GetY() < 0.0f) {
                uvIndex = UV_UP;
            }
            else if (0.0f < m_Direction.GetY()) {
                uvIndex = UV_DOWN;
            }
            else if (m_Direction.GetX() < 0.0f ) {
                uvIndex = UV_LEFT;
            }
            else if (0.0f < m_Direction.GetX()) {
                uvIndex = UV_RIGHT;
            }
        }
        SideActionLinkBlock* block = TB_NEW SideActionLinkBlock(uvIndex,w,h);
        block->SetLink(this);
        m_BlockList.PushBack(block);
    }
    m_BlockList.GetFront()->SetFront();
    if (enablePush) {
        m_BlockList.GetFront()->SetEnablePush();
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionLink::~SideActionLink()
{
    TbForIterator( ite , m_BlockList ) {
        (*ite)->Release();
    }
}

/*!
 * シーンに入った
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
void SideActionLink::OnEnter( void* userData )
{
    Super::OnEnter(userData);
    CrActionStage* stage = GetActionStage();
    TbActorScene* scene = &stage->GetActorScene();
    TbUint32 blockCount = m_BlockList.GetCount();
    for( TbUint32 i = 0; i < blockCount; ++i ) {
        SideActionLinkBlock* block = m_BlockList[i];
        block->SetStage(stage);
        TbVector2 targetPos = m_Direction * m_BlockSize * static_cast<TbFloat32>(blockCount-1-i);
        switch( m_Type )
        {
        case TYPE_OUT:
            block->SetTargetPos( targetPos );
            block->SetInitPos( GetInitPos() );
            break;
        case TYPE_IN:
            block->SetTargetPos( -targetPos );
            block->SetInitPos( targetPos + GetInitPos() );
            break;
        }
        block->Enter( scene , stage );
    }

    for (TbSint32 i = 0; i < m_InitOffsetCount; ++i) {
        TbSint32 offset = (m_InitOffsetCount - i);
        setInitOffsetPos(i, offset);
    }
}

/*!
 * ロジック更新
 * @author teco
 */ 
void SideActionLink::OnUpdateAction(TbFloat32 deltatime)
{
    if (m_IsEnableOpenByKey)
    {
        CrActor* actor = GetActionStage()->FindActor("Key");
        if (actor)
        {
            SideActionKey* key = TbDynamicCast<SideActionKey*>(actor);
            if (key && key->IsPlayerAttached() ) {
                NotifyEvent(SIDE_ACTION_EVENT_SWITCH_ON,nullptr);
                m_IsEnableOpenByKey = TB_FALSE;
            }
        }
    }
}

/*!
 * ブロックを押し戻された
 * @author Miyake_Shunske
 */ 
void SideActionLink::OnPushedBackBlock()
{
    TbForIterator(ite, m_BlockList) {
        (*ite)->GoPreTargetPos();
    }
}
    
/*!
 * シーンから出た
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
void SideActionLink::OnExit()
{
    TbForIterator( ite , m_BlockList ) {
        (*ite)->Exit();
    }
    Super::OnExit();
}

/*! 
 * イベント通知
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
TbSint32 SideActionLink::NotifyEvent( TbUint32 event , void* param )
{
    if (m_BlockList.IsEmpty()) {
        return 0;
    }
    SideActionLinkBlock::State state = m_BlockList.GetFront()->GetState();
    if( SIDE_ACTION_EVENT_SWITCH_ON == event ) {
        TbBool isResetDelay = state == SideActionLinkBlock::STATE_BEGIN;
        TbForIterator( ite , m_BlockList ) {
            (*ite)->Activate(TB_TRUE, isResetDelay);
            if (0 < m_InitOffsetCount) {
                (*ite)->SetSkipDelay();
            }
        }
    }else if( SIDE_ACTION_EVENT_SWITCH_OFF == event ) {
        TbBool isResetDelay = state == SideActionLinkBlock::STATE_END;
        TbForIterator(ite, m_BlockList) {
            (*ite)->Deactivate(TB_TRUE, isResetDelay);
        }
    }
    return 0;
}

/*!
 * オフセット初期位置設定
 * @author teco
 * @since 2015.01.28
 */ 
void SideActionLink::setInitOffsetPos(TbSint32 index, TbUint32 offsetBlock)
{
    TbUint32 blockCount = m_BlockList.GetCount();
    if (static_cast<TbUint32>(index) < blockCount) {
        TbVector2 pos = m_Direction * m_BlockSize * offsetBlock;
        m_BlockList[index]->SetPos(pos+ GetInitPos());

    }
}

/*!
 * ブロックを別途アクティブ
 * @author Miyake_Shunske
 * @since 2015.01.28
 */ 
void SideActionLink::Activate( TbSint32 index , TbBool isInterpolated )
{
    if( static_cast<TbUint32>(index) < m_BlockList.GetCount() ){
        SideActionLinkBlock::State state = m_BlockList.GetFront()->GetState();
        TbBool isResetDelay = state == SideActionLinkBlock::STATE_BEGIN;
        TbSint32 i = m_BlockList.GetCount()-1-index;
        m_BlockList[i]->Activate(isInterpolated, isResetDelay);
    }
}

