/*!
 * UI用フレームスプライト
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_frame_sprite.h"
#include "ui/tb_ui_manager.h"
#include "ui/tb_ui_window.h"


static const TbFloat32 BASE_SIZE = 8.0f;
static const TbFloat32 WIDTH_MIN = BASE_SIZE*1.2f;
static const TbFloat32 HEIGHT_MIN = BASE_SIZE*1.2f;

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbUiFrameSprite::TbUiFrameSprite()
    : Super()
{
    // スプライト設定
    static const TbRectF32Pod SPRITE_RECT = 
    {
        0.0f , 0.0f , WIDTH_MIN , HEIGHT_MIN ,
    };
    TbElasticSprite::InitParam spriteParam;
    spriteParam.basePlaceSize = TbDimensionF32(BASE_SIZE,BASE_SIZE);
    spriteParam.rect = SPRITE_RECT;
    const TbElasticSprite::UvParam* uv = TbUiManager::GetInstance().GetUV( TB_UI_UV_FRAME );
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(spriteParam.rectUV); ++i ) {
        spriteParam.rectUV[i] = uv->rectUV[i];
    }
    Initialize(spriteParam);
    SetZ(TB_UI_SPRITE_Z_FRONT_2);
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiFrameSprite::~TbUiFrameSprite()
{
}

/*!
 * フィル設定
 * @author teco
 */
void TbUiFrameSprite::SetFill( TbBool isFill )
{
    TbElasticSprite::UvParam param = *TbUiManager::GetInstance().GetUV( isFill ? TB_UI_UV_FRAME_FILL : TB_UI_UV_FRAME );
    SetUV(param);
}

}