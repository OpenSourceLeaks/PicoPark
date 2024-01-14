/*!
 * 鍵
 * @author teco
 */

#include "fwd.h"
#include "side_action_move_energy.h"

#include <graphics/render/tb_draw_2d.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <crayon/stage/cr_action_stage.h>
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "common/cmn_util.h"
#include "root.h"

static const TbFloat32 WIDTH = 64.0f;
static const TbFloat32 HEIGHT = 16.0f;

static const TbRectF32Pod SPRITE_RECT = 
{
    -WIDTH*2.0f , -HEIGHT*2.0f , WIDTH*4.0f , HEIGHT*4.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(464,432,WIDTH,HEIGHT)
};

/*!
 * コンストラクタ
 * @author teco
 */
SideActionMoveEnergy::SideActionMoveEnergy()
    : Super()
    , m_Energy(1.0f)
    , m_CosumeEnergy(-0.01f)
    , m_RecoverEnergy(0.01f)
{
    SetEnableScroll(TB_FALSE);

    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_5);

    m_SpriteHooker.SetDrawFunc(TbCreateMemFunc(this,&Self::OnDrawRect));
    m_SpriteHooker.SetZ(CMN_SPRITE_Z_FRONT_6);
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionMoveEnergy::~SideActionMoveEnergy()
{
}

// セットアップ
void SideActionMoveEnergy::Setup(const CrActorFactory::UserData& userData)
{
    enum
    {
        PARAM_CONSUME_ENERGY , 
        PARAM_RECOVER_ENEGY ,
        PARAM_CONSUME_ENERGY_OFS,
    };
    if ( PARAM_CONSUME_ENERGY < userData.GetCount()) {
        m_CosumeEnergy = userData.AsFloat32(PARAM_CONSUME_ENERGY);
    }
    if (PARAM_RECOVER_ENEGY < userData.GetCount()) {
        m_RecoverEnergy = userData.AsFloat32(PARAM_RECOVER_ENEGY);
    }
    if (PARAM_CONSUME_ENERGY_OFS < userData.GetCount()) {
        TbFloat32 offset = userData.AsFloat32(PARAM_CONSUME_ENERGY_OFS);
        offset *= (Root::GetInstance().GetPlayerCount()-CMN_PLAYER_MIN);
        m_CosumeEnergy += offset;
    }
}

/*!
 * ロジック更新
 * @author teco
 */
void SideActionMoveEnergy::OnUpdateAction( TbFloat32 deltatime )
{
    if( 0.0f < m_Energy )
    {
        SideActionStage* stage = static_cast<SideActionStage*>(GetActionStage());
        TbUint32 count = stage->GetPlayerCount();

        TbFloat32 velo = 0.0f;
        for (TbUint32 i = 0; i < count; ++i)
        {
            SideActionPlayer* player = stage->GetPlayer(i);
            if (player->GetState() == SideActionPlayer::STATE_DEFAULT) {
                velo += player->GetVelo().Length();
            }
        }
        if( 0.0f < velo )
        {
            // 動いているとエネルギーを消費
            m_Energy += m_CosumeEnergy;
        }
        else
        {
            // 止まっているとエネルギーを回復
            m_Energy += m_RecoverEnergy;
        }
        m_Energy = TbClamp(m_Energy,0.0f,1.0f);

        if ( m_Energy <= 0.0f )
        {
            TbPlaySound("blip");
            // 動いているキャラがいたら死亡
            for (TbUint32 i = 0; i < count; ++i)
            {
                SideActionPlayer* player = stage->GetPlayer(i);
                player->NotifyEvent(SIDE_ACTION_EVENT_DEAD, nullptr);
            }
        }
    }
}

/*! 
 * シーンに入った
 * @author teco
 */
void SideActionMoveEnergy::OnEnter( void* userData )
{
    Super::OnEnter(userData);
    CrStage* stage = reinterpret_cast<CrStage*>(userData);
    CrActionStage* actionStage = TbDynamicCast<CrActionStage*>(stage);
    if( TB_VERIFY(actionStage) ) {
        m_SpriteHooker.Enter( &actionStage->GetSpriteScene(0) );
    }
}
    
/*!
 * シーンから出た
 * @author teco
 */
void SideActionMoveEnergy::OnExit()
{
    m_SpriteHooker.Exit();
    Super::OnExit();
}

static const TbRectF32Pod ENEGY_RECT =
{
    -120.0f , -24.0f , 242.0f ,49.0f
};

/*!
 * 前景描画
 * @author teco
 */
void SideActionMoveEnergy::OnDrawRect( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    CrActionStage* stage = TbDynamicCast<CrActionStage*>(GetStage());

    TbRectF32 rect = ENEGY_RECT;
    if( m_Energy > 0.0f ){
        rect.x += GetPosX();
        rect.y += GetPosY();
        rect.width *= m_Energy;
        TbRectF32 uvRect(CMN_TEX_UV_RECT(512 - 8, 0, 8, 8));
        TbMatrix mtx = TbMatrix::IDENTITY;
        material->Draw(rect, uvRect, CmnUtil::GetPlayerColor(1), poseMatrix);
    }
}

