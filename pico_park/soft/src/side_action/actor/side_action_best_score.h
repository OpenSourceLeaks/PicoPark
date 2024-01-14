/*!
 * ベストスコア
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_BEST_SCORE_H_
#define _INCLUDED_SIDE_ACTION_BEST_SCORE_H_

#include <crayon/actor/cr_action_actor.h>
#include "ui/ui_message_window.h"

class SideActionBestScore : public CrActionActor
{
public:
    
    typedef SideActionBestScore Self;
    typedef CrActionActor Super;

public:

    // コンストラクタ
    SideActionBestScore();

    // デストラクタ
    virtual ~SideActionBestScore();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE;

public:

    // ロジック更新
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    // ライン描画
    void onDraw( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbBool                      m_IsWhite;
    CrActionActorSpriteHooker   m_SpriteHooker;

};

#endif
