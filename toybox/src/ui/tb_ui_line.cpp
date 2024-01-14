/*!
 * UI用ライン
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_line.h"
#include "ui/tb_ui_manager.h"
#include "ui/tb_ui_window.h"

namespace toybox
{

static const TbRectF32Pod SPRITE_RECT = 
{
    0.0f , -2.0f , 1.0f , 4.0f 
};

/*!
 * コンストラクタ
 * @author teco
 */
TbUiLine::TbUiLine()
    : Super()
{
    const TbRectF32* uvRect = TbUiManager::GetInstance().GetUVRect(TB_UI_UV_RECT_LINE);
    m_Sprite.Initialize(SPRITE_RECT,*uvRect);
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_2);
    SetSize( SPRITE_RECT.width , SPRITE_RECT.height );
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiLine::~TbUiLine()
{
}
/*!
 * 生成時コールバック
 * @author Miyake_Shunsuke
 */
void TbUiLine::OnCreated()
{
    TbUiManager& manager = TbUiManager::GetInstance();
    m_Sprite.SetZ(TB_UI_SPRITE_Z_FRONT_2+GetWindow()->GetSpriteOffsetZ());
    m_Sprite.Enter(&manager.GetSpriteScene());
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void TbUiLine::OnDeleted()
{
    m_Sprite.Exit();
}

/*!
 * 定期処理
 * 破棄時コールバック
 * @author Miyake_Shunsuke
 */
void TbUiLine::OnUpdate( TbFloat32 deltaSec )
{
    if( !IsCreated() ){ 
        return;
    }
    if( m_Sprite.IsInitialized() ){
        m_Sprite.SetTranslate( GetGlobalPos() );
        m_Sprite.SetScale( GetGlobalRegion().width , 1.0f );
    }
}

}