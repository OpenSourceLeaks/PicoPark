/*!
 * 鍵
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_KEY_H_
#define _INCLUDED_SIDE_ACTION_KEY_H_

#include <graphics/sprite/tb_sprite.h>
#include <graphics/sprite/tb_elastic_sprite.h>
#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionKey : public ActionActor
{
public:
    
    typedef SideActionKey Self;
    typedef ActionActor Super;

    enum Type
    {
        TYPE_NORMAL ,
        TYPE_BREAKOUT
    };

public:

    // コンストラクタ
    SideActionKey( Type type = TYPE_NORMAL );

    // デストラクタ
    virtual ~SideActionKey();

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

public:

    // 初期位置計算
    TbVector2 CalcInitPos( const TbVector2& pos , TbFloat32 offset );

    // アクティブ設定
    void SetActive( TbBool isActive );

    // アクティブ設定取得
    TbBool IsActive() const {
        return m_BitArray.Test(FLAG_ACTIVE);
    }

    // アタッチプレイヤー番号取得
    TbBool IsPlayerAttached() const {
        return m_AttachTarget != nullptr;
    }

    // スイッチターゲット
    void SetSwitchTarget( const char* target ) {
        m_SwitchTarget = target;
    }

private:

    enum Flag
    {
        FLAG_ACTIVE , 
        FLAG_MAX
    };

    enum State
    {
        STATE_READY , 
        STATE_REQ_END ,
        STATE_END ,
    };

private:

    // 衝突コールバック(押し戻しは無しバージョン)
    void onCollided(ActionCollision* target);

    // 前景描画
    void onDrawText( const TbMatrix& poseMatrix , TbSpriteMaterial* material );

    // 壊せるブロックが残っているか
    TbBool existsBreakBlock();

public:

    Type             m_Type;
    State            m_State;
    TbBitArray32     m_BitArray;
    ActionActor*     m_AttachTarget;
    TbRectF32        m_AttachRect;
    TbStaticString64 m_SwitchTarget;

    ActionActorSpriteHooker  m_SpriteHooker;

};

#endif
