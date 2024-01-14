/*!
 * テキスト
 * @author teco
 */

#include "fwd.h"
#include "side_action_text.h"
#include "root.h"

/*! 
 * コンストラクタ
 * @author teco
 */
SideActionText::SideActionText()
    : m_FontSize(-1)
{
    AddComponent(&m_SpriteHooker);
    m_SpriteHooker.GetSprite().SetDrawFunc(TbCreateMemFunc(this,&Self::onDrawText));
    m_SpriteHooker.GetSprite().SetZ(CMN_SPRITE_Z_FRONT_4);
};

/*! 
 * デストラクタ
 * @author teco
 */
SideActionText::~SideActionText()
{
}

/*! 
 * 前景描画
 * @author teco
 */
void SideActionText::onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material )
{
    const TbVector2& drawPos = GetDrawPos();
    TbFontBuffer2d*  fontBuffer = Root::GetInstance().GetLocalFont();
    fontBuffer->PushSetting();
    if( 0 <= m_FontSize ){
        fontBuffer->SetFontSize(m_FontSize);
    }
    fontBuffer->SetAlignX(TbFontBuffer2d::ALIGN_X_CENTER);
    fontBuffer->SetAlignY(TbFontBuffer2d::ALIGN_Y_CENTER);

    fontBuffer->Printf(static_cast<TbSint32>(drawPos.GetX()),
                        static_cast<TbSint32>(drawPos.GetY()),m_Text.GetBuffer());

    fontBuffer->PopSetting();
    fontBuffer->Flush();
}
