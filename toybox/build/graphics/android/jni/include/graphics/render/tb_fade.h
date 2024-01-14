/*!
 * フェード
 * @author teco
 */

#ifndef _INCLUDED_TB_FADE_H_
#define _INCLUDED_TB_FADE_H_

#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite_all.h>
#include <base/math/tb_geometry.h>

namespace toybox
{

class TbFade
{
public:

    // コンストラクタ
    TbFade();

    // デストラクタ
    ~TbFade();

public:

    // リセット
    void Reset() {
        m_State = STATE_NONE;
        m_Alpha = 0.0f;
    }

    // サイズ設定
    void SetSize( TbFloat32 w , TbFloat32 h ) {
        m_Size.width = w;
        m_Size.height = h;
    }

    // カラー設定
    void SetColor( const TbColorU32& color ){
        m_Color = color;
    }
    
    // カラー取得
    TbColorU32 GetColor() {
        return m_Color;
    }

    // フェードイン
    void FadeIn( TbFloat32 sec );

    // フェードアウト
    void FadeOut( TbFloat32 sec );

    // フェード中か
    TbBool IsFading() const
    {
        return STATE_NONE != m_State;
    }

public:

    // 定期処理
    void Update();

    // 描画
    void Draw();

private:

    enum
    {
        STATE_NONE ,
        STATE_IN ,
        STATE_OUT ,
    };
    
private:

    TbSint32        m_State; 
    TbColorU32      m_Color;
    TbFloat32       m_Alpha;
    TbFloat32       m_FadeRestSec;
    TbFloat32       m_FadeSec;
    TbDimensionF32  m_Size;
    
};

}

#endif
