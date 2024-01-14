/*!
 * テキスト
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TEXT_H_
#define _INCLUDED_SIDE_ACTION_TEXT_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionText : public ActionActor
{
public:
    
    typedef SideActionText Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionText();

    // デストラクタ
    virtual ~SideActionText();

public:

    // 文字列設定
    void SetText( const char* text ) {
        m_Text = text;
    }

    // フォントサイズ設定
    void SetFontSize(TbSint32 size) {
        m_FontSize = size;
    }

private:

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbStaticString64         m_Text;
    ActionActorSpriteHooker  m_SpriteHooker;
    TbSint32                 m_FontSize;

};
#endif
