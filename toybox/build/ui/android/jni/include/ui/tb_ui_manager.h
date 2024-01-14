/*!
 * UI管理クラス
 * @author toybox
 */

#ifndef _INCLUDED_TB_UI_MANAGER_H_
#define _INCLUDED_TB_UI_MANAGER_H_

#include <base/util/tb_singleton.h>
#include <graphics/sprite/tb_sprite_scene.h>
#include <graphics/sprite/tb_sprite_material.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <graphics/font/tb_font_buffer_2d.h>
#include <ui/tb_ui_input.h>

namespace toybox
{

class TbUiWindow;

enum TbUiUvType
{
    TB_UI_UV_WINDOW             , 
    TB_UI_UV_WINDOW_FULLSCREEN  , 
    TB_UI_UV_BUTTON             , 
    TB_UI_UV_BUTTON_FOCUS       , 
    TB_UI_UV_BUTTON_PRESS       , 
    TB_UI_UV_TEXT_BOX           , 
    TB_UI_UV_TEXT_BOX_FOCUS     , 
    TB_UI_UV_FRAME              , 
    TB_UI_UV_FRAME_FILL         , 
    TB_UI_UV_MAX
};

enum TbUiUvRectType
{
    TB_UI_UV_RECT_LINE             , 
    TB_UI_UV_RECT_WINDOW_CLOSE       , 
    TB_UI_UV_RECT_MAX
};

enum TbUiSeType
{
   TB_UI_SE_OPEN , 
   TB_UI_SE_SELECT , 
   TB_UI_SE_CANCEL , 
   TB_UI_SE_MAX
};

class TbUiManager : public toybox::TbSingletonRef<TbUiManager>
{
public:
    
    // コンストラクタ
    TbUiManager( TbUint32 renderPassIndex );

    // デストラクタ
    ~TbUiManager();

public:
    
    // 定期処理
    void Update();

    // 描画処理
    void Draw();

public:

    // レンダーパスインデックス取得
    TbUint32 GetRenderPassIndex() const {
        return m_RenderPassIndex;
    }

    // フォントバッファ取得
    TbFontBuffer2d* GetFontBuffer() {
        return m_FontBuffer;
    }

    // フォントバッファ取得
    void SetFontBuffer( TbFontBuffer2d*  font ) {
        m_FontBuffer = font;
    }

    // テクスチャ設定
    void SetTexture( TbTexture* texture );

    // ウィンドウ追加
    void AddWindow( TbUiWindow* window );

    // ウィンドウ除去
    void RemoveWindow( TbUiWindow* window );

public:

    // UVパラメータ取得
    const TbElasticSprite::UvParam* GetUV( TbUiUvType type ){
        if( 0 <= type && type < TB_UI_UV_MAX ) {
            return &m_UvParam[type];
        }
        return nullptr;
    }

    // UVパラメータ取得
    void SetUV( TbUiUvType type , const TbElasticSprite::UvParam& param ){
        if( 0 <= type && type < TB_UI_UV_MAX ) {
            m_UvParam[type] = param;
        }
    }

    // UVパラメータ取得
    void SetUV( TbUiUvType type , const TbRectF32Pod (&param)[TbElasticSprite::PLACE_MAX] ){
        if( 0 <= type && type < TB_UI_UV_MAX ) {
            for( TbSizeT i = 0; i < TB_ARRAY_COUNT_OF(param); ++i ){ 
                m_UvParam[type].rectUV[i] = param[i];
            }
        }
    }

    // UVパラメータ取得
    TbVector2 GetUVPlayerOffset( TbUiUvType type ){
        if( 0 <= type && type < TB_UI_UV_MAX ) {
            return m_UvPlayerOffset[type];
        }
        return TbVector2();
    }

    // UVパラメータ取得
    void SetUVPlayerOffset( TbUiUvType type , const TbVector2& offset ){
        if( 0 <= type && type < TB_UI_UV_MAX ) {
            m_UvPlayerOffset[type] = offset;
        }
    }

    // UVパラメータ取得
    const TbRectF32* GetUVRect( TbUiUvRectType type ){
        if( 0 <= type && TB_ARRAY_COUNT_OF(m_UvRectParam) ) {
            return &m_UvRectParam[type];
        }
        return nullptr;
    }

    // UVパラメータ取得
    void SetUVRect( TbUiUvRectType type , const TbRectF32& param ){
        if( 0 <= type && type < TB_ARRAY_COUNT_OF(m_UvRectParam) ) {
            m_UvRectParam[type] = param;
        }
    }

    // フルスクリーンサイズ設定
    void SetFullscreenSize( TbFloat32 width , TbFloat32 height ) {
        m_FullscreenSize.width = width;
        m_FullscreenSize.height = height;
    }

