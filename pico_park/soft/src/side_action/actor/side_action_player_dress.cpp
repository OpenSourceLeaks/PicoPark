/*!
 * プレイヤー
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "side_action_player_dress.h"
#include "side_action_player.h"
#include "side_action/stage/side_action_stage.h"
#include <crayon/map/cr_action_map.h>
#include "root.h"
#include "common/cmn_util.h"

//////////////////////////////////////////
// SideActionPlayerHeaddress
//////////////////////////////////////////

static TbFloat32 HEAD_WIDTH = 25.0f;
static TbFloat32 HEAD_HEIGHT = 13.0f;

static const TbRectF32Pod HEAD_SPRITE_RECT[] = 
{
    {-HEAD_WIDTH , -HEAD_HEIGHT*2 , HEAD_WIDTH*2.0f , HEAD_HEIGHT*2.0f} ,
    {-HEAD_WIDTH , -(HEAD_HEIGHT+1)*2 , HEAD_WIDTH*2.0f , (HEAD_HEIGHT+1)*2.0f},
    {-HEAD_WIDTH , -(HEAD_HEIGHT+2)*2 , HEAD_WIDTH*2.0f , (HEAD_HEIGHT+1)*2.0f},
    { -HEAD_WIDTH , -HEAD_HEIGHT * 2 , HEAD_WIDTH*2.0f , HEAD_HEIGHT*2.0f } ,
};

static const TbCircleF32 HEAD_COLLISION_CIRCLE( 0.0f , 0.0f , HEAD_WIDTH );

static const TbRectF32Pod HEAD_SPRITE_UV_RECT[] = 
{
    CMN_TEX_UV_RECT(288,464,HEAD_WIDTH-1,HEAD_HEIGHT-1),
    CMN_TEX_UV_RECT(320,464,HEAD_WIDTH-1,HEAD_HEIGHT),
    CMN_TEX_UV_RECT(352,464,HEAD_WIDTH-1,HEAD_HEIGHT+1),
    CMN_TEX_UV_RECT(256,464,HEAD_WIDTH - 1,HEAD_HEIGHT - 1),
};

static const TbVector2 HEAD_OFFSET_POS[] =
{
    TbVector2(0.0f,-34.0f) , 
    TbVector2(0.0f,-30.0f) , 
    TbVector2(0.0f,-34.0f) , 
    TbVector2(0.0f,-34.0f) ,
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayerHeaddress::SideActionPlayerHeaddress( SideActionPlayer* player , Type type , TbBool isEnableCollision)
    : Super()
    , m_Type(type)
    , m_Player(player)
{
    TB_STATIC_ASSERT(TYPE_MAX==TB_ARRAY_COUNT_OF(HEAD_SPRITE_RECT));
    TB_STATIC_ASSERT(TYPE_MAX==TB_ARRAY_COUNT_OF(HEAD_SPRITE_UV_RECT));
    TB_STATIC_ASSERT(TYPE_MAX==TB_ARRAY_COUNT_OF(HEAD_OFFSET_POS));

    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetEnableLoop(TB_TRUE);
    sprite.Initialize(HEAD_SPRITE_RECT[type],HEAD_SPRITE_UV_RECT[type]);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_4);
    if (TYPE_NUMBER == m_Type) {
        sprite.SetMaterial(m_Player->GetMaterial());

        AddComponent(&m_SpriteHooker);
        m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this, &Self::onDrawText));
        m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_5);
    }

    // コリジョン
    if (isEnableCollision)
    {
        TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_COLLISION;
        CrActionCollision* collision = CreateCollision(HEAD_COLLISION_CIRCLE, pushBackTarget, TB_TRUE);
        collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_SUB);
    }
    SetWeight(WEIGHT_MAX);

    m_Player->Retain();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayerHeaddress::~SideActionPlayerHeaddress()
{
    m_Player->Release();
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionPlayerHeaddress::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_GET_PLAYERINDEX == event )
    {
        return m_Player->GetPlayerIndex()+1;
    }
    return 0;
}

/*! 
 * ロジック更新
 * @author teco
 * @since 2013.09.08
 */
void SideActionPlayerHeaddress::OnPostAction( TbFloat32 deltatime )
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    if( !TB_VERIFY(stage) ) {
        return;
    }
    TbBool isVisible = m_Player->IsVisible();
    TbSprite& sprite = GetMainSprite();
    if (TYPE_NUMBER == m_Type && isVisible ) {
        sprite.SetColor(CmnUtil::GetPlayerColor(m_Player->GetPlayerIndex()));
        isVisible = SaveFile::GetInstance().IsVisiblePlayerNumber();
    }
    TbFloat32 z = m_Player->GetMainSprite().GetZ() - 0.005f;
    sprite.SetZ(z);
    m_SpriteHooker.GetSprite().SetZ(z-0.002f);
    TbFloat32 scale = m_Player->GetScale();
    SetVisible( isVisible );
    TbVector4 pos = m_Player->GetPos();
    pos.Add( HEAD_OFFSET_POS[m_Type].GetX() * scale, HEAD_OFFSET_POS[m_Type].GetY() * scale, 0.0f );
    SetPos( pos );
    SetOffsetDrawPos(m_Player->GetOffsetDrawPos() * scale);

    // スプライト反映
    if( m_Player->IsLookingRight() ){
        sprite.SetScale(scale, scale);
    }else{
        sprite.SetScale(-scale, scale);
    }
    sprite.SetTranslate( GetDrawPos() );
}

