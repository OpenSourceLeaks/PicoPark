/*!
 * イメージボックス
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_IMAGE_BOX_H_
#define _INCLUDED_TB_UI_IMAGE_BOX_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>
#include <graphics/render/3dapi/tb_texture.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <graphics/sprite/tb_sprite.h>

#include "ui/tb_ui_widget.h"
#include "ui/tb_ui_frame_sprite.h"

namespace toybox
{

class TbUiImageBox : public TbUiWidget
{
public:

    typedef TbUiWidget Super;
    typedef TbUiImageBox Self;

public:
    
    // コンストラクタ
    TbUiImageBox();

    // デストラクタ
    virtual ~TbUiImageBox();

public:

    // イメージ設定
    void SetImage( toybox::TbTexture* texture , const TbRectF32& uv , TbFloat32 offsetZ = 0.0f );

    // カラー設定
    void SetColor(const TbColor& color) {
        m_ImageSprite.SetColor( color );
    }

    // マテリアル設定
    void SetMaterial(TbSpriteMaterial* material) {
        m_ImageSprite.SetMaterial(material);
    }
    
    // フレームを利用するかどうか
    void SetEnableFrame( TbBool isEnable );

    // フレームを利用するかどうか
    TbBool IsEnableFrame() const {
        return m_BitArray.Test(FLAG_ENABLE_FRAME);
    }

    // フレームを表示するか
    void SetVisibleFrame( TbBool isVisible ) {
        m_Frame.SetVisible(isVisible);
    }

public:

    // フレーム用UV設定
    void SetFrameUV( const TbElasticSprite::UvParam& param );

public:

    // 定期処理
    virtual void OnUpdate( TbFloat32 deltaSec );

    // 生成時コールバック
    virtual void OnCreated();

    // 破棄時コールバック
    virtual void OnDeleted();

    // 可視情報変化時コールバック
    virtual void OnVisibleChanged( TbBool isVisible ) {
        m_ImageSprite.SetVisible(isVisible);
        m_Frame.SetVisible(isVisible);
    }

private:

    enum
    {
        FLAG_ENABLE_FRAME ,
        FLAG_MAX
    };

private:
    TbBitArray32                m_BitArray;     
    TbSprite                    m_ImageSprite;  // イメージ表示用スプライト
    TbSpriteFixedMaterial       m_Material;     // 自前テクスチャ用
    TbUiFrameSprite             m_Frame;
};

}

#endif