    // フルスクリーンサイズ取得
    const TbDimensionF32& GetFullscreenSize() const {
        return m_FullscreenSize;
    }

public:

    // スプライトシーン取得
    TbSpriteScene& GetSpriteScene() {
        return m_SpriteScene;
    }

public: // 入力

    // 入力設定
    void SetInput( TbUiInput* input ){
        m_Input = input;
    }

    // 押した
    TbBool IsPress( TbUiInput::Button button , TbUint32 playerBit ) const {
        if( m_Input ){
            return m_Input->IsPress( button , playerBit );
        }
        return TB_FALSE;
    }

    // 押した
    TbBool IsPress(TbUiInput::Button button, TbUint32 playerBit , TbUint32* inputPlayer ) const {
        if (m_Input) {
            return m_Input->IsPress(button, playerBit,inputPlayer);
        }
        return TB_FALSE;
    }

    // 押した(リピート)
    TbBool IsOnRepeat( TbUiInput::Button button , TbUint32 playerBit ) const {
        if( m_Input ){
            return m_Input->IsOnRepeat( button , playerBit );
        }
        return TB_FALSE;
    }

    // 押している
    TbBool IsOn( TbUiInput::Button button , TbUint32 playerBit ) const {
        if( m_Input ){
            return m_Input->IsOn( button , playerBit );
        }
        return TB_FALSE;
    }

    // カーソル数
    TbUint32 GetCursorCount() const {
        if( m_Input ){
            return m_Input->GetCursorCount();
        }
        return 0;
    }

    // カーソル座標
    TbBool GetCursorPos( TbVector2* dst , TbUint32 index ) const {
        if( m_Input ){
            return m_Input->GetCursorPos(dst,index);
        }
        return TB_FALSE;
    }

    // カーソル押した
    TbBool IsCursorPress( TbUint32 index ) {
        if( m_Input ){
            return m_Input->IsCursorPress(index);
        }
        return TB_FALSE;
    }

    // カーソル押している
    TbBool IsCursorOn( TbUint32 index ) {
        if( m_Input ){
            return m_Input->IsCursorOn(index);
        }
        return TB_FALSE;
    }

    // カーソルオーバーしているか
    TbBool IsCursorOver( TbUint32 index , const TbRectF32& rect , TbBool isEnableCursorRadius ) {
        if( m_Input ){
            return m_Input->IsCursorOver(index,rect,isEnableCursorRadius);
        }
        return TB_FALSE;
    }

    // カーソル動いている
    TbBool IsCursorMoving( TbUint32 index) const {
        if( m_Input ){
            return m_Input->IsCursorMoving(index);
        }
        return TB_FALSE;
    }

    // カーソルオーバーしている
    TbBool IsAnyoneCursorOver( const TbRectF32& rect , TbBool isEnableCursorRadius , TbUint32* dstIndex = nullptr ) const {
        if( m_Input ){
            return m_Input->IsAnyoneCursorOver(rect,isEnableCursorRadius,dstIndex);
        }
        return TB_FALSE;
    }

    // カーソル押した
    TbBool IsAnyoneCursorPress( const TbRectF32& rect , TbBool isEnableCursorRadius ,  TbUint32* dstIndex = nullptr ) {
        if( m_Input ){
            return m_Input->IsAnyoneCursorPress(rect,isEnableCursorRadius,dstIndex);
        }
        return TB_FALSE;
    }

public: // SE

    // SE読み込み
    void LoadSE( TbUiSeType type , const TbChar8* fpath );

    // SE利用は全て可能か
    TbBool IsReadySE() const;

    // SE再生
    void PlaySE( TbUiSeType type );

private:

    //
    void collectGabage();

private:

    TbUint32                    m_RenderPassIndex;
    TbArray<TbUiWindow*>        m_Windows;          // ウィンドウリスト
    TbArray<TbUiWindow*>        m_RemoveWindows;    // ウィンドウリスト
    TbSpriteScene               m_SpriteScene;      // スプライトシーン
    TbSpriteFixedMaterial       m_SpriteMaterial;   // スプライト用マテリアル
    TbFontBuffer2d*             m_FontBuffer;       // フォントバッファ
    TbDimensionF32              m_FullscreenSize;   // フルスクリーンサイズ
    TbElasticSprite::UvParam    m_UvParam[TB_UI_UV_MAX];
    TbVector2                   m_UvPlayerOffset[TB_UI_UV_MAX];
    TbRectF32                   m_UvRectParam[TB_UI_UV_RECT_MAX];
    TbUiInput*                  m_Input;

};

}

#endif
