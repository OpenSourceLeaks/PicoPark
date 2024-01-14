/*!
 * UI矢印ボタン
 * @author teco
 */

#include "fwd.h"
#include "ui/ui_arrow_button.h"
#include "ui/ui_manager.h"
#include <crayon/input/cr_input.h>

#include <ui/tb_ui_window.h>

static const TbFloat32 SPRITE_WIDTH     = 38.0f;
static const TbFloat32 SPRITE_HEIGHT    = 40.0f;
static const TbFloat32 SPRITE_UV_WIDTH  = 19.0f;
static const TbFloat32 SPRITE_UV_HEIGHT = 20.0f;
static const TbRectF32Pod SPRITE_UV[] = 
{
    { CMN_TEX_UV_RECT(0,0,0,0) } , // 左
    { CMN_TEX_UV_RECT(720,64,SPRITE_UV_WIDTH,SPRITE_UV_HEIGHT) } , // 左
    { CMN_TEX_UV_RECT(720,0,SPRITE_UV_WIDTH,SPRITE_UV_HEIGHT) } , // 右
};

static const TbRectF32Pod SPRITE_UV_PRESS[] = 
{
    { CMN_TEX_UV_RECT(0,0,0,0) } , // 左
    { CMN_TEX_UV_RECT(720,96,SPRITE_UV_WIDTH,SPRITE_UV_HEIGHT) } , // 左
    { CMN_TEX_UV_RECT(720,32,SPRITE_UV_WIDTH,SPRITE_UV_HEIGHT) } , // 右
};

/*!
 * コンストラクタ
 * @author teco
 */
UiArrowButtonSkin::UiArrowButtonSkin()
    : m_Type(TYPE_INVALID)
{
    m_Sprite.SetName("Arrow");
}

/*!
 * デストラクタ
 * @author teco
 */
UiArrowButtonSkin::~UiArrowButtonSkin()
{
}

/*!
 * デストラクタ
 * @author teco
 */
void UiArrowButtonSkin::Initilaize( Type type )
{
    m_Type = type;
    // スプライト設定
    static const TbRectF32Pod SPRITE_RECT = 
    {
        0.0f , 0.0f , 1.0f , 1.0f ,
    };
    m_Sprite.Initialize(SPRITE_RECT,SPRITE_UV[m_Type]);
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_2);
    OnSizeChanged(SPRITE_WIDTH,SPRITE_HEIGHT);
}

/*!
 * 表示領域取得
 * @author teco
 */
TbRectF32    UiArrowButtonSkin::GetRegion() const
{
    TbRectF32 rect(m_Sprite.GetTranslate().GetX(),
                   m_Sprite.GetTranslate().GetY(),
                   m_Sprite.GetScale().GetX(),
                   m_Sprite.GetScale().GetY());
    return rect;
}

/*!
 * 生成時コールバック
 * @author teco
 */
void UiArrowButtonSkin::OnCreated()
{
    TbUiManager& manager = TbUiManager::GetInstance();
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_2+GetOwner()->GetWindow()->GetSpriteOffsetZ());
    m_Sprite.Enter(&manager.GetSpriteScene());
    m_Sprite.SetTranslate(GetOwner()->GetGlobalPos());
    OnFocusChanged(TB_FALSE);
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void UiArrowButtonSkin::OnDeleted()
{
    m_Sprite.Exit();
}

/*!
 * フォーカス状態変更
 * @author teco
 */
void UiArrowButtonSkin::OnFocusChanged( TbBool isFocus )
{
    TbRectF32 uv(SPRITE_UV[m_Type]);
    TbUint32 playerBit = GetOwner()->GetPlayerBit();
    TbFloat32 offsetX = 0.0f;
    if( TbBitPopulation(playerBit) > 1 || m_IsCommonColor) { // 複数立っている場合
        uv.x += CMN_TEX_POS(SPRITE_UV_WIDTH*4.0f);
    }else{
        TbUint32 index = CrInput::GetPlayerPadIndex(TbBitLeastSignificant(playerBit));
        uv.x += CMN_TEX_POS((SPRITE_UV_WIDTH)*(index));
    }
    m_Sprite.SetUV(uv);
}

/*!
 * 押された状態変更
 * @author teco
 */
void UiArrowButtonSkin::OnPressChanged( TbBool isPress )
{
    if( isPress ){
        TbRectF32 uv(SPRITE_UV_PRESS[m_Type]);

        TbUint32 playerBit = GetOwner()->GetPlayerBit();
        TbFloat32 offsetX = 0.0f;
        if( TbBitPopulation(playerBit) > 1 || m_IsCommonColor ) { // 複数立っている場合
            uv.x += CMN_TEX_POS(SPRITE_UV_WIDTH*4.0f);
        }else{
            TbUint32 index = CrInput::GetPlayerPadIndex(TbBitLeastSignificant(playerBit));
            uv.x += CMN_TEX_POS((SPRITE_UV_WIDTH)*(index));
        }
        m_Sprite.SetUV(uv);
    }else{
        OnFocusChanged( GetOwner()->IsFocused() );
    }
}
