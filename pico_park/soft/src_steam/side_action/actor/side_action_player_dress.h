/*!
 * プレイヤー用被り物
 * @author teco
 */

#ifndef _INCLUDED_ACTION_PLAYER_DRESS_H_
#define _INCLUDED_ACTION_PLAYER_DRESS_H_

#include "common/actor/action_actor.h"
#include "side_action/side_action_types.h"

class SideActionPlayer;

class SideActionPlayerHeaddress : public ActionActor
{
public:

    enum Type
    {
        TYPE_HELMET , 
        TYPE_HALOWEEN1 ,
        TYPE_HALOWEEN2 ,
        TYPE_MAX
    };

public:

    typedef SideActionPlayerHeaddress Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerHeaddress( SideActionPlayer* player , Type type );

    // デストラクタ
    virtual ~SideActionPlayerHeaddress();

public:

    // ロジック後処理
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    Type                m_Type;
    TbBitArray32        m_BitArray;
    SideActionPlayer*   m_Player;
};

class SideActionPlayerBackdress : public ActionActor
{
public:

    enum Type
    {
        TYPE_POWERED , 
        TYPE_MAX
    };

public:

    typedef SideActionPlayerBackdress Self;
    typedef ActionActor Super;

public:

    // コンストラクタ
    SideActionPlayerBackdress( SideActionPlayer* player , Type type );

    // デストラクタ
    virtual ~SideActionPlayerBackdress();

public:

    // ロジック後処理
    virtual void OnPostAction( TbFloat32 deltatime );

    // イベント通知
    virtual TbSint32 NotifyEvent( TbUint32 event , void* param );

private:

    Type                m_Type;
    TbBitArray32        m_BitArray;
    SideActionPlayer*   m_Player;
};

#endif
