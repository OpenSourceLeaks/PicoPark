/*!
 * 落下ボックス
 * @author teco
 */

#include "fwd.h"
#include "side_action_fall_box.h"
#include "side_action/side_action_types.h"
#include "common/material/cmn_sprite_player_material.h"
#include "side_action/stage/side_action_stage.h"
#include "root.h"
#include "common/cmn_util.h"

static const TbRectF32Pod SPRITE_UV_PLACE_RECT[] = 
{
    { CMN_TEX_UV_RECT(144 + 0  ,640     ,8,8) } , // PLACE_LU
    { CMN_TEX_UV_RECT(144 + 0  ,640 + 16,8,8) } , // PLACE_LC
    { CMN_TEX_UV_RECT(144 + 0  ,640 + 32+8,8,8) } , // PLACE_LB

    { CMN_TEX_UV_RECT(144 + 16 ,640     ,8,8) } , // PLACE_UP
    { CMN_TEX_UV_RECT(144 + 16 ,640 + 16,8,8) } , // PLACE_IN
    { CMN_TEX_UV_RECT(144 + 16 ,640 + 32 + 8,8,8) } , // PLACE_BT

    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640     ,8,8) } , // PLACE_RU
    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640 + 16,8,8) } , // PLACE_RC
    { CMN_TEX_UV_RECT(144 + 32 + 8 ,640 + 32 + 8,8,8) } , // PLACE_RB
};

TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT)==TbElasticSprite::PLACE_MAX);

// コンストラクタ
SideActionFallBox::SideActionFallBox()
    : m_FallTimer(0.0f)
{
}

/*!
 * デストラクタ
 * @author teco
 */
SideActionFallBox::~SideActionFallBox()
{
}

/*!
 * セットアップ
 * @author teco
 */
void SideActionFallBox::Setup(const CrActorFactory::UserData& userData)
{
    TbFloat32 width = userData.AsFloat32(0);
    TbFloat32 height = userData.AsFloat32(1);
    TbRectF32Pod rect = { -width / 2.0f , -height , width , height };
    initialize(rect);
}

/*!
 * 初期化
 * @author teco
 */
void SideActionFallBox::initialize(const TbRectF32& rect)
{
    m_Rect = rect;

    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);


    // 落下矢印
    {
        const TbRectF32Pod FALL_ARROW_SPRITE_RECT =
        {
            -16.0f , -16.0f - m_Rect.height/2.0f , 32.0f , 32.0f
        };
        static const TbRectF32Pod FALL_ARROW_SPRITE_UV_RECT =
        {
            CMN_TEX_UV_RECT(144,400,16,16)
        };
        TbSprite& sprite = GetMainSprite();
        sprite.Initialize(FALL_ARROW_SPRITE_RECT, FALL_ARROW_SPRITE_UV_RECT);
        sprite.SetMaterial(Root::GetInstance().GetPlayerMaterial());
        sprite.SetEnableLoop(TB_TRUE);
        sprite.SetZ(CMN_SPRITE_Z_FRONT_5);
    }

    // スプライト
    AddComponent(&m_Sprite);
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(16.0f, 16.0f);
    spriteParam.rect = m_Rect;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT); ++i) {
        spriteParam.rectUV[i] = SPRITE_UV_PLACE_RECT[i];
    }
    m_Sprite.GetSprite().Initialize(spriteParam);
    m_Sprite.GetSprite().SetMaterial(Root::GetInstance().GetPlayerMaterial());
    m_Sprite.GetSprite().SetColor(CmnUtil::GetPlayerColor(9));
    m_Sprite.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_4);

    // コリジョン
    TbRectF32 collisionRect = m_Rect;
    TbFloat32 MARGIN = 4.0f;
    collisionRect.x     += MARGIN/2.0f;
    collisionRect.y     += MARGIN/2.0f;
    collisionRect.width -= MARGIN;
    collisionRect.height -= MARGIN;
    TbUint32 pushBackTarget = CrActionCollision::PUSH_BACK_TARGET_MAP |
        CrActionCollision::PUSH_BACK_TARGET_COLLISION;
    CrActionCollision* collision = CreateCollision(collisionRect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_PLAYER_OBJECT);
    collision->SetContactedCallback(TbCreateMemFunc(this, &Self::onContacted));
    collision->SetActive(TB_FALSE);
}

/*!
 * 定期処理
 * @author teco
 */
void SideActionFallBox::OnUpdateAction( TbFloat32 deltatime )
{
    static TB_NDEBUG_CONST TbFloat32 FALL_START = 0.22f;
    // 接しているオブジェクトが押してきていたら
    CrActionCollision* collision = GetCollision();
    SideActionStage* stage = TbDynamicCast<SideActionStage*>(GetStage());
    if(m_BitArray.Test(FLAG_FALL))
    {
        if(FALL_START < m_FallTimer ){
            // 自由落下
            if( !collision->IsContacted(CR_CONTACT_DOWN) ) {
                AddVelo(0.0f,SIDE_ACTION_GRAVITY);
            }else{
                SetVeloY(0.0f);
            }
            if (stage->GetWindowHeight()*2.0f < GetPosY() )
            {
                Exit();
                return;
            }
        }
        else {
            m_FallTimer += deltatime;
        }
    }

    TbFloat32 x = GetDrawPos().GetX()+ m_Rect.x;;
    if ((0.0f <= x + m_Rect.width+1.0f) && (x-1.0f < stage->GetWindowWidth()) ) 
    {
        collision->SetActive(TB_TRUE);
        m_Sprite.GetSprite().SetVisible(TB_TRUE);
    }
    else 
    {
        collision->SetActive(TB_FALSE);
        m_Sprite.GetSprite().SetVisible(TB_FALSE);
    }
}

/*!
 * 接触開始コールバック
 * @author teco
 */
void SideActionFallBox::onContacted(const TbVector2& normal, CrActionCollision::ContactTarget target, const CrActionCollisionInfo& collision)
{
    if (normal.GetY() < 0.0f) {
        m_BitArray.SetBit(FLAG_FALL);
    }
}
