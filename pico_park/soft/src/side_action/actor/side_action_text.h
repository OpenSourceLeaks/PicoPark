/*!
 * テキスト
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_TEXT_H_
#define _INCLUDED_SIDE_ACTION_TEXT_H_

#include <graphics/sprite/tb_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/side_action_types.h"

class SideActionText : public CrActionActor
{
public:
    
    typedef SideActionText Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionText();

    // デストラクタ
    virtual ~SideActionText();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;


    // 定期処理
    virtual void OnUpdateAction(TbFloat32 deltatime);

public:

    // 文字列設定
    void SetText( const char* text ) {
        m_Text = text;
    }

    // フォントサイズ設定
    void SetFontSize(TbSint32 size) {
        m_FontSize = size;
    }

    // 生存時間設定
    void SetLifeSec(TbFloat32 sec)
    {
        m_LifeSec = sec;
    }

private:

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbStaticString64            m_Text;
    CrActionActorSpriteHooker   m_SpriteHooker;
    TbSint32                    m_FontSize;
    TbColor                     m_FontColor;
    TbColor                     m_FontBorderColor;
    TbFloat32                   m_LifeSec; // 生存時間
    TbBool                      m_HasBorder;

};
#endif
