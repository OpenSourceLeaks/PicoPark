/*!
 * UI用イメージボックス
 * @author teco
 */

#include "tb_fwd.h"
#include "ui/tb_ui_image_box.h"
#include "ui/tb_ui_manager.h"
#include "ui/tb_ui_window.h"


static const TbFloat32 FRAME_MARGIN = 16.0f;

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbUiImageBox::TbUiImageBox()
    : Super()
    , m_Material(TbUiManager::GetInstance().GetRenderPassIndex())
{
//    SetEnableFrame(TB_TRUE);
}

/*!
 * デストラクタ
 * @author teco
 */
TbUiImageBox::~TbUiImageBox()
{

}

/*!
 * イメージ設定
 * @author Miyake_Shunsuke
 * @since 2014.12.18
 */
void TbUiImageBox::SetImage( TbTexture* texture , const TbRectF32& uv , TbFloat32 offsetZ )
{
    if( m_ImageSprite.IsInitialized() ){
        m_ImageSprite.SetUV(uv);
    }else{
        m_ImageSprite.Initialize(TbRectF32(0.0f,0.0f,1.0f,1.0f),uv);
        m_ImageSprite.SetZ(TB_UI_SPRITE_Z_FRONT_3+ offsetZ);
        if( IsCreated() ){
            TbSpriteScene& scene = TbUiManager::GetInstance().GetSpriteScene();
            m_ImageSprite.SetZ(TB_UI_SPRITE_Z_FRONT_3 + offsetZ +GetWindow()->GetSpriteOffsetZ());
            m_ImageSprite.Enter(&scene);
        }
    }
    if( texture ) {
        m_Material.SetTexture(texture);
        m_ImageSprite.SetMaterial(&m_Material);
    }else{
        m_ImageSprite.SetMaterial(nullptr);
    }
}

/*!
 * 定期処理
 * @author teco
 */
void TbUiImageBox::OnUpdate( TbFloat32 deltaSec )
{
    if( !IsCreated() ){
        return;
    }

    TbRectF32 region = GetGlobalRegion();;
    m_ImageSprite.SetTranslate(region.x,region.y);
    m_ImageSprite.SetScale(region.width,region.height);
    region.x -= FRAME_MARGIN;
    region.y -= FRAME_MARGIN;
    region.width += FRAME_MARGIN*2.0f;
    region.height += FRAME_MARGIN*2.0f;
    m_Frame.SetRegion(region);
}

/*!
 * 生成時コールバック
 * @author teco
 */
void TbUiImageBox::OnCreated()
{
    TbSpriteScene& scene = TbUiManager::GetInstance().GetSpriteScene();
    TbRectF32 region = GetGlobalRegion();;
    if( m_ImageSprite.IsInitialized() ){
        m_ImageSprite.SetZ(TB_UI_SPRITE_Z_FRONT_3+GetWindow()->GetSpriteOffsetZ());
        m_ImageSprite.Enter(&scene);   
    }
    if( IsEnableFrame() ){
        m_Frame.SetZ(TB_UI_SPRITE_Z_FRONT_2+GetWindow()->GetSpriteOffsetZ());
        m_Frame.Enter(&scene);
    }
    OnUpdate(0.0f);
}

/*!
 * 破棄時コールバック
 * @author teco
 */
void TbUiImageBox::OnDeleted()
{
    m_ImageSprite.Exit();
    m_Frame.Exit();
}

/*!
 * フレームを利用するかどうか
 * @author teco
 */
void TbUiImageBox::SetEnableFrame( TbBool isEnable )
{
    if( TB_BOOL_NOT_EQUAL(isEnable,m_BitArray.Test(FLAG_ENABLE_FRAME)) )
    {
        if( IsCreated() ){
            if( isEnable ){
                TbSpriteScene& scene = TbUiManager::GetInstance().GetSpriteScene();
                m_Frame.SetZ(TB_UI_SPRITE_Z_FRONT_2+GetWindow()->GetSpriteOffsetZ());
                m_Frame.Enter(&scene);
            }else{
                m_Frame.Exit();
            }
        }
        m_BitArray.SetBit(FLAG_ENABLE_FRAME,isEnable);
    }
}

/*!
 * フレーム用UV設定
 * @author teco
 */
void TbUiImageBox::SetFrameUV( const TbElasticSprite::UvParam& param )
{
    m_Frame.SetUV(param);
}

}
