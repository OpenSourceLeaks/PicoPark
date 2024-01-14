/*!
 * 時計
 */

#ifndef _INCLUDED_SIDE_ACTION_WATCH_H_
#define _INCLUDED_SIDE_ACTION_WATCH_H_

#include <crayon/actor/cr_action_actor.h>
#include <graphics/font/tb_font.h>
#include <graphics/font/tb_font_buffer_2d.h>

class SideActionWatch : public CrActionActor
{
public:
    
    typedef SideActionWatch Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionWatch();

    // デストラクタ
    virtual ~SideActionWatch();

private:

    // 前景描画
    void onDrawTime( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // タイムアップ
    void onTimeUp();

private:

    CrActionActorSpriteHooker m_SpriteHooker;
    TbFontSjis*             m_Font;         // 専用フォント
    TbFontBuffer2d*         m_FontBuffer;   // フォントバッファ

};

#endif
