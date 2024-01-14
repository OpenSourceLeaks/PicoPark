/*!
 * UIボタン
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_button_skin.h"
#include "ui/tb_ui_button.h"
#include "ui/tb_ui_manager.h"
#include "ui/tb_ui_window.h"
#include <graphics/render/tb_draw_2d.h>

namespace toybox
{

static const TbFloat32 BASE_SIZE = 8.0f;
static const TbFloat32 WIDTH_MIN = BASE_SIZE*1.2f;
static const TbFloat32 HEIGHT_MIN = BASE_SIZE*1.2f;

/*!
 * コンストラクタ
 * @author teco
 */
TbUiButtonDefaultSkin::TbUiButtonDefaultSkin()
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
    const TbElasticSprite::UvParam* uv = TbUiManager::GetInstance().GetUV( TB_UI_UV_BUTTON );
    for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(spriteParam.rectUV); ++i ) {
        spriteParam.rectUV[i] = uv->rectUV[i];
    }
    m_Sprite.Initialize(spriteParam);
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_2);
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiButtonDefaultSkin::~TbUiButtonDefaultSkin()
{
}

/*!
 * 表示領域取得
 * @author teco
 */
TbRectF32    TbUiButtonDefaultSkin::GetRegion() const
{
    TbRectF32 rect(m_Sprite.GetTranslate().GetX(),
                   m_Sprite.GetTranslate().GetY(),
                   m_Sprite.GetWidth(),
                   m_Sprite.GetHeight());
    return rect;
}

/*!
 * 生成時コールバック
 * @author teco
 */
void TbUiButtonDefaultSkin::OnCreated()
{
    TbUiManager& manager = TbUiManager::GetInstance();
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_5+GetOwner()->GetWindow()->GetSpriteOffsetZ());
    m_Sprite.Enter(&manager.GetSpriteScene());
    m_Sprite.SetTranslate(GetOwner()->GetGlobalPos());
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void TbUiButtonDefaultSkin::OnDeleted()
{
    m_Sprite.Exit();
}

/*!
 * フォーカス状態変更
 * @author teco
 */
void TbUiButtonDefaultSkin::OnFocusChanged( TbBool isFocus )
{
    if( !GetOwner()->IsOn() )
    {
        TbElasticSprite::UvParam param;
        const TbElasticSprite::UvParam* uv = TbUiManager::GetInstance().GetUV( isFocus ? TB_UI_UV_BUTTON_FOCUS : TB_UI_UV_BUTTON );
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(uv->rectUV); ++i ) {
            param.rectUV[i] = uv->rectUV[i];
        }
        m_Sprite.SetUV(param);
    }
}

/*!
 * 押された状態変更
 * @author teco
 */
void TbUiButtonDefaultSkin::OnPressChanged( TbBool isPress )
{
    TbElasticSprite::UvParam param;
    if( isPress ){
        param = *TbUiManager::GetInstance().GetUV( TB_UI_UV_BUTTON_PRESS );
    }else{
        param = *TbUiManager::GetInstance().GetUV( GetOwner()->IsFocused() ? TB_UI_UV_BUTTON_FOCUS : TB_UI_UV_BUTTON );
    }
    m_Sprite.SetUV(param);
}

}
