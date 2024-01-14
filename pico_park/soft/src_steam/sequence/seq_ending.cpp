/*!
 * エンディングシーケンス
 * @author teco
 * @since 2012.05.25
 */

#include "fwd.h"
#include "seq_ending.h"
#include "seq_manager.h"
#include "root.h"
#include <graphics/font/tb_font_buffer_2d.h>

/*!
 * コンストラクタ
 * @author teco
 */
SeqEnding::SeqEnding()
    : Super(SEQ_ENDING,"SeqEnding")
    , m_Phase(0)
    , m_PhaseSec(0.0f)
{
    TbRectF32 rect(-64.0f,-64.0f,128.0f , 128.0f);
    TbRectF32 uvRect(CMN_TEX_UV_RECT(528,512,96,96));
    m_Sprite.Initialize(rect,uvRect);
}

/*!
 * デストラクタ
 * @author teco
 */
SeqEnding::~SeqEnding()
{
}

/*!
 * 更新
 * @author teco
 */
void SeqEnding::OnUpdate()
{
    switch (m_Phase)
    {
    case 0:
        FadeIn(2.0f);
        ++m_Phase;
        break;
    case 1:
        if (IsFading()) {
            break;
        }
        m_PhaseSec += 1.0f/60.0f;
        if (m_PhaseSec < 5.0f) {
            break;
        }
        FadeOut(2.0f);
        ++m_Phase;
    case 2:
        if (IsFading()) {
            break;
        }
        SeqManager::GetInstance().RequestSequence(SEQ_TITLE,0);
        break;
    }
}

/*!
* 描画関数
* @author teco
*/
void SeqEnding::OnDraw()
{
    {
        static TB_NDEBUG_CONST TbFloat32 TEXT_Y = 300.0f;
        static TB_NDEBUG_CONST TbFloat32 FONT_SIZE = 64.0f;
        TbFontBuffer2d* font = Root::GetInstance().GetFont();
        font->PushSetting();
        font->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
        font->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);
        font->SetFontSize(FONT_SIZE);

        TbFloat32 x = (CMN_WINDOW_WIDTH / 2.0f);
        font->Printf(x, TEXT_Y, "THANK YOU FOR PLAYING");
        font->PopSetting();
    }

    static TB_NDEBUG_CONST TbFloat32 SPRITE_Y = 460.0f;
    static TB_NDEBUG_CONST TbFloat32 SCALE = 2.0f;
    m_Sprite.Calculate();
    TbMatrix mtx;
    mtx.Translate(CMN_WINDOW_WIDTH/2.0f,SPRITE_Y,0.0f);
    mtx.Scale(SCALE, SCALE,1.0f);
    m_Sprite.Draw(mtx);
}

