/*!
 * UI用フレームスプライト
 * @author teco
 */

#ifndef _INCLUDED_TB_UI_FRAME_SPRITE_H_
#define _INCLUDED_TB_UI_FRAME_SPRITE_H_

#include <base/string/tb_static_string.h>
#include <base/util/tb_function.h>
#include <graphics/sprite/tb_sprite_scene.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <ui/tb_ui_widget.h>
#include <ui/tb_ui_types.h>

namespace toybox
{

class TbUiFrameSprite : private TbElasticSprite
{
public:
    typedef TbElasticSprite Super;
public:
    
    // コンストラクタ
    TbUiFrameSprite();
    
    // デストラクタ
    virtual ~TbUiFrameSprite();

public:

    // フィル設定
    void SetFill( TbBool isFill );

    // 可視設定
    void SetVisible( TbBool isVisible ) {
        Super::SetVisible( isVisible );
    }

    // 可視設定取得
    TbBool IsVisible() const {
        return Super::IsVisible();
    }

public:

    // シーンに入る
    void Enter( TbSpriteScene* scene ) {
        Super::Enter(scene);
    }

    // シーンから解除
    void Exit() {
        Super::Exit();
    }

public:

    // 位置取得
    const TbVector2& GetTranslate() const {
        return Super::GetTranslate();
    }
    
    // 位置設定(X座標)
    void SetTranslateX( TbFloat32 x ) {
        Super::SetTranslateX(x);
    }
    
    // 位置設定(Y座標)
    void SetTranslateY( TbFloat32 y ) {
        Super::SetTranslateY(y);
    }

    // 位置設定
    void SetTranslate( const TbVector2& pos ) {
        Super::SetTranslate(pos);
    }

    // 位置設定
    void SetTranslate( TbFloat32 x , TbFloat32 y ) {
        Super::SetTranslate(x,y);
    }

    // 位置加算
    void AddTranslate( TbFloat32 x , TbFloat32 y ) {
        Super::AddTranslate(x,y);
    }

    // 位置加算
    void AddTranslate( const TbVector2& vec ) {
        Super::AddTranslate(vec);
    }

    // 奥行き設定
    void SetZ( TbFloat32 z ) {
        Super::SetZ(z);
    }

    // 横幅設定
    void SetWidth( TbFloat32 width ) {
        Super::SetWidth(width);
    }

    // 高さ設定
    void SetHeight( TbFloat32 height ) {
        Super::SetHeight(height);
    }

    // サイズ設定
    void SetSize( TbFloat32 width , TbFloat32 height ) {
        Super::SetSize(width,height);
    }

    // サイズ取得
    const TbDimensionF32& GetSize() const {
        return Super::GetSize();
    }

    // 横幅取得
    TbFloat32 GetWidth() const {
        return Super::GetWidth();
    }

    // 縦幅取得
    TbFloat32 GetHeight() const {
        return Super::GetHeight();
    }

    // 領域設定
    void SetRegion( const TbRectF32& region ){
        SetTranslate(region.x,region.y);
        SetSize(region.width,region.height);
    }

    // 強制UV設定
    void SetUV( const TbElasticSprite::UvParam& param ) {
        Super::SetUV(param);
    }

};

}

#endif
