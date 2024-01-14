/*!
 * チェックポイント
 * @author teco
 * @since 2014.05.25
 */

#include "fwd.h"
#include "side_action_check_point.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbRectF32Pod SPRITE_RECT = 
{
    -16.0f , -64.0f , 32.0f , 64.0f
};

static const TbRectF32Pod SPRITE_UV_RECT = 
{
    CMN_TEX_UV_RECT(32,320,16,32)
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCheckPoint::SideActionCheckPoint()
    : Super()
{
    TbSprite& sprite = GetMainSprite();
    sprite.Initialize(SPRITE_RECT,SPRITE_UV_RECT);
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);

    TbUint32 pushBackTarget = 0;
    ActionCollision* collision = CreateCollision( SPRITE_RECT , pushBackTarget , TB_TRUE );
    if( TB_VERIFY(collision)) {
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_KEY);
        collision->SetCollidedCallback( TbCreateMemFunc(this,&Self::onCollided));
    }
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.19
 */
SideActionCheckPoint::~SideActionCheckPoint()
{
}

/*!
 * ロジック更新
 * @author teco
 * @since 2013.09.19
 */
void SideActionCheckPoint::OnUpdateAction( TbFloat32 deltatime )
{
    static TB_NDEBUG_CONST TbSint32 FONT_SIZE = 16;
    static TB_NDEBUG_CONST TbFloat32 HEIGHT_SCALE = 1.2f;
    if( m_BitArray.Test(FLAG_CHECK) )
    {
        ActionCollision* collision = GetCollision();
        const TbRectF32& rect = collision->GetRect();
        TbFontBuffer2d* font = Root::GetInstance().GetFont();
        font->PushSetting();
        font->SetFontSize(FONT_SIZE);
        font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        font->SetAlignY(TbFontBuffer2d::ALIGN_Y_DOWN);
        Root::GetInstance().GetFont()->Printf(static_cast<TbSint32>(GetPosX()),
                                              static_cast<TbSint32>(GetPosY()-rect.height*HEIGHT_SCALE),
                                              "CHECK");
        font->PopSetting();
    }
}

/*!
 * 衝突コールバック(押し戻しは無しバージョン)
 * @author teco
 * @since 2013.10.16
 */
void SideActionCheckPoint::onCollided(ActionCollision* target)
{
    switch( target->GetLayer() )
    {
    case SIDE_ACTION_COLLISION_LAYER_PLAYER:
        {
            if( !m_BitArray.Test(FLAG_CHECK) ){
                Stage* stage = GetStage();
                // このレイヤーにはプレイヤーしかいないので、その前提でキャストする
                SideActionPlayer* player = static_cast<SideActionPlayer*>(target->GetActor());

                // チェックポイント保存
                TbUint32 playerIndex = player->GetPlayerIndex();
                const TbRectF32& playerRect = player->GetCollision()->GetRect();
                CmnGlobalData& globalData = Root::GetInstance().GetGlobalData();
                TbVector2 pos( GetPosX() - playerRect.width/2 , GetPosY() - playerRect.height/2-1 );
                globalData.SetCheckPoint(stage->GetId(),player->GetPlayerIndex(),pos,stage->GetPlaySec());

                m_BitArray.SetBit(FLAG_CHECK);
            }
        }
        break;
    }
}
