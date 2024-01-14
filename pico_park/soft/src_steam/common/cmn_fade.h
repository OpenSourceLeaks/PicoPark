/*!
 * フェード
 * @author teco
 */

#ifndef _INCLUDED_CMN_FADE_H_
#define _INCLUDED_CMN_FADE_H_

#include <graphics/render/3dapi/tb_color.h>
#include <base/math/tb_vector2.h>
#include <graphics/sprite/tb_sprite_all.h>
#include <base/math/tb_geometry.h>

#include "common/cmn_simple_sprite.h"
#include <base/util/tb_singleton.h>

namespace toybox
{

class CmnFade : public TbSingletonRef<CmnFade>
{
public:

    // コンストラクタ
    CmnFade();

    // デストラクタ
    ~CmnFade();

public:

    // リセット
    void Reset() {
        m_State = STATE_NONE;
        m_Alpha = 0.0f;
    }

    // カラー設定
    void SetColor( const TbColor& color ){
        m_Color = color;
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
    TbColor         m_Color;
    TbFloat32       m_Alpha;
    TbFloat32       m_FadeRestSec;
    TbFloat32       m_FadeSec;
    CmnWhiteSprite  m_Sprite;
    
};

}

#endif
