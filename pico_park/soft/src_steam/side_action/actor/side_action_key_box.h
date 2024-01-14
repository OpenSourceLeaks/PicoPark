/*!
 * 鍵箱
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_KEY_BOX_H_
#define _INCLUDED_SIDE_ACTION_KEY_BOX_H_

#include "common/actor/action_actor.h"
#include "side_action_key.h"

class SideActionBallKeyBox : public ActionActor
{
public:
    
    typedef SideActionBallKeyBox Self;
    typedef ActionActor Super;
public:

    // コンストラクタ
    SideActionBallKeyBox();

    // デストラクタ
    virtual ~SideActionBallKeyBox();

public:

    // 任意の方向から押されているか
    TbBool isPushed( CmnContactFace type );

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 接触開始コールバック
    virtual void onCollided(ActionCollision* collision);

private:

    TbUint32        m_DeadTime;
    SideActionKey*  m_Key;

};

class SideActionLaserKeyBox : public ActionActor
{
public:
    
    typedef SideActionLaserKeyBox Self;
    typedef ActionActor Super;
public:

    // コンストラクタ
    SideActionLaserKeyBox();

    // デストラクタ
    virtual ~SideActionLaserKeyBox();

public:

    // レベルアップ対象設定
    void SetLevelUpTarget( const char* target , TbFloat32 param ){
        m_LevelUpTarget = target;
        m_LevelUpValue = param;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal,ActionCollision::ContactTarget target,const ActionCollisionInfo& collision);

private:

    TbUint32            m_DeadTime;
    TbSint32            m_Count;
    SideActionKey*      m_Key;
    TbStaticString32    m_LevelUpTarget;
    TbFloat32           m_LevelUpValue;


};


#endif
