/*!
 * UIボタン
 * @author teco
 */

#include "fwd.h"
#include "ui/ui_button.h"
#include "ui/ui_manager.h"

/*!
 * フォーカス状態変更
 * @author teco
 */
void UiButtonSkin::OnFocusChanged( TbBool isFocus )
{
    TbElasticSprite::UvParam param;
    if( isFocus ){
        param = *TbUiManager::GetInstance().GetUV(TB_UI_UV_BUTTON_FOCUS);
        TbUint32 playerBit = GetOwner()->GetPlayerBit();
        TbFloat32 offsetX = 0.0f;
        if( TbBitPopulation(playerBit) > 1 ) { // 複数立っている場合
            offsetX += CMN_TEX_POS(32.0f);
        }else{
            TbUint32 index = CrInput::GetPlayerPadIndex(TbBitLeastSignificant(playerBit));
            offsetX += CMN_TEX_POS(16.0f*index);
        }
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(param.rectUV); ++i ) {
            param.rectUV[i].x += offsetX;
        }
    }else{
        param = *TbUiManager::GetInstance().GetUV(TB_UI_UV_BUTTON);
    }
    SetUV(param);
}

/*!
 * 押された状態変更
 * @author teco
 */
void UiButtonSkin::OnPressChanged( TbBool isPress )
{
    if( isPress ){
        TbElasticSprite::UvParam param;
        param = *TbUiManager::GetInstance().GetUV( TB_UI_UV_BUTTON_PRESS );
        TbUint32 playerBit = GetOwner()->GetPlayerBit();
        TbFloat32 offsetX = 0.0f;
        if( TbBitPopulation(playerBit) > 1 ) { // 複数立っている場合
            offsetX += CMN_TEX_POS(32.0f);
        }else{
            TbUint32 index = CrInput::GetPlayerPadIndex(TbBitLeastSignificant(playerBit));
            offsetX += CMN_TEX_POS(16.0f*index);
        }
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(param.rectUV); ++i ) {
            param.rectUV[i].x += offsetX;
        }
        SetUV(param);
    }else{
        OnFocusChanged( GetOwner()->IsFocused() );
    }
}