/*!
* 前景描画
* @author teco
*/
void SideActionPlayerHeaddress::onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material)
{
    static TB_NDEBUG_CONST TbSint32 OFFSET_X = 2;
    static TB_NDEBUG_CONST TbSint32 OFFSET_Y = -11;
    static TB_NDEBUG_CONST TbUint32 FONT_SIZE = 16;
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    TbSint32 offsetX = OFFSET_X;
    TbFloat32 scale = m_Player->GetScale();
    if (m_Player->GetMainSprite().GetScale().GetX() < 0.0f) {
        offsetX *= -1;
    }
    TbUint32 fontSize = static_cast<TbUint32>(static_cast<TbFloat32>(FONT_SIZE) * scale);
    fontBuffer->PushSetting();
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
    fontBuffer->SetFontSize(fontSize);
    fontBuffer->SetColor(TbColor::Black());
    fontBuffer->SetPoseMatrix(poseMatrix);

    TbSint32 x = drawPos.GetX() + static_cast<TbFloat32>(offsetX) * scale;
    TbSint32 y = drawPos.GetY() + static_cast<TbFloat32>(OFFSET_Y) * scale;
    fontBuffer->Printf(x,y,"%d",m_Player->GetPlayerIndex()+1);

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}

//////////////////////////////////////////
// SideActionPlayerBackdress
//////////////////////////////////////////

static TbFloat32 BACK_WIDTH = 9.0f;
static TbFloat32 BACK_HEIGHT = 11.0f;

static const TbRectF32Pod BACK_SPRITE_RECT[] = 
{
    {-BACK_WIDTH , -BACK_HEIGHT*2 , BACK_WIDTH*2.0f , BACK_HEIGHT*2.0f} ,
};

static const TbRectF32Pod BACK_SPRITE_UV_RECT[] = 
{
    CMN_TEX_UV_RECT(496,96,BACK_WIDTH-1,BACK_HEIGHT-1),
};

static const TbVector2 BACK_OFFSET_POS[] =
{
    TbVector2(-6.0f,-4.0f) , 
};

/*!
 * コンストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayerBackdress::SideActionPlayerBackdress( SideActionPlayer* player , Type type )
    : Super()
    , m_Type(type)
    , m_Player(player)
{
    TB_STATIC_ASSERT(TYPE_MAX==TB_ARRAY_COUNT_OF(BACK_SPRITE_RECT));
    TB_STATIC_ASSERT(TYPE_MAX==TB_ARRAY_COUNT_OF(BACK_SPRITE_UV_RECT));
    TB_STATIC_ASSERT(TYPE_MAX==TB_ARRAY_COUNT_OF(BACK_OFFSET_POS));

    // スプライト
    TbSprite& sprite = GetMainSprite();
    sprite.SetEnableLoop(TB_TRUE);
    sprite.SetZ(CMN_SPRITE_Z_FRONT_2);
    sprite.Initialize(BACK_SPRITE_RECT[type],BACK_SPRITE_UV_RECT[type]);

    m_Player->Retain();
}

/*!
 * デストラクタ
 * @author teco
 * @since 2013.09.02
 */
SideActionPlayerBackdress::~SideActionPlayerBackdress()
{
    m_Player->Release();
}

/*!
 * イベント通知
 * @author teco
 */
TbSint32 SideActionPlayerBackdress::NotifyEvent( TbUint32 event , void* param )
{
    if( SIDE_ACTION_EVENT_GET_PLAYERINDEX == event )
    {
        return m_Player->GetPlayerIndex()+1;
    }
    return 0;
}

/*! 
 * ロジック更新
 * @author teco
 * @since 2013.09.08
 */
void SideActionPlayerBackdress::OnPostAction( TbFloat32 deltatime )
{
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    if( !TB_VERIFY(stage) ) {
        return;
    }

    GetMainSprite().SetVisible( m_Player->IsVisible() );
    TbVector4 pos = m_Player->GetPos();
    // スプライト反映
    TbSprite& sprite = GetMainSprite();
    if( m_Player->IsLookingRight() ){
        pos.Add( BACK_OFFSET_POS[m_Type].GetX() , BACK_OFFSET_POS[m_Type].GetY() , 0.0f );
        sprite.SetScale(1.0f,1.0f);
    }else{
        pos.Add( -BACK_OFFSET_POS[m_Type].GetX() , BACK_OFFSET_POS[m_Type].GetY() , 0.0f );
        sprite.SetScale(-1.0f,1.0f);
    }
    SetPos( pos );
    sprite.SetTranslate( GetDrawPos() );
}


