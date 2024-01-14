/*!
 * ゴール
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_GOAL_H_
#define _INCLUDED_SIDE_ACTION_GOAL_H_

#include <graphics/sprite/tb_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionGoal : public ActionActor
{
public:

    typedef SideActionGoal  Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionGoal();

    // デストラクタ
    virtual ~SideActionGoal();

public:

    // 開く
    void Open();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    enum Flag
    {
        FLAG_OPEN ,
    };

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

    // テキスト描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

private:

    TbBitArray32     m_BitArray;
    TbSpriteNodeTree m_SpriteNode;
    ActionActorSpriteHooker  m_SpriteHooker;

};

#endif
