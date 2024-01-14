/*!
 * UIボタン
 * @author teco
 */

#include "fwd.h"
#include "ui/ui_button.h"
#include "ui/ui_manager.h"
#include "system/sys_input.h"

/*!
 * フォーカス状態変更
 * @author teco
 */
void UiButtonSkin::OnFocusChanged( TbBool isFocus )
{
    if( GetOwner()->IsEnable() )
    {
        TbElasticSprite::UvParam param;
        if( isFocus ){
            param = *TbUiManager::GetInstance().GetUV(TB_UI_UV_BUTTON_FOCUS);
        }else{
            param = *TbUiManager::GetInstance().GetUV(TB_UI_UV_BUTTON);
        }
        SetUV(param);
    }
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
//        TbUint32 playerBit = GetOwner()->GetPlayerBit();
        SetUV(param);
    }else{
        OnFocusChanged( GetOwner()->IsFocused() );
    }
}

/*!
 * 有効設定変更
 * @author teco
 */
void UiButtonSkin::OnEnableChanged( TbBool isEnable )
{
    if( isEnable ) {
        OnFocusChanged(GetOwner()->IsFocused());
    }else{
        TbElasticSprite::UvParam param;
        param = *TbUiManager::GetInstance().GetUV( TB_UI_UV_BUTTON );
        for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(param.rectUV); ++i ) {
            param.rectUV[i].x += CMN_TEX_POS(16.0f);
        }
        SetUV(param);
    }
}
