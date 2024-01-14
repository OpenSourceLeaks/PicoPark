/*!
 * フェード
 * @author teco
 */

#include "fwd.h"
#include "common/cmn_fade.h"
#include "common/cmn_simple_sprite.h"

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
CmnFade::CmnFade()
    : m_State(STATE_NONE)
    , m_Alpha(0.0f)
    , m_FadeSec(0.0f)
    , m_FadeRestSec(0.0f)
{
    m_Color = TbColor::White();
    TbRectF32 rect(0,0,CMN_WINDOW_WIDTH, CMN_WINDOW_HEIGHT);
    m_Sprite.Initialize(rect);
    m_Sprite.SetColor(m_Color);
}

/*!
 * デストラクタ
 * @author teco
 */
CmnFade::~CmnFade()
{
}
    
/*!
 * フェードイン
 * @author teco
 */
void CmnFade::FadeIn( TbFloat32 sec )
{
    m_FadeSec = sec;
    m_FadeRestSec = sec;
    m_State = STATE_IN;
}

/*!
 * フェードアウト
 * @author teco
 */
void CmnFade::FadeOut( TbFloat32 sec )
{
    m_FadeSec = sec;
    m_FadeRestSec = sec;
    m_State = STATE_OUT;
}

/*!
 * 定期処理
 * @author teco
 */
void CmnFade::Update()
{
    TbFloat32 deltasec = 1/60.0f; // 要検討
    if( m_State != STATE_NONE ){
        m_FadeRestSec = TbMax(m_FadeRestSec-deltasec,0.0f);
    }
    TbFloat32 value = TB_FLOAT32_EQUAL_ZERO(m_FadeSec) ? 0.0f : m_FadeRestSec/m_FadeSec;
    switch( m_State )
    {
    case STATE_IN:
        m_Alpha = value;
        if( TB_FLOAT32_EQUAL_ZERO(m_Alpha) ){
            m_State= STATE_NONE;
        }
        break;
    case STATE_OUT:
        m_Alpha = 1.0f-value;
        if( TB_FLOAT32_EQUAL_ZERO(m_Alpha-1.0f) ){
            m_State= STATE_NONE;
        }
        break;
    default:
        break;
    }
}

/*!
 * 描画処理
 * @author teco
 */
void CmnFade::Draw()
{
    if( TB_FLOAT32_EQUAL_ZERO(m_Alpha) ){
        return;
    }
    m_Sprite.SetAlpha(m_Alpha);
    m_Sprite.Calculate();
    m_Sprite.Draw(TbMatrix::IDENTITY);
}

}
