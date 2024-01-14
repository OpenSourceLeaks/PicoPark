/*!
 * エンドレススクロール用判定
 * @author teco
 */

#ifndef _INCLUDED_SIDE_ACTION_ENDLESS_SCROLL_SCORE_H_
#define _INCLUDED_SIDE_ACTION_ENDLESS_SCROLL_SCORE_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include <crayon/actor/cr_action_actor.h>
#include "side_action/actor/side_action_score_base.h"
#include "side_action/side_action_types.h"

class SideActionEndlessScrollScore : public SideActionScoreBase
{
public:

    typedef SideActionEndlessScrollScore Self;
    typedef SideActionScoreBase Super;

public:

    // コンストラクタ
    SideActionEndlessScrollScore();

    // デストラクタ
    virtual ~SideActionEndlessScrollScore();

public:

    // イベント通知
    TbSint32 NotifyEvent(TbUint32 event, void* param);

public:

    // ロジック更新
    virtual void OnUpdateAction(TbFloat32 deltatime) TB_OVERRIDE;

private:

    // 前景描画
    void onDrawText(const TbMatrix& poseMatrix, TbSpriteMaterial* material);

private:

    TbUint32                  m_Score;
    CrActionActorSpriteHooker m_SpriteHooker;

};

#endif
