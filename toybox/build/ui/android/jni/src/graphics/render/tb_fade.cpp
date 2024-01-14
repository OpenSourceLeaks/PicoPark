/*!
 * フェード
 * @author teco
 */

#include <tb_fwd.h>
#include <graphics/render/tb_fade.h>
#include <graphics/render/tb_draw_2d.h>

namespace toybox
{

/*!
 * コンストラクタ
 * @author teco
 */
TbFade::TbFade()
    : m_State(STATE_NONE)
    , m_Alpha(0.0f)
    , m_FadeSec(0.0f)
    , m_FadeRestSec(0.0f)
{
    m_Color = TbColorU32::White();
    m_Size.width = 10.0f;
    m_Size.height = 10.0f;
}

/*!
 * デストラクタ
 * @author teco
 */
TbFade::~TbFade()
{
}
    
/*!
 * フェードイン
 * @author teco
 */
void TbFade::FadeIn( TbFloat32 sec )
{
    m_FadeSec = sec;
    m_FadeRestSec = sec;
    m_State = STATE_IN;
}

/*!
 * フェードアウト
 * @author teco
 */
void TbFade::FadeOut( TbFloat32 sec )
{
    m_FadeSec = sec;
    m_FadeRestSec = sec;
    m_State = STATE_OUT;
}

/*!
 * 定期処理
 * @author teco
 */
void TbFade::Update()
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
void TbFade::Draw()
{
    if( TB_FLOAT32_EQUAL_ZERO(m_Alpha) ){
        return;
    }
    TbDraw2dBegin();
    TbDrawRectParam param;
    param.SetColor(TbColor::FromRgbaF32(m_Color.GetRedF32(),
                                        m_Color.GetGreenF32(),
                                        m_Color.GetBlueF32(),
                                        m_Alpha));
    param.SetRect(0.0f,0.0f,m_Size.width,m_Size.height);
    TbDrawRect(param);
    TbDraw2dEnd();
}

}
