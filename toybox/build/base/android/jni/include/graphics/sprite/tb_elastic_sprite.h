/*!
 * 伸縮性のあるスプライトクラス
 * @author teco
 */

#ifndef _INCLUDED_TB_ELASTIC_SPRITE_H_
#define _INCLUDED_TB_ELASTIC_SPRITE_H_

#include <base/math/tb_matrix.h>
#include <graphics/sprite/tb_sprite.h>

namespace toybox
{

class TbElasticSprite : private TbSprite
{
public:

    typedef TbSprite Super;
    typedef TbElasticSprite Self;

    enum PlaceType
    {
        PLACE_LU , 
        PLACE_LC , 
        PLACE_LB , 

        PLACE_UP , 
        PLACE_IN , 
        PLACE_BT , 

        PLACE_RU , 
        PLACE_RC ,
        PLACE_RB ,

        PLACE_MAX
    };

    struct InitParam
    {
        TbDimensionF32  basePlaceSize;                      // 場所の基準サイズ
        TbRectF32       rectUV[TbElasticSprite::PLACE_MAX]; // 場所別UV座標
        TbRectF32       rect;
    };

    // UV座標差し替え用
    struct UvParam
    {
        TbRectF32       rectUV[TbElasticSprite::PLACE_MAX]; // 場所別UV座標
    };

public:

    // コンストラクタ
    TbElasticSprite();

    // デストラクタ
    ~TbElasticSprite();

public:

    // ノードツリー設定
    TbResult Initialize( const InitParam& param );

    // 初期化済みか
    TbBool IsInitialized() const {
        return Super::IsInitialized();
    }

    // マテリアル設定
    void SetMaterial( TbUint32 index , TbSpriteMaterial* material ) {
        Super::SetMaterial(index,material);
    }

    // マテリアル設定
    void SetMaterial( TbSpriteMaterial* material ) {
        Super::SetMaterial(material);
    }

    // カラー設定
    void SetColor(const TbColor& color) {
        Super::SetColor(color);
    }

    // 可視設定
    void SetVisible( TbBool isVisible ) {
        Super::SetVisible( isVisible );
    }

    // 可視設定取得
    TbBool IsVisible() const {
        return Super::IsVisible();
    }

    // 名前設定
    void SetName(const char* name) {
        Super::SetName(name);
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

public: // TRS

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

    // 回転設定
    void SetRotation( TbAngle32 angle ) {
        Super::SetRotation( angle );
    }

    // アルファ設定
    void SetAlpha(TbFloat32 alpha) {
        Super::SetAlpha(alpha);
    }

    // アルファ設定
    TbFloat32 GetAlpha() const {
        return Super::GetAlpha();
    }

public:

    // 横幅設定
    void SetWidth( TbFloat32 width );

    // 高さ設定
    void SetHeight( TbFloat32 height );

    // サイズ設定
    void SetSize( TbFloat32 width , TbFloat32 height );

    // サイズ取得
    const TbDimensionF32& GetSize() const {
        return m_Size;
    }

    // 横幅取得
    TbFloat32 GetWidth() const {
        return m_Size.width;
    }

    // 縦幅取得
    TbFloat32 GetHeight() const {
        return m_Size.height;
    }

public:

    // UV座標のみ設定
    void SetUV( const UvParam& param );

private:

    // サイズ更新
    void updateSize();

private:

    TbDimensionF32      m_BasePlaceSize;
    TbDimensionF32      m_Size;
    TbSpriteNodeTree    m_NodeTree;

};

}

#endif
