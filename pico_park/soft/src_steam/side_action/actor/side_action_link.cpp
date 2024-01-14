/*!
 * ブロック崩し用ボール
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_link.h"
#include "system/sys_input.h"
#include "common/collision/action_collision.h"
#include "common/map/action_map.h"
#include "root.h"

#include "side_action/side_action_types.h"

static const TbFloat32 BLOCK_SIZE = 32.0f;

static const TbFloat32 VELO = 2.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    0.0f , 0.0f , BLOCK_SIZE , BLOCK_SIZE
};

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
    CMN_TEX_UV_RECT(16,592,16,16),
    CMN_TEX_UV_RECT(64,528,16,16),
    CMN_TEX_UV_RECT(64,560,16,16),
    CMN_TEX_UV_RECT(0,592,16,16),
    CMN_TEX_UV_RECT(32,592,16,16),
};

///////////////////////////////////
// SideActionLinkBlock
///////////////////////////////////

/*!
 * コンストラクタ
 * @author Miyake_Shunske
 * @since 2015.01.23
 */ 
SideActionLinkBlock::SideActionLinkBlock(TbUint32 uvIndex )
    : m_State(STATE_BEGIN)
{
    TbSprite& sprite = GetMainSprite();
    sprite.SetZ(CMN_SPRITE_Z_BACK_1);
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT[uvIndex]);

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision( SPRITE_RECT , pushBackTarget , TB_TRUE );
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_TO_PLAYER);
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
    switch( m_State )
    {
    case STATE_BEGIN:
    case STATE_END:
        break;
    case STATE_ACTIVE:
        {
            TbVector2 pos( GetPos().GetX() , GetPos().GetY() );
            TbVector2 dest = (m_TargetPos + GetInitPos());
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
        }
        break;
    };
}

///////////////////////////////////
// SideActionLink
///////////////////////////////////

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionLink::SideActionLink( Type type , TbUint32 blockCount ,TbUint32 blockSize ,const TbVector2& dir )
    : Super()
    , m_Type(type)
    , m_InitOffsetCount(0)
    , m_BlockSize(blockSize)
{
    if (0 == m_BlockSize) {
        m_BlockSize = BLOCK_SIZE;
    }
    TB_ASSERT(blockCount>0);
    m_Direction = dir;
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
        SideActionLinkBlock* block = TB_NEW SideActionLinkBlock(uvIndex);
        m_BlockList.PushBack(block);
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
    ActionStage* stage = GetActionStage();
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
    if( SIDE_ACTION_EVENT_SWITCH_ON == event ) {
        TbForIterator( ite , m_BlockList ) {
            (*ite)->Activate(TB_TRUE);
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
        TbSint32 i = m_BlockList.GetCount()-1-index;
        m_BlockList[i]->Activate(isInterpolated);
    }
}

