/*!
 * 鍵箱
 * @author teco
 * @since 2012.05.25
 */

#ifndef _INCLUDED_SIDE_ACTION_KEY_BOX_H_
#define _INCLUDED_SIDE_ACTION_KEY_BOX_H_

#include <crayon/actor/cr_action_actor.h>
#include "side_action_key.h"
#include "root.h"

class SideActionBallKeyBox : public CrActionActor
{
public:
    
    typedef SideActionBallKeyBox Self;
    typedef CrActionActor Super;
public:

    // コンストラクタ
    SideActionBallKeyBox();

    // デストラクタ
    virtual ~SideActionBallKeyBox();

public:

    // 任意の方向から押されているか
    TbBool isPushed( CrContactFace type );

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

private:

    // 接触開始コールバック
    virtual void onCollided(CrActionCollision* collision);

private:

    TbUint32        m_DeadTime;
    SideActionKey*  m_Key;

};

class SideActionLaserKeyBox : public CrActionActor
{
public:
    
    typedef SideActionLaserKeyBox Self;
    typedef CrActionActor Super;
public:

    // コンストラクタ
    SideActionLaserKeyBox();

    // デストラクタ
    virtual ~SideActionLaserKeyBox();

public:

    // セットアップ
    virtual void Setup(const CrActorFactory::UserData& userData) TB_OVERRIDE
    {
        if (userData.AsString(0)) {
            TbUint32 speedIndex = TbMax<TbUint32>(1, Root::GetInstance().GetPlayerCount() - 1);
            SetLevelUpTarget(userData.AsString(0), userData.AsFloat32(speedIndex));
        }
    }

public:

    // レベルアップ対象設定
    void SetLevelUpTarget( const char* target , TbFloat32 param ){
        m_LevelUpTarget = target;
        m_LevelUpValue = param;
    }

public:

    // ロジック更新
    virtual void OnUpdateAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent(TbUint32 event, void* param);

private:

    // 接触開始コールバック
    virtual void onContacted(const TbVector2& normal,CrActionCollision::ContactTarget target,const CrActionCollisionInfo& collision);

private:

    TbUint32            m_DeadTime;
    TbSint32            m_Count;
    SideActionKey*      m_Key;
    TbStaticString32    m_LevelUpTarget;
    TbFloat32           m_LevelUpValue;


};


#endif
