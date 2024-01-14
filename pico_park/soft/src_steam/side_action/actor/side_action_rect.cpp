/*!
 * シンプルな矩形
 * @author teco
 */

#include "fwd.h"
#include "side_action_rect.h"
#include "side_action/stage/side_action_stage.h"
#include "side_action/actor/side_action_player.h"
#include "root.h"

static const TbRectF32Pod SPRITE_UV_PLACE_RECT[] =
{
    { CMN_TEX_UV_RECT(0       ,528     ,16,16) } , // PLACE_LU
    { CMN_TEX_UV_RECT(0       ,528 + 16,16,16) } , // PLACE_LC
    { CMN_TEX_UV_RECT(0       ,528 + 32,16,16) } , // PLACE_LB
    { CMN_TEX_UV_RECT(16      ,528     ,16,16) } , // PLACE_UP
    { CMN_TEX_UV_RECT(16      ,528 + 16,16,16) } , // PLACE_IN
    { CMN_TEX_UV_RECT(16      ,528 + 32,16,16) } , // PLACE_BT
    { CMN_TEX_UV_RECT(32      ,528     ,16,16) } , // PLACE_RU
    { CMN_TEX_UV_RECT(32      ,528 + 16,16,16) } , // PLACE_RC
    { CMN_TEX_UV_RECT(32      ,528 + 32,16,16) } , // PLACE_RB
};

TB_STATIC_ASSERT(TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT) == TbElasticSprite::PLACE_MAX);


/*! 
 * コンストラクタ
 * @author teco
 */
SideActionRect::SideActionRect(const TbRectF32& rect )
{
    m_Rect = rect;

    SetLayerIndexDefault(SIDE_ACTION_ACTOR_LAYER_POST);
    // スプライト
    AddComponent(&m_Sprite);
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(32.0f, 32.0f);
    spriteParam.rect = m_Rect;
    for (TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(SPRITE_UV_PLACE_RECT); ++i) {
        spriteParam.rectUV[i] = SPRITE_UV_PLACE_RECT[i];
    }
    m_Sprite.GetSprite().Initialize(spriteParam);
    m_Sprite.GetSprite().SetZ(CMN_SPRITE_Z_BACK_2);

    // コリジョン
    TbUint32 pushBackTarget = ActionCollision::PUSH_BACK_TARGET_COLLISION;
    ActionCollision* collision = CreateCollision(rect, pushBackTarget, TB_TRUE);
    collision->SetLayer(SIDE_ACTION_COLLISION_LAYER_OBJECT_2);
};

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionRect::~SideActionRect()
{
}
